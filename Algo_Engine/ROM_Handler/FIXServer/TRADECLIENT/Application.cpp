/* -*- C++ -*- */
/* ====================================================================
* The QuickFIX Software License, Version 1.0
*
* Copyright (c) 2001 ThoughtWorks, Inc.  All rights
* reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*
* 3. The end-user documentation included with the redistribution,
*    if any, must include the following acknowledgment:
*       "This product includes software developed by
*        ThoughtWorks, Inc. (http://www.thoughtworks.com/)."
*    Alternately, this acknowledgment may appear in the software itself,
*    if and wherever such third-party acknowledgments normally appear.
*
* 4. The names "QuickFIX" and "ThoughtWorks, Inc." must
*    not be used to endorse or promote products derived from this
*    software without prior written permission. For written
*    permission, please contact quickfix-users@lists.sourceforge.net.
*
* 5. Products derived from this software may not be called "QuickFIX",
*    nor may "QuickFIX" appear in their name, without prior written
*    permission of ThoughtWorks, Inc.
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED.  IN NO EVENT SHALL THOUGHTWORKS INC OR
* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
* USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
* ====================================================================
*/

#include "Application.h"
#include "quickfix/include/Session.h"
#include <iostream>

void Application::onLogon( const FIX::SessionID& sessionID )
{
  std::cout << std::endl
	    << "Logon - " << sessionID << std::endl << std::endl;
}

void Application::onLogout( const FIX::SessionID& sessionID )
{
  std::cout << std::endl
	    << "Logoff - " << sessionID << std::endl << std::endl;
}

void Application::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
  throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&)
{
  std::cout  << std::endl
    << "IN: " << message.getString() << std::endl << std::endl;

  crack(message, sessionID);
}

void Application::toApp( FIX::Message& message, const FIX::SessionID& sessionID ) 
  throw(FIX::DoNotSend&)
{
  try
  {
    FIX::PossDupFlag possDupFlag;
    message.getHeader().getField(possDupFlag);
    if(possDupFlag) throw FIX::DoNotSend();
  } catch(FIX::FieldNotFound&) {}

  std::cout  << std::endl
    << "OUT: " << message.getString() << std::endl << std::endl;
}

void Application::onMessage
( const FIX40::ExecutionReport&, const FIX::SessionID& ) {}
void Application::onMessage
( const FIX40::OrderCancelReject&, const FIX::SessionID& ) {}
void Application::onMessage
( const FIX41::ExecutionReport&, const FIX::SessionID& ) {}
void Application::onMessage
( const FIX41::OrderCancelReject&, const FIX::SessionID& ) {}


void Application::onMessage( const FIX42::ExecutionReport& execRpt, const FIX::SessionID& sessId ) 
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

void Application::onMessage( const FIX42::OrderCancelReject& execRpt, const FIX::SessionID& sessId )
{

}

void Application::onRun() 
{
  while(true)
  {
    try
    {
      char action = queryAction();
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

void Application::queryEnterOrder(int version)
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

void Application::queryCancelOrder(int version)
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

void Application::queryReplaceOrder(int version)
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

FIX40::NewOrderSingle Application::queryNewOrderSingle40()
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

FIX41::NewOrderSingle Application::queryNewOrderSingle41()
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

FIX42::NewOrderSingle Application::queryNewOrderSingle42()
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

FIX40::OrderCancelRequest Application::queryOrderCancelRequest40()
{
  FIX40::OrderCancelRequest orderCancelRequest(
    queryOrigClOrdID(), queryClOrdID(), FIX::CxlType('F'),
    querySymbol(), querySide(), queryOrderQty() );

  queryHeader(orderCancelRequest.getHeader());
  return orderCancelRequest;
}

FIX41::OrderCancelRequest Application::queryOrderCancelRequest41()
{
  FIX41::OrderCancelRequest orderCancelRequest(
    queryOrigClOrdID(), queryClOrdID(), querySymbol(), querySide() );

  orderCancelRequest.set(queryOrderQty());
  queryHeader(orderCancelRequest.getHeader());
  return orderCancelRequest;
}

FIX42::OrderCancelRequest Application::queryOrderCancelRequest42()
{
  FIX42::OrderCancelRequest orderCancelRequest( queryOrigClOrdID(), 
    queryClOrdID(), querySymbol(), querySide(), FIX::TransactTime() );

  orderCancelRequest.set(queryOrderQty());
  queryHeader(orderCancelRequest.getHeader());
  return orderCancelRequest;
}

FIX40::OrderCancelReplaceRequest Application::queryCancelReplaceRequest40()
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

FIX41::OrderCancelReplaceRequest Application::queryCancelReplaceRequest41()
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

FIX42::OrderCancelReplaceRequest Application::queryCancelReplaceRequest42()
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

void Application::queryHeader(FIX::Header& header)
{
  header.setField(querySenderCompID());
  header.setField(queryTargetCompID());
  
  if(queryConfirm("Use a TargetSubID"))
    header.setField(queryTargetSubID());
} 

char Application::queryAction()
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

int Application::queryVersion()
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

bool Application::queryConfirm(const std::string& query)
{
  std::string value;
  std::cout << std::endl << query << "?: ";
  std::cin >> value;
  return toupper(*value.c_str()) == 'Y';
}

FIX::SenderCompID Application::querySenderCompID()
{
  std::string value;
  std::cout << std::endl << "SenderCompID: ";
  std::cin >> value;
  return FIX::SenderCompID(value);
}

FIX::TargetCompID Application::queryTargetCompID()
{
  std::string value;
  std::cout << std::endl << "TargetCompID: ";
  std::cin >> value;
  return FIX::TargetCompID(value);
}

FIX::TargetSubID Application::queryTargetSubID()
{
  std::string value;
  std::cout << std::endl << "TargetSubID: ";
  std::cin >> value;
  return FIX::TargetSubID(value);
}

FIX::ClOrdID Application::queryClOrdID()
{
  std::string value;
  std::cout << std::endl << "ClOrdID: ";
  std::cin >> value;
  return FIX::ClOrdID(value);
}

FIX::OrigClOrdID Application::queryOrigClOrdID()
{
  std::string value;
  std::cout << std::endl << "OrigClOrdID: ";
  std::cin >> value;
  return FIX::OrigClOrdID(value);
}

FIX::Symbol Application::querySymbol()
{
  std::string value;
  std::cout << std::endl << "Symbol: ";
  std::cin >> value;
  return FIX::Symbol(value);
}

FIX::Side Application::querySide()
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

FIX::OrderQty Application::queryOrderQty()
{
  long value;
  std::cout << std::endl << "OrderQty: ";
  std::cin >> value;
  return FIX::OrderQty(value);
}

FIX::OrdType Application::queryOrdType()
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

FIX::Price Application::queryPrice()
{
  double value;
  std::cout << std::endl << "Price: ";
  std::cin >> value;
  return FIX::Price(value);
}

FIX::StopPx Application::queryStopPx()
{
  double value;
  std::cout << std::endl << "StopPx: ";
  std::cin >> value;
  return FIX::StopPx(value);
}

FIX::TimeInForce Application::queryTimeInForce()
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

FIX::Account Application::queryAccount()
{
  std::cout << std::endl << "Account: ";
  
  char c;
  std::cin.get(c);
  
  std::string value;
  value.resize(50);
  std::cin.getline(const_cast<char*>(value.data()), 50);
  
  return FIX::Account(value);
}
