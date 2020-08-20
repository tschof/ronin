#ifndef __SESSION_CONTROL_STRATEGY_HPP__
#define __SESSION_CONTROL_STRATEGY_HPP__

/**
 * @class Session_Control_Strategy
 * @brief Changes event responses according to classes that implement this
 * interface.
 *
 * The Session object keeps all the standard functionality needed of a Session.
 * Each Session is configured with a Session_Control_Strategy that determines
 * how the Session reacts to different events.  The main differences are between
 * client and server.
 */

class ACE_Message_Block;
class Session;

#include <ostream>

  enum Session_States
  {
    DISCONNECTED,
    CONNECTED,
    LOGGED_ON,
  };


std::ostream &operator<<(std::ostream&, const Session_States);

class Session_Control_Strategy
{
public:
  Session_Control_Strategy();
  void session(Session*);
  Session *session() const;
  Session_States current_state() const;
  virtual void on_connect()=0;
  virtual void on_disconnect()=0;
  virtual void on_message(ACE_Message_Block*)=0;
  virtual bool on_send(ACE_Message_Block*);

protected:
  Session_States currstate_;
  Session *session_;
};


class Client_Session_Control_Strategy
  : public Session_Control_Strategy
{
public:
  virtual void on_connect();
  virtual void on_disconnect();
  virtual void on_message(ACE_Message_Block*);
};

class Server_Session_Control_Strategy
  : public Session_Control_Strategy
{
public:
  virtual void on_connect();
  virtual void on_disconnect();
  virtual void on_message(ACE_Message_Block*);
};

#endif
