#include <ace/OS_NS_Time.h>
#include "Server_Message_Driver.hpp"
#include "Connection_Manager.hpp"

Server_Msg_Driver::Server_Msg_Driver ()
  : conn_mgr_ (0)
{
}

void Server_Msg_Driver::connection_manager (Connection_Manager *conn_mgr)
{
  this->conn_mgr_ = conn_mgr;
}

int Server_Msg_Driver::put (ACE_Message_Block *mb, ACE_Time_Value*)
{
  if (mb->msg_type () == ACE_Message_Block::MB_STOP)
    this->reactor ()->end_reactor_event_loop ();
  mb->release ();
  return 0;
}

int Server_Msg_Driver::svc ()
{
  this->reactor (&(this->reactor_));
  this->reactor ()->owner (ACE_Thread::self ());
  if (this->reactor ()->schedule_timer
    (this, 0, ACE_Time_Value (1), ACE_Time_Value (1)) == -1)
    return -1;
  this->conn_mgr_->reg (this);
  this->reactor ()->run_reactor_event_loop ();
  return 0;
}

int Server_Msg_Driver::handle_timeout (const ACE_Time_Value&, const void*)
{
  if (this->next ())
  {
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (32), -1);
    time_t now = ACE_OS::time ();
    int written = ACE_OS::snprintf (mb->wr_ptr (),
                                    mb->space (),
                                    "%s",
                                    ACE_OS::ctime (&now));
    if (written > 0)
      mb->wr_ptr (written);

    ACE_DEBUG ((LM_DEBUG, "%s Writing to queue: %s\n", __FUNCTION__, mb->rd_ptr ()));
    this->put_next (mb);
  }
  else
    ACE_DEBUG ((LM_DEBUG, "%s: No next pointer\n", __FUNCTION__));

  return 0;
}
