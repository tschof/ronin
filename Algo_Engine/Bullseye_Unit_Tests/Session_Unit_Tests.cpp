#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>
#include <boost/test/unit_test.hpp>
#include <Bullseye/DART_CSV_Message.hpp>
#include "ROM_Handler/romcsv.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

#pragma region Utility_functions

struct Logon
{
  std::string username;
  std::string password;
  int  sequence_number;
  bool resend_required;
  std::string errormsg;
};

/**
 * @arg mb Must point to a valid message, rd_ptr() must be set to first char of
 * message and total length must be indicated by length().  No newline
 * terminator is necessary.
 * @arg logon Logon structure, only modified if return value is true, otherwise
 * contents are unchanged.
 * @returns True if contents of logon structure were filled in.
 */
bool parse_logon_message(ACE_Message_Block *mb, Logon *logon)
{
  bool result(false);
  if (mb && logon)
  {
    Csv_Msg message(mb->rd_ptr(),mb->length());
    const char *p = message.getstring(RFIX::CSV_TRADER);
    if (p)
      logon->username=p;

    p=message.getstring(RFIX::CSV_PASSWORD);
    if (p)
      logon->password=p;

    if (!logon->username.empty() && !logon->password.empty())
    {
      const char *seqn = message.getstring(RFIX::CSV_LASTRECEIVED_SEQ);
      logon->sequence_number = seqn ? atoi(seqn) : -1;
      result = true;
    }
  }
  return result;
}

#pragma endregion

#pragma region IO_Handler_Event_Callback_interface_declaration

/*
 * The IO_Handler_Event_Callback is closely tied to the IO_Handler, it is the
 * only interface the IO_Handler knows about.  The IO_Handler dispatches events
 * to the object with this interface registered with it.
 */

class IO_Handler_Event_Callback
{
public:
  virtual void on_connect()=0;
  virtual void on_disconnect()=0;
  virtual void on_data(ACE_Message_Block*)=0;
};

#pragma endregion

#pragma region Data_Sender_interface_declaration

/*
 * The Data_Sender is a simple interface from internal components to the
 * outside world.
 */
class Data_Sender
{
public:
  virtual bool send(ACE_Message_Block*)=0;
};

#pragma endregion

#pragma region Session_interface_declaration

class Session_Controller;

class Session
{
public:
  virtual void next_state(Session_Controller*)=0;
  virtual bool logon()=0;
  virtual bool logoff()=0;
  virtual bool validate_logon(ACE_Message_Block*,Logon*)=0;
  virtual bool start_resend_request()=0;
  virtual bool reject_logon()=0;
  virtual bool pass_along_message(ACE_Message_Block*)=0;
  virtual bool disconnect()=0;
};

#pragma endregion

#pragma region Session_Controller_interface_declaration

class Session_Controller
{
public:
  Session_Controller();
  virtual void session(Session*);
  virtual Session *session() const;
  virtual void on_connect()=0;
  virtual void on_disconnect()=0;
  virtual void on_message(ACE_Message_Block*)=0;
protected:
  Session *session_;
};

Session_Controller::Session_Controller()
  : session_(0)
{
}

void Session_Controller::session(Session *session)
{
  this->session_ = session;
}

Session *Session_Controller::session() const
{
  return this->session_;
}

#pragma endregion

#pragma region Session_Impl

#pragma region Session_Impl_declaration

class Session_Impl
  : public Session
  , public IO_Handler_Event_Callback
{
public:
  // Session_Impl methods
  Session_Impl();
  Session_Controller *get_controller() const;

  // Session methods
  virtual void next_state(Session_Controller*);
  virtual bool logoff() { return false; }
  virtual bool validate_logon(ACE_Message_Block*,Logon*);
  virtual bool start_resend_request();
  virtual bool reject_logon() { return false; }
  virtual bool logon();
  virtual bool pass_along_message(ACE_Message_Block*) { return false; }
  virtual bool disconnect() { return false; }

  // IO_Handler_Event_Callback methods
  virtual void on_connect();
  virtual void on_disconnect();
  virtual void on_data(ACE_Message_Block*);

protected:
  Session_Controller *controller_;
};

#pragma endregion

#pragma region Session_Impl_methods

Session_Impl::Session_Impl()
: controller_(0)
{
}

Session_Controller *Session_Impl::get_controller() const
{
  return this->controller_;
}

#pragma message("TODO: Implement creation of logon message")
bool Session_Impl::logon()
{
  return false;
}

#pragma message("TODO: Further implement logon validation")
bool Session_Impl::validate_logon(ACE_Message_Block*,Logon*)
{
  return true;
}

bool Session_Impl::start_resend_request()
{
  return false;
}

#pragma endregion

#pragma region Session_methods

void Session_Impl::next_state(Session_Controller *next)
{
  this->controller_ = next;
}

#pragma endregion

#pragma region IO_Handler_Event_Callback_methods

void Session_Impl::on_connect()
{
  if (this->controller_)
    this->controller_->on_connect();
  else
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("%s controller_ is null\n"),__FUNCTION__));
}

void Session_Impl::on_disconnect()
{
}

void Session_Impl::on_data(ACE_Message_Block *message_block)
{
#pragma message("TODO: Frame the message here")
  if (this->controller_)
    this->controller_->on_message(message_block);
  else
    ACE_DEBUG((LM_DEBUG,ACE_TEXT("%s controller_ is null\n"),__FUNCTION__));
}

#pragma endregion

#pragma endregion

#pragma region Runtime_Client_State

class Runtime_Client_State : public Session_Controller
{
public:
  virtual void on_connect() {}
  virtual void on_disconnect() {}
  virtual void on_message(ACE_Message_Block*) {}
};

#pragma endregion

#pragma region Initial_Client_State

/**
 * @class Unauthenticated_State
 * @brief Used when disconnected or connected
 * without having completed the logon handshake.
 */
class Initial_Client_State : public Session_Controller
{
public:
  virtual void on_connect();
  virtual void on_disconnect() {}
  virtual void on_message(ACE_Message_Block*);
};

void Initial_Client_State::on_connect()
{
  // As a client, the first thing we would like to do is send a logon message
  if (this->session_)
    this->session_->logon();
  else
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("%s session_ is null\n"), __FUNCTION__));
}

void Initial_Client_State::on_message(ACE_Message_Block *mb)
{
  if (mb && mb->length() && mb->rd_ptr()[0] == ROMVALUE::COMMAND_LOGON)
  {
    if (this->session_)
    {
      Logon logon;
      if (this->session_->validate_logon(mb,&logon))
      {
        if (logon.resend_required)
          this->session_->start_resend_request();
        this->session_->next_state(new Runtime_Client_State);
        delete this;
      }
    }
  }
}

#pragma endregion

#pragma region Session_Impl_Mock

class Session_Impl_Mock
  : public Session_Impl
{
public:
  typedef Session_Impl base;

  // Session_Impl_Mock methods
  Session_Impl_Mock();
  void reset();

  // Session_Impl_Mock state
  bool logon_called;
  bool next_state_called;

  // Session methods
  virtual bool logon();
  virtual void next_state(Session_Controller*);
};

Session_Impl_Mock::Session_Impl_Mock()
{
  this->reset();
}

void Session_Impl_Mock::reset()
{
  this->logon_called = false;
  this->next_state_called = false;
}

bool Session_Impl_Mock::logon()
{
  this->logon_called = true;
  return base::logon();
}

void Session_Impl_Mock::next_state(Session_Controller *controller)
{
  this->next_state_called = true;
  return base::next_state(controller);
}

#pragma endregion

BOOST_AUTO_TEST_CASE(Basic_session_test)
{
  Session_Impl_Mock test_session;

  BOOST_CHECK(test_session.get_controller() == 0);

  Initial_Client_State *initial_client_state = new Initial_Client_State;
  test_session.next_state(initial_client_state);
  initial_client_state->session(&test_session);

  BOOST_CHECK(test_session.get_controller() == initial_client_state);

  test_session.reset();

  IO_Handler_Event_Callback &io_handler_callback = test_session;
  io_handler_callback.on_connect();
  BOOST_CHECK_EQUAL(test_session.logon_called,true);
  BOOST_CHECK_EQUAL(test_session.next_state_called,false);

  CRomCsv logonmsg;
  logonmsg.SetAt(RFIX::CSV_COMMAND,ROMVALUE::COMMAND_LOGON);
  logonmsg.SetAt(RFIX::CSV_TRADER,"TRADER-A");
  const std::string logonmsgstr(logonmsg.Message());
  ACE_Message_Block *message_block = new ACE_Message_Block(logonmsgstr.size() + 1);
  message_block->copy(logonmsgstr.c_str());
  io_handler_callback.on_data(message_block);

  BOOST_CHECK_EQUAL(test_session.next_state_called,true);
}

BOOST_AUTO_TEST_CASE(Null_pointer_Session_test)
{
  Session_Impl_Mock test_session;
  Initial_Client_State *initial_state = new Initial_Client_State;
  initial_state;
  const char *msgstr = "L,,FOOBAR\n";
  ACE_Message_Block *msg = new ACE_Message_Block(strlen(msgstr)+1);
  msg;
}
