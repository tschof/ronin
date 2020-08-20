#include "Session.hpp"
#include <Decision_Engine/Algo_Manager.hpp>
#include "Connection.hpp"
#include "Message_File_Store.hpp"
#include "Session_Control_Strategy.hpp"
#include <Bullseye/DART_CSV_Message.hpp>
#include "COMMON\ROMFIXENGINE\FixEngine\romfielddefs.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include <Configurator/configuration.h>
#include "ROM_Handler/romcsv.h"
#include "Message_Types.hpp"
#include "Bullseye/DART_Log.hpp"
#include <cassert>

bool create_logon_msg (ACE_Message_Block *mb, const std::string &username, const std::string &password)
{
  bool result(false);
  if (mb)
  {
    CRomCsv logonmsg;
    logonmsg.SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_LOGON);
    if (!username.empty() && !password.empty())
    {
      logonmsg.SetAt(RFIX::CSV_TRADER, username);
      logonmsg.SetAt(RFIX::CSV_PASSWORD, password);
      const std::string &logonstr(logonmsg.Message());
      mb->msg_type(ACE_Message_Block::MB_DATA);
      if (!logonstr.empty()
          && mb->size(logonstr.size()) == 0
          && mb->copy(logonstr.c_str(),logonstr.size()) == 0)
        result = true;
    }
  }
  return result;
}

Session::Session (Session_Control_Strategy *control_strategy)
: io_handler_ (0)
, msgs_ ()
, id_ ()
, received_logon_ (false)
, sent_logon_ (false)
, seq_num_in_(1)
, seq_num_out_(1)
, control_strategy_(control_strategy)
{
  if (control_strategy_)
    this->control_strategy_->session(this);
}

void Session::begin_filestore_message_request (size_t /* starting_sequence_number */)
{
  ACE_Message_Block *mb = 0;
  ACE_NEW (mb, ACE_Message_Block (sizeof (Session*) + this->id_.length () + 1));
  if (mb)
  {
    mb->msg_type (MB_REQUEST_MESSAGES_FROM_FILESTORE);
    Session *psession = this;
    if (mb->copy ((const char*)&psession, sizeof (Session*)) != -1)
    {
      if (mb->copy (this->id_.c_str (), this->id_.length ()) != -1)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("Requesting messages for session %s\n"),
                    this->id_.c_str ()));
        if (MESSAGE_FILE_STORE::instance ()->putq (mb) == -1)
          ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%p\n"),
                      ACE_TEXT ("putting message")));
      }
    }
  }
}

void Session::io_handler (dart::Connection *io_handler)
{
  this->io_handler_ = io_handler;
}

void Session::on_connect ()
{
  if (this->control_strategy_)
    this->control_strategy_->on_connect();
}

void Session::on_disconnect()
{
  if (this->control_strategy_)
    this->control_strategy_->on_disconnect();
}

bool Session::logon (ACE_Message_Block *mb)
{
  bool result(false);

  if (mb == 0)
    mb = new ACE_Message_Block (32);

  // on_send is used to bypass the seqnum incrementing and storage, etc.
  if (create_logon_msg (mb,
                        this->logon_credentials_.username,
                        this->logon_credentials_.password)
      && this->on_send(mb))
    result = true;

  return result;
}

bool Session::validate_logon_message(ACE_Message_Block *mb, Logon_Credentials &credentials)
{
  bool result(false);
  if (mb && mb->length())
  {
    Csv_Msg msg(mb->rd_ptr(), mb->length());
    const char *command = msg.getstring(RFIX::CSV_COMMAND);
    if (command && command[0] == ROMVALUE::COMMAND_LOGON && command[1] == '\0')
    {
      // TODO: Complete logon validation, possibly user, password, and seq num.
      const char *username = msg.getstring(RFIX::CSV_TRADER);
      if (username)
      {
        credentials.username = username;
        result = true;
      }
    }
  }
  return result;
}

bool
Session::shutdown_io_handler ()
{
  bool result = false;
  if (this->io_handler_)
  {
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (0, ACE_Message_Block::MB_STOP),
                    false);
    if (this->io_handler_->put (mb) != -1)
      result = true;
  }
  return result;
}

Session_States Session::current_state() const
{
  return this->control_strategy_->current_state();
}

bool
Session::send (CRomCsv &csv)
{
  bool result(false);
  const std::string &msg(csv.Message());
  ACE_Message_Block *mb = new ACE_Message_Block(msg.size()+1);
  if (mb->copy(msg.c_str(), msg.length()) == 0)
  {
    assert (mb->rd_ptr()[mb->length()-1] == '\n');
    result = this->send(mb);
  }
  else
    delete mb;
  return result;
}

bool
Session::send (ACE_Message_Block *mb)
{
  // Regardless the outcome of actually sending the data we need to also always
  // store the message in a file for later recovery and increment the sequence
  // number.
  // TODO: Incrementing the sequence numbers and writing to a recovery file might
  // be better off being directed by the control strategy object.  This way, we
  // could allow these things to be turned off and the session would be none the
  // wiser.

  assert (mb->rd_ptr()[mb->length()-1] == '\n');

  this->store_message(mb->clone());
  this->increment_outbound_sequence_number();

  return this->control_strategy_->on_send(mb);
}

void Session::increment_inbound_sequence_number()
{
  ++this->seq_num_in_;
}

void Session::increment_outbound_sequence_number()
{
  ++this->seq_num_out_;
}

void
Session::store_message(ACE_Message_Block *mb)
{
  if (mb)
  {

  }
}

/**
 * This method only gets called when the connection is open and data can be
 * sent on the connection.  This only queues data to be sent, it does not
 * track sequence numbers, etc.
 */
bool
Session::on_send(ACE_Message_Block *mb)
{
  bool result(false);
  if (mb)
  {
    if (this->io_handler_)
      result = (this->io_handler_->put (mb) != -1);
  }

  return result;
}

int
Session::put (ACE_Message_Block *mb, ACE_Time_Value*)
{
  int result(-1);
  if (mb)
  {
    if (this->msgframer_.add(mb))
    {
      result = 0;
      bool done(false);
      while (!done)
      {
        ACE_Message_Block *msg = new ACE_Message_Block(512);
        if (this->msgframer_.next_message(msg))
        {
          DART_INFO ("Received message: {|%.*C|}\n",
            msg->length() ? msg->length()-1 : 0,
            msg->rd_ptr());
          this->control_strategy_->on_message(msg);
        }
        else
        {
          msg->release();
          done = true;
        }
      }
    }
    mb->release();
  }
  return result;
}

bool
Session::end_of_rebroadcast()
{
  bool result(false);
  CRomCsv msg;
  msg.SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_QUERY);
  const std::string msgstr(msg.Message());
  ACE_Message_Block *mb = new ACE_Message_Block(msgstr.size()+1);
  if (mb->copy(msgstr.c_str(),msgstr.size())==0 && this->io_handler_->put(mb)!=-1)
    result = true;
  else
    mb->release();
  return result;
}

void
Session::on_message_retrieval (ACE_Message_Block*)
{

}

void
Session::pass_message_to_application(ACE_Message_Block *mb)
{
  if (mb)
  {
    ACE_Message_Block *mb_iterator=mb;
    while (mb_iterator->cont())
      mb_iterator=mb_iterator->cont();
    mb_iterator->cont
      (new ACE_Message_Block(this->peer_credentials_.username.size()+1));
    mb_iterator->cont()->copy(this->peer_credentials_.username.c_str());
    mb_iterator->cont()->msg_type(MB_SESSION_SOURCE_NAME);
    if (dart::ALGO_MANAGER::instance()->put(mb) == -1)
      ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t) %s: %p\n"),ACE_TEXT(__FUNCTION__),
                 ACE_TEXT("putting message to algo_manager")));
  }
}
