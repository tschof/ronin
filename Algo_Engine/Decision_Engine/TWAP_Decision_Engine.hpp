#ifndef __TWAP_DECISION_ENGINE_HPP__
#define __TWAP_DECISION_ENGINE_HPP__

#include "Decision_Engine.hpp"
#include "TWAP_Order.hpp"
#include "ROM_Handler/RomCsv.h"
#include "bullseye/bullseye_common.hpp"
#include "Order_management_system.hpp"

// Std C++
#include <set>

class Time_slice_manager;
struct Order_execution;

namespace dart
{
  class Algo_Manager;
  class TWAP_Time_Slice_Manager;

#ifdef USE_AGGRESSIVENESS
  enum Execution_Style
  {
    EXEC_STYLE_AGGRESSIVE,
    EXEC_STYLE_ACTIVE,
    EXEC_STYLE_PASSIVE,
  };
#endif

  class TWAP_Decision_Engine
    : public Decision_Engine
    , public Parent_order_event_callback
  {

  public:

    TWAP_Decision_Engine (const CRomCsv&, Algo_Manager*);

    // ACE overrides
    virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
    virtual int handle_timeout (const ACE_Time_Value&, const void* = 0);
    virtual int handle_exit (ACE_Process *);

    virtual int setup_initial_timer ();
    virtual bool cancel (const std::string& = "");
    virtual void on_child_order_cancelled ();

    virtual void on_execution(const Order_execution&);

    // Parent_order_event_callback methods
    virtual void all_out(const char *);

  protected:

    virtual int on_status_msg (const CRomCsv&);
    virtual void confirm_cancelled (const std::string& = "");

  private:

    // Differentiates the main reason for setting a timer in this object
    enum Timer_Type {
      SPAWN_MGR_OBJ = 0,
      VERIFY_CANCELS,
      ALL_DONE
    };

    // For tracking our general order state
    enum Order_State { LIVE = 0, PENDING_CANCEL, DEAD };

    TWAP_Order *order_;
    long timer_id;
    size_t shares_worked_;

#ifdef USE_AGGRESSIVENESS
    typedef std::set <TWAP_Time_Slice_Manager*> IM_Cont;
    IM_Cont interval_managers_;
    Execution_Style exec_style_;
#else
    typedef std::set<Time_slice_manager*> Slice_Mgr_Set;
    Slice_Mgr_Set slice_mgrs_;
#endif

    Order_State state_;


    // Threading Note: The ACE entry points can be entered by multiple threads
    // in the ACE thread pool simultaneously, these must be protected by this
    // mutex.
    bullseye::Mutex mutex_;
  };
}

#endif
