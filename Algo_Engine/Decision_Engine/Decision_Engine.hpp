#ifndef __DECISION_ENGINE_HPP__
#define __DECISION_ENGINE_HPP__

#include <ace/Task.h>
#include <ace/Reactor.h>
#include <ace/Reactor_Notification_Strategy.h>
#include "Customer_Order.hpp"

class CRomCsv;
class ACE_Event_Handler;

namespace dart
{
  class Algo_Manager;

  class Decision_Engine
    : public ACE_Task <ACE_MT_SYNCH>
  {
  public:
    Decision_Engine (const CRomCsv&, Algo_Manager*);

    // ACE_Task overrides
    virtual int open(void * = 0);
    virtual int handle_timeout (const ACE_Time_Value&,
                                const void * = 0);
    virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
    virtual int put (ACE_Message_Block *, ACE_Time_Value * = 0);

  protected:
    virtual int setup_initial_timer () = 0;

    Algo_Manager *algo_manager_;

  private:
    ACE_Reactor reactor_;
    ACE_Reactor_Notification_Strategy notifier_;
  };
}

#endif
