// ACE
#include <ace/Task.h>

// Application
#include "Bullseye/Bullseye_Common.hpp"

// Decision Engine
#include "Algo_Manager.hpp"
#include "Order_Book.hpp"

// ROM_Handler
#include "ROM_Handler/RomCsv.h"
#include "ROM_Handler/Session.hpp"

// DART
#include "COMMON\ROMFIXENGINE\FixEngine\RomValueDefs.h"

// Std C++
#include <map>
#include <string>


#include "Order_management_system.hpp"

enum Order_manager_message_types {
  MSG_TYPE_ORDER_MANAGER_REPORT_OUT=ACE_Message_Block::MB_USER+1000
};


class Order_manager_impl
  : ACE_Task <ACE_MT_SYNCH> {
public:
  Order_manager_impl();
  virtual int open(void* = 0);
  virtual int handle_output(
    ACE_HANDLE=ACE_INVALID_HANDLE);
  virtual int put(
    ACE_Message_Block *mb,
    ACE_Time_Value *tv=0);
  virtual void send_order(
    CRomCsv &order,
    Order_event_callback *callback,
    const char *parent);
  virtual bool change_order(CRomCsv&);
  virtual bool cancel_order(CRomCsv&);
  virtual bool cancel_order(const char *);
  virtual CRomCsv *find_order(const char*);
  void cancel_all_suborders(
    const char*,
    Parent_order_event_callback*);
private:
  ACE_Reactor reactor_;
  ACE_Reactor_Notification_Strategy notifier_;

  struct Order_details {
    Order_details()
      : order(0),
        callback(0),
        cumulative_volume(0),
        parent_id(),
        status_(ROMVALUE::STATUS_PENDINGNEW)
    {}

    ~Order_details() {
      delete order;
    }

    bool cancel() {
      if (!this->out()) {
        CRomCsv *cancel_message=new CRomCsv;
        cancel_message->SetAt(RFIX::CSV_COMMAND,ROMVALUE::COMMAND_CANCEL);
        cancel_message->SetAt(RFIX::CSV_TAG,this->order->Get(RFIX::CSV_TAG));
        const bool send_successful=
          dart::ALGO_MANAGER::instance()->outbound_connection()->send
          (*(cancel_message));
        if (send_successful) {
          this->messages_.push_back(cancel_message);
          this->status(ROMVALUE::STATUS_PENDINGCANCEL);
          this->cancel_sent_=ACE_OS::gettimeofday();
        }
        return send_successful;
      }
      return false;
    }

    void status(int new_status) {
      this->status_=new_status;
    }

    // Is this order out of the market?
    bool out() const {
      switch (this->status_) {
      case ROMVALUE::STATUS_CANCELED:
      case ROMVALUE::STATUS_FILLED:
        return true;
      default:
        return false;
      }
    }

    CRomCsv *order;
    Order_event_callback *callback;
    size_t cumulative_volume;
    std::string parent_id;
    typedef std::vector <CRomCsv*> Messages;
    Messages messages_;
    ACE_Time_Value cancel_sent_;
  private:
    int status_; // Uses ROM CSV status enumerations
  };

  typedef std::map <std::string, Order_details*> Orders_By_ID;
  Orders_By_ID orders_;

  typedef std::multimap <std::string,std::string> Parent_Child_Mappings;
  Parent_Child_Mappings parent_child_mappings_;

  typedef std::map<std::string, Parent_order_event_callback*> Parent_Callbacks;
  Parent_Callbacks parent_callbacks_;

  bullseye::Mutex mutex_;
};

Order_manager_impl::Order_manager_impl()
: reactor_(), notifier_(0, this, ACE_Event_Handler::WRITE_MASK)
{
}

int Order_manager_impl::open(void*) {
  bullseye::Guard guard(this->mutex_);
  this->reactor(ACE_Reactor::instance());
  this->notifier_.reactor(this->reactor());
  this->msg_queue()->notification_strategy(&this->notifier_);
  return 0;
}

int Order_manager_impl::handle_output(ACE_HANDLE) {
  bullseye::Guard guard(this->mutex_);
  ACE_Message_Block *mb = 0;
  if (this->getq (mb)==-1) {
    DART_WARNING("Queue get failed: %p");
    return -1;
  }
  ACE_ASSERT(mb);

  std::string rawmsg(mb->rd_ptr(), mb->length());
  CRomCsv msg(rawmsg);

  if (mb->msg_type()==MSG_TYPE_ORDER_MANAGER_REPORT_OUT) {
    // Find the callback
    const std::string parent_id(mb->rd_ptr());
    Parent_Callbacks::const_iterator callback_iterator=
      this->parent_callbacks_.find(parent_id);
    if (callback_iterator!=this->parent_callbacks_.end()
        && callback_iterator->second)
      callback_iterator->second->all_out(parent_id.c_str());
  }

  const std::string command(msg.Get(RFIX::CSV_COMMAND));

  if (command.empty()) {
    DART_WARNING(
      "No command specified: {|%.*C|}\n",
      mb->length(),
      mb->rd_ptr());
    return 0;
  }

  if (command.size()>1) {
    DART_WARNING(
      "Invalid command: %s\n",
      command.c_str());
    return 0;
  }

  // Find our book keeping information for this
  const std::string id=msg.Get(RFIX::CSV_TAG);
  Orders_By_ID::iterator ord_info_iterator=this->orders_.find(id);
  if (ord_info_iterator==this->orders_.end()) {
    DART_WARNING("Cannot find order information for order %s\n",id.c_str());
    return 0;
  }

  switch (command.at(0))
  {
  case ROMVALUE::COMMAND_STATUS:
    {
      const std::string ordstatstr=msg.Get(RFIX::CSV_STATUS);
      int ordstat=-1;
      std::istringstream str_to_int(ordstatstr);

      if (str_to_int>>ordstat) {
        switch (ordstat) {
        case ROMVALUE::STATUS_OPEN:
          ord_info_iterator->second->callback->on_accept(id.c_str());
          break;
        case ROMVALUE::STATUS_ORDERREJECTED:
          {
            // Is there a reason?
            Order_reject reject;
            reject.order_id=id;
            reject.reason=msg.Get(RFIX::CSV_TEXT);
            ord_info_iterator->second->callback->on_reject(reject);
          }
          break;
        case ROMVALUE::STATUS_PARTIALLYFILLED:
        case ROMVALUE::STATUS_FILLED:
          {
            Order_execution execution;
            execution.order_id=id;
            execution.price=msg.dGet(RFIX::CSV_EXEC_PRICE);
            execution.qty=msg.lGet(RFIX::CSV_LAST_SHARES);
            ord_info_iterator->second->cumulative_volume+=execution.qty;
            execution.cum_qty=ord_info_iterator->second->cumulative_volume;
            ord_info_iterator->second->callback->on_fill(execution);
            ord_info_iterator->second->status(ordstat);
          }
          break;
        case ROMVALUE::STATUS_CANCELED:
          {
            // If this order is canceled due to a cancel operation pending (a
            // status of STATUS_PENDINGCANCEL), check to see if all of the
            // other parent's child orders have been canceled and, if so, send
            // an "all out".
            Order_details *details=ord_info_iterator->second;
            ord_info_iterator->second->status(ordstat);
            bool all_out(true);
            Parent_Child_Mappings::const_iterator child_iterator=
              this->parent_child_mappings_.lower_bound(details->parent_id);
            Parent_Child_Mappings::const_iterator end_iterator=
              this->parent_child_mappings_.upper_bound(details->parent_id);
            for (;child_iterator!=end_iterator;++child_iterator) {
              Orders_By_ID::const_iterator order_iterator=
                this->orders_.find(child_iterator->second);
              if (order_iterator!=this->orders_.end()) {
                if (!order_iterator->second->out())
                  all_out=false;
              }
              else {
                DART_ERROR(
                  "Logical error! Child (%s) of parent (%s) not found in detail map\n",
                  child_iterator->second.c_str(),
                  details->parent_id);
              }
            }

            if (all_out) {
              Parent_Callbacks::const_iterator callback_iterator=
                this->parent_callbacks_.find(details->parent_id);
              if (callback_iterator!=this->parent_callbacks_.end()
                  && callback_iterator->second)
                callback_iterator->second->all_out(details->parent_id.c_str());
            }
          }
          break;
        case ROMVALUE::STATUS_CANCELREJECTED:
          break;
        default:
          DART_WARNING("Unknown order status %d\n",ordstat);
        }
      }
      else {
        DART_WARNING("Non-numeric orders status: %s\n",ordstatstr.c_str());
        return 0;
      }
    }
    break;
  default:
    break;
  }

  return -1;
}

int Order_manager_impl::put(ACE_Message_Block *mb, ACE_Time_Value *tv) {
  bullseye::Guard guard(this->mutex_);
  return this->putq(mb,tv);
}

void Order_manager_impl::send_order(CRomCsv &order,
                                    Order_event_callback *callback,
                                    const char *parent)
{
  bullseye::Guard guard(this->mutex_);
  if (parent==0)
    throw std::invalid_argument("Null parent ptr");

  const std::string order_id=order.Get(RFIX::CSV_TAG);
  if (order_id.empty())
    throw std::invalid_argument("No order tag provided");

  if (callback==0)
    throw std::invalid_argument("No callback provided");

  Orders_By_ID::iterator order_info_iterator=this->orders_.find(order_id);
  if (order_info_iterator!=this->orders_.end())
    throw std::runtime_error("Duplicate order ID");

  if (!dart::ALGO_MANAGER::instance()->outbound_connection()->send(order))
    throw std::runtime_error("Send order failed");

  Order_details *details=new Order_details;
  details->order=new CRomCsv(order.Message());
  details->callback=callback;
  details->parent_id=parent;

  std::pair <Orders_By_ID::iterator,bool> result=
    this->orders_.insert(std::make_pair(order_id,details));

  if (!result.second) {
    delete details;
    throw std::runtime_error("Map insertion failed");
  }

  if (dart::ORDER_BOOK::instance()->register_callback(order_id,this) == -1) {
    this->orders_.erase(result.first);
    delete details;
    throw std::runtime_error("Could not register callback");
  }

  this->parent_child_mappings_.insert
    (std::make_pair(std::string(parent),order_id));
}

bool Order_manager_impl::change_order(CRomCsv&) {
  bullseye::Guard guard(this->mutex_);
  return false;
}

bool Order_manager_impl::cancel_order(CRomCsv&) {
//bullseye::Guard guard(this->mutex_);
  return false;
}

bool Order_manager_impl::cancel_order(const char *order_id) {
  bullseye::Guard guard(this->mutex_);
  bool result=false;
  // Find the order object for this id
  Orders_By_ID::const_iterator detail_iterator=this->orders_.find(order_id);
  if (detail_iterator!=this->orders_.end()
      && detail_iterator->second->cancel()) {
    result=true;
  }
  return result;
}

CRomCsv *Order_manager_impl::find_order(const char*) {
  bullseye::Guard guard(this->mutex_);
  return 0;
}

void Order_manager_impl::cancel_all_suborders(
  const char *parent,
  Parent_order_event_callback *callback)
{
  bullseye::Guard guard(this->mutex_);

  bool all_out=true;

  // Store away the callback pointer for later
  if (this->parent_callbacks_.insert
        (std::make_pair(std::string(parent),callback)).second==false)
    throw std::runtime_error("Callback for parent already exists");

  // Find which suborders are still live
  Parent_Child_Mappings::const_iterator mapping_iterator=
    this->parent_child_mappings_.lower_bound(parent);
  Parent_Child_Mappings::const_iterator upper_mapping_iterator=
    this->parent_child_mappings_.upper_bound(parent);
  for ( ; mapping_iterator!=upper_mapping_iterator; ++mapping_iterator) {
    const std::string &order_id=mapping_iterator->second;
    Orders_By_ID::const_iterator detail_iterator=this->orders_.find(order_id);
    if (detail_iterator==this->orders_.end()) {
      DART_ERROR(
        "Serious problem, data structure out of whack: line %d, %s\n",
        __LINE__,__FILE__);
    }
    else if (!detail_iterator->second->out()) {
      all_out=false;
      detail_iterator->second->cancel();
    }
  }

  if (all_out) {
    ACE_Message_Block *mb=new ACE_Message_Block
      (strlen(parent)+1,MSG_TYPE_ORDER_MANAGER_REPORT_OUT);
    if (mb->copy(parent)==-1)
      DART_WARNING("Could not copy string to message block\n");
    else if (this->put(mb)==-1)
      DART_WARNING("Could not put message block\n");
  }
}

Order_manager::Order_manager() : pimpl(new Order_manager_impl) {
  this->pimpl->open();
}

Order_manager::~Order_manager() {
  delete this->pimpl;
}

void Order_manager::send_order(CRomCsv &order,
                               Order_event_callback *callback,
                               const char *parent)
{
  return this->pimpl->send_order(order,callback,parent);
}

bool Order_manager::change_order(CRomCsv &order) {
  return this->pimpl->change_order(order);
}

bool Order_manager::cancel_order(CRomCsv &order) {
  return this->pimpl->cancel_order(order);
}

bool Order_manager::cancel_order(const char *order_id) {
  return this->pimpl->cancel_order(order_id);
}

CRomCsv *Order_manager::find_order(const char *id) {
  return this->pimpl->find_order(id);
}

void Order_manager::cancel_all_suborders(
    const char *parent_order_id,
    Parent_order_event_callback *callback)
{
  this->pimpl->cancel_all_suborders(parent_order_id,callback);
}
