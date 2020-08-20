#ifndef __ORDER_MANAGER_SERVER_HPP__
#define __ORDER_MANAGER_SERVER_HPP__

#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/Reactor.h>
#include <ace/Reactor_Notification_Strategy.h>
#include <ace/Task.h>

class Connection_Manager;
class Session;

namespace dart
{
  class Connection
    : public ACE_Svc_Handler <ACE_SOCK_Stream, ACE_MT_SYNCH>
  {
  public:
    Connection ();
    virtual ~Connection ();
    virtual int open (void * = 0);
    virtual int handle_input  (ACE_HANDLE = ACE_INVALID_HANDLE);
    virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
    virtual int handle_close  (ACE_HANDLE, ACE_Reactor_Mask);
    virtual int handle_timeout (const ACE_Time_Value&, const void*);
    virtual int put (ACE_Message_Block*, ACE_Time_Value* = 0);
    Session *session() const;
    void session (Session*);
  private:
    typedef ACE_Svc_Handler <ACE_SOCK_Stream, ACE_MT_SYNCH> super;
    ACE_Reactor_Notification_Strategy notifier_;
    Connection_Manager *conn_mgr_;
    Session *session_;
  };
}

#endif
