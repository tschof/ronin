#include <Decision_Engine/Time_Source.hpp>
#include <boost/test/unit_test.hpp>
#include <Decision_Engine/Customer_Order.hpp>
#include "romvaluedefs.h"

namespace
{
  // Creates a reference order that is valid for a Customer_Order object.  This
  // can then have fields changed and/or removed to test individual field checks.
  void make_valid_algo_order (CRomCsv &csv)
  {
    csv.SetAt (RFIX::CSV_SYMBOL, "MSFT");
    csv.SetAt (RFIX::CSV_SHARES, 100);
    csv.SetAt (RFIX::CSV_SIDE, ROMVALUE::SIDE_BUY);
    csv.SetAt (RFIX::CSV_TIF, ROMVALUE::TIF_DAY);
    csv.SetAt (RFIX::CSV_TYPE, ROMVALUE::TYPE_MARKET);
    csv.SetAt (RFIX::CSV_CAPACITY, "A");
    csv.SetAt (RFIX::CSV_ALGOTYPE, 1);
  }
}

BOOST_AUTO_TEST_CASE (Customer_Order_Validation_Tests)
{
  CRomCsv csv;
  std::string errmsg;

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

  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  make_valid_algo_order (csv);

  csv.Erase (RFIX::CSV_SYMBOL);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_SYMBOL, "MSFT");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

  // We're not allowing algo orders for less than 500 shares
  csv.SetAt (RFIX::CSV_SHARES, 50);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_SHARES, 500);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

  // Only valid sides are buy, sell, and sell short
  csv.SetAt (RFIX::CSV_SIDE, ROMVALUE::SIDE_BUYMINUS);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_SIDE, ROMVALUE::SIDE_BUY);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

  // In general, the acceptable TIF is day or gtd for algo orders,
  // TIF is not required in the DART system and defaults to Day.
  csv.Erase (RFIX::CSV_TIF);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

  csv.SetAt (RFIX::CSV_TIF, ROMVALUE::TIF_DAY);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

  // The base class for Customer_Order does not validate TWAP orders
  csv.SetAt (RFIX::CSV_TIF, 8 /* TWAP */);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_TIF, ROMVALUE::TIF_GTD);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

  csv.Erase (RFIX::CSV_TYPE);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_TYPE, ROMVALUE::TYPE_MARKET);
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);

}

BOOST_AUTO_TEST_CASE (Customer_Order_Start_Time_Validation_Test)
{
  CRomCsv csv;
  make_valid_algo_order (csv);
  std::string errmsg;

  // Set the time to after the New York market open
  tm current_time;
  memset (&current_time, 0xff, sizeof (current_time));
  current_time.tm_year = 109;
  current_time.tm_mon  =   9;
  current_time.tm_mday =   6;
  current_time.tm_hour =   9;
  current_time.tm_min  =   0;
  current_time.tm_sec  =   0;
  current_time.tm_wday =   2; // Tue
  dart::time::TIME_SOURCE::instance ()->freeze_time (mktime (&current_time));


  // Try invalid date/time formats, we're going to be very picky with these
  csv.SetAt (RFIX::CSV_START_TIME, "1330");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_START_TIME, "13:30");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_START_TIME, "2009-10-06 13:30");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_START_TIME, "2009-10-06 13:30:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_START_TIME, "2009-10-06 13:30:00.000");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Valid time format but a start time is before the current time
  csv.SetAt (RFIX::CSV_START_TIME, "20091006-13:30:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_START_TIME, "20091006-13:30:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Set the time to before NY open
  current_time.tm_hour = 8; // 1300 UTC
  dart::time::TIME_SOURCE::instance ()->freeze_time (mktime (&current_time));
  csv.SetAt (RFIX::CSV_START_TIME, "20091006-13:29:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Set the time to after NY open... yesterday
  csv.SetAt (RFIX::CSV_START_TIME, "20091005-15:00:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Set the time to the close
  csv.SetAt (RFIX::CSV_START_TIME, "20091006-20:00:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Set after the close
  csv.SetAt (RFIX::CSV_START_TIME, "20091006-21:00:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Set the start time to the wall clock time, this should be okay
  csv.SetAt (RFIX::CSV_START_TIME, "20091006-14:00:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);
}

BOOST_AUTO_TEST_CASE (Customer_Order_Stop_Time_Validation_Test)
{
  CRomCsv csv;
  make_valid_algo_order (csv);
  std::string errmsg;

  // Set the time to after the New York market open
  tm current_time;
  memset (&current_time, 0xff, sizeof (current_time));
  current_time.tm_year = 109;
  current_time.tm_mon  =   9;
  current_time.tm_mday =   6;
  current_time.tm_hour =   9;
  current_time.tm_min  =   0;
  current_time.tm_sec  =   0;
  current_time.tm_wday =   2; // Tue
  dart::time::TIME_SOURCE::instance ()->freeze_time (mktime (&current_time));

  // Try invalid date/time formats, we're going to be very picky with these
  csv.SetAt (RFIX::CSV_END_TIME, "1330");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_END_TIME, "13:30");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_END_TIME, "2009-10-06 13:30");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_END_TIME, "2009-10-06 13:30:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_END_TIME, "2009-10-06 13:30:00.000");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);


  // Valid time format but the end time is before the current time
  csv.SetAt (RFIX::CSV_END_TIME, "20091006-13:30:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  csv.SetAt (RFIX::CSV_END_TIME, "20091006-13:30:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

# if 0
  // Set the start time to the wall clock time, this should fail, there is a
  // minimum amount of time that an order can be worked, by default we're
  // setting this at 15 minutes.
  csv.SetAt (RFIX::CSV_END_TIME, "20091006-14:00:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), false);

  // Try total worked time of 15 minutes exactly
  csv.SetAt (RFIX::CSV_END_TIME, "20091006-14:15:00");
  BOOST_CHECK_EQUAL (dart::Customer_Order (csv).validate (errmsg), true);
# endif
}
