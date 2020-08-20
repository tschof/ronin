// ACE
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/Time_Value.h>
#include <ace/OS.h>
#include <ace/Date_Time.h>

// Bullseye
#include <bullseye/bullseye_common.hpp>
#include "Configurator/configuration.h"
#include "bullseye/DART_Log.hpp"

// Market data
#include <Market_Data/Price_Block.cpp>
#include <Market_Data/Market_Data_Source.hpp>

// Connections
#include <ROM_Handler/ROM_Connection.hpp>
#include <ROM_Handler/Session.hpp>

// Decision engine
#include "TWAP_Time_Slice_Manager.hpp"
#include "Order_Book.hpp"
#include "TWAP_Order.hpp"
#include "TWAP_Decision_Engine.hpp"

// Std C++
#include <stdexcept>

// CRT
#include <cassert>

// ROM
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

namespace
{
  std::string local_time_str (const time_t &t)
  {
    std::string str;
    const tm *ptm = localtime (&t);
    if (ptm) {
      char buffer [64];
      memset (buffer, 0, sizeof (buffer));
      if (strftime (buffer, sizeof (buffer), "%H:%M:%S", ptm) > 0) {
        str = buffer;
      }
    }

    return str;
  }
}

namespace dart
{
  TWAP_Time_Slice_Manager::TWAP_Time_Slice_Manager
    (dart::TWAP_Order *customer_order,
     dart::TWAP_Decision_Engine *parent,
     dart::Market_Data_Source *data_source,
     Session *market_destination,
     size_t interval_shares)
  : notifier_ (0, this, ACE_Event_Handler::WRITE_MASK)
  , start_(ACE_OS::gettimeofday())
  , stop_(ACE_OS::gettimeofday())
  , data_source_ (data_source)
  , market_dest_ (market_destination)
  , timer_id_ (-1)
  , order_ (0)
  , customer_order_ (customer_order)
  , shares_filled_ (0)
  , parent_ (parent)
  , status_ (-1)
  , data_poll_interval_ (1000) // Default is 1000ms==1s
  , max_data_retry_count_ (5) // Retry 5 times with no data before rejecting
  , current_data_retry_count_ (0)
  , interval_shares_(interval_shares)
  {
    if (customer_order_ == 0 || parent_ == 0)
      throw std::invalid_argument ("null ptr");

    if (customer_order_->shares_per_interval () < 100)
      throw std::invalid_argument ("shares must be at least 100");

    this->stop_ = this->start_ + customer_order_->interval_duration();

    // Get overrides for the market data timeout settings
    this->data_poll_interval_ = CONFIGURATION::instance()->get_int
      ("Bullseye","MdNoDataPollIntervalMs",this->data_poll_interval_);

    this->max_data_retry_count_ = CONFIGURATION::instance()->get_int
      ("Bullseye","MdNoDataMaxPollCount",this->max_data_retry_count_);

    ACE_Date_Time start_time(this->start_);
    ACE_Date_Time stop_time(this->stop_);
    DART_DEBUG ("%s: Start=%d:%d:%d:%d, Stop=%d:%d:%d:%d\n",
      __FUNCTION__,
      start_time.hour(),
      start_time.minute(),
      start_time.second(),
      start_time.microsec(),
      stop_time.hour(),
      stop_time.minute(),
      stop_time.second(),
      stop_time.microsec());
  }

  int TWAP_Time_Slice_Manager::open(void*)
  {
    // Steal the event handlers reactor, we don't want to run our own thread,
    // we just want to use the thread of the decision engine that evt_handler
    // points to.
    this->reactor(ACE_Reactor::instance());
    assert(this->reactor ());
    if (this->reactor () == 0)
      return -1;

//    const time_t halfway_point (customer_order_->seconds_per_interval () / 2);
    // We will set the first timer to be the halfway point within the interval.
    this->timer_id_ = this->reactor()->schedule_timer
      (this,
       reinterpret_cast <void*> (INITIAL_TIMER),
       ACE_Time_Value (0));

    // We will utilize the message queue notifier to tie our message queue to
    // the reactor, notifying us via callback when any messages are pushed onto
    // this queue.
    this->notifier_.reactor(this->reactor ());
    this->msg_queue()->notification_strategy(&this->notifier_);

    return 0;
  }

  int TWAP_Time_Slice_Manager::put (ACE_Message_Block *pmb, ACE_Time_Value *)
  {
    return this->putq (pmb);
  }

  void TWAP_Time_Slice_Manager::cancel_outstanding_order ()
  {
    CRomCsv cancel_msg;
    cancel_msg.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
    cancel_msg.SetAt (RFIX::CSV_TAG, this->order_->GetAt (RFIX::CSV_TAG));
    this->market_dest_->send (cancel_msg);
  }

  void TWAP_Time_Slice_Manager::process_fill (ACE_Message_Block *pmessage, CRomCsv &fill)
  {
    const long shares_filled (fill.lGet (RFIX::CSV_EXEC_SHARES));
    assert (shares_filled);
    if (shares_filled) {
      this->shares_filled_ += shares_filled;
      if (this->parent_->put (pmessage) == -1) {
        ACE_DEBUG ((LM_DEBUG,
        DART_LOG ("%s: parent queue put failed\n"),
        __FUNCTION__));
      }
    }
    else {
      ACE_DEBUG ((LM_DEBUG, DART_LOG ("shares filled is 0!\n")));
    }
  }

  int TWAP_Time_Slice_Manager::handle_output (ACE_HANDLE)
  {
    ACE_Message_Block *pmessage = 0;
    if (this->getq (pmessage) == 0 && pmessage && pmessage->length ()) {
      const char *p = pmessage->rd_ptr ();
      const char command (p [0]);
      if (command == ROMVALUE::COMMAND_STATUS) {
        std::string msg (pmessage->rd_ptr (), pmessage->length ());
        CRomCsv status (msg);
        this->status_ = status.lGet (RFIX::CSV_STATUS);
        switch (this->status_) {
          case ROMVALUE::STATUS_OPEN:
            ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Order %s OPEN\n"),
              status.GetAt (RFIX::CSV_TAG).c_str ()));
            break;

          case ROMVALUE::STATUS_FILLED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Order filled\n")));
            this->process_fill (pmessage, status);
            this->cancel_timers ();
            break;

          case ROMVALUE::STATUS_PARTIALLYFILLED:
            this->process_fill (pmessage, status);
            break;

          case ROMVALUE::STATUS_CANCELED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Canceled\n")));
            this->cancel_timers ();
            this->parent_->on_child_order_cancelled ();
            break;

          case ROMVALUE::STATUS_ORDERREJECTED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Order rejected!\n")));
            // With the original order rejected and no reliable way to
            // determine why, we will just stop our timers and make sure no
            // further orders are submitted by this object.
            this->cancel_timers();
            this->parent_->cancel("orders rejected at exchange");
            this->parent_->on_child_order_cancelled();
            break;

          case ROMVALUE::STATUS_REPLACEREJECTED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Replace rejected!\n")));
            this->cancel_timers();
            this->parent_->cancel();
            break;

          case ROMVALUE::STATUS_REPLACED:
            break;

          case ROMVALUE::STATUS_CANCELREJECTED:
            // This is probably the worst of all cases as we have no way
            // to exit the market, except possibly by getting filled.
            // TODO: Flag an operator at this point, this is bad.
            // TODO: This is an appropriate reason to send email.
            DART_CRITICAL
              ("Cancel for order %s rejected!\n",
               this->order_->GetAt(RFIX::CSV_TAG));
            this->cancel_timers ();

          default:
            ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Unknown status: %d\n"),
              this->status_));
            break;
        }
      }
    }
    return 0;
  }

  int TWAP_Time_Slice_Manager::handle_timeout (const ACE_Time_Value &current_time, const void *arg)
  {
    TIMER_TYPE timer_type
      (static_cast <TIMER_TYPE>
        (reinterpret_cast <int>
          (const_cast <void*> (arg))));

    ACE_DEBUG ((LM_DEBUG,
      DART_LOG ("%s: arg=%d\n"),
      __FUNCTION__,
      (int)timer_type));

    Price_Block *price_block =
      this->data_source_->get_price_block
        (this->customer_order_->symbol ());

    if (price_block == 0)
    {
      DART_WARNING ("Market data for %s is unavailable\n",
                    this->customer_order_->symbol().c_str());

      ++this->current_data_retry_count_;

      if (this->current_data_retry_count_ > this->max_data_retry_count_) {
        assert(this->order_);
        if (this->order_) { // TODO: Why would this be null?
          const std::string ordertag(this->order_->GetAt(RFIX::CSV_TAG));
          DART_WARNING
            ("Unable to get market data for %s after %d retries "
            "of %d milliseconds, canceling order %s\n",
            this->customer_order_->symbol(),
            this->current_data_retry_count_,
            this->data_poll_interval_,
            ordertag.empty() ? "<ORDER_NOT_SENT>" : ordertag.c_str());

          // Did not get market data within the constraints
          // set, reject the customer order.
          this->cancel_outstanding_order();
          this->cancel_timers();
        }
        return -1;
      }

      this->timer_id_ = this->reactor ()->schedule_timer
        (this, 0, ACE_Time_Value
          (0,
           this->data_poll_interval_*1000 /*var is in ms, arg is us*/));

      if (this->timer_id_ == -1) {
        ACE_DEBUG ((LM_DEBUG, DART_LOG ("Error setting timer\n")));
      }
      return 0;
    }

    std::ostringstream pblockout;
    dart::dump (pblockout, *price_block);
    const std::string str (pblockout.str ());
    ACE_DEBUG ((LM_DEBUG,
      DART_LOG ("TWAP Decision: %s %s\n"),
      this->customer_order_->symbol ().c_str (),
      str.c_str ()));

    std::string price_str;

    switch (timer_type) {
      case INITIAL_TIMER:
        price_str = this->on_timer (INITIAL_TIMER, price_block);
        break;

      case MIDPOINT_TIMER:
        price_str = this->on_timer (MIDPOINT_TIMER, price_block);
        break;

      case THREE_QUARTER_TIMER:
        price_str = this->on_timer (THREE_QUARTER_TIMER, price_block);
        break;

      case FINAL_TIMER:
        break;

      default:
        ACE_DEBUG ((LM_DEBUG, DART_LOG ("Unknown timer type %d\n"), arg));
        break;
    }

    if (this->order_) {
      this->change_order_price (price_str.c_str ());
    }
    else {
      this->create_and_send_order
        (this->interval_shares_, price_str.c_str ());
    }

    if ((this->shares_filled_ < this->customer_order_->shares_per_interval())
        && timer_type != FINAL_TIMER) {
      // Set a timer at the 3/4 point in the time interval
      // to check if the order has been completely filled.
      ACE_Time_Value next_timer(this->stop_-current_time);
      next_timer *= 0.5;
      assert (next_timer);
      this->timer_id_ = this->reactor()->schedule_timer
        (this,
        reinterpret_cast <void*> (++timer_type),
        next_timer);
    }

    return 0;
  }

  void TWAP_Time_Slice_Manager::cancel_timers ()
  {
    ACE_DEBUG ((LM_DEBUG,
      DART_LOG ("%s: Cancelling all timers\n"),
      __FUNCTION__));

    if (this->reactor ()->cancel_timer (this) == -1) {
      ACE_DEBUG ((LM_DEBUG, DART_LOG ("Error canceling timers\n")));
    }
  }

  bool TWAP_Time_Slice_Manager::is_dead () const
  {
    switch (this->status_) {
      case ROMVALUE::STATUS_CANCELED:
      case ROMVALUE::STATUS_FILLED:
      case ROMVALUE::STATUS_ORDERREJECTED:
      case -1:
        return true;
      default:
        return false;
    }
  }

  void TWAP_Time_Slice_Manager::create_and_send_order (size_t vol, const char* price)
  {
    // Haven't submitted an order yet
    this->order_ = new CRomCsv();

    try {
      // We'll initially start out on our side of the market,
      // i.e. Buys at the Bid and Sells at the Ask.
      this->customer_order_->new_order(*order_, vol, price);

    }
    catch (const std::exception &e) {
      DART_CRITICAL ("%s: %s", __FUNCTION__, e.what());
      delete this->order_;
      this->order_=0;
      return;
    }

    if (ORDER_BOOK::instance ()->register_callback
        (this->order_->GetAt (RFIX::CSV_TAG), this) == -1)
    {
      ACE_DEBUG ((LM_DEBUG, DART_LOG ("Callback registration failed\n")));
      delete this->order_;
      this->order_ = 0;
      return;
    }

    this->status_ = ROMVALUE::STATUS_NEW;
    this->market_dest_->send (*this->order_);
  }

  void TWAP_Time_Slice_Manager::change_order_price (const char *new_price)
  {
    // Not filled yet, price might be too aggressive; go for the midpoint
    CRomCsv *change_order = new CRomCsv (*this->order_);
    change_order->SetAt (RFIX::CSV_PRICE, new_price);
    if (strlen(new_price)==0)
      change_order->SetAt (RFIX::CSV_TYPE, ROMVALUE::TYPE_MARKET);
    change_order->SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_REPLACE);

    this->market_dest_->send (*change_order);
    this->sent_.push_back (change_order);
  }

  bool TWAP_Time_Slice_Manager::cancel ()
  {
    bool cancelled (false);
    switch (this->status_) {
      case ROMVALUE::STATUS_NEW:
      case ROMVALUE::STATUS_OPEN:
      case ROMVALUE::STATUS_PARTIALLYFILLED:
      case ROMVALUE::STATUS_REPLACED:
      case ROMVALUE::STATUS_REPLACEPENDING:
      case ROMVALUE::STATUS_REPLACEREJECTED:
        this->cancel_outstanding_order ();
        this->cancel_timers ();
        break;
      case -1:
        ACE_DEBUG ((LM_DEBUG,
          DART_LOG ("%s: nothing to cancel on the outbound side\n"),
          __FUNCTION__));
      case ROMVALUE::STATUS_FILLED:
      case ROMVALUE::STATUS_ORDERREJECTED:
        this->cancel_timers ();
        cancelled = true;
        break;
      default:
        ACE_DEBUG ((LM_DEBUG,
          DART_LOG ("Invalid state (%d) for canceling order"),
          this->status_));
        break;
    }

    return cancelled;
  }

  bool TWAP_Time_Slice_Manager::out ()
  {
    if (this->status_ == ROMVALUE::STATUS_CANCELED ||
        this->status_ == ROMVALUE::STATUS_ORDERREJECTED)
      return true;

    return false;
  }
}
