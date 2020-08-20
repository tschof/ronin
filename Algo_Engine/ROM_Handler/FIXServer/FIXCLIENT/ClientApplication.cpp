#include "stdafx.h"
#include "ClientApplication.h"
#include "Session.h"
#include <iostream>

void ClientApplication::onLogon( const FIX::SessionID& sessionID )
{
}

void ClientApplication::onLogout( const FIX::SessionID& sessionID )
{
}

void ClientApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
  throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&)
{
  crack(message, sessionID);
}

void ClientApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID ) 
  throw(FIX::DoNotSend&)
{
  try
  {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField(possDupFlag);
    if(possDupFlag) throw FIX::DoNotSend();
  } catch(FIX::FieldNotFound&) {}

}

void ClientApplication::onMessage
( const FIX40::ExecutionReport&, const FIX::SessionID& ) {}
void ClientApplication::onMessage
( const FIX40::OrderCancelReject&, const FIX::SessionID& ) {}
void ClientApplication::onMessage
( const FIX41::ExecutionReport&, const FIX::SessionID& ) {}
void ClientApplication::onMessage
( const FIX41::OrderCancelReject&, const FIX::SessionID& ) {}


void ClientApplication::onMessage( const FIX42::ExecutionReport& execRpt, const FIX::SessionID& sessId ) 
{
  
  switch( execRpt.get(FIX::ExecType()) )
  {
    case FIX::ExecType_NEW:
		std::cout << "NEW: ";
		break;
	case FIX::ExecType_PARTIAL_FILL:
		std::cout << "PARTIAL FILL: ";
		break;
	case FIX::ExecType_FILL:
		std::cout << "FILL: ";
		break;
	case FIX::ExecType_DONE_FOR_DAY:
		std::cout << "DONE FOR DAY: ";
		break;
	case FIX::ExecType_CANCELED:
		std::cout << "CANCELED: ";
		break;
	case FIX::ExecType_REPLACE:
		std::cout << "REPLACE: ";
		break;
	case FIX::ExecType_PENDING_CANCEL:
		std::cout << "PENDING CANCEL: ";
		break;
	case FIX::ExecType_STOPPED:
		std::cout << "STOPPED: ";
		break;
	case FIX::ExecType_REJECTED:
		std::cout << "REJECTED: ";
		break;
	case FIX::ExecType_SUSPENDED:
		std::cout << "SUSPENDED: ";
		break;
	case FIX::ExecType_PENDING_NEW:
		std::cout << "PENDING NEW: ";
		break;
	case FIX::ExecType_CALCULATED:
		std::cout << "CALCULATED: ";
		break;
	case FIX::ExecType_EXPIRED:
		std::cout << "EXPIRED: ";
		break;
	case FIX::ExecType_RESTATED:
		std::cout << "RESTATED: ";
		break;
	case FIX::ExecType_PENDING_REPLACE:
		std::cout << "PENDING REPLACE: ";
		break;
  }

  switch( execRpt.get(FIX::Side()) )
  {
	case FIX::Side_BUY:
	  std::cout << "BUY ";
	  break;
	case FIX::Side_SELL:
	  std::cout << "SELL ";
	  break;
	case FIX::Side_SELL_SHORT:
	  std::cout << "SHORT ";
	case FIX::Side_SELL_SHORT_EXEMPT:
	  std::cout << "SHORT (EXEMPT) ";
	  break;
  }

  if( execRpt.isSetField(FIX::OrderQty()) )
	std::cout << execRpt.get(FIX::OrderQty()) << " ";

  std::cout << execRpt.get(FIX::Symbol()) << " ";

  if( execRpt.isSetField(FIX::OrdType()) )
  {
    switch( execRpt.get(FIX::OrdType()) )
	{
	  case FIX::OrdType_MARKET:
	    std::cout << "@ MKT ";
		break;
	  case FIX::OrdType_LIMIT:
		std::cout << "@ " << execRpt.get(FIX::Price()) << " ";
		break;
	  case FIX::OrdType_STOP:
	  case FIX::OrdType_STOP_LIMIT:
	    break;
	}
  }
  
  std::cout << "- ClOrdID: " << execRpt.get(FIX::ClOrdID()) << " ";

  if( execRpt.isSetField(FIX::OrigClOrdID()) )
	std::cout << "(" << execRpt.get(FIX::OrigClOrdID()) << ")  ";
  else
	std::cout << " ";

  std::cout << "OrdStatus: ";
  
  switch( execRpt.get(FIX::OrdStatus()) )
  {
    case FIX::OrdStatus_NEW:
		std::cout << "New  ";
		break;
	case FIX::OrdStatus_PARTIALLY_FILLED:
		std::cout << "Partially Filled  ";
		break;
	case FIX::OrdStatus_FILLED:
		std::cout << "Filled  ";
		break;
	case FIX::OrdStatus_DONE_FOR_DAY:
		std::cout << "Done For Day  ";
		break;
	case FIX::OrdStatus_CANCELED:
		std::cout << "Canceled  ";
		break;
	case FIX::OrdStatus_REPLACED:
		std::cout << "Replace  ";
		break;
	case FIX::OrdStatus_PENDING_CANCEL:
		std::cout << "Pending Cancel  ";
		break;
	case '7':
		std::cout << "Stopped  ";
		break;
	case FIX::OrdStatus_REJECTED:
		std::cout << "Rejected  ";
		break;
	case FIX::OrdStatus_SUSPENDED:
		std::cout << "Suspended  ";
		break;
	case FIX::OrdStatus_PENDING_NEW:
		std::cout << "Pending New  ";
		break;
	case FIX::OrdStatus_CALCULATED:
		std::cout << "Calculated  ";
		break;
	case FIX::OrdStatus_EXPIRED:
		std::cout << "Expired  ";
		break;
	case FIX::OrdStatus_PENDING_REPLACE:
		std::cout << "Pending Replace  ";
		break;
	default:
		std::cout << "???  ";
  }

  std::cout << "AvgPx: " << execRpt.get(FIX::AvgPx()) << "  ";
  std::cout << "CumQty: " << execRpt.get(FIX::CumQty()) << "  ";
  std::cout << "LeavesQty: " << execRpt.get(FIX::LeavesQty()) << "  ";

  std::cout << std::endl << std::endl;
}

void ClientApplication::onMessage( const FIX42::OrderCancelReject& execRpt, const FIX::SessionID& sessId )
{

}

void ClientApplication::onRun() 
{
  while(true)
  {
  	Sleep(3000);
	continue;
    try
    {
      char action = '1'; //queryAction();
      int version = 42;

      if(action == '1')
        queryEnterOrder(version);
      else if(action == '2')
        queryCancelOrder(version);
      else if(action == '3')
        queryReplaceOrder(version);
     } 
     catch(std::exception& e) 
     {
       std::cout << "Message Not Sent: " << e.what() << std::endl;
     }
  }
}

void ClientApplication::queryEnterOrder(int version)
{
  std::cout << "\nNewOrderSingle\n";
  FIX::Message order;

  if(version == 40)
    order = queryNewOrderSingle40();
  else if(version == 41)
    order = queryNewOrderSingle41();
  else if(version == 42)
    order = queryNewOrderSingle42();

  if(queryConfirm("Send order"))
    FIX::Session::sendToTarget(order);
}

void ClientApplication::queryCancelOrder(int version)
{
  std::cout << "\nOrderCancelRequest\n";
  FIX::Message cancel;

  if(version == 40)
    cancel = queryOrderCancelRequest40();
  else if(version == 41)
    cancel = queryOrderCancelRequest41();
  else if(version == 42)
    cancel = queryOrderCancelRequest42();

  if(queryConfirm("Send cancel"))
    FIX::Session::sendToTarget(cancel);
}

void ClientApplication::queryReplaceOrder(int version)
{
  std::cout << "\nCancelReplaceRequest\n";
  FIX::Message replace;

  if(version == 40)
    replace = queryCancelReplaceRequest40();
  else if(version == 41)
    replace = queryCancelReplaceRequest41();
  else if(version == 42)
    replace = queryCancelReplaceRequest42();

  if(queryConfirm("Send replace"))
    FIX::Session::sendToTarget(replace);
}

FIX40::NewOrderSingle ClientApplication::queryNewOrderSingle40()
{
  FIX::OrdType ordType;

  FIX40::NewOrderSingle newOrderSingle(
    queryClOrdID(), FIX::HandlInst('1'), querySymbol(), querySide(),
    queryOrderQty(), ordType = queryOrdType() );

  newOrderSingle.set(queryTimeInForce());
  if(ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
    newOrderSingle.set(queryPrice());
  if(ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
    newOrderSingle.set(queryStopPx());

  if( queryConfirm("Specify Account") )
	newOrderSingle.set(queryAccount());
 
  queryHeader(newOrderSingle.getHeader());
  return newOrderSingle;
}

FIX41::NewOrderSingle ClientApplication::queryNewOrderSingle41()
{
  FIX::OrdType ordType;

  FIX41::NewOrderSingle newOrderSingle(
    queryClOrdID(), FIX::HandlInst('1'), querySymbol(), querySide(),
    ordType = queryOrdType() );

  newOrderSingle.set(queryOrderQty());
  newOrderSingle.set(queryTimeInForce());
  if(ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
    newOrderSingle.set(queryPrice());
  if(ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
    newOrderSingle.set(queryStopPx());

  if( queryConfirm("Specify Account") )
	newOrderSingle.set(queryAccount());
 
  queryHeader(newOrderSingle.getHeader());
  return newOrderSingle;
}

FIX42::NewOrderSingle ClientApplication::queryNewOrderSingle42()
{
  FIX::OrdType ordType;

  FIX42::NewOrderSingle newOrderSingle(
    queryClOrdID(), FIX::HandlInst('1'), querySymbol(), querySide(),
    FIX::TransactTime(), ordType = queryOrdType() );

  newOrderSingle.setField(FIX::IntField(10001, 0));

  newOrderSingle.set(queryOrderQty());
  newOrderSingle.set(queryTimeInForce());
  if(ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT)
    newOrderSingle.set(queryPrice());
  if(ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT)
    newOrderSingle.set(queryStopPx());

  if( queryConfirm("Specify Account") )
	newOrderSingle.set(queryAccount());
 
  queryHeader(newOrderSingle.getHeader());
  return newOrderSingle;
}

FIX40::OrderCancelRequest ClientApplication::queryOrderCancelRequest40()
{
  FIX40::OrderCancelRequest orderCancelRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::CxlType('F'),
    querySymbol(), querySide(), queryOrderQty() );

  queryHeader(orderCancelRequest.getHeader());
  return orderCancelRequest;
}

FIX41::OrderCancelRequest ClientApplication::queryOrderCancelRequest41()
{
  FIX41::OrderCancelRequest orderCancelRequest(
    queryOrigClOrdID(), queryClOrdID(), querySymbol(), querySide() );

  orderCancelRequest.set(queryOrderQty());
  queryHeader(orderCancelRequest.getHeader());
  return orderCancelRequest;
}

FIX42::OrderCancelRequest ClientApplication::queryOrderCancelRequest42()
{
  FIX42::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(), 
    queryClOrdID(), querySymbol(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set(queryOrderQty());
  queryHeader(orderCancelRequest.getHeader());
  return orderCancelRequest;
}

FIX40::OrderCancelReplaceRequest ClientApplication::queryCancelReplaceRequest40()
{
  FIX40::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst('1'),
    querySymbol(), querySide(), queryOrderQty(), queryOrdType() );

  cancelReplaceRequest.set(queryPrice());

  if(queryConfirm("New price"))
    cancelReplaceRequest.set(queryPrice());
  if(queryConfirm("New quantity"))
    cancelReplaceRequest.set(queryOrderQty());

  queryHeader(cancelReplaceRequest.getHeader());
  return cancelReplaceRequest;
}

FIX41::OrderCancelReplaceRequest ClientApplication::queryCancelReplaceRequest41()
{
  FIX41::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst('1'),
    querySymbol(), querySide(), queryOrdType() );

  cancelReplaceRequest.set(queryOrderQty());
  cancelReplaceRequest.set(queryPrice());

  if(queryConfirm("New price"))
    cancelReplaceRequest.set(queryPrice());
  if(queryConfirm("New quantity"))
    cancelReplaceRequest.set(queryOrderQty());

  queryHeader(cancelReplaceRequest.getHeader());
  return cancelReplaceRequest;
}

FIX42::OrderCancelReplaceRequest ClientApplication::queryCancelReplaceRequest42()
{
  FIX42::OrderCancelReplaceRequest cancelReplaceRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::HandlInst('1'),
    querySymbol(), querySide(), FIX::TransactTime(), queryOrdType() );

  cancelReplaceRequest.set(queryOrderQty());
  cancelReplaceRequest.set(queryPrice());

  if(queryConfirm("New price"))
    cancelReplaceRequest.set(queryPrice());
  if(queryConfirm("New quantity"))
    cancelReplaceRequest.set(queryOrderQty());

  queryHeader(cancelReplaceRequest.getHeader());
  return cancelReplaceRequest;
}

void ClientApplication::queryHeader(FIX::Header& header)
{
  header.setField(querySenderCompID());
  header.setField(queryTargetCompID());
  
  if(queryConfirm("Use a TargetSubID"))
    header.setField(queryTargetSubID());
} 

char ClientApplication::queryAction()
{
  char value;
  std::cout << std::endl
    << "1) Enter Order" << std::endl
    << "2) Cancel Order" << std::endl
    << "3) Replace Order" << std::endl
    << "Action: ";
  std::cin >> value;
  switch(value)
  {
    case '1': case '2': case '3': break;
    default: throw std::exception();
  }
  return value;
}

int ClientApplication::queryVersion()
{
  char value;
  std::cout << std::endl
    << "1) FIX.4.0" << std::endl
    << "2) FIX.4.1" << std::endl
    << "3) FIX.4.2" << std::endl
    << "BeginString: ";
  std::cin >> value;
  switch(value)
  {
    case '1': return 40;
    case '2': return 41;
    case '3': return 42;
    default: throw std::exception();
  }
}

bool ClientApplication::queryConfirm(const std::string& query)
{
  std::string value;
  std::cout << std::endl << query << "?: ";
  std::cin >> value;
  return toupper(*value.c_str()) == 'Y';
}

FIX::SenderCompID ClientApplication::querySenderCompID()
{
  std::string value;
  std::cout << std::endl << "SenderCompID: ";
  std::cin >> value;
  return FIX::SenderCompID(value);
}

FIX::TargetCompID ClientApplication::queryTargetCompID()
{
  std::string value;
  std::cout << std::endl << "TargetCompID: ";
  std::cin >> value;
  return FIX::TargetCompID(value);
}

FIX::TargetSubID ClientApplication::queryTargetSubID()
{
  std::string value;
  std::cout << std::endl << "TargetSubID: ";
  std::cin >> value;
  return FIX::TargetSubID(value);
}

FIX::ClOrdID ClientApplication::queryClOrdID()
{
  std::string value;
  std::cout << std::endl << "ClOrdID: ";
  std::cin >> value;
  return FIX::ClOrdID(value);
}

FIX::OrigClOrdID ClientApplication::queryOrigClOrdID()
{
  std::string value;
  std::cout << std::endl << "OrigClOrdID: ";
  std::cin >> value;
  return FIX::OrigClOrdID(value);
}

FIX::Symbol ClientApplication::querySymbol()
{
  std::string value;
  std::cout << std::endl << "Symbol: ";
  std::cin >> value;
  return FIX::Symbol(value);
}

FIX::Side ClientApplication::querySide()
{
  char value;
  std::cout << std::endl 
    << "1) Buy" << std::endl
    << "2) Sell" << std::endl
    << "3) Sell Short" << std::endl
    << "4) Sell Short Exempt" << std::endl
    << "5) Cross" << std::endl
    << "6) Cross Short" << std::endl
    << "7) Cross Short Exempt" << std::endl
    << "Side: ";

  std::cin >> value;
  switch(value)
  {
  case '1': return FIX::Side(FIX::Side_BUY);
  case '2': return FIX::Side(FIX::Side_SELL);
  case '3': return FIX::Side(FIX::Side_SELL_SHORT);
  case '4': return FIX::Side(FIX::Side_SELL_SHORT_EXEMPT);
  case '5': return FIX::Side(FIX::Side_CROSS);
  case '6': return FIX::Side(FIX::Side_CROSS_SHORT);
  case '7': return FIX::Side('A');
  default: throw std::exception();
  }
}

FIX::OrderQty ClientApplication::queryOrderQty()
{
  long value;
  std::cout << std::endl << "OrderQty: ";
  std::cin >> value;
  return FIX::OrderQty(value);
}

FIX::OrdType ClientApplication::queryOrdType()
{
  char value;
  std::cout << std::endl 
    << "1) Market" << std::endl
    << "2) Limit" << std::endl
    << "3) Stop" << std::endl
    << "4) Stop Limit" << std::endl
    << "OrdType: ";

  std::cin >> value;
  switch(value)
  {
  case '1': return FIX::OrdType(FIX::OrdType_MARKET);
  case '2': return FIX::OrdType(FIX::OrdType_LIMIT);
  case '3': return FIX::OrdType(FIX::OrdType_STOP);
  case '4': return FIX::OrdType(FIX::OrdType_STOP_LIMIT);
  default: throw std::exception();
  }
}

FIX::Price ClientApplication::queryPrice()
{
  double value;
  std::cout << std::endl << "Price: ";
  std::cin >> value;
  return FIX::Price(value);
}

FIX::StopPx ClientApplication::queryStopPx()
{
  double value;
  std::cout << std::endl << "StopPx: ";
  std::cin >> value;
  return FIX::StopPx(value);
}

FIX::TimeInForce ClientApplication::queryTimeInForce()
{
  char value;
  std::cout << std::endl 
    << "1) Day" << std::endl
    << "2) IOC" << std::endl
    << "3) OPG" << std::endl
    << "4) GTC" << std::endl
    << "5) GTX" << std::endl
    << "TimeInForce: ";

  std::cin >> value;
  switch(value)
  {
  case '1': return FIX::TimeInForce(FIX::TimeInForce_DAY);
  case '2': return FIX::TimeInForce(FIX::TimeInForce_IMMEDIATE_OR_CANCEL);
  case '3': return FIX::TimeInForce(FIX::TimeInForce_AT_THE_OPENING);
  case '4': return FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL);
  case '5': return FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CROSSING);
  default: throw std::exception();
  }
}

FIX::Account ClientApplication::queryAccount()
{
  std::cout << std::endl << "Account: ";
  
  char c;
  std::cin.get(c);
  
  std::string value;
  value.resize(50);
  std::cin.getline(const_cast<char*>(value.data()), 50);
  
  return FIX::Account(value);
}
