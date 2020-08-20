#ifndef __TWAP_CALCULATOR_FUNCTION_OBJECTS_HPP__
#define __TWAP_CALCULATOR_FUNCTION_OBJECTS_HPP__

#include <ctime>
#include <stdexcept>

namespace dart
{
  struct calculation_error
    : public std::runtime_error
  {
    calculation_error (const std::string &errmsg)
      : std::runtime_error (errmsg)
    {
    }
  };

  void round_lot_calculator (size_t, time_t, size_t*, time_t*);
  void round_lot_calculator_one_minute_minimum (size_t, time_t, size_t*, time_t*);

  void twap_interval_calculator_v3
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     time_t *interval_length);
  void twap_interval_calculator_v4
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     size_t *intervals,
     double *interval_length_sec);
  void twap_interval_calculator_v5
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     size_t *intervals,
     double *interval_length_sec);

}

#endif
