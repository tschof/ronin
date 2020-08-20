#ifndef __VOL_CURVE_PARSER_HPP__
#define __VOL_CURVE_PARSER_HPP__

#include <vector>
#include "Volume_Profile.hpp"

void parse_vol_curve_file
  (const char *filename,
   Volume_Profile &entries);

#endif
