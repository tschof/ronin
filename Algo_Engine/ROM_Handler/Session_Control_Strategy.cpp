#include "Session_Control_Strategy.hpp"
#include <cassert>
#include "Session.hpp"
#include "Bullseye/DART_Log.hpp"

std::ostream &operator<<(std::ostream &stream, const Session_States state)
{
  switch (state)
  {
  case DISCONNECTED:
    stream << "Disconnected";
    break;
  case CONNECTED:
    stream << "Connected";
    break;
  case LOGGED_ON:
    stream << "Logged On";
    break;
  default:
    stream << "Unknown";
    break;
  }
  return stream;
}

Session_Control_Strategy::Session_Control_Strategy()
: currstate_(DISCONNECTED)
, session_(0)
{
}

void Session_Control_Strategy::session(Session *session)
{
  this->session_ = session;
}

Session_States Session_Control_Strategy::current_state() const
{
  return this->currstate_;
}

Session *Session_Control_Strategy::session() const
{
  return this->session_;
}

bool Session_Control_Strategy::on_send(ACE_Message_Block *mb)
{
  bool result(false);
  if (this->currstate_ == LOGGED_ON)
    result = this->session()->on_send(mb);
  return result;
}

void Client_Session_Control_Strategy::on_connect()
{
  switch (this->currstate_)
  {
  case DISCONNECTED:
    assert(this->session_);
    if (this->session_->logon())
      this->currstate_ = CONNECTED;
    DART_DEBUG ("%s: Moving from DISCONNECTED to CONNECTED\n",__FUNCTION__);
    break;
  case CONNECTED:
    assert(false);
    break;
  case LOGGED_ON:
    assert(false);
    break;
  default:
    DART_WARNING ("%s: Unknown state %d\n",__FUNCTION__,this->currstate_);
    assert(false);
    break;
  }
}

void Client_Session_Control_Strategy::on_disconnect()
{
  switch (this->currstate_)
  {
  case DISCONNECTED:
    // This will get called by Connection::handle_close even
    // when a connection wasn't completed.  Do nothing.
    break;
  case CONNECTED:
    this->currstate_ = DISCONNECTED;
    DART_DEBUG ("%s: Moving from CONNECTED to DISCONNECTED\n",__FUNCTION__);
    break;
  case LOGGED_ON:
    DART_DEBUG ("%s: Moving from LOGGED_ON to DISCONNECTED\n",__FUNCTION__);
    this->currstate_ = DISCONNECTED;
    break;
  default:
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t) %s: Unknown state %d\n"),
               ACE_TEXT(__FUNCTION__),this->currstate_));
    assert(false);
    break;
  }
}

void Client_Session_Control_Strategy::on_message(ACE_Message_Block *mb)
{
  switch (this->currstate_)
  {
  case DISCONNECTED:
    DART_WARNING ("%s: Received message while in DISCONNECTED state: %.*C\n",
      __FUNCTION__,
      mb->length() ? mb->length()-1 : 0,
      mb->rd_ptr());
    break;

  case CONNECTED:
    // Expecting a logon message, check here
    if (mb && mb->length())
    {
      bool shutdown(false);
      switch (mb->rd_ptr()[0])
      {
      case 'L':
        {
          Logon_Credentials credentials;
          if (this->session()->validate_logon_message(mb,credentials))
          {
            this->currstate_ = LOGGED_ON;
            DART_DEBUG ("%s: Moving from CONNECTED to LOGGED_ON\n",
              __FUNCTION__);
          }
        }
        mb->release();
        break;
      case 'K':
        DART_DEBUG ("%s: Logon rejected, dropping connection: %.*C\n",
          __FUNCTION__,
          mb->length() ? mb->length()-1 : 0,
          mb->rd_ptr());
        shutdown = true;
        break;
      default:
        DART_DEBUG ("%s: Unexpected message, dropping connection: %.*C\n",
          __FUNCTION__,
          mb->length() ? mb->length()-1 : 0,
          mb->rd_ptr());
        shutdown = true;
        break;
      }

      if (shutdown)
      {
        this->session()->shutdown_io_handler();
        this->currstate_ = DISCONNECTED;
        DART_DEBUG ("%s: Moving from CONNECTED to DISCONNECTED\n",__FUNCTION__);
      }

    }
    break;

  case LOGGED_ON:
    this->session()->pass_message_to_application(mb);
    break;

  default:
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("(%t) %s: Unknown state %d\n"),
               ACE_TEXT(__FUNCTION__),this->currstate_));
    assert(false);
    break;
  }
}

void Server_Session_Control_Strategy::on_connect()
{
  switch (this->currstate_)
  {
  case DISCONNECTED:
    assert(this->session_);
    this->currstate_ = CONNECTED;
    DART_DEBUG ("%s: Moving from DISCONNECTED to CONNECTED\n",__FUNCTION__);
    break;
  case CONNECTED:
    DART_CRITICAL ("%s: Received CONNECT event while in CONNECTED state\n",
      __FUNCTION__);
    assert(false);
    break;
  case LOGGED_ON:
    DART_CRITICAL ("%s: Received CONNECT event while in LOGGED_ON state\n",
      __FUNCTION__);
    assert(false);
    break;
  default:
    DART_WARNING ("%s: Unknown state %d\n",__FUNCTION__,this->currstate_);
    assert(false);
    break;
  }
}

void Server_Session_Control_Strategy::on_disconnect()
{
  switch (this->currstate_)
  {
  case DISCONNECTED:
    // This will get called by Connection::handle_close even
    // when a connection wasn't completed.  Do nothing.
    break;
  case CONNECTED:
    this->currstate_ = DISCONNECTED;
    DART_DEBUG ("%s: Moving from CONNECTED to DISCONNECTED\n",__FUNCTION__);
    break;
  case LOGGED_ON:
    DART_DEBUG ("%s: Moving from LOGGED_ON to DISCONNECTED\n",__FUNCTION__);
    this->currstate_ = DISCONNECTED;
    break;
  default:
    DART_WARNING ("%s: Unknown state %d\n",__FUNCTION__,this->currstate_);
    assert(false);
    break;
  }
}

void Server_Session_Control_Strategy::on_message(ACE_Message_Block *mb)
{
  switch (this->currstate_)
  {
  case DISCONNECTED:
    DART_CRITICAL ("%s: Received message while in DISCONNECTED state\n");
    DART_INFO ("%s: Moving from DISCONNECTED to CONNECTED state\n");
    this->currstate_=CONNECTED;
    // Fall through to CONNECTED state
  case CONNECTED:
    // Expecting a logon message, check here
    if (mb && mb->length() && mb->rd_ptr()[0] == 'L' &&
        this->session()->validate_logon_message
          (mb,this->session()->peer_credentials_))
    {
      if (this->session()->logon(0))
      {
        if (this->session()->end_of_rebroadcast())
        {
          this->currstate_ = LOGGED_ON;
          DART_DEBUG ("%s: Moving from CONNECTED to LOGGED_ON\n",__FUNCTION__);
        }
        else
        {
          this->currstate_ = DISCONNECTED;
          DART_DEBUG ("%s: Could not send end of rebroadcast message\n",
            __FUNCTION__);
        }
      }
      else
      {
        this->session()->shutdown_io_handler();
        this->currstate_ = DISCONNECTED;
        DART_DEBUG ("%s: Problem in generating logon, shutting down\n",
          __FUNCTION__);
      }
    }
    else
    {
      DART_DEBUG ("%s: Unexpected message, dropping connection\n",__FUNCTION__);
      this->session()->shutdown_io_handler();
      this->currstate_ = DISCONNECTED;
      DART_DEBUG ("%s: Moving from CONNECTED to DISCONNECTED\n",__FUNCTION__);
    }
    break;

  case LOGGED_ON:
    this->session()->pass_message_to_application(mb);
    break;

  default:
    DART_WARNING ("%s: Unknown state %d\n",__FUNCTION__,this->currstate_);
    assert(false);
    break;
  }
}
