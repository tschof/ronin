#include "TWAP_Decision_Engine.hpp"
#include "TWAP_Order.hpp"
#include <ace/time_value.h>
#include <Bullseye/Bullseye_Common.hpp>
#include "Algo_Manager.hpp"

// ACE
#include <ace/OS.h>
#include <ace/Date_Time.h>

// ROM
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

// Application

// Market Data
#include <Market_Data/Market_Data_Source.hpp>
#include <Market_Data/Price_Block.hpp>
#include <ROM_Handler/ROM_Connection.hpp>

// Decision_Engine
#include "Order_Book.hpp"
#include "TWAP_Interval_Calculators.hpp"
#include "TWAP_Time_Slice_Manager.hpp"
#include "Aggressive_TWAP_Time_Slice_Manager.hpp"
#include "Active_TWAP_Time_Slice_Manager.hpp"
#include "Passive_TWAP_Time_Slice_Manager.hpp"
#include "Customer_directed_time_slice_manager.hpp"

namespace dart
{
  TIMER_TYPE operator++ (const TIMER_TYPE &timer_type)
  {
    switch (timer_type) {
      case INITIAL_TIMER:
        return MIDPOINT_TIMER;
      case MIDPOINT_TIMER:
        return THREE_QUARTER_TIMER;
      case THREE_QUARTER_TIMER:
        return FINAL_TIMER;
      default:
        throw std::runtime_error ("Cannot increment enumeration further");
    }
  }


  TWAP_Decision_Engine::TWAP_Decision_Engine (const CRomCsv &csv, Algo_Manager *pmanager)
    : Decision_Engine (csv,pmanager)
    , timer_id (-1)
    , shares_worked_ (0)
    , state_ (LIVE)
#ifdef USE_AGGRESSIVENESS
    , exec_style_ (EXEC_STYLE_AGGRESSIVE)
#endif
  {
    std::string errmsg;
    this->order_ = new TWAP_Order (csv, 0, 0, dart::twap_interval_calculator_v3);
    if (this->order_->validate (errmsg) == false)
      throw std::runtime_error (errmsg);

    ACE_Date_Time start_time(this->order_->start());
    ACE_Date_Time stop_time(this->order_->stop());
    DART_DEBUG(
      "%s: START %02d:%02d:%02d.%06d  STOP %02d:%02d:%02d.%06d\n",
      __FUNCTION__,
      start_time.hour(),
      start_time.minute(),
      start_time.second(),
      start_time.microsec(),
      stop_time.hour(),
      stop_time.minute(),
      stop_time.second(),
      stop_time.microsec());


    ACE_Time_Value duration(this->order_->stop()-this->order_->start());
    this->order_->calculate(this->order_->shares(), duration.sec());

    DART_DEBUG
      ("TWAP Strategy: trade %d every %d seconds %d microseconds\n",
       this->order_->shares_per_interval(),
       this->order_->interval_duration().sec(),
       this->order_->interval_duration().usec());
  }

  int
  TWAP_Decision_Engine::setup_initial_timer()
  {
    bullseye::Guard guard (this->mutex_);

    ACE_Time_Value now(ACE_OS::gettimeofday());
    ACE_Time_Value first_timer(this->order_->start()-now);
    ACE_Time_Value interval(this->order_->interval_duration());
    this->timer_id = this->reactor()->schedule_timer
      (this, 0, first_timer);

    if (this->timer_id) {
      // Setup the last timer, at which time we
      // clean up and make sure we are all out.
      ACE_Time_Value all_done_timer(this->order_->stop()-now);
      long all_done_timer_id=this->reactor()->schedule_timer
        (this,reinterpret_cast<void*>(ALL_DONE),all_done_timer);
      if (all_done_timer_id)
        return 0;
      else
        return -1;
    }
    else
      return -1;
  }

  int TWAP_Decision_Engine::handle_timeout(
    const ACE_Time_Value &timeout,
    const void *timeout_arg)
  {
    bullseye::Guard guard(this->mutex_);

    DART_INFO("%s\n",__FUNCTION__);

    if (this->state_!=LIVE) {
      DART_DEBUG("%s: Nothing to do, state is PENDING_CANCEL\n",__FUNCTION__);
      return 0;
    }

    // First, see if we're done
    if (!this->order_->check_invariants()) {
      ACE_DEBUG ((LM_DEBUG, DART_LOG ("invariants check failed!\n")));
      dart::ALGO_MANAGER::instance ()->reject_order
        (this->order_->original_order (), "internal error: invariants violated\n");
      return -1;
    }

    Timer_Type timer_type=(Timer_Type)(int)(timeout_arg);
      // Seriously? This is the cast that's needed to shove a const void* into an enum?!@#@$
      // The (int) is needed.  And forget the "proper" casts like const_cast<>(), etc.

    if (timer_type==ALL_DONE) {
      const std::string parent_id=this->order_->original_order().GetAt
        (RFIX::CSV_STAFFORD_TAG);
      ORDER_MANAGER::instance()->cancel_all_suborders(parent_id.c_str(),this);
      return 0;
    }

    if (this->shares_worked_ < this->order_->shares()) {
      size_t shares_to_work(this->order_->shares_per_interval());
      if ((this->order_->shares() - this->shares_worked_) < this->order_->shares_per_interval()) {
        // This is an exception, this really should not happen
        shares_to_work = (this->order_->shares() - this->shares_worked_);
      }

#ifdef USE_AGGRESSIVENESS
      dart::TWAP_Time_Slice_Manager *interval_manager=
        new Customer_directed_time_slice_manager;
      switch (this->exec_style_) {
        case EXEC_STYLE_AGGRESSIVE:
          interval_manager = new dart::Aggressive_TWAP_Time_Slice_Manager
            (this->order_,
             this,
             this->algo_manager_->get_market_data_source(),
             this->algo_manager_->outbound_connection(),
             shares_to_work);
          break;
        case EXEC_STYLE_ACTIVE:
          interval_manager = new dart::Active_TWAP_Time_Slice_Manager
            (this->order_,
             this,
             this->algo_manager_->get_market_data_source(),
             this->algo_manager_->outbound_connection(),
             shares_to_work);
          break;
        case EXEC_STYLE_PASSIVE:
          interval_manager = new dart::Passive_TWAP_Time_Slice_Manager
            (this->order_,
             this,
             this->algo_manager_->get_market_data_source(),
             this->algo_manager_->outbound_connection(),
             shares_to_work);
          break;
        default:
          throw std::runtime_error ("Unknown execution style");
      }

      if (interval_manager->open() == -1)
      {
        ACE_DEBUG((LM_DEBUG, "Couldn't open interval manager\n"));
        delete interval_manager;
        return -1;
      }

      if (this->interval_managers_.insert(interval_manager).second == false)
      {
        ACE_DEBUG ((LM_DEBUG, DART_LOG ("Error inserting interval manager pointer\n")));
      }
#else

      // This is a new, parallel scheme for tracking time slices but is only
      // used for configurations not looking at the aggressiveness option.  If
      // this proves to be a better design we can leave this and convert the
      // aggressiveness-configured time slice managers to implement this
      // interface.

      Time_slice_manager *slice_manager=new Customer_directed_time_slice_manager;
      Interval interval(timeout,timeout+this->order_->interval_duration());
      slice_manager->time_slice(interval);
      slice_manager->volume(this->order_->shares_per_interval());
      slice_manager->decision_engine(this);
      slice_manager->customer_order(const_cast<CRomCsv&>(this->order_->original_order()));
        // TODO: Fixup this damn const CRomCsv stuff, CRomCsv::Get() method
        // should really be const so we can pass it around to these const
        // functions.
      if (slice_manager->init() && slice_manager->start())
        this->slice_mgrs_.insert(slice_manager);
      else {
        dart::ALGO_MANAGER::instance()->reject_order
          (this->order_->original_order(),slice_manager->lasterror());
        delete slice_manager;
        return 0;
      }

#endif

      this->shares_worked_ += shares_to_work;

      if (this->shares_worked_ < this->order_->shares ())
      {
        // We'd like to setup another timer callback to create the next interval
        this->timer_id = this->reactor()->schedule_timer
          (this, reinterpret_cast <void*> (SPAWN_MGR_OBJ),
           ACE_Time_Value (this->order_->interval_duration()));
        if (this->timer_id == -1)
        {
          ACE_DEBUG ((LM_DEBUG,
            DART_LOG ("%s: Unable to set timer!\n"),
            __FUNCTION__));
        }
      }
    }

    return 0;
  }

  int
  TWAP_Decision_Engine::handle_output (ACE_HANDLE)
  {
    bullseye::Guard guard (this->mutex_);

    ACE_Message_Block *mb = 0;
    if (this->getq (mb) == -1)
    {
      ACE_DEBUG ((LM_DEBUG,
                  DART_LOG ("getq () call failed in %s"),
                  __FUNCTION__));
      return 0; // Not sure what to do here, what to return
    }

    ACE_ASSERT (mb);

    // TODO: Don't create a new CRomCsv message just yet.  It's easy enough to
    // just check the first byte of the message to find the command and dispatch
    // off that.  Cancels will be handled in the decision engine, but status
    // messages will be handled in the interval managers where they will only
    // receive a message block, the interim CRomCsv is just wasted.
    std::string rawmsg (mb->rd_ptr (), mb->length ());
    CRomCsv msg (rawmsg);
    const char *command = msg.Get (RFIX::CSV_COMMAND);
    if (command == 0 || command [0] == '\0' || command [1] != '\0') {
      ACE_DEBUG ((LM_DEBUG,
                  DART_LOG ("invalid command: %s"),
                  msg.Message ().c_str ()));
      return 0;
    }

    switch (command [0]) {
      case ROMVALUE::COMMAND_CANCEL:
        if (!this->cancel ()) {
          // TODO: Reject the cancel
        }
        break;
      case ROMVALUE::COMMAND_STATUS:
        this->on_status_msg (msg);
        break;
      default:
        ACE_DEBUG ((LM_DEBUG,
                    DART_LOG ("unexpected command in %s: %s"),
                    __FUNCTION__,
                    msg.Message ().c_str ()));
        break;
    }

    return 0;
  }

  int TWAP_Decision_Engine::handle_exit (ACE_Process *)
  {
    return 0;
  }

  void TWAP_Decision_Engine::on_execution(const Order_execution &exchange_fill) {
    // Update the total shares filled and send back a fill to the client
    CRomCsv customer_fill;
    this->order_->handle_order_execution (exchange_fill, customer_fill);
    this->algo_manager_->send_csv_to_client (customer_fill);
  }

  int
  TWAP_Decision_Engine::on_status_msg (const CRomCsv &csv)
  {
    bullseye::Guard guard (this->mutex_);

    // 1. Find the TWAP_Time_Slice_Manager to process 
    const long order_status (csv.lGet (RFIX::CSV_STATUS));
    const std::string order_id (csv.GetAt (RFIX::CSV_TAG));

    // Using the Tag field, find the appropriate ACE_Task <> to handle this msg

    switch (order_status) {
      case ROMVALUE::STATUS_OPEN:
        ACE_DEBUG ((LM_DEBUG,
                    DART_LOG ("Order %s accepted\n"),
                    order_id.c_str ()));
        break;

      case ROMVALUE::STATUS_PARTIALLYFILLED:
      case ROMVALUE::STATUS_FILLED:
        {
          const long shares_filled (csv.lGet (RFIX::CSV_LAST_SHARES));
          const long exec_shares (csv.lGet (RFIX::CSV_EXEC_SHARES));
          const long original_order_shares (csv.lGet (RFIX::CSV_SHARES));
          ACE_DEBUG ((LM_DEBUG,
                      DART_LOG ("%d filled (%d/%d)\n"),
                      shares_filled,
                      exec_shares,
                      original_order_shares));

          // Update the total shares filled and send back a fill to the client
          CRomCsv customer_fill;
          this->order_->handle_order_execution (csv, customer_fill);
          this->algo_manager_->send_csv_to_client (customer_fill);
        }
        break;

      default:
        ACE_DEBUG ((LM_DEBUG,
          DART_LOG ("Unhandled status: %d\n"),
          order_status));
        break;
    }
    return 0;
  }

  void TWAP_Decision_Engine::all_out(const char *id) {
    this->state_=DEAD;
    if (this->reactor()->cancel_timer(this)==-1)
      DART_WARNING("%s: Error cancelling timers\n",__FUNCTION__);
    this->confirm_cancelled();
    DART_INFO("%s(%s)\n",__FUNCTION__,id);
  }

  bool TWAP_Decision_Engine::cancel(const std::string &/*cancel_reason*/)
  {
    bullseye::Guard guard (this->mutex_);

#if 0
    if (state_ != LIVE)
      return false;

    this->state_=PENDING_CANCEL; // Stop all further order placements or replaces

    // Cancel outstanding timers so no new time interval managers are created
    if (this->reactor ()->cancel_timer (this) == -1) {
      ACE_DEBUG ((LM_DEBUG, DART_LOG ("Error canceling timers\n"),
                  this->timer_id));
    }

#ifdef USE_AGGRESSIVENESS
    // Degenerate case: no interval managers created:
    if (this->interval_managers_.empty()) {
      this->state_ = DEAD;
      this->confirm_cancelled();
      return true;
    }
#else
    // TODO: Check cancel against new interface
#endif

    // Don't delete the interval managers, they may still have
    // outstanding work to do to cancel the orders in the market.

    bool all_out (true);
#ifdef USE_AGGRESSIVENESS
    for (IM_Cont::iterator i (this->interval_managers_.begin ());
         i != this->interval_managers_.end (); ++i) {
      // cancel () returns true if cancel is immediate, false
      // if there are operations to cancel the order pending
      if ((*i)->cancel () == false)
        all_out = false;
    }
#else
    // TODO: Use new interface to cancel orders
#endif

    // Schedule one last timer to make sure we have an out on every suborder
    if (!all_out) {
      this->timer_id = this->reactor ()->schedule_timer
        (this, reinterpret_cast <void*> (VERIFY_CANCELS), ACE_Time_Value (30));
      if (this->timer_id < 0) {
        ACE_DEBUG ((LM_DEBUG,
          DART_LOG ("%s: Unable to schedule timer\n"),
          __FUNCTION__));
      }
    }

    if (all_out) {
      this->state_ = DEAD;
      this->confirm_cancelled (cancel_reason);
    }
    else
      this->state_ = PENDING_CANCEL;

    return true;
#else
    // Let's use the order manager here to cancel all our suborders.
    this->state_=PENDING_CANCEL;
    ORDER_MANAGER::instance()->cancel_all_suborders(
      this->order_->original_order().GetAt(RFIX::CSV_STAFFORD_TAG).c_str(),
      this);
    return true;
#endif
  }

  void TWAP_Decision_Engine::confirm_cancelled(const std::string &reason)
  {
    bullseye::Guard guard (this->mutex_);

    // Send an "out" to the client for their order
    CRomCsv cancel_response (this->order_->original_order ());
    cancel_response.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
    cancel_response.SetAt (RFIX::CSV_STATUS, ROMVALUE::STATUS_CANCELED);
    cancel_response.SetAt (RFIX::CSV_TEXT, reason);
    this->algo_manager_->send_csv_to_client (cancel_response);
  }

  void TWAP_Decision_Engine::on_child_order_cancelled() {
    bullseye::Guard guard (this->mutex_);

    // One of the child orders had been cancelled, iterate the list of all
    // child orders and, if they're all out of the market, send back a status
    // message to the customer indicating they're out of the market as well.
#ifdef USE_AGGRESSIVENESS
    for (IM_Cont::iterator i (this->interval_managers_.begin ());
         i != this->interval_managers_.end (); ++i) {
      if (!(*i)->is_dead ())
        return;
    }
#else
    // TODO: Use new interface to report this as dead
#endif

    // If we got here, we're all good, every order is out
    // of the market, send a cancel back to the customer.
    this->confirm_cancelled();
  }
}
