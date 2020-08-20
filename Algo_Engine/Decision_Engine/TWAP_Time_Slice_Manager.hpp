#ifndef __TWAP_INTERVAL_MANAGER_HPP__
#define __TWAP_INTERVAL_MANAGER_HPP__

#include <ace/Task.h>
#include <ace/Reactor_Notification_Strategy.h>

class ACE_Time_Value;
class CRomCsv;
class Session;

// CRT
#include <ctime>
#include <vector>
#include <cassert>

// Market data
#include <Market_Data/DART_Price_Block.h>

namespace dart
{
  class TWAP_Order;
  class TWAP_Decision_Engine;
  class Market_Data_Source;
  class ROM_Connection;

  enum TIMER_TYPE
  {
    INITIAL_TIMER = 0,
    MIDPOINT_TIMER,
    THREE_QUARTER_TIMER,
    FINAL_TIMER
  };

  TIMER_TYPE operator++ (const TIMER_TYPE&);


  /**
   * @class TWAP_Time_Slice_Manager
   * @brief Manages working a given order volume within a given time frame
   * according to certain criteria.
   *
   * Once the intervals and shares per interval are calculated according to the
   * TWAP algorithm, each interval must be explicitly managed.   This class and
   * those derived from it manage the possibly complex interactions needed to
   * make sure that volume is traded within the interval.
   */
  class TWAP_Time_Slice_Manager
    : public ACE_Task <ACE_MT_SYNCH>
  {
  public:
    TWAP_Time_Slice_Manager
      (dart::TWAP_Order*,
       TWAP_Decision_Engine*,
       dart::Market_Data_Source*,
       Session*,
       size_t interval_shares);

    TWAP_Time_Slice_Manager (TWAP_Order*, ACE_Event_Handler*);

    virtual int open(void* = 0);
    virtual int put (ACE_Message_Block *, ACE_Time_Value * = 0);
    virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
    virtual int handle_timeout (const ACE_Time_Value&, const void* = 0);
    virtual bool cancel ();
    virtual bool out ();

    bool is_dead () const;

  protected:
    // A string representation of the number is returned since that is what is
    // put into the CSV message.  Converting back and forth from/to an STL
    // string and a double may introduce errors, the string should be quite
    // unambiguous.
    virtual std::string on_timer (TIMER_TYPE, const Price_Block*)
    {
      assert(false);
      return "";
    }

    virtual void cancel_timers ();
    virtual void cancel_outstanding_order ();
    virtual void process_fill (ACE_Message_Block*,CRomCsv&);
    virtual void create_and_send_order (size_t, const char*);
    virtual void change_order_price (const char*);

    ACE_Reactor_Notification_Strategy notifier_;
    ACE_Time_Value start_;
    ACE_Time_Value stop_;
    Market_Data_Source *data_source_;
    Session *market_dest_;
    long timer_id_;
    CRomCsv *order_;
    TWAP_Order *customer_order_;
    size_t shares_filled_;
    std::vector <CRomCsv*> sent_;
    TWAP_Decision_Engine *parent_;
    long status_;

    // TODO: Pushup to base time slice manager class
    size_t data_poll_interval_;
    size_t max_data_retry_count_;
    size_t current_data_retry_count_;
    size_t interval_shares_;
  };
}

#endif
