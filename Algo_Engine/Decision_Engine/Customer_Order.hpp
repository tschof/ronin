#ifndef __CUSTOMER_ORDER_HPP__
#define __CUSTOMER_ORDER_HPP__

#include <ace/time_value.h>
#include "ROM_Handler/romcsv.h"

struct Order_execution;

namespace dart
{
  class ROM_Connection;

  /**
   * @class Customer_Order
   * @brief A Customer_Order object is a placeholder for a customer's original
   * algorithm order and is a base class interface for all specific algorithm
   * orders such as TWAP and VWAP orders.
   *
   * The system needs a generic interface to represent an algorithm order
   * regardless of the type.  This is that interface.  Each different algorithm
   * is a different type of order such as VWAP or TWAP.  A strategy object is
   * not used primarily because once a customer order comes in as a particular
   * algorithm it cannot be changed to another type of algorithm during its
   * lifetime.
   */
  class Customer_Order
  {
  public:
    Customer_Order (const CRomCsv&);
    virtual bool validate  (std::string&);
    virtual bool configure (std::string&);
    virtual bool check_invariants () const;
    const std::string &symbol () const;
    virtual bool handle_order_execution(const CRomCsv&, CRomCsv&);
    virtual bool handle_order_execution(const Order_execution&,CRomCsv&);

    virtual size_t remaining_shares () const;

    virtual void report_progress () const;

    // Accessors
    ACE_Time_Value start() const;
    ACE_Time_Value stop() const;
    size_t shares () const;

    const CRomCsv& original_order () const;

  protected:
    virtual bool validate_symbol     (std::string&);
    virtual bool validate_volume     (std::string&);
    virtual bool validate_side       (std::string&);
    virtual bool validate_tif        (std::string&);
    virtual bool validate_ordtype    (std::string&);
    virtual bool validate_start_time (std::string&);
    virtual bool validate_stop_time  (std::string&);
    virtual bool validate_interval   (std::string&);
    virtual bool validate_capacity   (std::string&);
    virtual bool validate_algo_id    (std::string&);
    virtual bool validate_parameters (std::string&);

    virtual void mark_dead ();

    ACE_Time_Value created_; // When received
    size_t original_shares_; // Customer order volume
    size_t live_shares_;     // Shares out in the market
    size_t side_;            // Maps to values in romvaluedefs.h
    ACE_Time_Value start_;   // As specified, at market open or when created
    ACE_Time_Value stop_;    // As specified or when market closes
    size_t shares_executed_; // Sum of all executions on this customer order
    short  status_;

    CRomCsv original_order_;
    std::string symbol_;

    // 15 minutes minimum length of time allowable to work an algorithmic order

    // TODO: Make configurable
    static const time_t MIN_ORDER_TIME=60;
   };
}

#endif
