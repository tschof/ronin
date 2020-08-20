#include <ace/Message_Block.h>
#include <boost/test/unit_test.hpp>

// There are a couple of components here.  To make the coding of the session
// state a little easier to understand and possibly more easily extensible,
// the logic is encapsulated in a series of state machine classes.  Each
// state machine class implements a common interface.  This interface lists
// the common things that would be done in a session.  Each class implementing
// the interface represents a specific state of the session and will respond
// accordingly.  The common actions done to a state are:
// 1. Connect
// 2. Disconnect
// 3. Login
// 4. Logout
// 5. Receive Message
// 6. Send Message
//
// The state machine keeps a reference to a generic Session interface that has
// methods for all possible actions in response to the above mentioned events.
// The same class that implements the Session interface can also implement an
// interface known by Connection objects for indicating common actions such as:
// 1. Connection
// 2. Disconnection
// 3. Sending data
// 4. Receiving data

/*
class External_Connection_Interface
{
public:
  virtual bool send_message(ACE_Message_Block*)=0;
};

class Internal_Message_Handler_Interface
{
public:
  virtual bool on_connect()=0;
  virtual bool on_data(ACE_Message_Block*)=0;
  virtual bool on_disconnect()=0;
};

class Session_Controller
{
public:
  virtual bool disconnect()=0;

};
*/

#if 0

class Session_State;

// The main class will implement the session interface.
class Session_Interface
{
public:
  Session_Interface (Session_State *state)
    : state_ (state) {}

  virtual void next_state (Session_State *next_state)
  {
    this->state_ = next_state;
  }

  virtual Session_State *current_state() const
  {
    return this->state_;
  }

  virtual void logon () = 0;

protected:
  Session_State *state_;
};

struct Test_Session : public Session_Interface
{
  Test_Session (Session_State *state)
    : Session_Interface (state)
  {
    this->reset ();
  }

  // Session_Interface virtual methods
  virtual void logon ()
  {
    this->logon_called = true;
  }

  void reset ()
  {
    logon_called = false;
  }

  bool logon_called;
};

// The Session_State class hierarchy contains objects swapped out at runtime
// based on events happening.  Each class in the hierarchy should not contain
// any state, just a reference to the Session object; the Session object will
// contain all state.
class Session_State
{
public:
  Session_State ()
    : session_ (0) {}

  void session (Session_Interface *session)
  {
    this->session_ = session;
  }

  virtual void on_connect () = 0;
#if 0
  virtual void on_logon (ACE_Message_Block*) = 0;
  virtual void on_send_data (ACE_Message_Block*) = 0;
  virtual void on_receive_data (ACE_Message_Block*) = 0;
#endif
protected:
  Session_Interface *session_;
};


class Client_Disconnected_State : public Session_State
{
public:
  Client_Disconnected_State ()
    : sent_logon_ (false)
    , received_logon_ (false) {}

  virtual void on_connect ()
  {
    session_->logon ();
    this->sent_logon_ = true;
    if (this->received_logon_)
    {
//      this->session_->set_session_state (new Client_Connected_State);
      delete this;
    }
  }

  virtual void on_logon (ACE_Message_Block*)
  {
  }

protected:
  bool sent_logon_;
  bool received_logon_;
};


BOOST_AUTO_TEST_CASE (Session_Test_1)
{
  Session_State *state = new Client_Disconnected_State;
  Test_Session session (state);
  state->session(&session);

  session.current_state()->on_connect();
  BOOST_CHECK(session.logon_called);

  ACE_Message_Block *msg = new ACE_Message_Block(32);
  BOOST_CHECK_EQUAL(msg->copy ("L,,,FOO\n"),0);

}
#endif
