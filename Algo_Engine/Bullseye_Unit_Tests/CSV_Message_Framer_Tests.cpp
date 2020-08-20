#include <boost/test/unit_test.hpp>
#include <ROM_Handler/CSV_Message_Framer.hpp>

BOOST_AUTO_TEST_CASE (CSV_Message_Framer_Empty_Buffer_Test)
{
  dart::CSV_Message_Framer framer;
  BOOST_CHECK_THROW (framer.frame (0,0), std::invalid_argument);
  ACE_Message_Block *pmsg = new ACE_Message_Block ();
  BOOST_CHECK_THROW (framer.frame (pmsg,0), std::invalid_argument);
  ACE_Message_Block *presult = 0;
  BOOST_CHECK_THROW (framer.frame (0,&presult), std::invalid_argument);
  BOOST_CHECK_EQUAL (framer.frame (pmsg,&presult), false);
  pmsg->size (1);
  BOOST_CHECK_EQUAL (framer.frame (pmsg,&presult), false);
  BOOST_REQUIRE (pmsg->copy ("\n", 1) == 0);
  BOOST_CHECK_EQUAL (framer.frame (pmsg,&presult), false);
  BOOST_REQUIRE (pmsg->size (2) == 0);
  BOOST_REQUIRE (pmsg->copy ("\n", 1) == 0);
  BOOST_CHECK_EQUAL (framer.frame (pmsg,&presult), false);
}

BOOST_AUTO_TEST_CASE (CSV_Message_Framer_Basic_Message_Test)
{
  dart::CSV_Message_Framer framer;
  ACE_Message_Block mb (64), *result = 0;
  BOOST_REQUIRE (mb.copy ("H", 1) == 0);
  BOOST_CHECK_EQUAL (framer.frame (&mb,&result), false);
  BOOST_REQUIRE (mb.copy ("\n", 1) == 0);
  BOOST_CHECK_EQUAL (framer.frame (&mb, &result), true);
  BOOST_REQUIRE (result);
  BOOST_CHECK_EQUAL (result->length (), 1u);
  BOOST_CHECK_EQUAL (mb.length (), 0u);
}

BOOST_AUTO_TEST_CASE (CSV_Message_Framer_Normal_Usage_Test)
{
  // I consider normal usage to be one message per message block
  dart::CSV_Message_Framer framer;
  ACE_Message_Block mb (128), *result = 0;
  const char *msg1 = "L,,,,,123,2345,346,576457\n";
  BOOST_REQUIRE (mb.copy (msg1) == 0);
  BOOST_CHECK_EQUAL (framer.frame (&mb, &result), true);
  BOOST_CHECK_EQUAL (result->length (), strlen (msg1) - 1);
  std::string m1 (msg1);
  m1.erase (m1.size () - 1);
  BOOST_REQUIRE (strlen (msg1) == (m1.size () + 1));
  std::string m2 (result->rd_ptr (), result->length ());
  BOOST_CHECK_EQUAL (m1, m2);

  const char *msg2 = "E,aslkjwer,wer8023,23.2345,45345,34,,,,,,,,,,,,,,,,,,,,,,,,,\n";
  BOOST_REQUIRE (mb.copy (msg2) == 0);
}

// TODO: Implement partial message buffer framing
#if 0
BOOST_AUTO_TEST_CASE (CSV_Message_Framer_Partial_Message_Test)
{
  dart::CSV_Message_Framer framer;
  ACE_Message_Block mb (128), *result = 0;
  const char *msg1 = "L,234,234,234,2345";
  const char *msg2 = ",,,234,23,4,2,34,234\n";
  BOOST_REQUIRE (mb.copy (msg1, strlen (msg1)) == 0);
  BOOST_CHECK_EQUAL (framer.frame (&mb, &result), false);
  ACE_Message_Block mb2 (128);
  mb2.copy (msg2, strlen (msg2));
  BOOST_CHECK_EQUAL (framer.frame (&mb2, &result), true);
  std::string check (msg1);
  check += msg2;
  check.erase (check.find ('\n'));
  BOOST_CHECK_EQUAL (check, std::string (result->rd_ptr (), result->length ()));
}
#endif
