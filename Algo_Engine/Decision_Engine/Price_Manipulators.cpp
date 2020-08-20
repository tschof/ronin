#include "Price_Manipulators.hpp"
#include <cstdio>
#include <stdexcept>

namespace dart
{
  std::string calc_midpoint (double x, double y)
  {
    const double mid ((x+y) / 2);
    return double_to_string (mid);
  }

  std::string double_to_string (double d)
  {
    char buffer [32];
    memset (buffer, 0, sizeof (buffer));
    if (sprintf (buffer, "%.2f", d) < 0)
      throw std::runtime_error ("Cannot convert double to string");
    return std::string (buffer);
  }
}
