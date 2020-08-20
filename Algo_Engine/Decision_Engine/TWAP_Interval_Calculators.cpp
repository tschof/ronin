#include "TWAP_Interval_Calculators.hpp"
#include "bullseye/bullseye_common.hpp"

namespace dart
{
  void round_lot_calculator (size_t shares,
                             time_t secs,
                             size_t *shares_per_interval,
                             time_t *interval_length)
  {
    if ((shares % 100) != 0) {
      throw calculation_error ("only round lots accepted");
    }

    if (secs < 900) {
      throw calculation_error ("will only work orders for 15 min or more");
    }

    *shares_per_interval = 100;
    *interval_length = secs / (shares / 100);

    if (((secs / *interval_length) * (*shares_per_interval)) != shares) {
      throw calculation_error ("could not calculate intervals");
    }
  }

  void round_lot_calculator_one_minute_minimum
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     time_t *interval_length)
  {
    if ((shares % 100) != 0)
      throw calculation_error ("only round lots accepted");

    if (secs < 60)
      throw calculation_error ("minimum time interval is 1 min");

    *shares_per_interval = 100;
    *interval_length = secs / (shares / 100);

    if (((secs / *interval_length) * (*shares_per_interval)) != shares) {
      const size_t total_intervals (static_cast <size_t> (secs / (*interval_length)));
      const size_t shares_across_intervals (total_intervals * (*shares_per_interval));
      if (shares_across_intervals > shares)
        throw calculation_error ("error calculating interval");
      else if (shares_across_intervals == shares)
        DART_DEBUG
          ("Interval length: %ds; even distribution of shares across intervals\n",
           *interval_length);
      else
        DART_DEBUG
          ("Interval length: %ds; uneven distribution of shares, last interval is %d\n",
           (shares-shares_across_intervals));
    }


#if 0
    if (((secs / *interval_length) * (*shares_per_interval)) != shares) {
      throw calculation_error ("could not calculate intervals");
    }
#endif
  }

  void twap_interval_calculator_v3
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     time_t *interval_length)
  {
    if ((shares % 100) != 0)
      throw calculation_error ("only round lots accepted");

    if (secs < 60)
      throw calculation_error ("minimum time interval is 1 min");

    *shares_per_interval = 100;
    *interval_length = secs;

    for (;;)
    {
      if ((*shares_per_interval)==0 || (*shares_per_interval>shares))
        throw calculation_error("cannot calculate time interval");
      // Number of intervals?
      ldiv_t result=ldiv(shares,*shares_per_interval);
      --result.quot; // Number of intervals is one less
      if (result.rem>0)
        --result.quot; // We've got even one less interval with the last one
                       // being for the remainder of the shares.
      const size_t numintervals (result.quot);
      // result.quot is now the # of intervals

      // Number of seconds per interval?
      result=ldiv(static_cast<size_t>(secs),numintervals);
      *interval_length=result.quot;
      if (result.rem && result.rem>10) {
        *shares_per_interval+=100;
      }
      else {
        const size_t totshares (numintervals*(*shares_per_interval));
        if (totshares>shares)
          *shares_per_interval+=100;
        else
          break;
      }
    }
  }

  void twap_interval_calculator_v4
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     size_t *intervals,
     double *interval_length_sec)
  {
    if ((shares % 100) != 0)
      throw calculation_error ("only round lots accepted");

    if (secs < 60)
      throw calculation_error ("minimum time interval is 1 min");

    *shares_per_interval = 100;
    *interval_length_sec = 0.0;

    for (;;)
    {
      if ((*shares_per_interval)==0)
        throw calculation_error("cannot calculate time interval");

      // Number of intervals?
      *intervals=shares/(*shares_per_interval); // Truncation is fine

      *interval_length_sec=static_cast<double>(secs)/static_cast<double>(*intervals);
      if ((*interval_length_sec)>=10.0)
        break;
      else
        *shares_per_interval+=100;
    }
  }

  void twap_interval_calculator_uneven_distribution
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     size_t *intervals,
     double *interval_length_sec)
  {
    if ((shares % 100) != 0)
      throw calculation_error ("only round lots accepted");

    if (secs < 60)
      throw calculation_error ("minimum time interval is 1 min");

    *shares_per_interval=0;

    for (size_t i=0;i<1000;++i) {
      *shares_per_interval+=100;
      *intervals=shares/(*shares_per_interval);
      *interval_length_sec=static_cast<double>(secs)/static_cast<double>(*intervals);

      if ((*interval_length_sec)>1.0) {
        const size_t total_shares_according_to_plan=
          (*shares_per_interval)*(*intervals);
        if (total_shares_according_to_plan>shares) {
          throw std::runtime_error
            ("TWAP calc error, trading plan exceeds order qty");
        }
        if ((*intervals)==1 && (*shares_per_interval)>100) {
          throw std::runtime_error
            ("TWAP calc error, only one interval calculated");
        }
        if (total_shares_according_to_plan==shares
            || (shares-total_shares_according_to_plan)<201) {
          return;
        }
      }
    }

    throw std::runtime_error("TWAP calc error, trading plan not calculated");
  }


  void twap_interval_calculator_v5
    (size_t shares,
     time_t secs,
     size_t *shares_per_interval,
     size_t *intervals,
     double *interval_length_sec)
  {
    if ((shares % 100) != 0)
      throw calculation_error ("only round lots accepted");

    if (secs < 60)
      throw calculation_error ("minimum time interval is 1 min");

    *shares_per_interval=0;

    for (size_t i=0;i<1000;++i) {
      *shares_per_interval+=100;
      *intervals=shares/(*shares_per_interval);
      *interval_length_sec=static_cast<double>(secs)/static_cast<double>(*intervals);

      if ((*interval_length_sec)>1.0) {
        const size_t total_shares_according_to_plan=
          (*shares_per_interval)*(*intervals);
        if (total_shares_according_to_plan>shares) {
          throw std::runtime_error
            ("TWAP calc error, trading plan exceeds order qty");
        }
        if ((*intervals)==1 && (*shares_per_interval)>100) {
          return twap_interval_calculator_uneven_distribution
            (shares,
             secs,
             shares_per_interval,
             intervals,
             interval_length_sec);
#if 0
          throw std::runtime_error
            ("TWAP calc error, only one interval calculated");
#endif
        }
        if (total_shares_according_to_plan==shares) {
          return;
        }
      }
    }

    throw std::runtime_error("TWAP calc error, trading plan not calculated");
  }
}
