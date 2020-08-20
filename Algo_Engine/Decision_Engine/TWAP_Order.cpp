// ACE
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include <ace/log_msg.h>
#include <ace/Message_Block.h>

// CRT
#include <cassert>

// ROM
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include "ROM_Handler/RomCsv.h"

// Bullseye libs
#include <Market_Data/Price_Block.hpp>
#include <Bullseye/Bullseye_Common.hpp>
#include <ROM_Handler/ROM_Connection.hpp>

// Algo_Manager
#include "CSV_Enumerated_Value_Tools.hpp"
#include "TWAP_Order.hpp"
#include "Order_Book.hpp"
#include "Algo_Manager.hpp"
#include "Parameter_Parsing.hpp"
#include "TWAP_Interval_Calculators.hpp"

namespace
{
  const time_t ONE_HOUR (60 /*min*/ * 60 /*sec/min*/);
  const time_t TOTAL_TIME (ONE_HOUR); // Figure we'll start by working everything for one hour
  const size_t ROUND_LOT_SIZE (100);
}

namespace dart
{

TWAP_Order::TWAP_Order (const CRomCsv &csvmsg,
                        ACE_Event_Handler *evt_handler,
                        Algo_Manager *algo_manager,
                        TWAP_Calculator_Function calc_function)
: Customer_Order (csvmsg)
, evt_handler_ (evt_handler)
, timer_id_ (-1)
, algo_manager_ (algo_manager)
, calc_function_ (calc_function)
#if USE_AGGRESSIVENESS
, aggressiveness_ (TWAP_Order::Active)
#endif
{
}

bool
TWAP_Order::is_long () const
{
  const long side (this->original_order ().lGet (RFIX::CSV_SIDE));
  switch (side) {
    case ROMVALUE::SIDE_BUY:
      return true;
    case ROMVALUE::SIDE_SELL:
    case ROMVALUE::SIDE_SHORT:
      return false;
    default:
      throw std::runtime_error ("invalid side");
  }
}

#if USE_AGGRESSIVENESS
TWAP_Order::Aggressiveness
TWAP_Order::aggressiveness () const
{
  return this->aggressiveness_;
}
#endif


bool TWAP_Order::validate_parameters(std::string&) {
  const std::string &parameter_str
    (this->original_order_.GetAt (RFIX::CSV_INSTRUCTIONS));
  dart::bullseye::Parameters parameters;
  if (dart::bullseye::extract_pairs (parameter_str, parameters)) {
    bullseye::Parameters::const_iterator agg_iter (parameters.find ("Aggressiveness"));
    if (agg_iter != parameters.end ()) {
#if USE_AGGRESSIVENESS
      size_t aggressiveness (0);
      std::istringstream converter (agg_iter->second);
      if ((converter >> aggressiveness) &&
          (aggressiveness > 0) &&
          (aggressiveness < 4)) {
        this->aggressiveness_ =
          static_cast <TWAP_Order::Aggressiveness> (aggressiveness);
      }
      else {
        errmsg = "invalid aggressiveness specified";
        return false;
      }
#endif
    }
  }

  return true;
}

bool
TWAP_Order::validate_algo_id (std::string &errmsg)
{
  bool result (false);

  if (Customer_Order::validate_algo_id (errmsg) &&
      this->original_order_.lGet (RFIX::CSV_ALGOTYPE) == 1)
    result = true;

  return result;
}


bool
TWAP_Order::configure (std::string &)
{
  ACE_Time_Value diff (this->stop_ - this->start_);

  // Basic sanity check, difference cannot be larger than an entire trading day
  static const time_t MAX_INTERVAL
    (((7/*hr*/*60/*min/hr*/)+30/*min*/)*60/*secs/min*/);

  assert (diff <= ACE_Time_Value (MAX_INTERVAL));
  if (diff > ACE_Time_Value (MAX_INTERVAL)) {
    return 0;
  }

  return true;
}

ACE_Time_Value TWAP_Order::interval_duration() const {
  return this->interval_duration_;
}

size_t
TWAP_Order::intervals () const
{
  return this->intervals_;
}

size_t
TWAP_Order::shares_per_interval () const
{
  return this->shares_per_interval_;
}

size_t
TWAP_Order::excess_shares () const
{
  return this->excess_shares_;
}

void
TWAP_Order::calculate (size_t shares, time_t secs)
{
  double interval_in_seconds=0.0;
  dart::twap_interval_calculator_v5
    (shares,
     secs,
     &(this->shares_per_interval_),
     &(this->intervals_),
     &interval_in_seconds);
#if 0
  if (this->calc_function_) {
    this->calc_function_
      (shares,
       secs,
       &(this->shares_per_interval_),
       &(this->interval_duration));
    this->intervals_ = static_cast <size_t> (secs / this->interval_duration_);
#endif
    double quot,rem;
    rem=modf(interval_in_seconds,&quot);
    this->interval_duration_.sec(static_cast <time_t> (quot));
    this->interval_duration_.usec(static_cast <int> (rem) * 1000000);
    this->excess_shares_ = shares -
      (this->intervals_ * this->shares_per_interval_);
#if 0
  }
  else {
    throw std::runtime_error ("internal error: calculation function not configured");
  }
#endif
}

void
TWAP_Order::new_order (CRomCsv &ord, size_t vol, const char *price)
{
  Order *order=dart::ORDER_FACTORY::instance()->create_new_order_with_default_settings();
  assert (order);
  if (order==0)
    throw std::runtime_error("cannot generate common fields");
  ord = order->order ();

  Order_Book *pbook = ORDER_BOOK::instance ();
  std::string order_id (pbook->generate_order_id ());

  ord.SetAt (RFIX::CSV_TAG, order_id);
  ord.SetAt (RFIX::CSV_SIDE, (side_ == ROMVALUE::SIDE_BUY)
    ? ROMVALUE::SIDE_BUY : ROMVALUE::SIDE_SELL);

  ord.SetAt (RFIX::CSV_SYMBOL, this->symbol ());

  ord.SetAt (RFIX::CSV_SHARES, static_cast <long> (vol));

  this->live_shares_ += vol;

  // For now we're just meeting the other side of the market, we'll get sophisticated later
  ord.SetAt (RFIX::CSV_PRICE, price);

  ord.SetAt (RFIX::CSV_TYPE, ROMVALUE::TYPE_LIMIT);
  ord.SetAt (RFIX::CSV_TIF, ROMVALUE::TIF_DAY);

  // No routing at this time.  Any user sending orders to the DART_ALGO
  // destination must also specify the actual exchange destination for the algo
  // to direct orders to.  This is specified in the CSV_EXDEST field and will
  // not be changed by the algo engine for the time being.
  ord.SetAt (
    RFIX::CSV_EXCHANGE,
    this->original_order_.GetAt (RFIX::CSV_EXDEST));

  ord.SetAt (RFIX::CSV_SECURITY_TYPE, 1);

  // TODO: Is this needed for equities?  Options only?
  ord.SetAt (RFIX::CSV_OPENCLOSE, 1);

  ord.SetAt(RFIX::CSV_PARENT_TAG,
    this->original_order_.Get(RFIX::CSV_STAFFORD_TAG));
}

size_t
TWAP_Order::determine_order_volume ()
{
#if 0
  if ((this->shares_executed_ +
       this->shares_in_the_market_ +
       this->shares_per_tslice_) > this->original_order_shares_)
  {
    const size_t shares_outstanding (this->shares_executed_ +
                                     this->shares_in_the_market_);
  }
#endif
  return 0;
}

std::string
TWAP_Order::dump_timing () const
{
  std::ostringstream parm_dump;
  parm_dump
    << this->shares_per_interval () << " every "
    << this->interval_duration().sec() << " sec"
    << this->interval_duration().usec() << " usec";
  return parm_dump.str ();
}

}
