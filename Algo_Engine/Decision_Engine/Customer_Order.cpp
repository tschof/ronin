#include <Bullseye/Bullseye_Common.hpp>
#include "Time_Source.hpp"
#include "Customer_Order.hpp"
#include <windows.h>
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include "ROM_Handler/romcsv.h"
#include "Parameter_Parsing.hpp"
#include <cassert>
#include <stdexcept>
#include <ace/OS.h>
#include "Order_management_system.hpp"

namespace dart
{
  Customer_Order::Customer_Order (const CRomCsv &msg)
    : created_ (dart::time::TIME_SOURCE::instance()->current()),
      original_shares_ (0),
      live_shares_ (0),
      side_   (0),
      start_  (0),
      stop_   (0),
      shares_executed_ (0),
      status_ (ROMVALUE::STATUS_NEW),
      original_order_ (msg),
      symbol_ ( )
  {
    time_t nystart,nystop;
    if (dart::time::get_ny_market_start_time (&nystart) == false ||
        dart::time::get_ny_market_stop_time  (&nystop)  == false) {
      throw std::runtime_error ("internal error getting market open/close time");
    }
    this->start_=nystart;
    this->stop_=nystop;
  }

  ACE_Time_Value Customer_Order::start() const {
    return this->start_;
  }

  ACE_Time_Value Customer_Order::stop() const {
    return this->stop_;
  }

  size_t Customer_Order::shares() const {
    return this->original_shares_;
  }

  bool
  Customer_Order::validate (std::string &errmsg)
  {
    return (this->validate_symbol     (errmsg) &&
            this->validate_volume     (errmsg) &&
            this->validate_side       (errmsg) &&
            this->validate_tif        (errmsg) &&
            this->validate_ordtype    (errmsg) &&
            this->validate_start_time (errmsg) &&
            this->validate_stop_time  (errmsg) &&
            this->validate_interval   (errmsg) &&
            this->validate_capacity   (errmsg) &&
            this->validate_algo_id    (errmsg) &&
            this->validate_parameters (errmsg));
  }

  bool
  Customer_Order::validate_symbol (std::string &errmsg)
  {
    this->symbol_ = this->original_order_.GetAt (RFIX::CSV_SYMBOL);
    if (this->symbol_.empty ()) {
      errmsg = "no symbol";
      return false;
    }

    // TODO: Do TPOS lookups to determine if the symbol is valid.

    return true;
  }

  bool
  Customer_Order::validate_volume (std::string &errmsg)
  {
    this->original_shares_ = this->original_order_.lGet (RFIX::CSV_SHARES);
    if (this->original_shares_ < 100) {
      errmsg = "Order must be for at least 500 shares";
      return false;
    }

    return true;
  }

  bool
  Customer_Order::validate_side (std::string &errmsg)
  {
    this->side_ = this->original_order_.lGet (RFIX::CSV_SIDE);

    bool result (true);

    if (this->side_ != ROMVALUE::SIDE_BUY  &&
        this->side_ != ROMVALUE::SIDE_SELL &&
        this->side_ != ROMVALUE::SIDE_SHORT)
    {
      errmsg = "invalid side";
      result = false;
    }

    return result;
  }

  bool
  Customer_Order::validate_tif (std::string &errmsg)
  {
    const long tif (this->original_order_.lGet (RFIX::CSV_TIF));
    bool result (true);
    if (tif != ROMVALUE::TIF_DAY && tif != ROMVALUE::TIF_GTD) {
      errmsg = "invalid time in force";
      result = false;
    }

    return result;
  }

  bool
  Customer_Order::validate_ordtype (std::string &errmsg)
  {
    bool result (false);
    if (this->original_order_.IsFieldEmpty (RFIX::CSV_TYPE) == false) {
      const long ordtype (this->original_order_.lGet (RFIX::CSV_TYPE));
      if (ordtype == ROMVALUE::TYPE_LIMIT || ordtype == ROMVALUE::TYPE_MARKET) {
        result = true;
      }
      else {
        errmsg = "Invalid order type, only market or limit allowed";
      }
    }
    else {
      errmsg = "Missing order type";
    }

    return result;
  }

  bool
  Customer_Order::validate_start_time (std::string &errmsg)
  {
    if (this->created_ > this->start_)
      this->start_ = this->created_;

    // this->stop_ was defaulst to market close in the constructor
    if (this->created_ > this->stop_) {
      errmsg = "market is closed";
      return false;
    }

    if (this->original_order_.IsFieldEmpty (RFIX::CSV_START_TIME) == false) {
      const std::string start_time_str
        (this->original_order_.GetAt (RFIX::CSV_START_TIME));
      time_t specified_start_time(0);
      if (dart::time::parse_time_str (start_time_str, specified_start_time)) {
        ACE_Time_Value reqstart(specified_start_time);
        // Do some checks on the start time
        if (reqstart < start_) {
          errmsg = "start time either in the past or before market open";
          return false;
        }
        start_ = reqstart;
      }
      else {
        errmsg = "invalid start time format";
        return false;
      }
    }

    return true;
  }

  bool
  Customer_Order::validate_stop_time (std::string &errmsg)
  {
    if (this->original_order_.IsFieldEmpty (RFIX::CSV_END_TIME) == false) {
      const std::string end_time_str
        (this->original_order_.GetAt (RFIX::CSV_END_TIME));
      time_t specified_stop_time (0);
      if (dart::time::parse_time_str (end_time_str, specified_stop_time)) {
        ACE_Time_Value reqend(specified_stop_time);
        if (reqend > stop_) {
          errmsg = "stop time after the close";
          return false;
        }
        stop_ = reqend;
      }
      else {
        errmsg = "invalid stop time format";
        return false;
      }
    }

    return true;
  }

  bool
  Customer_Order::validate_interval (std::string &errmsg)
  {
    if (this->stop_ < this->start_) {
      errmsg = "stop time occurs before start time";
      return false;
    }

    if ((this->stop_ - this->start_) < ACE_Time_Value(MIN_ORDER_TIME)) {
      errmsg = "does not meet minimum time length requirements";
      return false;
    }

    return true;
  }

  bool
  Customer_Order::validate_capacity (std::string &errmsg)
  {
    // Capacity must be Agency
    const char *capacity = this->original_order_.Get (RFIX::CSV_CAPACITY);
    if (capacity == 0        || // Null ptr
        capacity [0] == '\0' || // Empty string
        capacity [0] != 'A'  || // Non-agency
        capacity [1] != '\0')   // Multiple chars
    {
      errmsg = "Capacity must be Agency ('A')";
      return false;
    }

    return true;
  }

  bool
  Customer_Order::validate_algo_id (std::string &errmsg)
  {
    if (this->original_order_.IsFieldEmpty (RFIX::CSV_ALGOTYPE)) {
      errmsg = "AlgoType (73) field missing";
      return false;
    }

    return true;
  }

  const std::string&
  Customer_Order::symbol () const
  {
    return this->symbol_;
  }

  void
  Customer_Order::mark_dead ()
  {
    this->status_ = ROMVALUE::STATUS_CANCELED;
  }

  bool
  Customer_Order::validate_parameters (std::string&)
  {
    return true;
  }

  bool
  Customer_Order::configure (std::string&)
  {
    return true;
  }

  const CRomCsv&
  Customer_Order::original_order () const
  {
    return this->original_order_;
  }

  bool
  Customer_Order::check_invariants () const
  {
    // Invariant 1: NEVER transact more shares than the original order.
    assert (this->shares_executed_ <= this->original_shares_);
    if (this->shares_executed_ > this->original_shares_) {
      ACE_DEBUG ((LM_DEBUG,
                  "Executed more shares than the original order! %d > %d",
                  this->shares_executed_,
                  this->original_shares_));
    }

    // Invariant 2: The number of shares already executed plus the number of
    // shares currently in the market but are not yet executed should NEVER
    // be more than the original order.
    const size_t total_outstanding_shares (this->shares_executed_ +
                                           this->live_shares_);
    assert (total_outstanding_shares <= this->original_shares_);
    if (total_outstanding_shares > this->original_shares_) {
      ACE_DEBUG ((LM_DEBUG,
                  "More shares in the market and/or already executed "
                  "than the original order! %d > %d",
                  total_outstanding_shares,
                  this->original_shares_));
    }

    return true;
  }

  bool Customer_Order::handle_order_execution(
    const Order_execution &exchange_fill,
    CRomCsv &customer_fill)
  {
    customer_fill.SetAt(RFIX::CSV_EXEC_TIME,exchange_fill.exec_time);
    customer_fill.SetAt(RFIX::CSV_COMMAND,ROMVALUE::COMMAND_STATUS);
    customer_fill.SetAt(RFIX::CSV_STAFFORD_TAG,
      this->original_order().GetAt(RFIX::CSV_STAFFORD_TAG));
    const long executed_shares(exchange_fill.qty);
    assert (executed_shares);
    this->shares_executed_ += executed_shares;
    this->live_shares_ -= executed_shares;
    customer_fill.SetAt(RFIX::CSV_LAST_SHARES,executed_shares);
    customer_fill.SetAt(RFIX::CSV_LAST_PRICE,exchange_fill.price);
    customer_fill.SetAt(RFIX::CSV_STATUS,
      (this->shares_executed_ < this->original_shares_)
        ? ROMVALUE::STATUS_PARTIALLYFILLED
        : ROMVALUE::STATUS_FILLED);
    customer_fill.SetAt(RFIX::CSV_ALGOTYPE,
      this->original_order().GetAt (RFIX::CSV_ALGOTYPE));
    return true;
  }

  bool Customer_Order::handle_order_execution(
    const CRomCsv &exchange_fill,
    CRomCsv &customer_fill)
  {
    assert(
      !exchange_fill.IsFieldEmpty (RFIX::CSV_LAST_SHARES)
      && !exchange_fill.IsFieldEmpty (RFIX::CSV_LAST_PRICE));

    customer_fill.SetAt
      (RFIX::CSV_EXEC_TIME,
       exchange_fill.GetAt (RFIX::CSV_EXEC_TIME));

    customer_fill.SetAt(
      RFIX::CSV_COMMAND,
      ROMVALUE::COMMAND_STATUS);

    customer_fill.SetAt(
      RFIX::CSV_STAFFORD_TAG,
      this->original_order().GetAt(RFIX::CSV_STAFFORD_TAG));

    const long executed_shares(exchange_fill.lGet(RFIX::CSV_LAST_SHARES));
    assert (executed_shares);
    this->shares_executed_ += executed_shares;
    this->live_shares_ -= executed_shares;

    customer_fill.SetAt(
      RFIX::CSV_LAST_SHARES,
      executed_shares);

    customer_fill.SetAt(
      RFIX::CSV_LAST_PRICE,
      exchange_fill.GetAt (RFIX::CSV_LAST_PRICE));

    customer_fill.SetAt(
      RFIX::CSV_STATUS,
      (this->shares_executed_ < this->original_shares_)
        ? ROMVALUE::STATUS_PARTIALLYFILLED
        : ROMVALUE::STATUS_FILLED);

    customer_fill.SetAt(
      RFIX::CSV_ALGOTYPE,
      this->original_order().GetAt (RFIX::CSV_ALGOTYPE));

    return true;
  }

  size_t Customer_Order::remaining_shares() const {
    const size_t total_exposure(this->live_shares_ + this->shares_executed_);
    size_t result (0);
    if (total_exposure < this->original_shares_)
      result = this->original_shares_ - total_exposure;
    return result;
  }

  void Customer_Order::report_progress () const
  {

  }
}
