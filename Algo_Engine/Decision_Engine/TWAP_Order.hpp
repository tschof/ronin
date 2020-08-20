#ifndef __TWAP_ORDER_HPP__
#define __TWAP_ORDER_HPP__

#include "New_Order.hpp"
#include "Order_Factory.hpp"
#include "Customer_Order.hpp"

#include <string>
#include <vector>

class ACE_Event_Handler;

namespace dart
{
  class ROM_Connection;
  class Algo_Manager;

  class TWAP_Order
    : public Customer_Order
  {
  public:
#if USE_AGGRESSIVENESS
    enum Aggressiveness {
      Aggressive = 1,
      Active,
      Passive,
      Directed=100,
    };
#endif

    typedef void (*TWAP_Calculator_Function) (size_t,time_t,size_t*,time_t*);

    TWAP_Order(
      const CRomCsv&,
      ACE_Event_Handler*,
      Algo_Manager*,
      TWAP_Calculator_Function);

    virtual bool configure (std::string&);

    ACE_Time_Value interval_duration() const;
    size_t intervals () const;
    size_t shares_per_interval () const;
    size_t excess_shares () const;
    bool is_long () const;
#if USE_AGGRESSIVENESS
    Aggressiveness aggressiveness () const;
#endif

    std::string dump_timing () const;
    virtual void new_order (CRomCsv&, size_t, const char*);
    void calculate (size_t, time_t);

  protected:
    virtual bool validate_algo_id (std::string&);
    virtual bool validate_parameters (std::string&);
    virtual size_t determine_order_volume ();

  private:
    ACE_Event_Handler *evt_handler_;
    long timer_id_;

    ACE_Time_Value interval_duration_;
    size_t intervals_;
    size_t shares_per_interval_;
    size_t excess_shares_;

    // Eventually there will probably be some sort of order manager
    // to manage outstanding orders, etc.  An object of that class
    // would most likely be contained within an object of this class.
    std::vector <Order> open_orders_;
    Algo_Manager *algo_manager_;

    TWAP_Calculator_Function calc_function_;

#if USE_AGGRESSIVENESS
    Aggressiveness aggressiveness_;
#endif
  };
}

#endif
