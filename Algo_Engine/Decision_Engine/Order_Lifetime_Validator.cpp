#include "Order_Lifetime_Validator.hpp"
#include "Parameter_Parsing.hpp"

Order_Lifetime_Validator::Order_Lifetime_Validator(
  int hourstart,
  int minstart,
  int hourend,
  int minend)
  : hourstart_(hourstart)
  , minstart_(minstart)
  , hourend_(hourend)
  , minend_(minend)
{
  // Validation
  if (hourstart<0 || hourstart>23
      || minstart<0 || minstart>59
      || hourend<0 || hourend>23
      || minend<0 || minend>59
      || hourstart>hourend
      || (hourstart==hourend && minstart>minend))
    throw std::invalid_argument("invalid time(s)");
}

bool
Order_Lifetime_Validator::interval_falls_within_bounds(
  time_t* start,
  time_t* stop,
  std::string errmsg
  )
{
  time_t now(0);
  time(&now);

  const time_t nyopen(0);
  if (!dart::time::get_ny_market_start_time(&now))
  {
    errmsg="Internal error getting market open time";
    return false;
  }

  const time_t nyclose(0);
  if (!dart::time::get_ny_market_stop_time(&now))
  {
    errmsg="Internal error getting market close time";
    return false;
  }

  if (*start==0)
    start=nyopen;
  if (*stop==0)
    stop=nyclose;

  if (stop<start)
  {
    errmsg="Stop time before start time";
    return false;
  }

  if (*start<now && (now-*start)>5) // Allowing 5 seconds of "slop" to be nice
  {
    errmsg="Start time in the past";
    return false;
  }

  // Cannot start before the open
  if (*start<nyopen)
    *start=nyopen;

  if (*stop>nyclose)
  {
    errmsg="Stop time after market close";
    return false;
  }

  return true;
}
