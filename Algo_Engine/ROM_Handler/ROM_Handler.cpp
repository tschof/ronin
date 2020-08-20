#include "ROM_Handler.hpp"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include <Bullseye/Bullseye_Common.hpp>
#include <ace/Proactor.h>
#include "ROM_Handler/RomCsv.h"
#include <Decision_Engine/Algo_Manager.hpp>

namespace dart {

ROM_Handler::ROM_Handler (ROM_Session *psession, const std::string &name)
  : session_ (psession)
  , name_ (name)
  , notifier_ (ACE_Reactor::instance (),
               this,
               ACE_Event_Handler::WRITE_MASK)
{
  // Setup a notifier for this task so that message put
  // into our queue notify us so we can send the message.
  this->msg_queue ()->notification_strategy (&this->notifier_);
  session_->set_connection (this);
}

ROM_Handler::~ROM_Handler ()
{
  if (this->handle() != ACE_INVALID_HANDLE) {
    ACE_OS::closesocket (this->handle ());
  }
}

void
ROM_Handler::open (ACE_HANDLE h, ACE_Message_Block&)
{
  this->handle (h);

  // When initializing each of these, the default process-wide
  // Proactor is used; in the future we might explore whether
  // using a different model is appropriate, for instance using
  // one Proactor for ROM connections and another for Exchange
  // destination connections.
  if (this->reader_.open (*this) != 0 ||
      this->writer_.open (*this) != 0) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p\n"),
                ACE_TEXT (__FUNCTION__)));
    delete this;
    return;
  }

  ACE_Message_Block *mb (0);
  ACE_NEW_NORETURN (mb, ACE_Message_Block (1024));
  if (this->reader_.read (*mb, mb->space ()) != 0) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p\n"),
                ACE_TEXT ("ROM_Handler begin read")));
    mb->release ();
    delete this;
    return;
  }

  this->session_->on_connect ();

  // mb is now controlled by the Proactor framework.

  // Setup heartbeat timers
  ACE_Proactor *proactor = this->proactor ();
  if (proactor) {
    proactor->schedule_timer (*this,
                              0,
                              ACE_Time_Value (30),
                              ACE_Time_Value (30));
  }

  return;
}

void
ROM_Handler::handle_time_out (const ACE_Time_Value&,
                              const void*)
{
  // Send a heartbeat message
  static const char *heartbeat_str = "H,\n";
  ACE_Message_Block *mb = new ACE_Message_Block (strlen (heartbeat_str) + 1);
  if (mb->copy (heartbeat_str, strlen (heartbeat_str)) == -1) {
    ACE_DEBUG ((LM_DEBUG, "Heartbeat message generation failed"));
    return;
  }

  ACE_DEBUG ((LM_DEBUG, DART_LOG ("Sending heartbeat")));
  this->send (mb);
}

int
ROM_Handler::handle_output (ACE_HANDLE)
{
  ACE_Message_Block *mb = 0;
  ACE_Time_Value tv (ACE_Time_Value::zero);
  if (this->getq (mb, &tv) != -1) {
    if (this->writer_.write (*mb, mb->length ()) == -1) {
      ACE_ERROR_RETURN ((LM_ERROR,
        DART_LOG ("%p Write initiate.\n"), "ROM_Handler"),
        -1);
    }
  }
  return 0;
}

void ROM_Handler::die ()
{
  if (this->session_->is_upstream_connection ()) {
    dart::ALGO_MANAGER::instance ()->inbound_connection (0);
  }
  else {
    dart::ALGO_MANAGER::instance ()->outbound_connection (0);
  }
}

void
ROM_Handler::handle_read_stream (const ACE_Asynch_Read_Stream::Result &result)
{
  ACE_Message_Block &mb (result.message_block ());
  if (!result.success () || result.bytes_transferred () == 0) {
    ACE_DEBUG ((LM_DEBUG,
      "Read failed, deleting ROM_Handler %sf\n",
      this->name_.c_str ()));
    mb.release ();
    delete this;
    return;
  }

  ACE_Message_Block *msg = 0;
  while (this->framer_.frame (&mb, &msg)) {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%T (P:%P|T:%t) %s received: {|%.*C|}\n"),
                ACE_TEXT (this->name_.c_str ()),
                msg->length() ? msg->length()-1 : 0,
                msg->rd_ptr()));
    this->session_->handle_message (msg);
  }

  ACE_Message_Block *new_mb = 0;
  ACE_NEW_NORETURN (new_mb,
                    ACE_Message_Block (1024));
  if (new_mb) {
    if (this->reader_.read (*new_mb, new_mb->space ()) == -1) {
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("%p\n"),
                  ACE_TEXT ("ROM_Handler::handle_read_stream ()")));
    }
  }
}

void
ROM_Handler::handle_write_stream (const ACE_Asynch_Write_Stream::Result &result)
{
  if (!result.success () || result.bytes_transferred () == 0) {
    ACE_DEBUG ((LM_DEBUG,
      "Write failed, deleting ROM_Handler %sf\n",
      this->name_.c_str ()));
    result.message_block ().release ();
    delete this;
    return;
  }

  result.message_block ().release ();
  return;
}

void
ROM_Handler::handle_rom_message (ACE_Message_Block&)
{
}

void
ROM_Handler::disconnect ()
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (DART_LOG ("%s %s\n")),
              ACE_TEXT (this->name_.c_str ()),
              ACE_TEXT (__FUNCTION__)));
}

void
ROM_Handler::send (ACE_Message_Block *mb)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (DART_LOG ("%s sent: {|%.*C|}\n")),
              ACE_TEXT (this->name_.c_str ()),
              mb->length() ? mb->length()-1 : 0,
              mb->rd_ptr ()));

  size_t bytes_to_write (mb->length ());
  if (bytes_to_write > 0 && mb->rd_ptr () [bytes_to_write-1] == '\0') {
    --bytes_to_write;
  }

  assert (mb->rd_ptr () [0]);

  if (this->writer_.write (*mb, mb->length ()) != 0) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("%p\n"),
                ACE_TEXT ("starting write")));
    mb->release ();
  }
}

bool ROM_Handler::send (CRomCsv *pcsv)
{
  bool result (false);
  if (pcsv) {
    std::string &msgbuffer = pcsv->Message ();
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (msgbuffer.size ()), false);
    if (mb->copy (msgbuffer.c_str (), msgbuffer.size ()) == 0) {
      this->send (mb);
      result = true;
    }
    else {
      ACE_DEBUG ((LM_DEBUG,
        DART_LOG ("%s: Message block buffer copy failed\n"),
        __FUNCTION__));
    }
  }
  return result;
}

std::string
ROM_Handler::identify ()
{
  return this->name_;
}

}
