#ifndef __ORDER_MANAGEMENT_SYSTEM_HPP__
#define __ORDER_MANAGEMENT_SYSTEM_HPP__

struct Order_reject {
  std::string order_id;
  std::string reason;
};

struct Order_execution {
  std::string order_id;
  double price;
  size_t qty;
  size_t cum_qty;
  std::string exec_time;
};

class Order_event_callback {
public:
  virtual void on_accept(const char *)=0;
  virtual void on_reject(Order_reject&)=0;
  virtual void on_fill(const Order_execution&)=0;
  virtual void on_cancelled(const char *)=0;
};

class Parent_order_event_callback {
public:
  virtual void all_out(const char *)=0;
};

class CRomCsv;
class Order_manager_impl;

class Order_manager {
public:
  Order_manager();
  virtual ~Order_manager();
  virtual void send_order(
    CRomCsv&,
    Order_event_callback*,
    const char *parent);
  virtual bool change_order(CRomCsv&);
  virtual bool cancel_order(CRomCsv&);
  virtual bool cancel_order(const char *);
  virtual CRomCsv *find_order(const char*);
  virtual void cancel_all_suborders(
    const char *parent_order_id,
    Parent_order_event_callback*);
private:
  Order_manager_impl *pimpl;
};

typedef ACE_Singleton<Order_manager,ACE_SYNCH_MUTEX> ORDER_MANAGER;
#endif
