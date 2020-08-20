#ifndef __PARAMETER_PARSING_IMP_HPP__
#define __PARAMETER_PARSING_IMP_HPP__

namespace dart
{
  namespace time
  {
    bool isdst (const time_t &t, bool &dst_flag);
    bool get_ny_time (time_t *nytime, int hour, int min);
    bool get_ny_utc_offset (time_t t, int *poffset);
    bool get_ny_market_start_time_imp (time_t *start_time);
    bool get_ny_market_stop_time_imp (time_t *stop_time);
  }
}
#endif
