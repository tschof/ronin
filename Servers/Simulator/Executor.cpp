#include "Executor.h"
#include "Session.h"

#include "fix40/ExecutionReport.h"
#include "fix41/ExecutionReport.h"
#include "fix42/ExecutionReport.h"
#include "fix43/ExecutionReport.h"
#include "fix44/ExecutionReport.h"
#include "fix42/OrderCancelRequest.h"
#include "fix42/OrderCancelReject.h"
#include "fix42/OrderCancelReplaceRequest.h"
#include "FieldTypes.h"

std::string CExecutor::SPECIALORDER = std::string("X");


CExecutor::CExecutor(bool* end) : m_Orders()
{
    m_orderID = 0;
    m_execID = 0;
    m_End = end;
    //std::list<COrder*> testList;

    //testList.sort(my_compare);
    //std::sort(testList.begin(), testList.end(), my_compare);
}

CExecutor::~CExecutor()
{
    std::map<std::string, CSessionWrap*>::iterator it = m_SessionMap.begin();
    while (it != m_SessionMap.end())
    {
        delete it->second;
        it++;
    }

    if (m_End)
    {
        m_Orders.ZapAllDayOrders();
    }
}

void CExecutor::onCreate( const FIX::SessionID& sessionID ) {}

CSessionWrap* CExecutor::FindSession(const std::string& Login)
{
    CSessionWrap* rc = NULL;

    std::map<std::string, CSessionWrap*>::iterator it = m_SessionMap.find(Login);
    if (it != m_SessionMap.end())
    {
        rc = it->second;
    }

    return rc;
}

void CExecutor::AddSession(const FIX::SessionID& sessionID)
{
    if (!FindSession(sessionID.getTargetCompID().getValue()))
    {
        CSessionWrap* pSession = new CSessionWrap(sessionID);
        AddSession(sessionID.getTargetCompID().getValue(), pSession);
    }
}

void CExecutor::AddSession(const std::string& key, CSessionWrap* pSession)
{
    m_SessionMap.insert(std::make_pair<std::string, CSessionWrap*>(key, pSession));
}

void CExecutor::onLogon( const FIX::SessionID& sessionID )
{
    AddSession(sessionID);
    FIX::Session* pSession = FIX::Session::lookupSession( sessionID );
    if (pSession)
    {
        pSession->setCheckLatency(false);
    }
}

void CExecutor::onLogout( const FIX::SessionID& sessionID ) {}
void CExecutor::toAdmin( FIX::Message& message,
                         const FIX::SessionID& sessionID ) {}
void CExecutor::toApp( FIX::Message& message,
                       const FIX::SessionID& sessionID )
throw( FIX::DoNotSend ) {}

void CExecutor::fromAdmin( const FIX::Message& message,
                           const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {}

void CExecutor::fromApp( const FIX::Message& message,
                         const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
    crack( message, sessionID );
}

/*
void CExecutor::onMessage( const FIX40::NewOrderSingle& message,
                             const FIX::SessionID& sessionID )
{
	FIX::Symbol symbol;
	FIX::Side side;
	FIX::OrdType ordType;
	FIX::OrderQty orderQty;
	FIX::Price price;
	FIX::ClOrdID clOrdID;
	FIX::Account account;

	message.get( ordType );

  //if ( ordType != FIX::OrdType_LIMIT )
   // throw FIX::IncorrectTagValue( ordType.getField() );

	message.get( symbol );
	message.get( side );
	message.get( orderQty );
	message.get( price );
	message.get( clOrdID );

	char status;
	unsigned int ordQty = orderQty.getValue();
  switch (ordQty % 100)
  {
  case 0:
	  //open
	  status = FIX::OrdStatus_NEW;
	  break;
  case 1:
	  //open followed by filled
	  status = FIX::OrdStatus_FILLED;
	  break;
  case 2:
	  //open followed by partially filled
	  break;
  case 3:
	  //reject
	  break;
  }

  FIX40::ExecutionReport executionReport = FIX40::ExecutionReport
      ( FIX::OrderID( genOrderID() ),
        FIX::ExecID( genExecID() ),
        FIX::ExecTransType( FIX::ExecTransType_NEW ),
        FIX::OrdStatus( FIX::OrdStatus_FILLED ),
        symbol,
        side,
        orderQty,
        FIX::LastShares( orderQty ),
        FIX::LastPx( price ),
        FIX::CumQty( orderQty ),
        FIX::AvgPx( price ) );

  executionReport.set( clOrdID );

  if( message.isSet(account) )
    executionReport.setField( message.get(account) );

  try
  {
    FIX::Session::sendToTarget( executionReport, sessionID );
  }
  catch ( FIX::SessionNotFound& ) {}
}

void CExecutor::onMessage( const FIX41::NewOrderSingle& message,
                             const FIX::SessionID& sessionID )
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get( ordType );

  if ( ordType != FIX::OrdType_LIMIT )
    throw FIX::IncorrectTagValue( ordType.getField() );

  message.get( symbol );
  message.get( side );
  message.get( orderQty );
  message.get( price );
  message.get( clOrdID );

  FIX41::ExecutionReport executionReport = FIX41::ExecutionReport
      ( FIX::OrderID( genOrderID() ),
        FIX::ExecID( genExecID() ),
        FIX::ExecTransType( FIX::ExecTransType_NEW ),
        FIX::ExecType( FIX::ExecType_FILL ),
        FIX::OrdStatus( FIX::OrdStatus_FILLED ),
        symbol,
        side,
        orderQty,
        FIX::LastShares( orderQty ),
        FIX::LastPx( price ),
        FIX::LeavesQty( 0 ),
        FIX::CumQty( orderQty ),
        FIX::AvgPx( price ) );

  executionReport.set( clOrdID );

  if( message.isSet(account) )
    executionReport.setField( message.get(account) );

  try
  {
    FIX::Session::sendToTarget( executionReport, sessionID );
  }
  catch ( FIX::SessionNotFound& ) {}
}*/

void CExecutor::SendExecReport(const COrder& order,
                               CSessionWrap& Session,
                               const std::string& ClOrdID,
                               const std::string& OrigClOrdID,
                               char status,
                               unsigned int lastShares,
                               double lastPrice,
                               const std::string& text)
{
    FIX42::ExecutionReport* pExecutionReport = new FIX42::ExecutionReport
    ( FIX::OrderID( order.OrdID() ),
      FIX::ExecID( m_Orders.genExecID() ),
      FIX::ExecTransType( FIX::ExecTransType_NEW ),
      FIX::ExecType( status ),
      FIX::OrdStatus( status ),
      order.Symbol(),
      order.Side(),
      FIX::LeavesQty( order.Leave() ),
      FIX::CumQty( order.Cum()),
      FIX::AvgPx( order.Price() ) );

    pExecutionReport->set( FIX::ClOrdID(ClOrdID));
    if (OrigClOrdID.empty() == false)
    {
        pExecutionReport->set( FIX::OrigClOrdID(OrigClOrdID));
    }
    pExecutionReport->set( FIX::OrderQty(order.OrderQty()));
    pExecutionReport->set( FIX::LastShares(lastShares));
    pExecutionReport->set( FIX::LastPx(lastPrice));
    pExecutionReport->set( FIX::Price(order.Price()));

    FIX::UtcTimeStamp tstamp;

    /*time_t t;
    time(&t);
    struct tm* ptm = gmtime(&t);
    char trsTime[20];
    sprintf(trsTime, "%04u%02u%02u-%02u:%02u:%02u", ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    */
    pExecutionReport->set( FIX::TransactTime(tstamp.getDate()));

    if(order.Account().empty() == false)
    {
        pExecutionReport->set( FIX::Account(order.Account()));
    }

    if (text.empty() == false)
    {
        pExecutionReport->set( FIX::Text(text));
    }

    Session.Send(pExecutionReport);
}

void CExecutor::DoFill42(COrder& order, CSessionWrap& Session)
{
    if (order.Type() == '2')
    {
        SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_FILLED, order.Leave(), order.Price(), "");
    }
    else
    {
        SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_FILLED, order.Leave(), 1, "");
    }
    m_Orders.DestroyOrder(&order);
}

void CExecutor::DoOpen42(COrder& order, CSessionWrap& Session)
{
    order.SetLeave(order.OrderQty());
    SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_NEW, 0, 0, "");
}

bool CExecutor::DoPartialFill42(COrder& order, CSessionWrap& Session, int lastShares)
{
    bool rc = true;
    if (order.Leave() > lastShares)
    {
        order.SetLeave(order.Leave() - lastShares);
        order.SetCum(order.Cum() + lastShares);
        char status = FIX::OrdStatus_PARTIALLY_FILLED;
        SendExecReport(order, Session, order.ClOrdID(), "", status, lastShares, order.Price(), "");
    }

    return rc;
}

void CExecutor::DoPartialFill42(COrder& order, CSessionWrap& Session)
{
    unsigned int lastShares = order.Leave() / 2;
    order.SetLeave(order.Leave() - lastShares);
    order.SetCum(order.Cum() + lastShares);

    char status = FIX::OrdStatus_PARTIALLY_FILLED;

    if (order.Type() == '2')
    {
        SendExecReport(order, Session, order.ClOrdID(), "", status, lastShares, order.Price(), "");
    }
    else
    {
        SendExecReport(order, Session, order.ClOrdID(), "", status, lastShares, 1, "");
    }
}

void CExecutor::DoMarketOrderMatch42(COrder& order, CSessionWrap& Session)
{
    if (m_Orders.FindMatch(&order))
    {
        DoOpen42(order, Session);
        DoMatch42(order);
        if (order.Used()) //leftover should be rejected
        {
            order.SetLeave(0);
            SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_CANCELED, 0, 0, "There is no market");
            m_Orders.DestroyOrder(&order);
        }
    }
    else
    {
        order.SetLeave(0);
        SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_REJECTED, 0, 0, "There is no market");
        m_Orders.DestroyOrder(&order);
    }
}

void CExecutor::DoMatch42(COrder& order)
{
    while (Match42(order))
    {
    }
}

bool CExecutor::Match42(COrder& order)
{
    COrder* pTargetOrder = m_Orders.FindMatch(&order);
    bool rc = false;
    unsigned int lastShares;
    double lastPrice;

    if (pTargetOrder)
    {
        lastPrice = pTargetOrder->Price();

        if (order.Leave() > pTargetOrder->Leave())
        {
            if (order.Tif() == FIX::TimeInForce_FILL_OR_KILL)
            {
                //the order should be OUT
                CSessionWrap* pSession = FindSession(order.Login());

                order.SetLeave(0);

                SendExecReport(order,
                               *pSession,
                               order.ClOrdID(),
                               order.ClOrdID(),
                               FIX::OrdStatus_CANCELED,
                               0,
                               0,
                               "No market for FOK order");

                m_Orders.DestroyOrder(&order);
            }
            else
            {
                //Fill the target and partially fill the source
                lastShares = pTargetOrder->Leave();
                DoNormalFill42(*pTargetOrder, lastShares, lastPrice);
                DoNormalFill42(order, lastShares, lastPrice);
                if (order.Tif() == FIX::TimeInForce_IMMEDIATE_OR_CANCEL)
                {
                    CSessionWrap* pSession = FindSession(order.Login());

                    order.SetLeave(0);

                    SendExecReport(order,
                                   *pSession,
                                   order.ClOrdID(),
                                   order.ClOrdID(),
                                   FIX::OrdStatus_CANCELED,
                                   0,
                                   0,
                                   "No market for remaining shares of IOC order");

                    m_Orders.DestroyOrder(&order);
                }
                rc = true;
            }
        }
        else if (order.Leave() < pTargetOrder->Leave())
        {
            //Fill the source and partially fill the target
            lastShares = order.Leave();
            DoNormalFill42(*pTargetOrder, lastShares, lastPrice);
            DoNormalFill42(order, lastShares, lastPrice);
        }
        else
        {
            //Fill both source and target
            lastShares = pTargetOrder->Leave();
            DoNormalFill42(*pTargetOrder, lastShares, lastPrice);
            DoNormalFill42(order, lastShares, lastPrice);
        }
    }
    else
    {
        if (order.Tif() == FIX::TimeInForce_IMMEDIATE_OR_CANCEL ||
                order.Tif() == FIX::TimeInForce_FILL_OR_KILL)
        {
            //the order should be OUT
            CSessionWrap* pSession = FindSession(order.Login());

            order.SetLeave(0);

            SendExecReport(order,
                           *pSession,
                           order.ClOrdID(),
                           order.ClOrdID(),
                           FIX::OrdStatus_CANCELED,
                           0,
                           0,
                           "No market for FOK order and IOC order");

            m_Orders.DestroyOrder(&order);
        }
    }

    return rc;
}

void CExecutor::DoNormalFill42(COrder& order, unsigned int lastShares, double lastPrice)
{
    order.SetLeave(order.Leave() - lastShares);
    order.SetCum(order.Cum() + lastShares);

    CSessionWrap* pSession = FindSession(order.Login());
    char status = FIX::OrdStatus_FILLED;

    if (order.Leave() == 0)
    {
        SendExecReport(order, *pSession, order.ClOrdID(), "", status, lastShares, lastPrice, "");
        m_Orders.DestroyOrder(&order);
    }
    else
    {
        status = FIX::OrdStatus_PARTIALLY_FILLED;
        SendExecReport(order, *pSession, order.ClOrdID(), "", status, lastShares, lastPrice, "");
    }
}


void CExecutor::DoReject42(COrder& order, CSessionWrap& Session, const std::string& reason)
{
    order.SetLeave(0);
    SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_REJECTED, 0, 0, reason);
    m_Orders.DestroyOrder(&order);
}

void CExecutor::DoOut42(COrder& order, CSessionWrap& Session, const std::string& reason)
{
    order.SetLeave(0);
    SendExecReport(order, Session, order.ClOrdID(), "", FIX::OrdStatus_CANCELED, 0, 0, reason);
    m_Orders.DestroyOrder(&order);
}

void CExecutor::onMessage( const FIX42::NewOrderSingle& message, const FIX::SessionID& sessionID )
{
    COrder* pOrder = m_Orders.GetNewOrder();
    pOrder->Set(message, sessionID.getTargetCompID().getValue());
    pOrder->SetOrdID(genOrderID().c_str());

    m_Orders.Add(pOrder);

    CSessionWrap* pSession = FindSession(sessionID.getTargetCompID().getValue());

    if (pOrder->OrderQty() == 0)
    {
        DoReject42(*pOrder, *pSession, "Order Quantity can not be zero!");
        return;
    }
    
    FIX::SecurityType secType;
    if (message.isSetField(secType))
    {
        message.get(secType);
    }

    FIX::Text text;
    if (message.isSetField(text))
    {
        message.get(text);
        std::string sText = text.getValue();
        if (sText.compare("0") == 0 ||
                sText.compare("1") == 0 ||
                sText.compare("2") == 0 ||
                sText.compare("3") == 0)
        {
            pOrder->SetnText(atoi(sText.c_str()));
        }
    }

    FIX::OrdType ordType;
    message.get( ordType );
    /*if (secType.getValue().compare(FIX::SecurityType_COMMON_STOCK) == 0 &&
            ordType ==  FIX::OrdType_LIMIT)
    {
        std::string strPrice = message.getField( FIX::FIELD::Price );
        int index = strPrice.find('.');
        if (index != std::string::npos &&
                index <= strPrice.size() - 3)
        {
            for (int i = strPrice.size() - index; i < strPrice.size(); i++)
            {
                if (strPrice[i] != '0' && i > index + 2)
                {
                    DoReject42(*pOrder, *pSession, "Price of Equity order can not have more than two decimals");
                    return;
                }
            }
        }
    }*/

    if (pOrder->ExecInst() == CExecutor::SPECIALORDER)
    {
        if (pOrder->Type() == FIX::OrdType_MARKET)
        {
            //pOrder->SetPrice(1);
            DoOpen42(*pOrder, *pSession);
            DoFill42(*pOrder, *pSession);
        }
        else
        {
            int nCase = -1;

            if (pOrder->nText() != -1)
            {
                nCase = pOrder->nText();
            }
            else
            {
                nCase = pOrder->OrderQty() % 10;
            }

            switch (nCase)
            {
            case 0:
                DoOpen42(*pOrder, *pSession);
                break;
            case 1:
                DoOpen42(*pOrder, *pSession);
                DoFill42(*pOrder, *pSession);
                break;
            case 2:
                DoOpen42(*pOrder, *pSession);
                DoPartialFill42(*pOrder, *pSession);
                break;
            case 3:
                DoReject42(*pOrder, *pSession, "The reject you asked");
                break;
            case 4:
                DoOut42(*pOrder, *pSession, "The Out you asked");
                break;
            default:
                DoOpen42(*pOrder, *pSession);
                break;
            }
        }
    }
    else
    {
        if (pOrder->Type() == FIX::OrdType_MARKET)
        {
            DoMarketOrderMatch42(*pOrder, *pSession);
        }
        else
        {
            m_Orders.AddToBook(pOrder);
            DoOpen42(*pOrder, *pSession);
            DoMatch42(*pOrder);
        }
    }
}

void CExecutor::onMessage( const FIX42::OrderCancelRequest& message, const FIX::SessionID& sessionID)
{
    FIX::OrigClOrdID origClOrdID;
    message.get(origClOrdID);
    //FIX::OrderID OrdID;
    //message.get(OrdID);
    FIX::ClOrdID clOrdID;
    message.get(clOrdID);

    std::string orderID = sessionID.getTargetCompID().getValue() + origClOrdID.getValue();
    COrder* pOrder = m_Orders.FindOrderFromClOrdID(orderID);
    if (!pOrder)
    {
        //pOrder = m_Orders.FindOrderFromOrdID(OrdID.getValue());
    }

    CSessionWrap* pSession = FindSession(sessionID.getTargetCompID().getValue());

    if (pOrder)
    {
        pOrder->SetLeave(0);

        SendExecReport(*pOrder,
                       *pSession,
                       clOrdID.getValue(),
                       origClOrdID.getValue(),
                       FIX::OrdStatus_CANCELED,
                       0,
                       0,
                       "");

        m_Orders.DestroyOrder(pOrder);
    }
    else
    {
        FIX::OrderID OrdID;
        FIX42::OrderCancelReject* pReject = new FIX42::OrderCancelReject(OrdID,
                clOrdID,
                origClOrdID,
                FIX::OrdStatus(FIX::OrdStatus_REJECTED),
                FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REQUEST));

        pSession->Send(pReject);
    }
}

void CExecutor::onMessage( const FIX42::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID)
{
    FIX::OrigClOrdID origClOrdID;
    message.get(origClOrdID);
    FIX::ClOrdID clOrdID;
    FIX::ExecInst execInst;
    message.get(clOrdID);
    FIX::OrderQty orderQty;
    message.get(orderQty);
    std::string execInstStr;
    if (message.isSetField(execInst))
    {
        message.get(execInst);
        execInstStr = execInst.getValue();
    }

    std::string oldOrderID = sessionID.getTargetCompID().getValue() + origClOrdID.getValue();
    COrder* pOrder = m_Orders.FindOrderFromClOrdID(oldOrderID);

    CSessionWrap* pSession = FindSession(sessionID.getTargetCompID().getValue());

    if (pOrder)
    {
        int delt = pOrder->OrderQty() - orderQty.getValue();
        pOrder->SetLeave(pOrder->Leave() - delt);
        pOrder->SetOrderQty(orderQty.getValue());

        bool doReplace = true;

        if ( execInstStr == "A")
        {
            DoFill42(*pOrder, *pSession);
            // do replace reject
            FIX::OrderID OrdID;
            FIX42::OrderCancelReject* pReject = new FIX42::OrderCancelReject(OrdID,
                    clOrdID,
                    origClOrdID,
                    FIX::OrdStatus(FIX::OrdStatus_REJECTED),
                    FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));

            pSession->Send(pReject);
            doReplace = false;
        }
        else if ( execInstStr == "B")
        {
            DoPartialFill42(*pOrder, *pSession, 50);
        }
        else if (execInstStr == "C")
        {
            DoPartialFill42(*pOrder, *pSession, 100);
        }
        else if (execInstStr == "D")
        {
            DoPartialFill42(*pOrder, *pSession, 250);
        }
        else if (execInstStr == "E")
        {
            DoPartialFill42(*pOrder, *pSession, 500);
        }

        FIX::OrdType type;
        if (message.isSetField(type))
        {
            message.get(type);
            if (type.getValue() == FIX::OrdType_MARKET)
            {
                pOrder->SetType(FIX::OrdType_MARKET);
                if (pOrder->ExecInst() != CExecutor::SPECIALORDER)
                {
                    SendExecReport(*pOrder,
                           *pSession,
                           clOrdID.getValue(),
                           origClOrdID.getValue(),
                           FIX::OrdStatus_REPLACED,
                           0,
                           0,
                           "");

                    pOrder->SetClOrdID(clOrdID.getValue().c_str());
                    std::string newOrderID = sessionID.getTargetCompID().getValue() + pOrder->ClOrdID();
                    m_Orders.ReplaceClOrdID(oldOrderID, newOrderID);
                    DoMarketOrderMatch42(*pOrder, *pSession);
                    return;
                }
            }
        }

        if (pOrder->Type() == FIX::OrdType_LIMIT)
        {
            FIX::Price price;
            message.get(price);
            pOrder->SetPrice(price.getValue());
            if (price.getValue() != pOrder->Price() &&
                    pOrder->ExecInst() != CExecutor::SPECIALORDER)
            {
                m_Orders.ResortBook(pOrder->Login(), pOrder->Symbol(), pOrder->Side());
            }
            //pOrder->SetPrice(price.getValue());
        }

        if (doReplace)
        {
            SendExecReport(*pOrder,
                           *pSession,
                           clOrdID.getValue(),
                           origClOrdID.getValue(),
                           FIX::OrdStatus_REPLACED,
                           0,
                           0,
                           "");

            pOrder->SetClOrdID(clOrdID.getValue().c_str());
            std::string newOrderID = sessionID.getTargetCompID().getValue() + pOrder->ClOrdID();
            m_Orders.ReplaceClOrdID(oldOrderID, newOrderID);
        }

        if (pOrder->Leave() > 0 &&
                (execInstStr != "A" &&
                 execInstStr != "B" &&
                 execInstStr != "C" &&
                 execInstStr != "D" &&
                 execInstStr != "E"))
        {
            if (pOrder->ExecInst() == CExecutor::SPECIALORDER)
            {
                if (delt != 0)
                {
                    switch(pOrder->OrderQty() % 10)
                    {
                    case 1:
                        DoFill42(*pOrder, *pSession);
                        break;
                    case 2:
                        DoPartialFill42(*pOrder, *pSession);
                        break;
                    case 3:
                        DoReject42(*pOrder, *pSession, "The reject you asked");
                        break;
                    case 4:
                        DoOut42(*pOrder, *pSession, "The Out you asked");
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                DoMatch42(*pOrder);
            }
        }
        else
        {
            m_Orders.DestroyOrder(pOrder);
        }
    }
    else
    {
        FIX::OrderID OrdID;
        FIX42::OrderCancelReject* pReject = new FIX42::OrderCancelReject(OrdID,
                clOrdID,
                origClOrdID,
                FIX::OrdStatus(FIX::OrdStatus_REJECTED),
                FIX::CxlRejResponseTo(FIX::CxlRejResponseTo_ORDER_CANCEL_REPLACE_REQUEST));

        pSession->Send(pReject);
    }
}

/*
void CExecutor::onMessage( const FIX43::NewOrderSingle& message,
                             const FIX::SessionID& sessionID )
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get( ordType );

  if ( ordType != FIX::OrdType_LIMIT )
    throw FIX::IncorrectTagValue( ordType.getField() );

  message.get( symbol );
  message.get( side );
  message.get( orderQty );
  message.get( price );
  message.get( clOrdID );

  FIX43::ExecutionReport executionReport = FIX43::ExecutionReport
      ( FIX::OrderID( genOrderID() ),
        FIX::ExecID( genExecID() ),
        FIX::ExecType( FIX::ExecType_FILL ),
        FIX::OrdStatus( FIX::OrdStatus_FILLED ),
        side,
        FIX::LeavesQty( 0 ),
        FIX::CumQty( orderQty ),
        FIX::AvgPx( price ) );

  executionReport.set( clOrdID );
  executionReport.set( symbol );
  executionReport.set( orderQty );
  executionReport.set( FIX::LastQty( orderQty ) );
  executionReport.set( FIX::LastPx( price ) );

  if( message.isSet(account) )
    executionReport.setField( message.get(account) );

  try
  {
    FIX::Session::sendToTarget( executionReport, sessionID );
  }
  catch ( FIX::SessionNotFound& ) {}
}

void CExecutor::onMessage( const FIX44::NewOrderSingle& message,
                             const FIX::SessionID& sessionID )
{
  FIX::Symbol symbol;
  FIX::Side side;
  FIX::OrdType ordType;
  FIX::OrderQty orderQty;
  FIX::Price price;
  FIX::ClOrdID clOrdID;
  FIX::Account account;

  message.get( ordType );

  if ( ordType != FIX::OrdType_LIMIT )
    throw FIX::IncorrectTagValue( ordType.getField() );

  message.get( symbol );
  message.get( side );
  message.get( orderQty );
  message.get( price );
  message.get( clOrdID );

  FIX44::ExecutionReport executionReport = FIX44::ExecutionReport
      ( FIX::OrderID( genOrderID() ),
        FIX::ExecID( genExecID() ),
        FIX::ExecType( FIX::ExecType_FILL ),
        FIX::OrdStatus( FIX::OrdStatus_FILLED ),
        side,
        FIX::LeavesQty( 0 ),
        FIX::CumQty( orderQty ),
        FIX::AvgPx( price ) );

  executionReport.set( clOrdID );
  executionReport.set( symbol );
  executionReport.set( orderQty );
  executionReport.set( FIX::LastQty( orderQty ) );
  executionReport.set( FIX::LastPx( price ) );

  if( message.isSet(account) )
    executionReport.setField( message.get(account) );

  try
  {
    FIX::Session::sendToTarget( executionReport, sessionID );
  }
  catch ( FIX::SessionNotFound& ) {}
}*/

//void CExecutor::onMessage( const FIX40::OrderCancelRequest& message, const FIX::SessionID& sessionID )
//{
//}

//void CExecutor::onMessage( const FIX41::OrderCancelRequest& message, const FIX::SessionID& sessionID)
//{
//}


//void CExecutor::onMessage( const FIX43::OrderCancelRequest& message, const FIX::SessionID& sessionID)
//{
//}

//void CExecutor::onMessage( const FIX44::OrderCancelRequest& message, const FIX::SessionID& sessionID)
//{
//}

//void CExecutor::onMessage( const FIX40::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID)
//{
//}

//void CExecutor::onMessage( const FIX41::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID)
//{
//}


//void CExecutor::onMessage( const FIX43::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID)
//{
//}

//void CExecutor::onMessage( const FIX44::OrderCancelReplaceRequest& message, const FIX::SessionID& sessionID)
//{
//}
