#include <boost/test/unit_test.hpp>
#include <Decision_Engine/Parameter_Parsing.hpp>
#include <Decision_Engine/Parameter_Parsing_Imp.hpp>
#include <ctime>
#include <windows.h>

BOOST_AUTO_TEST_CASE (Extract_Named_Pairs_From_Empty_String)
{
  dart::bullseye::Parameters parameters;
  std::string temp;
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (temp, parameters), false);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string (""), parameters), false);
}

BOOST_AUTO_TEST_CASE (Extract_Named_Pairs_Incomplete_Pair)
{
  dart::bullseye::Parameters parameters;
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo"), parameters), false);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo="), parameters), false);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo = bar"), parameters), false);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("="), parameters), false);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("=bar"), parameters), false);
}

BOOST_AUTO_TEST_CASE (Extract_Named_Pairs_Multiple_Incomplete_Pairs)
{
  dart::bullseye::Parameters parameters;
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo= something=else"), parameters), false);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo = bar"), parameters), false);
}

BOOST_AUTO_TEST_CASE (Extract_Named_Pairs_Complete_Pair)
{
  dart::bullseye::Parameters parameters;
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo=bar"), parameters), true);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("a=b"), parameters), true);
}

BOOST_AUTO_TEST_CASE (Extract_Named_Pairs_Multiple_Complete_Pairs)
{
  dart::bullseye::Parameters parameters;
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo=bar something=else"), parameters), true);
  BOOST_CHECK_EQUAL (dart::bullseye::extract_pairs (std::string ("foo=bar something=else aggressiveness=1"), parameters), true);
}

BOOST_AUTO_TEST_CASE (Parse_time_str_Empty_Test)
{
  time_t t;
  BOOST_CHECK_EQUAL (dart::time::parse_time_str (std::string (), t), false);
  BOOST_CHECK_EQUAL (dart::time::parse_time_str (std::string (""), t), false);
}

BOOST_AUTO_TEST_CASE (Parse_time_str_Valid_Time_Test)
{
  time_t t (0);
  const char *t1 = "20090930-21:34:00";
  BOOST_CHECK_EQUAL (dart::time::parse_time_str (t1, t), true);

  char buf [64];
  memset (buf, 0, sizeof (buf));

  BOOST_REQUIRE (strftime (buf, sizeof (buf), "%Y%m%d-%H:%M:%S", gmtime (&t)));
  BOOST_CHECK (strcmp (buf, t1) == 0);

  BOOST_CHECK_EQUAL (dart::time::parse_time_str ("20091001-01:01", t), true);
}

BOOST_AUTO_TEST_CASE (Parse_time_str_Invalid_Partial_Time_Test)
{
  time_t t (0);
  BOOST_CHECK_EQUAL (dart::time::parse_time_str ("2009", t), false);
  BOOST_CHECK_EQUAL (dart::time::parse_time_str ("20091001", t), false);
  BOOST_CHECK_EQUAL (dart::time::parse_time_str ("20091001-01:", t), false);
}

BOOST_AUTO_TEST_CASE (isdst_Test)
{
  time_t now (0);
  now = time (&now);
  bool dst_flag (false);
  BOOST_CHECK (dart::time::isdst (now, dst_flag));
  int current_offset (0);
  BOOST_CHECK_EQUAL (dart::time::get_ny_utc_offset (now, &current_offset), true);
  TIME_ZONE_INFORMATION tzinfo;
  const int offset (
    (GetTimeZoneInformation (&tzinfo) == TIME_ZONE_ID_STANDARD) ? -5 : -4);
  BOOST_CHECK_EQUAL (current_offset, offset);
}

BOOST_AUTO_TEST_CASE (get_ny_market_start_time_Tests)
{
  tm dst_time;
  memset (&dst_time, 0xff, sizeof (dst_time));
  dst_time.tm_year = 109; // 2009
  dst_time.tm_mon  =   9; // Oct
  dst_time.tm_mday =   1; // 1st
  dst_time.tm_hour =  10; // 10:00:00 am
  dst_time.tm_min  =  00; //
  dst_time.tm_sec  =  00; //
  time_t dst_time_t (mktime (&dst_time));

  BOOST_REQUIRE (dart::time::get_ny_market_start_time_imp (&dst_time_t));
  tm *ptm = gmtime (&dst_time_t);
  BOOST_REQUIRE (ptm);
  BOOST_CHECK_EQUAL (ptm->tm_hour, 13);
  BOOST_CHECK_EQUAL (ptm->tm_min, 30);

  dst_time.tm_mon = 11; // Dec
  dst_time.tm_mday = 1; // 1st
  dst_time_t = mktime (&dst_time);

  BOOST_REQUIRE (dart::time::get_ny_market_start_time_imp (&dst_time_t));
  ptm = gmtime (&dst_time_t);
  BOOST_REQUIRE (ptm);
  BOOST_CHECK_EQUAL (ptm->tm_hour, 14);
  BOOST_CHECK_EQUAL (ptm->tm_min, 30);
}

BOOST_AUTO_TEST_CASE (get_ny_market_stop_time_Tests)
{
  tm dst_time;
  memset (&dst_time, 0xff, sizeof (dst_time));
  dst_time.tm_year = 109; // 2009
  dst_time.tm_mon  =   9; // Oct
  dst_time.tm_mday =   1; // 1st
  dst_time.tm_hour =  10; // 10:00:00 am
  dst_time.tm_min  =  00; //
  dst_time.tm_sec  =  00; //
  time_t dst_time_t (mktime (&dst_time));

  BOOST_REQUIRE (dart::time::get_ny_market_stop_time_imp (&dst_time_t));
  tm *ptm = gmtime (&dst_time_t);
  BOOST_REQUIRE (ptm);
  BOOST_CHECK_EQUAL (ptm->tm_hour, 20);
  BOOST_CHECK_EQUAL (ptm->tm_min, 0);

  dst_time.tm_mon = 11; // Dec
  dst_time.tm_mday = 1; // 1st
  dst_time_t = mktime (&dst_time);

  BOOST_REQUIRE (dart::time::get_ny_market_stop_time_imp (&dst_time_t));
  ptm = gmtime (&dst_time_t);
  BOOST_REQUIRE (ptm);
  BOOST_CHECK_EQUAL (ptm->tm_hour, 21);
  BOOST_CHECK_EQUAL (ptm->tm_min, 0);
}
