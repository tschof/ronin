#include "Application_Reactor_Thread_Pool.hpp"
#include "Bullseye/DART_Log.hpp"

Application_Reactor_Thread_Pool::Application_Reactor_Thread_Pool ()
: thread_pool_reactor_ (0)
, application_reactor_ (0)
{
  thread_pool_reactor_ = new ACE_TP_Reactor ();
  application_reactor_ = new ACE_Reactor (thread_pool_reactor_);
  ACE_Reactor::instance (application_reactor_);
}

Application_Reactor_Thread_Pool::~Application_Reactor_Thread_Pool ()
{
  delete application_reactor_;
  delete thread_pool_reactor_;
}

int Application_Reactor_Thread_Pool::svc ()
{
  DART_DEBUG ("Running reactor event loop\n");
  int result = ACE_Reactor::instance ()->run_reactor_event_loop ();
  if (result == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("(%t) %p\n"),
                       ACE_TEXT ("Error handling events")),
                      0);
  DART_INFO ("Event loop shutdown\n");
  return 0;
}
