#include "Customer_directed_time_slice_manager.hpp"
#include "Order_Book.hpp"
#include "Algo_Manager.hpp"
#include "ROM_Handler/Session.hpp"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include "TWAP_Decision_Engine.hpp"
#include "Order_management_system.hpp"

Customer_directed_time_slice_manager::Customer_directed_time_slice_manager()
: interval_(),
  volume_(0),
  lasterror_(0),
  notifier_(0,this,ACE_Event_Handler::WRITE_MASK),
  order_(0),
  original_order_(0),
  parent_(0),
  status_(-1),
  shares_filled_(0),
  timer_id_(-1)
{
  DART_INFO("%s\n",__FUNCTION__);
}

void Customer_directed_time_slice_manager::time_slice(
  const Interval &interval)
{
  bullseye::Guard guard(this->mutex_);
  this->interval_=interval;
}

void Customer_directed_time_slice_manager::volume(size_t volume) {
  bullseye::Guard guard(this->mutex_);
  this->volume_=volume;
}

void Customer_directed_time_slice_manager::customer_order(CRomCsv &order) {
  bullseye::Guard guard(this->mutex_);
  this->symbol_=order.Get(RFIX::CSV_SYMBOL);
  if (this->original_order_)
    delete this->original_order_;
  this->original_order_=new CRomCsv(order);
}

void Customer_directed_time_slice_manager::decision_engine(
  dart::TWAP_Decision_Engine *decision_engine)
{
  bullseye::Guard guard(this->mutex_);
  this->parent_=decision_engine;
}

bool Customer_directed_time_slice_manager::init() {
  bullseye::Guard guard(this->mutex_);
  if (interval_.stop<interval_.start)
    lasterror_="stop time before start time";
  else if (interval_.stop==interval_.start)
    lasterror_="start/stop time are the same";
  else if (this->volume_==0)
    lasterror_="Volume is 0";
  else if (this->volume_%100 != 0)
    lasterror_="Volume not a multiple of 100";

  if (lasterror_)
    return false;
  else
    return this->open()==0;
}

bool Customer_directed_time_slice_manager::start() {
  bullseye::Guard guard(this->mutex_);
  if (lasterror_)
    return false;
  else {
    ACE_Time_Value now=ACE_OS::gettimeofday();
    if (this->interval_.stop>now)
      this->timer_id_=this->reactor()->schedule_timer
        (this,0,this->interval_.stop-now);
    if (this->timer_id_==-1)
      return false;
    else
      return create_and_send_order(*(this->original_order_));//Start working
  }
}

bool Customer_directed_time_slice_manager::abort() {
  bullseye::Guard guard(this->mutex_);
  return false;
}

bool Customer_directed_time_slice_manager::isdone() const {
  bullseye::Guard guard(this->mutex_);
  return true;
}

const char* Customer_directed_time_slice_manager::lasterror() const {
  bullseye::Guard guard(this->mutex_);
  if (lasterror_)
    return lasterror_;
  else
    return "No error";
}

int Customer_directed_time_slice_manager::open(void*) {
  bullseye::Guard guard(this->mutex_);
  int result=-1;
  // Use the thread pool reactor that has been setup for us
  this->reactor(ACE_Reactor::instance());
  if (this->reactor()) {
    // Using a ACE queue notifier
    this->notifier_.reactor(this->reactor());
    this->msg_queue()->notification_strategy(&this->notifier_);
    result=0;
  }
  return result;
}

int Customer_directed_time_slice_manager::put(
  ACE_Message_Block *mb,
  ACE_Time_Value *tv)
{
  bullseye::Guard guard(this->mutex_);
  return this->putq(mb,tv);
}

int Customer_directed_time_slice_manager::handle_output(ACE_HANDLE) {
  bullseye::Guard guard(this->mutex_);
  ACE_Message_Block *pmessage = 0;
  if (this->getq (pmessage) == 0 && pmessage && pmessage->length ()) {
    const char *p = pmessage->rd_ptr ();
    const char command (p [0]);
    if (command == ROMVALUE::COMMAND_STATUS) {
      std::string msg (pmessage->rd_ptr (), pmessage->length ());
      CRomCsv status (msg);
      this->status_ = status.lGet (RFIX::CSV_STATUS);
      switch (this->status_) {
          case ROMVALUE::STATUS_OPEN:
            ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Order %s OPEN\n"),
              status.GetAt (RFIX::CSV_TAG).c_str ()));
            break;

          case ROMVALUE::STATUS_FILLED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Order filled\n")));
            this->process_fill (pmessage, status);
            this->cancel_timers ();
            break;

          case ROMVALUE::STATUS_PARTIALLYFILLED:
            this->process_fill (pmessage, status);
            break;

          case ROMVALUE::STATUS_CANCELED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Canceled\n")));
            this->cancel_timers ();
            this->parent_->on_child_order_cancelled ();
            break;

          case ROMVALUE::STATUS_ORDERREJECTED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Order rejected!\n")));
            // With the original order rejected and no reliable way to
            // determine why, we will just stop our timers and make sure no
            // further orders are submitted by this object.
            this->cancel_timers();
            this->parent_->cancel("orders rejected at exchange");
            this->parent_->on_child_order_cancelled();
            break;

          case ROMVALUE::STATUS_REPLACEREJECTED:
            ACE_DEBUG ((LM_DEBUG, DART_LOG ("Replace rejected!\n")));
            this->cancel_timers();
            this->parent_->cancel();
            break;

          case ROMVALUE::STATUS_REPLACED:
            break;

          case ROMVALUE::STATUS_CANCELREJECTED:
            // This is probably the worst of all cases as we have no way
            // to exit the market, except possibly by getting filled.
            // TODO: Flag an operator at this point, this is bad.
            // TODO: This is an appropriate reason to send email.
            DART_CRITICAL
              ("Cancel for order %s rejected!\n",
              this->order_->GetAt(RFIX::CSV_TAG));
            this->cancel_timers ();

          default:
            ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Unknown status: %d\n"),
              this->status_));
            break;
      }
    }
  }
  return 0;
}

int Customer_directed_time_slice_manager::handle_timeout(
  const ACE_Time_Value&,
  const void*)
{
  bullseye::Guard guard(this->mutex_);
  if (this->status_!=ROMVALUE::STATUS_PENDINGCANCEL
      && this->status_!=ROMVALUE::STATUS_CANCELED) {
    const std::string order_id=this->order_->GetAt(RFIX::CSV_TAG);
    if (ORDER_MANAGER::instance()->cancel_order(order_id.c_str())==false)
      DART_WARNING("Cancelling order %s failed\n",order_id.c_str());
  }
  return 0;
}

bool Customer_directed_time_slice_manager::create_and_send_order(
  CRomCsv &customer_order)
{
  bullseye::Guard guard(this->mutex_);
  this->order_=new CRomCsv(customer_order.Message());//Most fields the same
    // Is the time copied as well?
  this->order_->SetAt(RFIX::CSV_COMMAND,ROMVALUE::COMMAND_ORDER);
  this->order_->SetAt(
    RFIX::CSV_TAG,
    dart::ORDER_BOOK::instance()->generate_order_id());
  this->order_->SetAt(
    RFIX::CSV_SHARES,
    static_cast<unsigned long>(this->volume_));
  this->order_->SetAt(RFIX::CSV_STAFFORD_TAG,"");
  this->order_->SetAt(RFIX::CSV_EXCHANGE,this->order_->Get(RFIX::CSV_EXDEST));
  this->order_->SetAt(RFIX::CSV_EXDEST,"");
  this->order_->SetAt(RFIX::CSV_ALGOTYPE,"");
  this->order_->SetAt(RFIX::CSV_START_TIME,"");
  this->order_->SetAt(RFIX::CSV_END_TIME,"");
  this->order_->SetAt(RFIX::CSV_OUTGOING_SEQ,"");

  try {
    ORDER_MANAGER::instance()->send_order(
      *(this->order_),
      this,
      this->original_order_->Get(RFIX::CSV_STAFFORD_TAG));
  }
  catch (const std::exception &e) {
    DART_WARNING ("Could not send order: %s\n", e.what());
  }

  return true;
}

void Customer_directed_time_slice_manager::process_fill(
  ACE_Message_Block *pmessage,
  CRomCsv &fill)
{
  bullseye::Guard guard(this->mutex_);
  const long shares_filled (fill.lGet (RFIX::CSV_EXEC_SHARES));
  assert (shares_filled);
  if (shares_filled) {
    this->shares_filled_ += shares_filled;
    if (this->parent_->put (pmessage) == -1) {
      ACE_DEBUG ((LM_DEBUG,
        DART_LOG ("%s: parent queue put failed\n"),
        __FUNCTION__));
    }
  }
  else {
    ACE_DEBUG ((LM_DEBUG, DART_LOG ("shares filled is 0!\n")));
  }
}

void Customer_directed_time_slice_manager::cancel_timers() {
  bullseye::Guard guard(this->mutex_);
  DART_DEBUG("%s: Cancelling all timers\n",__FUNCTION__);
  if (this->reactor ()->cancel_timer (this) == -1)
    DART_DEBUG("Error canceling timers\n");
}

void Customer_directed_time_slice_manager::on_accept(const char *order_id) {
  DART_INFO("Order %s accepted\n",order_id);
}

void Customer_directed_time_slice_manager::on_reject(Order_reject &reject) {
  DART_INFO("Order %s rejected: %s\n",
    reject.order_id.c_str(),
    reject.reason.c_str());
  this->reactor()->cancel_timer(this);
}

void Customer_directed_time_slice_manager::on_fill(const Order_execution &fill) {
  DART_INFO("Order %s %s filled: %d @ %f\n",
    fill.order_id.c_str(),
    fill.cum_qty==this->volume_ ? "completely" : "partially",
    fill.qty,
    fill.price);
  this->parent_->on_execution(fill);
}

void Customer_directed_time_slice_manager::on_cancelled(const char *order_id) {
  DART_INFO("Order %s cancelled\n",order_id);
}
