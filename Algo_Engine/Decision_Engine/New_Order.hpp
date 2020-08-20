#ifndef __NEW_ORDER_HPP__
#define __NEW_ORDER_HPP__

#include "ROM_Handler/RomCsv.h"

namespace dart
{
  class Order
  {
  public:
    Order (CRomCsv &new_order_msg)
      : order_msg_ (new_order_msg)
    {
    }

    CRomCsv &order ()
    {
      return order_msg_;
    }

  private:
    CRomCsv order_msg_;
    typedef std::vector <CRomCsv> CSV_Message_T;
    CSV_Message_T msgs_;
  };

}

#endif
