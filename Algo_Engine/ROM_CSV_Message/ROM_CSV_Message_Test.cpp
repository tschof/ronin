#define _CRT_SECURE_NO_WARNINGS
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "ROM_CSV_Message.hpp"
#include <iostream>

BOOST_AUTO_TEST_CASE (null_ptr_zero_length)
{
  dart::ROM_CSV_Read_Only_Message <80> m (0,0);
}

BOOST_AUTO_TEST_CASE (valid_buffer_zero_length)
{
  char buffer [64];
  memset (buffer, ',', sizeof (buffer));
  buffer [0] = '1';
  dart::ROM_CSV_Read_Only_Message <80> m (buffer, 0);
  BOOST_CHECK_EQUAL (buffer [1], ',');
}

BOOST_AUTO_TEST_CASE (null_ptr_nonzero_length)
{
  dart::ROM_CSV_Read_Only_Message <80> m (0, 64);
}

# if TEST_COMPILATION // These should not compile, define this symbol to test

BOOST_AUTO_TEST_CASE (zero_template_arg)
{
  dart::ROM_CSV_Read_Only_Message <0> m (0, 64);
}

BOOST_AUTO_TEST_CASE (neg_template_arg)
{
  dart::ROM_CSV_Read_Only_Message <-24> m (0, 0);
}

BOOST_AUTO_TEST_CASE (neg_var_template_arg)
{
  const int i (-1);
  dart::ROM_CSV_Read_Only_Message <i> m (0,0);
}

# endif

BOOST_AUTO_TEST_CASE (var_template_arg)
{
  const int i (80);
  dart::ROM_CSV_Read_Only_Message <i> m (0,0);
}

BOOST_AUTO_TEST_CASE (single_cached_field)
{
  char buffer [64];
  strcpy (buffer, "L,,,did it index this?");
  dart::ROM_CSV_Read_Only_Message <1> m (buffer, strlen (buffer));
  BOOST_CHECK (m.get (0) != 0);
  BOOST_CHECK_EQUAL (strcmp (m.get (0), "L"), 0);
  BOOST_CHECK (m.get (1) == 0); // Field 1 is in the message but not indexed
  BOOST_CHECK (m.get (3) == 0); // Field 3 is in the message but not indexed
  BOOST_CHECK (m.get (9) == 0); // Field 9 is not in the message
  std::ostringstream indexed_fields_dump;
  m.dump_indexed_fields (indexed_fields_dump);
  BOOST_CHECK (indexed_fields_dump.str () != std::string (buffer));

}

BOOST_AUTO_TEST_CASE (strtol_tests)
{
  const char *zero_buffer = "0";
  int result = strtol (zero_buffer, (char**) 0, 10);
  BOOST_CHECK_EQUAL (result, 0);
  BOOST_CHECK_EQUAL (errno, 0);

#if 0
  const char *x_buffer = "X";
  result = strtol (x_buffer, (char**) 0, 10);
  const int errno_copy (errno);
  BOOST_CHECK_EQUAL (result, 0);
  BOOST_CHECK (errno != 0);
#endif

  {
    std::istringstream convert ("X");
    BOOST_CHECK (! (convert >> result));
  }

  {
    std::istringstream convert ("   0");
    BOOST_CHECK (convert >> result);
    BOOST_CHECK_EQUAL (result, 0);
  }
}

#include <windows.h>

/**
 * @class Process_Affinity_Controller
 *
 * @brief Just to make sure there are no problems with Windows reading perf
 * counters in SMP systems, instantiating this class ties this process to a
 * single processor; the destructor reverts the process affinity mask to
 * previous values.
 */
struct Process_Affinity_Controller
{
  Process_Affinity_Controller ()
    : previous_mask_ (0)
  {
    DWORD_PTR sys_mask (0), curr_mask (0);
    if (!GetProcessAffinityMask (GetCurrentProcess (),
                                 &previous_mask_,
                                 &sys_mask) ||
        !SetProcessAffinityMask (GetCurrentProcess (),
                                 static_cast <DWORD_PTR> (1L)) ||
        !GetProcessAffinityMask (GetCurrentProcess (),
                                 &curr_mask,
                                 &sys_mask) ||
        curr_mask != 1L)

      throw std::runtime_error ("Error while setting process affinity mask");
  }

  ~Process_Affinity_Controller ()
  {
    SetProcessAffinityMask (GetCurrentProcess (),
                            previous_mask_);
  }

private:
  DWORD_PTR previous_mask_;
};

BOOST_AUTO_TEST_CASE (atoi_perf_test)
{
  Process_Affinity_Controller affinity_manager;
  affinity_manager;

  LARGE_INTEGER freq, start, stop;
  BOOST_REQUIRE (QueryPerformanceFrequency (&freq));
  const size_t iterations (1000);
  QueryPerformanceCounter (&start);
  for (size_t i (0); i < iterations; ++i) {
    int result = atoi ("12345");
    result;
  }
  QueryPerformanceCounter (&stop);
  double diff = static_cast <double> (stop.QuadPart - start.QuadPart);
  diff /= iterations;
  std::cout << "atoi conversion" << std::endl;
  std::cout << "Avg cycles: " << diff << std::endl;
  diff /= freq.QuadPart;
  std::cout << "Avg time: " << diff << " s" << std::endl;
}

BOOST_AUTO_TEST_CASE (strtol_perf_test)
{
  Process_Affinity_Controller affinity_manager;
  affinity_manager;

  LARGE_INTEGER freq, start, stop;
  BOOST_REQUIRE (QueryPerformanceFrequency (&freq));
  const size_t iterations (1000);
  QueryPerformanceCounter (&start);
  for (size_t i (0); i < iterations; ++i) {
    int result = strtol ("12345", (char**) 0, 10);
    result;
  }
  QueryPerformanceCounter (&stop);
  double diff = static_cast <double> (stop.QuadPart - start.QuadPart);
  diff /= iterations;
  std::cout << "strtol conversion" << std::endl;
  std::cout << "Avg cycles: " << diff << std::endl;
  diff /= freq.QuadPart;
  std::cout << "Avg time: " << diff << " s" << std::endl;
}

BOOST_AUTO_TEST_CASE (istringstream_perf_test)
{
  Process_Affinity_Controller affinity_manager;
  affinity_manager;

  LARGE_INTEGER freq, start, stop;
  BOOST_REQUIRE (QueryPerformanceFrequency (&freq));
  const size_t iterations (1000);
  QueryPerformanceCounter (&start);
  for (size_t i (0); i < iterations; ++i) {
    std::istringstream converter ("12345");
    int result (0);
    converter >> result;
  }
  QueryPerformanceCounter (&stop);
  double diff = static_cast <double> (stop.QuadPart - start.QuadPart);
  diff /= iterations;
  std::cout << "iostream conversion" << std::endl;
  std::cout << "Avg cycles: " << diff << std::endl;
  diff /= freq.QuadPart;
  std::cout << "Avg time: " << diff << " s" << std::endl;
}
