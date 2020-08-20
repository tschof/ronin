#ifndef __NY_INTERVAL_CHECKER_HPP__
#define __NY_INTERVAL_CHECKER_HPP__

#include <ctime>
#include <string>

/**
* @class NY_Interval_Checker
* @brief Keeps an interval of New York local time and can determine
* if a specific interval will fall within the configured time interval.
*/
class Order_Lifetime_Validator
{
public:
  Order_Lifetime_Validator(
    int hourstart,
    int minstart,
    int hourend,
    int minend
    );

  bool
  interval_falls_within_bounds(
    time_t *start,
    time_t *stop,
    std::string errmsg
    );

private:
  int hourstart_;
  int minstart_;
  int hourend_;
  int minend_;
};


#endif
