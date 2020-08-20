#include "Decision_Engine.hpp"
#include <Bullseye/Bullseye_Common.hpp>

namespace dart
{
  Decision_Engine::Decision_Engine(
    const CRomCsv&,
    Algo_Manager *algo_manager)
      : notifier_ (ACE_Reactor::instance(),this,ACE_Event_Handler::WRITE_MASK)
      , algo_manager_  (algo_manager)
  {
  }

  int Decision_Engine::open(void*)
  {
    this->reactor(ACE_Reactor::instance());
    this->msg_queue()->notification_strategy(&(this->notifier_));
    // Set the first timer to get things started
    return this->setup_initial_timer();
  }

  int Decision_Engine::handle_timeout(
    const ACE_Time_Value&,
    const void*)
  {
    return -1;
  }

  int Decision_Engine::handle_output (ACE_HANDLE)
  {
    return -1;
  }

  int Decision_Engine::put (
    ACE_Message_Block *mb,
    ACE_Time_Value *tv)
  {
    return this->putq (mb, tv);
  }
}
