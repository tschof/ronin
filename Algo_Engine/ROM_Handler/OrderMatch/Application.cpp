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
#include <Session.h>
#include "TibMsg.h"
#include "TibErr.h"

Application::Application( HINSTANCE hInstance, const std::string& service, const std::string& network, 
	const std::string& daemon )
	: m_rvThread( hInstance, service, network, daemon ), m_msgCount(0)
{
	m_rvThread.Start();
}

Application::~Application()
{
	for( std::map<std::string,RvListener*>::const_iterator i=m_listeners.begin(); i!=m_listeners.end(); ++i )
		delete i->second;

	m_rvThread.Stop();
	if( !m_rvThread.Wait(10000) )
		m_rvThread.Kill();
}

void Application::onData( const char* pSubject, RvSender* replySender, const RvDatum& data, RvListener* pListener )
{
	try
	{
		TibMsg tibMsg;
		if( tibMsg.UnPack(static_cast<char*>(data.data())) != TIBMSG_OK )
			return;
		
		std::string subject(pListener->subject());
		std::string::size_type pos1 = subject.rfind('.');
		std::string::size_type pos2 = subject.rfind('.', pos1-1);

		std::string symbol = subject.substr(pos2+1, pos1-pos2-1);

		double bidPrice;
		if( tibMsg.Get("BID", bidPrice) != TIBMSG_OK )
			return;
		
		Tib_i32 bidSize;
		if( tibMsg.Get("BIDSIZE", bidSize) != TIBMSG_OK )
			return;

		double askPrice;
		if( tibMsg.Get("ASK", askPrice) != TIBMSG_OK )
			return;

		Tib_i32 askSize;
		if( tibMsg.Get("ASKSIZE", askSize) != TIBMSG_OK )
			return;

		FIX::Locker lock(m_mutex);
		
//		std::string clientId = "DATAFEED";
//		if( m_msgCount > 0 )
		{


			try
			{
				Order& buyOrder = m_orderMatcher.find(symbol, Order::buy, MAKET_ID_PRICEFEED);
				buyOrder.cancel();
				m_orderMatcher.erase(buyOrder);
			}
			catch(std::exception&){}
			
			try
			{				
				Order& sellOrder = m_orderMatcher.find(symbol, Order::sell, MAKET_ID_PRICEFEED);
				sellOrder.cancel();
				m_orderMatcher.erase(sellOrder);
			}
			catch(std::exception&){}
		}
		
//		std::stringstream clientId;
//		clientId << "PF" << ++m_msgCount;

		if(bidPrice >= askPrice)
			bidPrice = askPrice - 0.01;

		Order buyOrder( MAKET_ID_PRICEFEED, symbol, "PRICE_FEED", "NONE", Order::buy, Order::limit, bidPrice, bidSize*100 );
		Order sellOrder( MAKET_ID_PRICEFEED, symbol, "PRICE_FEED", "NONE", Order::sell, Order::limit, askPrice, askSize*100 );

		m_orderMatcher.insert(buyOrder);
		m_orderMatcher.insert(sellOrder);
		
		std::queue<Order> orders;
		m_orderMatcher.match(symbol, orders);

		while(orders.size())
		{
		  fillOrder(orders.front());
		  orders.pop();
		}
	}
	catch(...){}	
}

void Application::onLogon( const FIX::SessionID& sessionID )
{
  std::cout << "Logon - " << sessionID << std::endl;
}

void Application::onLogout( const FIX::SessionID& sessionID )
{
  std::cout << "Logout - " << sessionID << std::endl;
}

void Application::fromApp( const FIX::Message& message, 
                           const FIX::SessionID& sessionID ) 
  throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&)
{
  crack(message, sessionID);
}

void Application::onRun() 
{ 
  while(true)
  {
    std::string value;
    std::cin >> value;

    if(value == "#symbols")
    {
	  FIX::Locker lock(m_mutex);
	  m_orderMatcher.display();
    }
	else
    { 
	  FIX::Locker lock(m_mutex);
	  m_orderMatcher.display(value);
	}

    std::cout << std::endl;
  }
}

void Application::onMessage( const FIX40::NewOrderSingle& message, const FIX::SessionID& )
{
  FIX::SenderCompID senderCompID;
  FIX::TargetCompID targetCompID;
  FIX::ClOrdID      clOrdID;
  FIX::Symbol       symbol;
  FIX::Side         side;
  FIX::OrdType      ordType;
  FIX::Price        price = 0.0;
  FIX::OrderQty     orderQty;
  FIX::TimeInForce  timeInForce(FIX::TimeInForce_DAY);

  message.getHeader().get(senderCompID);
  message.getHeader().get(targetCompID);
  message.get(clOrdID);
  message.get(symbol);
  message.get(side);
  message.get(ordType);
  if(ordType == FIX::OrdType_LIMIT)
    message.get(price);
  message.get(orderQty);
  if(message.isSetField(timeInForce))
    message.get(timeInForce);

  try
  {
    if(timeInForce != FIX::TimeInForce_DAY)
      throw std::logic_error("Unsupported TIF, use Day");

    Order order(clOrdID, symbol, senderCompID, targetCompID,
                convert(side), convert(ordType),
                price, orderQty );

    processOrder(order);
  }
  catch(std::exception& e)
  {
    rejectOrder(senderCompID, targetCompID, clOrdID, symbol, side, e.what());
  }
}

void Application::onMessage( const FIX40::OrderCancelRequest& message, const FIX::SessionID& )
{
  FIX::OrigClOrdID origClOrdID;
  FIX::Symbol  symbol;
  FIX::Side    side;

  message.get(origClOrdID);
  message.get(symbol);
  message.get(side);

  try
  {
    processCancel(origClOrdID, symbol, convert(side));
  }
  catch(std::exception&) {}
}

void Application::onMessage( const FIX42::NewOrderSingle& message, const FIX::SessionID& )
{
  FIX::SenderCompID senderCompID;
  FIX::TargetCompID targetCompID;
  FIX::ClOrdID      clOrdID;
  FIX::Symbol       symbol;
  FIX::Side         side;
  FIX::OrdType      ordType;
  FIX::Price        price = 0.0;
  FIX::OrderQty     orderQty;
  FIX::TimeInForce  timeInForce(FIX::TimeInForce_DAY);

  message.getHeader().get(senderCompID);
  message.getHeader().get(targetCompID);
  message.get(clOrdID);
  message.get(symbol);
  message.get(side);
  message.get(ordType);
  if(ordType == FIX::OrdType_LIMIT)
    message.get(price);
  message.get(orderQty);
  if(message.isSetField(timeInForce))
    message.get(timeInForce);

  try
  {
    if(timeInForce != FIX::TimeInForce_DAY)
      throw std::logic_error("Unsupported TIF, use Day");

    Order order(clOrdID, symbol, senderCompID, targetCompID,
                convert(side), convert(ordType),
                price, orderQty );

    processOrder(order);
  }
  catch(std::exception& e)
  {
    rejectOrder(senderCompID, targetCompID, clOrdID, symbol, side, e.what());
  }
}

void Application::onMessage( const FIX42::OrderCancelRequest& message, const FIX::SessionID& )
{
  FIX::OrigClOrdID origClOrdID;
  FIX::Symbol  symbol;
  FIX::Side    side;

  message.get(origClOrdID);
  message.get(symbol);
  message.get(side);

  try
  {
    processCancel(origClOrdID, symbol, convert(side));
  }
  catch(std::exception&) {}
}


void Application::updateOrder( const Order& order, char status )
{
  FIX::TargetCompID targetCompID(order.getOwner());
  FIX::SenderCompID senderCompID(order.getTarget());

  if( targetCompID == "PRICE_FEED" )
	return;

  FIX40::ExecutionReport fixOrder
  ( FIX::OrderID       ( order.getClientID()          ), 
    FIX::ExecID        ( m_generator.genExecutionID() ),
    FIX::ExecTransType ( FIX::ExecTransType_NEW       ),
    FIX::OrdStatus     ( status                       ), 
    FIX::Symbol        ( order.getSymbol()            ), 
    FIX::Side          ( convert(order.getSide())     ), 
    FIX::OrderQty      ( order.getQuantity()          ),
	FIX::LastShares	   ( order.getLastExecutedQuantity() ),
    FIX::LastPx        ( order.getLastExecutedPrice()  ),
	FIX::CumQty        ( order.getExecutedQuantity()  ),
    FIX::AvgPx         ( order.getAvgExecutedPrice()  ) );

  fixOrder.set(FIX::ClOrdID(order.getClientID()));
 
  try
  { FIX::Session::sendToTarget(fixOrder, senderCompID, targetCompID);
  } catch(FIX::SessionNotFound&) {}
}

void Application::rejectOrder
  (const FIX::SenderCompID& sender, const FIX::TargetCompID& target,
   const FIX::ClOrdID& clOrdID, const FIX::Symbol& symbol, 
   const FIX::Side& side, const std::string& message)
{
  FIX::TargetCompID targetCompID(sender.getValue());
  FIX::SenderCompID senderCompID(target.getValue());

  if( targetCompID == "PRICE_FEED" )
    return;

  FIX40::ExecutionReport fixOrder
  ( FIX::OrderID       ( clOrdID.getValue()           ), 
    FIX::ExecID        ( m_generator.genExecutionID() ),
    FIX::ExecTransType ( FIX::ExecTransType_NEW       ),
    FIX::OrdStatus     ( FIX::ExecType_REJECTED       ), 
    symbol, 
	side, 
	FIX::OrderQty(0), 
	FIX::LastShares(0),
	FIX::LastPx(0.0),
	FIX::CumQty(0), 
	FIX::AvgPx(0));

  fixOrder.set(clOrdID);
  fixOrder.set(FIX::Text(message));

  try
  { FIX::Session::sendToTarget(fixOrder, senderCompID, targetCompID);
  } catch(FIX::SessionNotFound&) {}
}

void Application::processOrder( const Order& order )
{
  FIX::Locker lock(m_mutex);
  if(m_orderMatcher.insert(order))
  {
    acceptOrder(order);

    addListener(order.getSymbol());

	std::queue<Order> orders;
    m_orderMatcher.match(order.getSymbol(), orders);

    while(orders.size())
    {
      fillOrder(orders.front());
      orders.pop();
    }
  }
  else
    rejectOrder(order);
}

void Application::processCancel( const std::string& id, 
                                 const std::string& symbol, Order::Side side )
{
  FIX::Locker lock(m_mutex);
  Order& order = m_orderMatcher.find(symbol, side, id);
  order.cancel();
  cancelOrder(order);
  m_orderMatcher.erase(order);
}

void Application::addListener( const std::string& symbol )
{
  if( !m_rvThread.IsRunning() )
	m_rvThread.Wait(5000);

  if( m_listeners.find(symbol) == m_listeners.end() )
  {
    std::string exchange;

    switch(symbol.size())
	{
	case 1:
	case 2:
	      exchange = "N";
		  break;

	  case 3:
	    if( symbol == "SPY" || symbol == "QQQ" )
		  exchange = "A";
	    else
	      exchange = "N";
		break;
	  case 4:
		if( symbol.substr(0,2) == "ES" && std::string("HMUZ").find(symbol[2]) != std::string::npos 
				&& symbol[3] >= '0' && symbol[3] <= '9' )
			exchange = "NaE";
		else
			exchange = "O";
		break;
	  default:
	    exchange = "O";
    }
	m_listeners[symbol] = m_rvThread.AddListener("RSF.REC." + symbol + "." + exchange, *this);
  }
}

Order::Side Application::convert( const FIX::Side& side )
{
  switch(side)
  {
    case FIX::Side_BUY: return Order::buy;
    case FIX::Side_SELL: return Order::sell;
	case FIX::Side_SELL_SHORT: return Order::sellShort;
    default: throw std::logic_error("Unsupported Side, use buy or sell");
  }
}

Order::Type Application::convert( const FIX::OrdType& ordType )
{
  switch(ordType)
  {
    case FIX::OrdType_LIMIT: return Order::limit;
    case FIX::OrdType_MARKET: return Order::market;
	default: throw std::logic_error("Unsupported Order Type, use limit");
  }
}

FIX::Side Application::convert( Order::Side side )
{
  switch(side)
  {
     case Order::buy: return FIX::Side(FIX::Side_BUY);
     case Order::sell: return FIX::Side(FIX::Side_SELL);
     case Order::sellShort: return FIX::Side(FIX::Side_SELL_SHORT);
	 default: throw std::logic_error("Unsupported Side, use buy or sell");
  }
}

FIX::OrdType Application::convert( Order::Type type )
{
  switch(type)
  {
     case Order::limit: return FIX::OrdType(FIX::OrdType_LIMIT);
     case Order::market: return FIX::OrdType(FIX::OrdType_MARKET);
	 default: throw std::logic_error("Unsupported Order Type, use limit");
  }
}
