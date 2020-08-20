#include "Order_Book.hpp"

#include <time.h>
#include <sstream>

#include "TWAP_Time_Slice_Manager.hpp"

namespace
{
  // These functions handle the meat of inserting and retrieving order IDs;
  // I want to create different types for internal orders and customer orders
  // for now.

  template <typename Order_Type, typename Assoc_Cont_Type>
  Order_Type *find_order (const Assoc_Cont_Type &container,
                          const std::string &order_id)
  {
    Assoc_Cont_Type::const_iterator result = container.find (order_id);
    if (result != container.end ())
      return result->second;
    return 0;
  }

  template <typename Order_Type, typename Assoc_Cont_Type>
  int register_order (Assoc_Cont_Type &container,
                      const std::string &id,
                      Order_Type *order)
  {
    if (id.empty () ||
        order == 0  ||
        container.find (id) != container.end () ||
        container.insert (std::make_pair (id, order)).second == false) {
      return -1;
    }
    return 0;
  }

}

namespace dart
{
  std::string
  Order_Book::generate_order_id ()
  {
    bullseye::Guard guard(this->mutex_);
    static size_t intrasecond_counter(0);
    static time_t last_time_used(0);
    time_t now;
    if (time(&now) != last_time_used)
    {
      last_time_used=now;
      intrasecond_counter=0;
    }
    else
      ++intrasecond_counter;
    std::ostringstream time_to_str;
    time_to_str << now << intrasecond_counter;
    return time_to_str.str ();
  }

  int Order_Book::register_callback (const std::string &key, ACE_Task_Base *value)
  {
    bullseye::Guard guard(this->mutex_);
    int result (-1);

    if (key.empty())
      ACE_DEBUG((LM_DEBUG, "%s: key is empty!\n", __FUNCTION__));
    else
    {
      if (value)
      {
        if (this->callbacks_.insert(std::make_pair(key,value)).second)
          result = 0;
        else
          ACE_DEBUG((LM_DEBUG, "%s: cannot insert into callback map!\n", __FUNCTION__));
      }
      else
        ACE_DEBUG((LM_DEBUG, "%s: value is null!\n", __FUNCTION__));
    }

    return result;
  }

  ACE_Task_Base *Order_Book::find_callback (const std::string &key) const
  {
    bullseye::Guard guard(this->mutex_);
    ACE_Task_Base *result = 0;
    Callbacks::const_iterator callback_iterator (this->callbacks_.find (key));
    if (callback_iterator != this->callbacks_.end ())
      result = callback_iterator->second;
    return result;
  }
}
