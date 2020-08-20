#include <boost/test/unit_test.hpp>
#include <ace/event_handler.h>

namespace dart
{
  namespace bullseye
  {
    /**
     * @class Order
     * @brief State machine representing an order,
     * manages state, required fields, etc.
     */
    class Order
      : ACE_Event_Handler
    {

    };
  }
}

BOOST_AUTO_TEST_CASE (Order_Ctor_Test)
{

}