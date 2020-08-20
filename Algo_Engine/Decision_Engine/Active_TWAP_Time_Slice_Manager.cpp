#include "Active_TWAP_Time_Slice_Manager.hpp"
#include "TWAP_Order.hpp"
#include <Market_Data/Price_Block.hpp>
#include "Price_Manipulators.hpp"
#include <cassert>
#include <bullseye/bullseye_common.hpp>
#include <Market_Data/DART_Price_Block.h>

namespace dart
{
  Active_TWAP_Time_Slice_Manager::Active_TWAP_Time_Slice_Manager
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
  Active_TWAP_Time_Slice_Manager::on_timer (TIMER_TYPE timer_type,
                                            const Price_Block *pb)
  {
    switch (timer_type) {
      case INITIAL_TIMER:
        return calc_midpoint (pb->bid_price, pb->ask_price);

      case MIDPOINT_TIMER:
        return calc_midpoint (pb->bid_price, pb->ask_price);

      case THREE_QUARTER_TIMER:
        return double_to_string (this->customer_order_->is_long ()
          ? pb->ask_price : pb->bid_price);

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
