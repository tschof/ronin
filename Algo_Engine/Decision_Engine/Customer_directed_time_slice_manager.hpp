#ifndef __CUSTOMER_DIRECTED_TIME_SLICE_MANAGER_HPP__
#define __CUSTOMER_DIRECTED_TIME_SLICE_MANAGER_HPP__
// ACE
#include "ACE/OS.h"
#include "ACE/Reactor.h"
#include "ACE/Reactor_Notification_Strategy.h"
#include "ACE/Task.h"
// Application
#include "Bullseye/Bullseye_Common.hpp"
// Std C++
#include <string>
// ROM
#include "ROM_Handler/RomCsv.h"
// Decision Engine
#include "Interval.hpp"
#include "Time_slice_manager.hpp"
#include "Order_management_system.hpp"

class Customer_directed_time_slice_manager
  : public Time_slice_manager
  , public ACE_Task<ACE_MT_SYNCH>
  , public Order_event_callback
{
public:
  Customer_directed_time_slice_manager();

  // Time_slice_manager overrides
  virtual void time_slice(const Interval &interval);
  virtual void volume(size_t volume);
  virtual void customer_order(CRomCsv &order);
  virtual void decision_engine(dart::TWAP_Decision_Engine*);
  virtual bool init();
  virtual bool start();
  virtual bool abort();
  virtual bool isdone() const;
  virtual const char *lasterror() const;

  // ACE_Task overrrides
  virtual int open(void* =0);
  virtual int put(ACE_Message_Block*,ACE_Time_Value* =0);
  virtual int handle_output(ACE_HANDLE=ACE_INVALID_HANDLE);
  virtual int handle_timeout(const ACE_Time_Value&, const void* =0);

  // Order_event_callback overrides
  virtual void on_accept(const char *);
  virtual void on_reject(Order_reject&);
  virtual void on_fill(const Order_execution&);
  virtual void on_cancelled(const char *);

protected:
  virtual bool create_and_send_order(CRomCsv&);

  // Methods copied from TWAP_Time_Slice_Manager
  virtual void process_fill (ACE_Message_Block*,CRomCsv&);
  virtual void cancel_timers();

private:
  Interval interval_;
  size_t volume_;
  std::string symbol_;
  const char *lasterror_;
  ACE_Reactor_Notification_Strategy notifier_;
  CRomCsv *order_;
  CRomCsv *original_order_;
  dart::TWAP_Decision_Engine *parent_;
  long status_;
  size_t shares_filled_;
  long timer_id_;
  mutable bullseye::Mutex mutex_;
};

#endif
