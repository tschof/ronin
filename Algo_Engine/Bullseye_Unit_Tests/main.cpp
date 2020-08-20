#include <ace/message_block.h>
#include <ROM_Handler/ROM_Session.hpp>
#include <ROM_Handler/ROM_Connection.hpp>

#include <ace/Task.h>
#include <ace/OS.h>


#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "Decision_Engine/Decision_Engine.hpp"

#if 0
class Test_Connection
  : public dart::ROM_Connection
{
public:
  Test_Connection ()
  {
    reset ();
  }

  virtual void disconnect ()
  {
    method_call_flags_ |= CALLED_DISCONNECT;
  }

  virtual void send (ACE_Message_Block* pmb)
  {
    delete pmb;
    method_call_flags_ |= CALLED_SEND;
  }

  virtual std::string identify (void)
  {
    method_call_flags_ |= CALLED_IDENTIFY;
    return "";
  }

  bool changed () const
  {
    return (method_call_flags_ != 0);
  }

  void reset ()
  {
    method_call_flags_ = 0;
  }

  unsigned int method_call_flags_;

  static const unsigned int CALLED_DISCONNECT = 0x00000001;
  static const unsigned int CALLED_SEND       = 0x00000002;
  static const unsigned int CALLED_IDENTIFY   = 0x00000004;
};
#endif

#if 0

BOOST_AUTO_TEST_CASE (Empty_Message_Block)
{
  Test_Connection connection;
  dart::ROM_Session session (connection);
  ACE_Message_Block mb;
  session.handle_message (mb);
  BOOST_CHECK (!connection.changed ());
}

BOOST_AUTO_TEST_CASE (Empty_Message)
{
  Test_Connection connection;
  dart::ROM_Session session (connection);
  const char *empty_string = "";
  ACE_Message_Block mb;
  mb.copy (empty_string, 0);
  session.handle_message (mb);
  BOOST_CHECK (connection.changed () == false);
}

BOOST_AUTO_TEST_CASE (Invalid_CSV_Message)
{
  Test_Connection connection;
  dart::ROM_Session session (connection);
  const char *bogus_string = "bogus";
  ACE_Message_Block mb;
  BOOST_REQUIRE (mb.size (128) != -1);
  BOOST_REQUIRE (mb.copy (bogus_string, strlen (bogus_string)) != -1);
  session.handle_message (mb);
  BOOST_CHECK (connection.changed ());
  BOOST_CHECK (connection.method_call_flags_ == Test_Connection::CALLED_DISCONNECT);
  BOOST_CHECK (session.is_connected () == false);
}

BOOST_AUTO_TEST_CASE (Incomplete_Logon_Message)
{
  Test_Connection connection;
  dart::ROM_Session session (connection);
  const char *bad_login = "L,";
  ACE_Message_Block mb;
  BOOST_REQUIRE (mb.size (128) != -1);
  BOOST_REQUIRE (mb.copy (bad_login, strlen (bad_login)) != -1);
  session.handle_message (mb);
  BOOST_CHECK (connection.changed ());
  BOOST_CHECK (connection.method_call_flags_ == Test_Connection::CALLED_DISCONNECT);
}

BOOST_AUTO_TEST_CASE (No_Sequence_Checking_Logon)
{
  Test_Connection connection;
  dart::ROM_Session session (connection);
  const char *valid_logon_no_seq = "L,154738,ROM9,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,";
  ACE_Message_Block mb;
  BOOST_REQUIRE (mb.size (128) != -1);
  BOOST_REQUIRE (mb.copy (valid_logon_no_seq, strlen (valid_logon_no_seq)) != -1);
  session.handle_message (mb);
  BOOST_CHECK (connection.changed ());
  BOOST_CHECK (connection.method_call_flags_ == Test_Connection::CALLED_SEND);
  BOOST_CHECK (session.is_connected () == true);
}

#endif

#if 0
BOOST_AUTO_TEST_CASE (Sequence_Checking_Logon)
{
  Test_Connection connection;
  dart::ROM_Session session (connection);
  const char *actual_login = "L,190503,ROM9,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,2,1,,,,,,,,";
  ACE_Message_Block mb;
  BOOST_REQUIRE (mb.size (128) != -1);
  BOOST_REQUIRE (mb.copy (actual_login, strlen (actual_login)) != -1);
  session.handle_message (mb);
  BOOST_CHECK (connection.changed ());
  BOOST_CHECK (connection.method_call_flags_ == Test_Connection::CALLED_SEND);
}
#endif
