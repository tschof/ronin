#include "Passive_TWAP_Time_Slice_Manager.hpp"
#include "Price_Manipulators.hpp"
#include "TWAP_Order.hpp"
#include <market_data/price_block.hpp>
#include <cassert>
#include <bullseye/bullseye_common.hpp>

namespace dart
{
  Passive_TWAP_Time_Slice_Manager::Passive_TWAP_Time_Slice_Manager
    (dart::TWAP_Order *twap_order,
     TWAP_Decision_Engine *twap_decision_engine,
     dart::Market_Data_Source *market_data_source,
     Session *rom_connection,
     size_t interval_shares)
  : TWAP_Time_Slice_Manager
    (twap_order,
     twap_decision_engine,
     market_data_source,
     rom_connection,
     interval_shares)
  {
  }

  std::string
  Passive_TWAP_Time_Slice_Manager::on_timer (TIMER_TYPE timer_type,
                                             const Price_Block *pb)
  {
    switch (timer_type) {
      case INITIAL_TIMER:
        {
          const double price (this->customer_order_->is_long ()
            ? pb->bid_price : pb->ask_price);
          return double_to_string (price);
        }
        break;

      case MIDPOINT_TIMER:
        return dart::calc_midpoint (pb->ask_price, pb->bid_price);
        break;

      case THREE_QUARTER_TIMER:
        {
          const double price (this->customer_order_->is_long ()
            ? (pb->ask_price) : (pb->bid_price));
          return double_to_string (price);
        }
        break;

      case FINAL_TIMER:
        assert (false);
        ACE_DEBUG ((LM_DEBUG, DART_LOG ("Unexpected final timer type\n")));
        break;

      default:
        assert (false);
        ACE_DEBUG ((LM_DEBUG, DART_LOG ("Unexpected timer type %d\n"),
          timer_type));
        break;
    }

    throw std::runtime_error ("could not process timer");
  }
}
