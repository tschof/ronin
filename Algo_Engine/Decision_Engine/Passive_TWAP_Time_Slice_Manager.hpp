#ifndef __PASSIVE_TWAP_TIME_SLICE_MANAGER_HPP__
#define __PASSIVE_TWAP_TIME_SLICE_MANAGER_HPP__

#include "TWAP_Time_Slice_Manager.hpp"

namespace dart
{
  class Passive_TWAP_Time_Slice_Manager
    : public TWAP_Time_Slice_Manager
  {
  public:
    Passive_TWAP_Time_Slice_Manager (dart::TWAP_Order*,
                                     dart::TWAP_Decision_Engine*,
                                     dart::Market_Data_Source*,
                                     Session*,
                                     size_t interval_shares);

  protected:
    virtual std::string on_timer (TIMER_TYPE, const Price_Block*);
  };
}

#endif
