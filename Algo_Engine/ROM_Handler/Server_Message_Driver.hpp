#ifndef __SERVER_MESSAGE_DRIVER_HPP__
#define __SERVER_MESSAGE_DRIVER_HPP__

#include <ace/Task.h>
#include <ace/Reactor.h>

class Connection_Manager;

class Server_Msg_Driver
  : public ACE_Task <ACE_MT_SYNCH>
{
public:
  Server_Msg_Driver ();
  void connection_manager (Connection_Manager*);
  virtual int svc ();
  virtual int handle_timeout (const ACE_Time_Value&, const void * = 0);
  virtual int put (ACE_Message_Block*, ACE_Time_Value * = 0);
private:
  ACE_Reactor reactor_;
  Connection_Manager *conn_mgr_;
};

#endif
