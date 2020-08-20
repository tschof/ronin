#include "Price_Block.hpp"
#include <iomanip>

namespace dart
{

struct Market_Center_Map_Entry
{
  char market_center_code;
  const char *market_center_name;
};

Market_Center_Map_Entry market_center_mappings [] =
{
  { 'A', "AMEX" },
  { 'B', "BX" },
  { 'C', "NSX" },
  { 'D', "FINRA ADF" },
  { 'E', "Market Independent" },
  { 'I', "ISE" },
  { 'J', "EDGA" },
  { 'K', "EDGX" },
  { 'M', "CHX" },
  { 'N', "NYSE" },
  { 'P', "ARCA" },
  { 'Q', "NAS" },
  { 'T', "NAS" },
  { 'W', "CBSX" },
  { 'Y', "BATS Y" },
  { 'Z', "BATS" }
};

const char *lookup_market_center_name (char c)
{
  for (size_t i (0); i < (sizeof (market_center_mappings) / sizeof (Market_Center_Map_Entry)); ++i) {
    if (market_center_mappings [i].market_center_code == c) {
      return market_center_mappings [i].market_center_name;
    }
  }
  return "Unknown";
}

std::ostream &set_price (std::ostream &stream)
{
  stream << std::fixed << std::setw (6) << std::setprecision (2);
  return stream;
}

std::ostream &dump (std::ostream &stream,
                    const Price_Block &price_block)
{
  if (!price_block.initialized) {
    stream << "Uninitialized";
    return stream;
  }

  stream << "Bid " << std::setw (5) << std::right
                   << (price_block.bid_size * price_block.lot_size)
                   << " @ "
                   << set_price
                   << price_block.bid_price
                   << " ("
                   << std::setw (4)
                   << lookup_market_center_name (price_block.bid_participant_id [0])
                   << ')'
         << " | "
         << "Ask " << std::setw (5) << std::right
                   << (price_block.ask_size * price_block.lot_size)
                   << " @ "
                   << set_price
                   << price_block.ask_price
                   << " ("
                   << std::setw (4)
                   << lookup_market_center_name (price_block.ask_participant_id [0])
                   << ')';
  return stream;
}

std::ostream & operator << (std::ostream &stream,
                            const Price_Block &price_block)
{
  return dump (stream, price_block);
}

std::ostream & operator << (std::ostream &stream,
                            Price_Block &price_block)
{
  return dump (stream, price_block);
}

}
