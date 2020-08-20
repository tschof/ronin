#ifndef ORDER_H
#define ORDER_H

#include "SessionID.h"
#include "Values.h"
#include <string>

namespace FIX42
{
	class OrderCancelRequest;
	class OrderCancelReplaceRequest;
}

namespace OM
{
	class Status;
	class EnterOrder;
}

class Order
{
	public:
		Order( const std::string& trader, const std::string& tag, const FIX::SessionID& sessionId, 
			const std::string& clOrdId, int orderQty, const FIX::OrdStatus& ordStatus = FIX::OrdStatus_PENDING_NEW, double avgPx = 0.0, int cumQty = 0, int leavesQty = -1, 
			const std::string& origClOrdId = "" );
		Order( const FIX::SessionID & sessionID, const OM::EnterOrder & order );

		const std::string& GetTrader() const
			{ return m_trader; }
		const std::string& GetTag() const
			{ return m_tag; }
		const FIX::SessionID& GetSessionID() const
			{ return m_sessionId; }
		const std::string& GetClOrdID() const
			{ return m_clOrdId; }
		const std::string& GetOrigClOrdID() const
			{ return m_origClOrdId; }
		int GetOrderQty() const
			{ return m_orderQty; }
		FIX::OrdStatus GetOrdStatus() const
			{ return m_ordStatus; }
		double GetAvgPx() const
			{ return m_avgPx; }
		int GetCumQty() const
			{ return m_cumQty; }
		int GetLeavesQty() const
			{ return m_leavesQty; }

		void Update( const FIX42::OrderCancelRequest& cancel );
		void Update( const FIX42::OrderCancelReplaceRequest& replace );
		void Update( const OM::Status& status );
		void UpdateOrderStatus( int status );

	private:
		std::string			m_trader;
		std::string			m_tag;
		FIX::SessionID		m_sessionId;
		std::string			m_clOrdId;
		std::string			m_origClOrdId;
		int					m_orderQty;
		FIX::OrdStatus		m_ordStatus;
		FIX::OrdStatus		m_secondaryOrdStatus;
		double				m_avgPx;
		int					m_cumQty;
		int					m_leavesQty;
};


#endif
