#include <boost/test/unit_test.hpp>
#include <Decision_Engine/TWAP_Interval_Calculators.hpp>

BOOST_AUTO_TEST_CASE (round_lot_test)
{
  size_t shares_per_interval (0);
  time_t interval_in_secs (0);
  dart::round_lot_calculator (1000, 900, &shares_per_interval, &interval_in_secs);
  BOOST_CHECK_EQUAL (shares_per_interval, 100u);
  BOOST_CHECK_EQUAL (interval_in_secs, 90u);

  dart::round_lot_calculator (1000, 1800, &shares_per_interval, &interval_in_secs);
  BOOST_CHECK_EQUAL (shares_per_interval, 100u);
  BOOST_CHECK_EQUAL (interval_in_secs, 180u);

  dart::round_lot_calculator (1500, 14400 /* 4hrs */, &shares_per_interval, &interval_in_secs);
  BOOST_CHECK_EQUAL (shares_per_interval, 100u);
  BOOST_CHECK_EQUAL (interval_in_secs, 960u);
}

#ifdef FULL_TEST_SUITE
BOOST_AUTO_TEST_CASE (v5_twap_algorithm)
{
  size_t shares_per_interval=0;
  size_t intervalnum;
  double d_interval_length=0.0;
  for (size_t shares=100;shares<=100000;shares+=100) {
    for (time_t seconds=63; seconds<(6*60*60); seconds+=360) {
      dart::twap_interval_calculator_v5(
        shares,
        seconds,
        &shares_per_interval,
        &intervalnum,
        &d_interval_length);
      const size_t total_shares=shares_per_interval*intervalnum;
      BOOST_CHECK((shares-total_shares)<=200);
      assert((shares-total_shares)<=200);
      double total_time=d_interval_length*intervalnum;
      BOOST_CHECK(abs(seconds-total_time)<.01);
      printf(
        "%d over %I64d sec: Trade %d every %f sec "
        "over %d intervals for a total of %d shares\n",
        shares,
        seconds,
        shares_per_interval,
        d_interval_length,
        intervalnum,
        (shares_per_interval*intervalnum));
    }
  }
}
#endif
