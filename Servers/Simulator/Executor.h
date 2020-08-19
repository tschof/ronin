#ifndef _EXECUTOR_H__
#define _EXECUTOR_H__
#include "FileStore.h"
#include "SocketAcceptor.h"
#include "SessionSettings.h"
#include "Application.h"
#include "MessageCracker.h"
#include "Values.h"
#include "Utility.h"
#include "Mutex.h"

#include "fix40/NewOrderSingle.h"
#include "fix41/NewOrderSingle.h"
#include "fix42/NewOrderSingle.h"
#include "fix43/NewOrderSingle.h"
#include "fix44/NewOrderSingle.h"

#include "Orders.h"
#include "SessionWrap.h"

class CExecutor
    : public FIX::Application, public FIX::MessageCracker
{
public:

    static std::string SPECIALORDER;

    CExecutor(bool* end);
    ~CExecutor();
    // Application overloads
    void onCreate( const FIX::SessionID& );
    void onLogon( const FIX::SessionID& sessionID );
    void onLogout( const FIX::SessionID& sessionID );
    void toAdmin( FIX::Message&, const FIX::SessionID& );
    void toApp( FIX::Message&, const FIX::SessionID& )
    throw( FIX::DoNotSend );
    void fromAdmin( const FIX::Message&, const FIX::SessionID& )
    throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
    void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
    throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

    // MessageCracker overloads
    //void onMessage( const FIX40::NewOrderSingle&, const FIX::SessionID& );
    //void onMessage( const FIX41::NewOrderSingle&, const FIX::SessionID& );
    void onMessage( const FIX42::NewOrderSingle&, const FIX::SessionID& );
    //void onMessage( const FIX43::NewOrderSingle&, const FIX::SessionID& );
    //void onMessage( const FIX44::NewOrderSingle&, const FIX::SessionID& );

    //void onMessage( const FIX40::OrderCancelRequest&, const FIX::SessionID& );
    //void onMessage( const FIX41::OrderCancelRequest&, const FIX::SessionID& );
    void onMessage( const FIX42::OrderCancelRequest&, const FIX::SessionID& );
    //void onMessage( const FIX43::OrderCancelRequest&, const FIX::SessionID& );
    //void onMessage( const FIX44::OrderCancelRequest&, const FIX::SessionID& );

    //void onMessage( const FIX40::OrderCancelReplaceRequest&, const FIX::SessionID& );
    //void onMessage( const FIX41::OrderCancelReplaceRequest&, const FIX::SessionID& );
    void onMessage( const FIX42::OrderCancelReplaceRequest&, const FIX::SessionID& );
    //void onMessage( const FIX43::OrderCancelReplaceRequest&, const FIX::SessionID& );
    //void onMessage( const FIX44::OrderCancelReplaceRequest&, const FIX::SessionID& );

    std::string genOrderID()
    {
        std::stringstream stream;
        stream << ++m_orderID;
        return stream.str();
    }
    std::string genExecID()
    {
        std::stringstream stream;
        stream << ++m_execID;
        return stream.str();
    }
private:
    int m_orderID, m_execID;
    void DoFill42(COrder& order, CSessionWrap& Session);
    void DoNormalFill42(COrder& order, unsigned int lastShares, double lastPrice);

    void DoOpen42(COrder& order, CSessionWrap& Session);
    void DoPartialFill42(COrder& order, CSessionWrap& Session);
    bool DoPartialFill42(COrder& order, CSessionWrap& Session, int lastShares);
    void DoReject42(COrder& order, CSessionWrap& Session, const std::string& reason);
    void DoOut42(COrder& order, CSessionWrap& Session, const std::string& reason);
    void DoMatch42(COrder& order);
    bool Match42(COrder& order);
    void DoMarketOrderMatch42(COrder& order, CSessionWrap& Session);

    void SendExecReport(const COrder& order,
                        CSessionWrap& Session,
                        const std::string& ClOrdID,
                        const std::string& OrigClOrdID,
                        char status,
                        unsigned int lastShares,
                        double lastPrice,
                        const std::string& text);

    CSessionWrap* FindSession(const std::string& Login);
    void AddSession(const FIX::SessionID& sessionID);
    void AddSession(const std::string& key, CSessionWrap* pSession);

    COrders m_Orders;
    bool* m_End;
    std::map<std::string, CSessionWrap*> m_SessionMap;
};

#endif //_EXECUTOR_H__
