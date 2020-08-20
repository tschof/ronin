#ifndef TRADECLIENT_APPLICATION_H
#define TRADECLIENT_APPLICATION_H

#include "Application.h"
#include "MessageCracker.h"
#include "Values.h"
#include "Messages.h"
#include <queue>


class ClientApplication : 
  public FIX::Application, 
  public FIX::MessageCracker
{
private:
  virtual void onCreate( const FIX::SessionID& ) {}
  virtual void onLogon( const FIX::SessionID& sessionID );
  virtual void onLogout( const FIX::SessionID& sessionID );
  virtual void toAdmin( FIX::Message&, const FIX::SessionID& ) {}
  virtual void toApp( FIX::Message&, const FIX::SessionID& )
    throw(FIX::DoNotSend&);
  virtual void fromAdmin( const FIX::Message&, const FIX::SessionID& )
    throw(FIX::FieldNotFound&, FIX::RejectLogon&) {}
  virtual void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
    throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&);
  void onRun();
  
  virtual void onMessage( const FIX40::ExecutionReport&, const FIX::SessionID& );
  virtual void onMessage( const FIX40::OrderCancelReject&, const FIX::SessionID& );
  virtual void onMessage( const FIX41::ExecutionReport&, const FIX::SessionID& );
  virtual void onMessage( const FIX41::OrderCancelReject&, const FIX::SessionID& );
  virtual void onMessage( const FIX42::ExecutionReport&, const FIX::SessionID& );
  virtual void onMessage( const FIX42::OrderCancelReject&, const FIX::SessionID& );

  void queryEnterOrder(int version);
  void queryCancelOrder(int version);
  void queryReplaceOrder(int version);
  
  FIX40::NewOrderSingle queryNewOrderSingle40();
  FIX41::NewOrderSingle queryNewOrderSingle41();
  FIX42::NewOrderSingle queryNewOrderSingle42();
  FIX40::OrderCancelRequest queryOrderCancelRequest40();
  FIX41::OrderCancelRequest queryOrderCancelRequest41();
  FIX42::OrderCancelRequest queryOrderCancelRequest42();
  FIX40::OrderCancelReplaceRequest queryCancelReplaceRequest40();
  FIX41::OrderCancelReplaceRequest queryCancelReplaceRequest41();
  FIX42::OrderCancelReplaceRequest queryCancelReplaceRequest42();
  
  void queryHeader(FIX::Header& header);
  char queryAction();
  int queryVersion();
  bool queryConfirm(const std::string& query);
  
  FIX::SenderCompID querySenderCompID();
  FIX::TargetCompID queryTargetCompID();
  FIX::TargetSubID queryTargetSubID();
  FIX::ClOrdID queryClOrdID();
  FIX::OrigClOrdID queryOrigClOrdID();
  FIX::Symbol querySymbol();
  FIX::Side querySide();
  FIX::OrderQty queryOrderQty();
  FIX::OrdType queryOrdType();
  FIX::Price queryPrice();
  FIX::StopPx queryStopPx();
  FIX::TimeInForce queryTimeInForce();
  FIX::Account queryAccount();

};

#endif
