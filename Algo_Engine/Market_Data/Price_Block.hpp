#ifndef __PRICE_BLOCK_HPP__
#define __PRICE_BLOCK_HPP__

#include <iostream>
#include "DART_Price_Block.h"

namespace dart
{
  const char *lookup_market_center_name (char); // Should be in DLL?
  std::ostream & operator << (std::ostream&, Price_Block&);
  std::ostream & operator << (std::ostream&, const Price_Block&);
  std::ostream & dump (std::ostream&, const Price_Block&);
}

#endif
