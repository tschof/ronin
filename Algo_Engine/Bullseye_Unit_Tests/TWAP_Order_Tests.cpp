#include <Decision_Engine/Time_Source.hpp>
#include <boost/test/unit_test.hpp>
#include <Decision_Engine/TWAP_Order.hpp>
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

BOOST_AUTO_TEST_CASE (TWAP_Order_Test)
{
  // Let's see what kinds of combinations of time slices and orders per slice
  // we get out of this algorithm.
  CRomCsv csv;
  std::string errmsg;
  BOOST_CHECK_EQUAL (dart::TWAP_Order (csv, 0, 0, 0).validate (errmsg), false);

  // Set the time to after the New York market open
  tm current_time;
  memset (&current_time, 0xff, sizeof (current_time));
  current_time.tm_year = 109;
  current_time.tm_mon  =   9;
  current_time.tm_mday =   6;
  current_time.tm_hour =   8;
  current_time.tm_min  =  30;
  current_time.tm_sec  =   0;
  current_time.tm_wday =   2; // Tue
  dart::time::TIME_SOURCE::instance ()->freeze_time (mktime (&current_time));

  csv.SetAt (RFIX::CSV_SYMBOL, "MSFT");
  csv.SetAt (RFIX::CSV_SHARES, 500);
  csv.SetAt (RFIX::CSV_SIDE, ROMVALUE::SIDE_BUY);
  csv.SetAt (RFIX::CSV_TIF, 8);
  csv.SetAt (RFIX::CSV_TYPE, ROMVALUE::TYPE_MARKET);
  csv.SetAt (RFIX::CSV_CAPACITY, "A");
  csv.SetAt (RFIX::CSV_END_TIME, "20091006-14:30:00");
}

/*
Since the TWAP algorithm unit tests take so many minutes to complete
(we're brute forcing the inputs and doing sanity checks against the
output values), the long-running tests are now wrapped in a conditional
preprocessor clause so that defining FULL_UNIT_TESTS will run all tests,
this is not normally defined so unit tests can still be run with each
and every build without slowing development.
*/

#ifdef FULL_UNIT_TESTS

// Testing various input values against the TWAP calculation and checking for
// invariants on the results, essentially a sanity check against the algorithm.
BOOST_AUTO_TEST_CASE (TWAP_Order_Algo_Parameter_Calculation_Coverage_Test)
{
  for (size_t total_shares (500); total_shares < 1000000; total_shares += 100) {
    for (size_t min_secs_per_interval (1); min_secs_per_interval < 3600; min_secs_per_interval *= 2) {
      for (size_t min_shares_per_interval (100); min_shares_per_interval <= 1000; min_shares_per_interval += 100) {
        dart::TWAP_Order order (CRomCsv (), 0, 0);
        time_t total_interval (4 * 60 * 60);
        try {
          if (order.calculate_twap_intervals (total_interval,
                                              total_shares,
                                              min_secs_per_interval,
                                              min_shares_per_interval)) {
              BOOST_CHECK (order.shares_per_interval () >= min_shares_per_interval);
              BOOST_CHECK (order.seconds_per_interval () >= min_secs_per_interval);
              BOOST_CHECK
                (((order.intervals () * order.shares_per_interval ()) + order.excess_shares ())
                  == total_shares);
              BOOST_REQUIRE (order.intervals () * order.seconds_per_interval () <= total_interval);
              BOOST_CHECK
                ((total_interval -
                  (order.intervals () * order.seconds_per_interval ())) <
                    order.seconds_per_interval ());
          }
        } catch (const std::exception &e) {
          std::cerr << "Cannot provide a solution for " << total_shares
            << " over " << total_interval << " seconds "
            << "with a minimum of " << min_secs_per_interval
            << " secs per interval and a minimum "
            << min_shares_per_interval
            << " shares per interval" << std::endl;
        }
      }
    }
  }
}
#endif // FULL_UNIT_TESTS
