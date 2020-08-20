#include "Connection.hpp"
#include "Connection_Manager.hpp"
#include "Null_Session.hpp"
#include "Bullseye/DART_Log.hpp"
#include <cassert>

namespace dart
{
  Connection::Connection ()
    : notifier_ (ACE_Reactor::instance (), this, ACE_Event_Handler::WRITE_MASK)
    , conn_mgr_ (0)
    , session_ (0)
  {
    DART_DEBUG("(%t) %s\n",__FUNCTION__);

#if 0 // What is this for again?
    // Enable reference counting.
    this->reference_counting_policy ().value
      (ACE_Event_Handler::Reference_Counting_Policy::ENABLED);
#endif
  }

  Connection::~Connection ()
  {
    DART_DEBUG ("%s\n",__FUNCTION__);
  }

  void
  Connection::session (Session *session)
  {
    this->session_ = session;
  }

  Session* Connection::session() const
  {
    return this->session_;
  }

  int Connection::open (void *p)
  {
    if (super::open (p) == -1)
      return -1;

    this->msg_queue ()->notification_strategy (&(this->notifier_));

    // Register with the connection manager to receive shutdown requests
    CONNECTION_MANAGER::instance ()->reg (this);

    this->session_->on_connect();

    return 0;
  }

  int Connection::handle_input (ACE_HANDLE)
  {
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (512), -1);
    ssize_t recv_cnt = this->peer().recv (mb->rd_ptr(), mb->space());
    if (recv_cnt <= 0)
    {
      DART_WARNING ("Connection closed\n");
      mb->release();
      return -1;
    }

    mb->wr_ptr (recv_cnt);

    if (this->session_ && this->session_->put(mb) == -1)
    {
      DART_DEBUG ("Unable to put message to session\n");
      return -1;
    }

    return 0;
  }

  int Connection::handle_output (ACE_HANDLE)
  {
    ACE_Message_Block *mb = 0;
    ACE_Time_Value nowait (ACE_OS::gettimeofday ());
    while (this->getq (mb, &nowait) != -1)
    {
      if (mb->msg_type () == ACE_Message_Block::MB_STOP)
      {
        mb->release ();
        return -1;
      }

      const char last_char=mb->rd_ptr()[mb->length()-1];
      assert (last_char=='\n');last_char;

      ssize_t send_cnt = this->peer ().send (mb->rd_ptr (), mb->length ());
      if (send_cnt == -1)
      {
        DART_ERROR ("%p\n","send");
        return -1;
      }
      else
      {
        DART_DEBUG ("Sent %d bytes: {|%.*C|}\n",
                    send_cnt,
                    mb->length() ? mb->length()-1 : 0,
                    mb->rd_ptr());
        mb->rd_ptr (static_cast <size_t> (send_cnt));
      }

      if (mb->length ())
      {
        if (this->ungetq (mb) == -1)
          DART_ERROR ("%p\n","ungetq");
        break;
      }

      mb->release ();
    }

#if 0
    if (this->msg_queue ()->is_empty ())
      return this->reactor ()->register_handler
        (this, ACE_Event_Handler::WRITE_MASK);
#endif

    return 0;
  }

  int Connection::handle_close (ACE_HANDLE h, ACE_Reactor_Mask mask)
  {
    if (mask == ACE_Event_Handler::WRITE_MASK)
      return 0;
    CONNECTION_MANAGER::instance ()->unreg (this);
    if (this->session_)
    {
      this->session_->on_disconnect();
      this->session_->io_handler (0);
    }
    return super::handle_close (h, mask);
  }

  int Connection::put (ACE_Message_Block *mb, ACE_Time_Value*)
  {
    return this->putq (mb);
  }

  int Connection::handle_timeout (const ACE_Time_Value&, const void*)
  {
    return -1;
  }
}
