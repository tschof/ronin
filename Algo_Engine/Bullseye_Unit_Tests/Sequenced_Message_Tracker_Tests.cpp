#include <boost/test/unit_test.hpp>
#include <ROM_Handler/Sequenced_Message_Tracker.hpp>
#include <io.h>
#include <windows.h> // For MAXPATH definition

#if 0
std::string make_temp_file_name ()
{
  char temp_file_name_buffer [MAX_PATH] = {'\0'};
  strcpy (temp_file_name_buffer, "Sequenced_Message_Tracker_Tests_XXXXXX");
  BOOST_REQUIRE (_mktemp (temp_file_name_buffer));
  return temp_file_name_buffer;
}

BOOST_AUTO_TEST_CASE (Sequenced_Message_Tracker_Test)
{
  BOOST_CHECK_THROW (dart::Sequenced_Message_Tracker (0), std::runtime_error);
  BOOST_CHECK_THROW (dart::Sequenced_Message_Tracker (""), std::runtime_error);

  const std::string file_name (make_temp_file_name ());
  {
    dart::Sequenced_Message_Tracker tracker (file_name.c_str ());
  }
  BOOST_REQUIRE (remove (file_name.c_str ()) == 0);
}

#endif
