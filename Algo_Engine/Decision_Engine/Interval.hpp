#ifndef __INTERVAL_HPP__
#define __INTERVAL_HPP__
#include "ACE/Time_Value.h"
struct Interval {
  Interval()
    : start(ACE_Time_Value::zero),stop(ACE_Time_Value::zero)
  {}

  Interval(const ACE_Time_Value &begin,const ACE_Time_Value &end)
    : start(begin),stop(end)
  {}

  Interval (const ACE_Time_Value &time_from_now)
    : start(ACE_OS::gettimeofday()),stop(start+time_from_now)
  {}

  // Do NOT rearrange these members! Ctor above depends on their order.
  ACE_Time_Value start;
  ACE_Time_Value stop;
};
#endif
