#ifndef __ORDER_FACTORY_HPP__
#define __ORDER_FACTORY_HPP__

#include <ace/Singleton.h>
#include "ROM_Handler/RomCsv.h"

namespace dart
{
  class Order;

  class Order_Factory
  {
  public:
    Order_Factory ();
    Order *create_new_order_with_default_settings();
  private:
    CRomCsv common_fields_;
    bool isconfigured_;
  };

  typedef ACE_Singleton <Order_Factory,ACE_SYNCH_MUTEX> ORDER_FACTORY;
}

#endif
