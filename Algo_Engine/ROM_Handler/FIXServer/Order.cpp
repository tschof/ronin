#include "stdafx.h"
#include "Order.h"
#include "OMMessages.h"
#include "FIX42_Messages.h"

Order::Order( const std::string& trader, const std::string& tag, const FIX::SessionID& sessionId, 
	const std::string& clOrdId, int orderQty, const FIX::OrdStatus& ordStatus, double avgPx, int cumQty, int leavesQty, 
	const std::string& origClOrdId )
	: m_trader(trader), m_tag(tag), m_sessionId(sessionId), m_clOrdId(clOrdId), m_orderQty(orderQty), 
	m_ordStatus(ordStatus), m_secondaryOrdStatus(ordStatus), m_avgPx(avgPx), m_cumQty(cumQty), m_leavesQty(leavesQty), 
	m_origClOrdId(origClOrdId)
{
	if( m_leavesQty == -1 )
		m_leavesQty = m_orderQty;
}

void Order::Update( const FIX42::OrderCancelRequest& cancel )
{
	m_clOrdId = cancel.get(FIX::ClOrdID());
	m_origClOrdId = cancel.get(FIX::OrigClOrdID());
}

void Order::Update( const FIX42::OrderCancelReplaceRequest& replace )
{
	m_clOrdId = replace.get(FIX::ClOrdID());
	m_origClOrdId = replace.get(FIX::OrigClOrdID());
}

void Order::Update( const OM::Status& status )
{
	m_leavesQty = status.GetLeaveQty();
	m_cumQty = status.GetCumQty();
	m_orderQty = m_leavesQty + m_cumQty;
	m_avgPx = status.GetAvgPx();
	UpdateOrderStatus( status.GetStatus() );
}

void Order::UpdateOrderStatus( int status )
{
	switch( status )
	{
		case OM::Status_PartiallyFilled:
			if( m_ordStatus != FIX::OrdStatus_PENDING_CANCEL && m_ordStatus != FIX::OrdStatus_PENDING_REPLACE )
				m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
			else
				m_secondaryOrdStatus = FIX::OrdStatus_PARTIALLY_FILLED;
			break;
		case OM::Status_Filled:
			if( m_ordStatus != FIX::OrdStatus_PENDING_CANCEL && m_ordStatus != FIX::OrdStatus_PENDING_REPLACE )
				m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_FILLED;
			else
				m_secondaryOrdStatus = FIX::OrdStatus_FILLED;
			break;
		case OM::Status_Cancelled:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_CANCELED;
			break;
		case OM::Status_Replaced:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_REPLACED;
			break;
		case OM::Status_PendingCancel:
			m_secondaryOrdStatus = m_ordStatus;
			m_ordStatus = FIX::OrdStatus_PENDING_CANCEL;
			break;
		case OM::Status_ReplacePending:
			m_secondaryOrdStatus = m_ordStatus;
			m_ordStatus = FIX::OrdStatus_PENDING_REPLACE;
			break;
		case OM::Status_PendingNew:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_PENDING_NEW;
			break;
		case OM::Status_Calculated:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_CALCULATED;
			break;
		case OM::Status_Expired:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_EXPIRED;
			break;
		case OM::Status_Open:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_NEW;
			break;
		case OM::Status_CancelRejected:
			if( m_ordStatus == FIX::OrdStatus_PENDING_CANCEL || m_ordStatus == FIX::OrdStatus_PENDING_REPLACE )
				m_ordStatus = m_secondaryOrdStatus;
			break;
		case OM::Status_ReplaceRejected:
			if( m_ordStatus == FIX::OrdStatus_PENDING_CANCEL || m_ordStatus == FIX::OrdStatus_PENDING_REPLACE )
				m_ordStatus = m_secondaryOrdStatus;
			break;
		case OM::Status_Corrected:
			break;
		case OM::Status_Busted:
			break;
		default:
			m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_NEW + status;
			break;
	}
}

Order::Order( const FIX::SessionID & sessionID, const OM::EnterOrder & order ) 
: m_sessionId(sessionID)
{
	m_ordStatus = m_secondaryOrdStatus = FIX::OrdStatus_NEW;
	UpdateOrderStatus( order.GetStatus() );
	m_avgPx = order.GetAvgPrc();
	m_cumQty = order.GetCumQty();
	m_leavesQty = order.GetLeaveQty();
	m_orderQty = m_cumQty+m_leavesQty;
	m_trader = order.GetTrader();
	m_tag = order.GetTag();
	int nPos = m_tag.find('-');
	if( nPos == -1 )
		throw exception("Order tag not created by fix server");
	m_clOrdId = m_tag.substr( nPos+1, -1 );
	m_origClOrdId = "";
}
