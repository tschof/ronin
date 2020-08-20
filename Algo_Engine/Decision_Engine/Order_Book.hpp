#ifndef __ORDER_MANAGER_HPP__
#define __ORDER_MANAGER_HPP__

#include <ace/Singleton.h>
#include <Bullseye/Bullseye_Common.hpp>

#include <string>
#include <map>

class ACE_Task_Base;

namespace dart
{
  class Order_Book
  {
  public:
    std::string generate_order_id ();
    int register_callback (const std::string&, ACE_Task_Base*);
    ACE_Task_Base *find_callback (const std::string&) const;
  private:
    typedef std::map <std::string, ACE_Task_Base*> Callbacks;
    Callbacks callbacks_;
    mutable bullseye::Mutex mutex_;
  };

  typedef ACE_Singleton <Order_Book, ACE_SYNCH_MUTEX> ORDER_BOOK;
}

#endif
