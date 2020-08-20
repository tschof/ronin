#include <boost/test/unit_test.hpp>
#include <bullseye/dart_message.hpp>
#include "ROM_Handler/romcsv.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

BOOST_AUTO_TEST_CASE(Basic_usage_test)
{
  CRomCsv csv;
  csv.SetAt(RFIX::CSV_COMMAND,ROMVALUE::COMMAND_LOGON);
  csv.SetAt(RFIX::CSV_TRADER,"DAVIDC");
  csv.SetAt(RFIX::CSV_LASTRECEIVED_SEQ,1000);
  const std::string csvstr(csv.Message());
  std::vector<char> buffer(csvstr.size()+1);
  strncpy(&buffer[0],csvstr.c_str(),buffer.size());
  dart::Read_Only_Fixed_Size_Message<80> fixedmsg(&buffer[0],buffer.size());
  const char *field=fixedmsg.get(RFIX::CSV_COMMAND);
  BOOST_REQUIRE(field!=0);
  BOOST_CHECK_EQUAL(strcmp(fixedmsg.get(RFIX::CSV_COMMAND),"L"),0);
  field=fixedmsg.get(RFIX::CSV_TRADER);
  BOOST_REQUIRE(field!=0);
  BOOST_CHECK_EQUAL(strcmp(field,"DAVIDC"),0);
  field=fixedmsg.get(RFIX::CSV_LASTRECEIVED_SEQ);
  BOOST_REQUIRE(field!=0);
  BOOST_CHECK_EQUAL(strcmp(field,"1000"),0);
}

BOOST_AUTO_TEST_CASE(Null_pointer_test)
{
  typedef dart::Read_Only_Fixed_Size_Message<80> Read_Only_Msg;
  Read_Only_Msg m(0,0);
  for(size_t i(0);i<Read_Only_Msg::MAX_FIELDS;++i)
    BOOST_CHECK(m.get(i)==0);
}

BOOST_AUTO_TEST_CASE(Zero_size_test)
{
  typedef dart::Read_Only_Fixed_Size_Message<80> Read_Only_Msg;
  const char *test_msg="1,2,3,4,5,6\n";
  std::vector<char> v;
  std::copy(test_msg,test_msg+strlen(test_msg),std::back_inserter(v));
  Read_Only_Msg m(&v[0],0);
  for(size_t i(0);i<Read_Only_Msg::MAX_FIELDS;++i)
    BOOST_CHECK(m.get(i)==0);
}

BOOST_AUTO_TEST_CASE(Default_template_argument_test)
{
  dart::Read_Only_Fixed_Size_Message<> m(0,0);
}

// testing a new csv message wrapper that doesn't modify the original buffer
#include <bullseye/DART_CSV_Message.hpp>

BOOST_AUTO_TEST_CASE(Non_Modifying_Read_Only_CSV_Indexer_test_case)
{
  typedef Non_Modifying_Read_Only_CSV_Indexer <6> Test_Indexer;
  BOOST_CHECK_EQUAL(Test_Indexer::Num_Fields, 6u);
  Test_Indexer idx1(0,1);
  BOOST_CHECK_EQUAL(idx1.error(),true);
  Test_Indexer idx2(reinterpret_cast<const char*>(0x80000000),0);
  BOOST_CHECK_EQUAL(idx2.error(),true);
  Test_Indexer idx3(0,0);
  BOOST_CHECK_EQUAL(idx3.error(),true);
  const char *test_1_msg = "L,,,,foo,bar,1000\n";
  Test_Indexer idx4(test_1_msg,strlen(test_1_msg));
  BOOST_CHECK_EQUAL(idx4.error(),false);
  const char *p = idx4.getstring(0);
  BOOST_REQUIRE(p);
  BOOST_CHECK(strcmp(p,"L")==0);
  p = idx4.getstring(0);
  BOOST_REQUIRE(p);
  BOOST_CHECK(strcmp(p,"L")==0);
  p = idx4.getstring(1);
  BOOST_CHECK(p==0);
  p = idx4.getstring(2);
  BOOST_CHECK(p==0);
  p = idx4.getstring(3);
  BOOST_CHECK(p==0);
  p = idx4.getstring(4);
  BOOST_CHECK(strcmp(p,"foo")==0);
  p = idx4.getstring(5);
  BOOST_CHECK(strcmp(p,"bar")==0);
  p = idx4.getstring(6);
  BOOST_CHECK(p==0);
  p = idx4.getstring(7);
  BOOST_CHECK(p==0);
}

BOOST_AUTO_TEST_CASE(Non_Modifying_Read_Only_CSV_Indexer_test_case_2)
{
  typedef Non_Modifying_Read_Only_CSV_Indexer <80> Test_Indexer;
  BOOST_CHECK_EQUAL(Test_Indexer::Num_Fields, 80u);
  // Well less than the field index limit
  const char *msg = "E,1,2,3,4,MSFT,6,7,8\n";
  Test_Indexer idx1(msg,strlen(msg));

  const char *p = 0;

  p = idx1.getstring(4);
  BOOST_CHECK(strcmp(p,"4")==0);

  p = idx1.getstring(3);
  BOOST_CHECK(strcmp(p,"3")==0);

  p = idx1.getstring(5);
  BOOST_CHECK(strcmp(p,"MSFT")==0);

  p = idx1.getstring(0);
  BOOST_CHECK(strcmp(p,"E")==0);

  p = idx1.getstring(1);
  BOOST_CHECK(strcmp(p,"1")==0);

  p = idx1.getstring(2);
  BOOST_CHECK(strcmp(p,"2")==0);

  p = idx1.getstring(6);
  BOOST_CHECK(strcmp(p,"6")==0);

  p = idx1.getstring(7);
  BOOST_CHECK(strcmp(p,"7")==0);

  p = idx1.getstring(8);
  BOOST_CHECK(strcmp(p,"8")==0);

  p = idx1.getstring(9);
  BOOST_CHECK(p==0);
}

BOOST_AUTO_TEST_CASE(Non_Modifying_Read_Only_CSV_Indexer_pathological_cases)
{
  typedef Non_Modifying_Read_Only_CSV_Indexer <80> Test_Indexer;

  {
    const char *emptystring = "";
    Test_Indexer indexer(emptystring,strlen(emptystring));
    const char *p = indexer.getstring(0);
    BOOST_CHECK(p==0);
  }

  {
    const char *newlinestr = "\n";
    Test_Indexer indexer(newlinestr,strlen(newlinestr));
    const char *p = indexer.getstring(0);
    BOOST_CHECK(p==0);
  }

  size_t siz=sizeof(Test_Indexer);
  printf("size=%d\n",siz);
}
