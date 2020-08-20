#include "Bullseye/Bullseye_Common.hpp"
#include "Order_Factory.hpp"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include "New_Order.hpp"

namespace dart
{
  Order_Factory::Order_Factory ()
    : isconfigured_(true)
  {
    const std::string username
      (CONFIGURATION::instance()->get ("Bullseye","ClientUsername"));
    if (username.empty())
    {
      DART_CRITICAL
        ("[Bullseye]ClientUsername configuration not found, cannot send orders\n");
      this->isconfigured_=false;
    }

    this->common_fields_.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_ORDER);
    this->common_fields_.SetAt (RFIX::CSV_TRADER,username);
    this->common_fields_.SetAt (RFIX::CSV_CAPACITY, 'A');
    this->common_fields_.SetAt (RFIX::CSV_CLEARINGID, "0551");
    this->common_fields_.SetAt (RFIX::CSV_CLEARINGACCOUNT, "RONIN");
    this->common_fields_.SetAt (RFIX::CSV_LOCALACCOUNT, "USER0001");
    this->common_fields_.SetAt (RFIX::CSV_FIRM, "WAG");
    this->common_fields_.SetAt (RFIX::CSV_TRADE_FOR,username);
  }

  Order*
  Order_Factory::create_new_order_with_default_settings ()
  {
    Order *porder=0;
    if (isconfigured_)
      porder = new Order (common_fields_);
    return porder;
  }
}