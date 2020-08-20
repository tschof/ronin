#ifndef __APPLICATION_REACTOR_THREAD_POOL_HPP__
#define __APPLICATION_REACTOR_THREAD_POOL_HPP__

#include <ace/Task.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>

/**
 * @class Application_Reactor_Thread_Pool
 * @brief Runs the reactor event loop for the entire application.  Activate n
 * of these objects to create n threads to service the Reactor.
 */
class Application_Reactor_Thread_Pool : public ACE_Task_Base
{
public:
  Application_Reactor_Thread_Pool ();
  virtual int svc ();
  ~Application_Reactor_Thread_Pool ();
private:
  ACE_TP_Reactor *thread_pool_reactor_;
  ACE_Reactor *application_reactor_;
};

#endif
