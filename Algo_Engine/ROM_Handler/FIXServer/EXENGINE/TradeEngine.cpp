// TradeEngine.cpp : Implementation of CTradeEngine
#include "stdafx.h"
#include "ExEngine.h"
#include "TradeEngine.h"
#include "Order.h"
#include "Error.h"
#include "FileStore.h"
#include "SocketInitiator.h"
#include "SessionSettings.h"

#include <string>
#include <sstream>

namespace FIX
{
	USER_DEFINE_INT(OMExchange,10001);
	USER_DEFINE_STRING(SSFirm,5700);
}

/////////////////////////////////////////////////////////////////////////////
// CTradeEngine
_bstr_t CTradeEngine::m_szPath = ".\\";

void TraceThreadID( const std::string & szFunction )
{
	DWORD dwThreadID = ::GetCurrentThreadId();
	std::stringstream stm;
	stm << szFunction << ": " << (long)dwThreadID << std::endl;
	OutputDebugString( stm.str().c_str() );
	return;
}

STDMETHODIMP CTradeEngine::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ITradeEngine
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (ATL::InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CTradeEngine::CreateNewOrder(IOrder **ppOrder)
{
	// TODO: Add your implementation code here
	CComObject<COrder> * spOrder = NULL;
	HRESULT hr = CComObject<COrder>::CreateInstance( &spOrder );
	if( FAILED( hr ) )
	{
		*ppOrder = NULL;
		CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::CreateNewOrder"), _T("Fail to CreateInstance"), 0, NULL );	
		return hr;
	}
	hr = spOrder->QueryInterface( ppOrder );
	if ( FAILED( hr ) )
		return hr;
	return S_OK;
}

STDMETHODIMP CTradeEngine::SubmitOrder(IOrder *pOrder)
{
	// TODO: Add your implementation code here
	TraceThreadID( "CTradeEngine::SubmitOrder" );
	try
	{
		FIX42::NewOrderSingle order;
		CComBSTR szTemp;
		pOrder->get_ClientOrderID( &szTemp );
		order.set( FIX::ClOrdID( (char*)_bstr_t(szTemp) ) );
		std::string szClientID;
		szClientID = _bstr_t(szTemp);

		pOrder->get_Account( &szTemp );
		order.set( FIX::Account( (char*)_bstr_t(szTemp) ) );

		pOrder->get_Symbol( &szTemp );
		order.set( FIX::Symbol( (char*)_bstr_t(szTemp) ) );

		long nTemp;

		pOrder->get_OrgQty( &nTemp );
		order.set( FIX::OrderQty(nTemp) );

		double dTemp;
		pOrder->get_Price( &dTemp );
		order.set( FIX::Price(dTemp) );

		enum OrderSide side;
		pOrder->get_OrderSide( &side );
		if( side == eBuy )
			order.set( FIX::Side(FIX::Side_BUY) );
		else if( side == eSell )
			order.set( FIX::Side(FIX::Side_SELL) );
		else if( side == eShort )
			order.set( FIX::Side(FIX::Side_SELL_SHORT) );
		else
			return E_FAIL;

		enum OrderType type;
		pOrder->get_OrderType( &type );
		if( type == eLimit )
			order.set( FIX::OrdType(FIX::OrdType_LIMIT) );
		else if( type == eMarket )
			order.set( FIX::OrdType(FIX::OrdType_MARKET) );
		else
			return E_FAIL;

		enum TimeInForce tif;
		pOrder->get_TimeInForce( &tif );
		if( tif == eDay )
			order.set( FIX::TimeInForce(FIX::TimeInForce_DAY) );
		else if( tif == eGTC )
			order.set( FIX::TimeInForce(FIX::TimeInForce_GOOD_TILL_CANCEL) );
		else if( tif == eMOC )
		{
			order.set( FIX::OrdType(FIX::OrdType_MARKET_ON_CLOSE) );
			order.set( FIX::TimeInForce(FIX::TimeInForce_DAY) );
		}
		else
			return E_FAIL;

		enum ExecInst inst;
		pOrder->get_ExecInst( &inst );
		if( inst == eExecInst_AON )
			order.set( FIX::ExecInst("G") );

		enum Capacity cap;
		pOrder->get_Capacity( &cap );
		char cCap = cap;
		order.set( FIX::Rule80A( cCap ) );

		enum SecType sec_type;
		pOrder->get_SecType( &sec_type );
		switch( sec_type )
		{
		case eEquity:
			order.set( FIX::SecurityType(FIX::SecurityType_PREFERRED_STOCK ) );
			if( side == eShort )
			{
				pOrder->get_ShortLender( &szTemp );
				if( !szTemp )
					throw exception( "Missing Short Lender Firm" );
				std::string szFirm = (char *)_bstr_t(szTemp); 
				if( szFirm == "EXEMPT" )
				{
					order.setField( FIX::LocateReqd( false ) );
				}
				else
				{
					order.setField( FIX::LocateReqd( true ) );
					FIX::SSFirm ssfirm( szFirm );
					order.setField( ssfirm );
				}
			}
			break;
		case eOption:
			{
				order.set( FIX::SecurityType(FIX::SecurityType_OPTION) );
				enum CallPut callput;
				pOrder->get_CallPut( &callput );
				if( callput == eCallPut_Call )
					order.set( FIX::PutOrCall( FIX::PutOrCall_CALL ) );
				else
					order.set( FIX::PutOrCall( FIX::PutOrCall_PUT ) );
				double strike;
				pOrder->get_Strike( &strike );
				order.set( FIX::StrikePrice(strike) );

				CComBSTR expire;
				pOrder->get_Expiration( &expire );
				order.set( FIX::MaturityMonthYear( (LPCSTR)_bstr_t(expire) ) );

				enum OpenClose openclose;
				pOrder->get_OpenClose( &openclose );
				if( openclose == eOpenClose_Open )
					order.set( FIX::OpenClose( FIX::OpenClose_OPEN ) );
				else
					order.set( FIX::OpenClose( FIX::OpenClose_CLOSE ) );

				pOrder->get_CMTA( &szTemp );
				if( szTemp.Length() != 0 )
					order.set( FIX::ClearingFirm( (LPCSTR)_bstr_t(szTemp) ) );
				pOrder->get_GiveUp( &szTemp );
				if( szTemp.Length() != 0 )
					order.set( FIX::ExecBroker( (LPCSTR)_bstr_t(szTemp) ) );

				CComBSTR szMMAccount = "";
				pOrder->get_HomeExchange( &szTemp );
				if( szTemp.Length() != 0 )
				{
					szMMAccount = szTemp;
					szMMAccount += ":";
				}
				pOrder->get_MMAccount( &szTemp );
				if( szTemp.Length() != 0 )
				{
					szMMAccount += szTemp;
					order.set( FIX::ClearingAccount( (LPCSTR)_bstr_t(szMMAccount) ) );
				}
			}

			break;
		case eFuture:
			order.set( FIX::SecurityType(FIX::SecurityType_FUTURE) );
			break;
		default:
			return E_FAIL;
		}

		pOrder->get_Destination( &nTemp );
		order.setField( FIX::OMExchange(nTemp) );
		std::ostringstream stm;
		stm << nTemp;
		order.setField( FIX::ExDestination(stm.str()) );
		order.getHeader().setField( FIX::SenderCompID(m_szSenderCompID) );
		order.getHeader().setField( FIX::TargetCompID(m_szTargetCompID) );
		{
			FIX::Locker lock(m_OrderMutex);
			m_Orders[szClientID] = pOrder;
		}
		FIX::Session::sendToTarget( order );
	}
	catch( exception & ex )
	{
		CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::SubmitOrder"), (char *)ex.what(), 0, NULL );	
		OutputDebugString( ex.what() );
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CTradeEngine::CancelOrder(IOrder *pOrder)
{
	// TODO: Add your implementation code here
	try
	{
		CComBSTR szTemp;
		HRESULT hr = pOrder->get_ClientOrderID( &szTemp );
		if( FAILED( hr ) )
			throw exception("fail to get client order id" );
		std::string szClientID;
		szClientID = _bstr_t(szTemp);

		FIX::ClOrdID aClOrdID;
		FIX::OrigClOrdID aOrigClOrdID = szClientID;
		FIX42::OrderCancelRequest cancel;
		cancel.set( aOrigClOrdID );
		cancel.set( FIX::ClOrdID( szClientID+"C" ) );
		enum OrderSide side;
		pOrder->get_OrderSide( &side );
		if( side == eBuy )
			cancel.set( FIX::Side( FIX::Side_BUY ) );
		else if( side == eSell )
			cancel.set( FIX::Side( FIX::Side_SELL ) );
		else
			cancel.set( FIX::Side( FIX::Side_SELL_SHORT ) );

		pOrder->get_Symbol( &szTemp );
		cancel.set( FIX::Symbol( (char*)_bstr_t(szTemp) ) );

		cancel.getHeader().setField( FIX::SenderCompID(m_szSenderCompID) );
		cancel.getHeader().setField( FIX::TargetCompID(m_szTargetCompID) );
		FIX::Session::sendToTarget( cancel );
	}
	catch(exception & ex)
	{
		CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::CancelOrder"), (char *)ex.what(), 0, NULL );	
		OutputDebugString( ex.what() );
		return E_FAIL;
	}

	return S_OK;
}

STDMETHODIMP CTradeEngine::ReplaceOrder(IOrder *pOrder, BSTR szNewClientID, long nNewQty, double dNewPrice)
{
	// TODO: Add your implementation code here
	try
	{
		CComBSTR szTemp;
		HRESULT hr = pOrder->get_ClientOrderID( &szTemp );
		if( FAILED( hr ) )
			throw exception("fail to get client order id");
		std::string szClientID;
		szClientID = _bstr_t(szTemp);

		FIX42::OrderCancelReplaceRequest replace;
		replace.set( FIX::OrigClOrdID( szClientID ) );
		replace.set( FIX::ClOrdID( (char *)_bstr_t(szNewClientID) ) );

		enum OrderSide side;
		pOrder->get_OrderSide( &side );
		if( side == eBuy )
			replace.set( FIX::Side( FIX::Side_BUY ) );
		else if( side == eSell )
			replace.set( FIX::Side( FIX::Side_SELL ) );
		else
			replace.set( FIX::Side( FIX::Side_SELL_SHORT ) );

		pOrder->get_Symbol( &szTemp );
		replace.set( FIX::Symbol( (char*)_bstr_t(szTemp) ) );

		enum OrderType type;
		pOrder->get_OrderType( &type );
		if( type == eMarket )
			replace.set( FIX::OrdType( FIX::OrdType_MARKET ) );
		else
			replace.set( FIX::OrdType( FIX::OrdType_LIMIT ) );

		if( nNewQty > 0 )
			replace.set( FIX::OrderQty( nNewQty ) );
		else
			replace.set( FIX::OrderQty( 0 ) );

		if( dNewPrice > 0.0 )
			replace.set( FIX::Price( dNewPrice ) );
		else
			replace.set( FIX::Price( 0.0 ) );


		replace.getHeader().setField( FIX::SenderCompID(m_szSenderCompID) );
		replace.getHeader().setField( FIX::TargetCompID(m_szTargetCompID) );
		{
			FIX::Locker lock(m_OrderMutex);
			m_Orders[(char *)_bstr_t(szNewClientID)] = pOrder;
		}

		FIX::Session::sendToTarget( replace );
	}
	catch(exception &ex)
	{
		CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::ReplaceOrder"), (char *)ex.what(), 0, NULL );	
		OutputDebugString( ex.what() );
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CTradeEngine::Init(BSTR szPath, IStatusHandler * pHandler)
{
	// TODO: Add your implementation code here
	TraceThreadID( "CTradeEngine::Init" );
	if( m_bInit )
		return S_FALSE;
	m_pHandler = NULL;
	HRESULT hr = ::CoMarshalInterThreadInterfaceInStream( __uuidof(IStatusHandler), pHandler, &m_pStream );
	if( FAILED( hr ) )
	{
		CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::Init"), _T("CoMarshalInterThreadInterfaceInStream failed"), 0, NULL );	
		return hr;
	}

	m_bInit = true;
	m_szPath = szPath;
	_beginthread( ThreadProc, 0, this );
	return S_OK;
}

void CTradeEngine::ThreadProc( void * pParam )
{
	try
	{
		HRESULT hr = ::CoInitialize( NULL );
		TraceThreadID( "CTradeEngine::ThreadProc" );
		CTradeEngine * pThis = (CTradeEngine *)pParam;
		FIX::SessionSettings settings((char *)m_szPath);
		FIX::FileStoreFactory factory(".\\");
		pThis->SetSenderCompID( (*settings.getSessions().begin()).getSenderCompID() );
		pThis->SetTargetCompID( (*settings.getSessions().begin()).getTargetCompID() );
		FIX::SocketInitiator initiator( *pThis, factory, settings );
		initiator.start();
		::CoUninitialize();
	}
	catch(...)
	{
		CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::ThreadProc"), _T("Unknown exception"), 0, NULL );	
		return;
	}
	return;
}

STDMETHODIMP CTradeEngine::Shutdown()
{
	// TODO: Add your implementation code here
	TraceThreadID( "CTradeEngine::Shutdown" );
	::SetEvent( m_hKill );
	::WaitForSingleObject( m_hKilled, INFINITE );
	return S_OK;
}

void CTradeEngine::onCreate( const FIX::SessionID& sessionID )
{
	TraceThreadID( "CTradeEngine::onCreate" );
	HRESULT hr;
	if( !m_pHandler )
	{
		hr = ::CoUnmarshalInterface( m_pStream, __uuidof(IStatusHandler), (LPVOID *)&m_pHandler );
		if( FAILED( hr ) )
		{
			CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::onCreate"), _T("CoUnmarshalInterface failed"), 0, NULL );	
			return;
		}
	}
	m_pHandler->OnSystemStatus( _bstr_t("FIX Session Created") );
}

void CTradeEngine::onLogon( const FIX::SessionID& sessionID )
{
	TraceThreadID( "CTradeEngine::onLogon" );
	HRESULT hr;
	if( !m_pHandler )
	{
		hr = ::CoUnmarshalInterface( m_pStream, __uuidof(IStatusHandler), (LPVOID *)&m_pHandler );
		if( FAILED( hr ) )
		{
			CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::onLogon"), _T("CoUnmarshalInterface failed"), 0, NULL );	
			return;
		}
	}
	m_pHandler->OnSystemStatus( _bstr_t("FIX Client Logged On") );
}

void CTradeEngine::onLogout( const FIX::SessionID& sessionID )
{
	TraceThreadID( "CTradeEngine::onLogout" );
	HRESULT hr;
	if( !m_pHandler )
	{
		hr = ::CoUnmarshalInterface( m_pStream, __uuidof(IStatusHandler), (LPVOID *)&m_pHandler );
		if( FAILED( hr ) )
		{
			CError::DispatchError( E_FAIL, CLSID_TradeEngine, _T("CTradeEngine::onLogout"), _T("CoUnmarshalInterface failed"), 0, NULL );	
			return;
		}
	}
	m_pHandler->OnSystemStatus( _bstr_t("FIX Client Logged Off") );
}

void CTradeEngine::toAdmin( FIX::Message& message, const FIX::SessionID& )
{
	TraceThreadID( "CTradeEngine::toAdmin" );

}

void CTradeEngine::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
{
	TraceThreadID( "CTradeEngine::toApp" );

}

void CTradeEngine::fromAdmin( const FIX::Message& message, const FIX::SessionID& sessionID )
{
	TraceThreadID( "CTradeEngine::fromAdmin" );

}

void CTradeEngine::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
{
	TraceThreadID( "CTradeEngine::fromApp" );
	crack( message, sessionID );
}

void CTradeEngine::onRun()
{
	TraceThreadID( "CTradeEngine::onRun" );
	::WaitForSingleObject( m_hKill, INFINITE );
	::SetEvent( m_hKilled );
}

void CTradeEngine::onMessage( const FIX42::ExecutionReport& report, const FIX::SessionID& sessionID )
{
	//m_pHandler->OnSystemStatus( _bstr_t("Received Execution Report") );
	//TraceThreadID( "CTradeEngine::onMessage( const FIX42::ExecutionReport& report, const FIX::SessionID& sessionID )" );
	try
	{
		HRESULT hr;
		if( !m_pHandler )
		{
			hr = ::CoUnmarshalInterface( m_pStream, __uuidof(IStatusHandler), (LPVOID *)&m_pHandler );
			if( FAILED( hr ) )
			{
				m_pHandler->OnSystemStatus( _bstr_t("CoUnmarshalInterface failed") );	
				return;
			}
		}

		FIX::ClOrdID cid;
		report.get( cid );

		CComPtr<IOrder> spOrder = NULL;
		{
			FIX::Locker lock(m_OrderMutex);
			tOrderIterator it = m_Orders.find( cid );
			if( it == m_Orders.end() )
			{
				FIX::OrigClOrdID org_cid;
				if( report.isSetField( FIX::OrigClOrdID() ) )
				{
					report.get( org_cid );
					it = m_Orders.find( FIX::ClOrdID( org_cid.getValue() ) );
				}
				else
					it = m_Orders.end();
				if( it == m_Orders.end() )
				{
					CComObject<COrder> * order = NULL;
					hr = CComObject<COrder>::CreateInstance( &order );
					if( FAILED( hr ) )
					{
						m_pHandler->OnSystemStatus( _bstr_t("Fail to create new order") );	
						return;
					}
					hr = order->QueryInterface( &spOrder );
					if( FAILED( hr ) )
					{
						m_pHandler->OnSystemStatus( _bstr_t("Fail to query IOrder interface") );	
						return;
					}
					std::string szOID;
					if( report.isSetField( FIX::OrigClOrdID() ) )
						szOID = org_cid;
					else
						szOID = cid;
					spOrder->put_ClientOrderID( _bstr_t(szOID.c_str()) );
				}
				else
					spOrder = (*it).second;
			}
			else
				spOrder = (*it).second;
		}
		
		FIX::OrdStatus status;
		report.get( status );
		double last_price = report.get(FIX::LastPx());
		long last_shares = report.get(FIX::LastShares());
		float avg_price = report.get(FIX::AvgPx());
		int cum_qty = report.get(FIX::CumQty());
		int leave_qty = report.get(FIX::LeavesQty());
		if( report.isSetField(FIX::Price()) )
		{
			spOrder->put_Price(report.get(FIX::Price()));
		}

		if( report.isSetField(FIX::Text()) )
		{
			std::string szText = report.get(FIX::Text());
			spOrder->put_Text( _bstr_t(szText.c_str()) );
		}

		if( report.isSetField(FIX::ExecID()) )
		{
			std::string szExecID = report.get(FIX::ExecID());
			spOrder->put_ExecID( _bstr_t(szExecID.c_str()) );
		}

		spOrder->put_LastShares( last_shares );
		spOrder->put_ExecPrice( last_price );
		spOrder->put_CumQty( cum_qty );
		spOrder->put_LeaveQty( leave_qty );
		spOrder->put_AvgPrice( avg_price );

		switch( status )
		{
		case FIX::OrdStatus_NEW:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( eAcknowledged );
			}
			break;
		case FIX::OrdStatus_PARTIALLY_FILLED:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( ePartialFilled );
			}
			break;
		case FIX::OrdStatus_FILLED:
			{
				spOrder->put_OrderState( eClose );
				spOrder->put_OrderModifier( eFilled );
			}
			break;
		case FIX::OrdStatus_DONE_FOR_DAY:
			{
			}
			break;
		case FIX::OrdStatus_CANCELED:
			{
				spOrder->put_OrderState( eClose );
				spOrder->put_OrderModifier( eCancelled );
			}
			break;
		case FIX::OrdStatus_REPLACED:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( eReplaced );
			}
			break;
		case FIX::OrdStatus_PENDING_CANCEL:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( eCancelPending );
			}
			break;
		case FIX::OrdStatus_STOPPED:
			{
				spOrder->put_OrderState( eClose );
				spOrder->put_OrderModifier( eStopped );
			}
			break;
		case FIX::OrdStatus_REJECTED:
			{
				spOrder->put_OrderState( eClose );
				spOrder->put_OrderModifier( eRejected );
			}
			break;
		case FIX::OrdStatus_SUSPENDED:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( eSuspended );
			}
			break;
		case FIX::OrdStatus_PENDING_NEW:
			{
				spOrder->put_OrderState( eNew );
				spOrder->put_OrderModifier( ePending );
			}
			break;
		case FIX::OrdStatus_CALCULATED:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( eCalculated );
			}
			break;
		case FIX::OrdStatus_EXPIRED:
			{
				spOrder->put_OrderState( eClose );
				spOrder->put_OrderModifier( eExpired );
			}
			break;
		case FIX::OrdStatus_ACCEPTED_FOR_BIDDING:
			{

			}
			break;
		case FIX::OrdStatus_PENDING_REPLACE:
			{
				spOrder->put_OrderState( eOpen );
				spOrder->put_OrderModifier( eReplacePending );
			}
			break;
		default:
			break;
		}
		m_pHandler->OnOrderStatus( spOrder );
	}
	catch( exception &ex )
	{
		m_pHandler->OnSystemStatus( _bstr_t(ex.what() ) );
		return;
	}
	catch(...)
	{
		m_pHandler->OnSystemStatus( _bstr_t("Unknown exception") );
	}
}

void CTradeEngine::onMessage( const FIX42::OrderCancelReject& reject, const FIX::SessionID& sessionID )
{
	//TraceThreadID( "CTradeEngine::onMessageonMessage( const FIX42::OrderCancelReject& reject, const FIX::SessionID& sessionID )" );
	//m_pHandler->OnSystemStatus( _bstr_t("Recieved Cancel Rejected") );
	try
	{
		FIX::ClOrdID cid;
		reject.get( cid );
		CComPtr<IOrder> spOrder = NULL;
		{
			FIX::Locker lock(m_OrderMutex);
			tOrderIterator it = m_Orders.find( cid );
			if( it == m_Orders.end() )
			{
				FIX::OrigClOrdID org_cid;
				reject.get( org_cid );
				it = m_Orders.find( FIX::ClOrdID( org_cid.getValue() ) );
				if( it == m_Orders.end() )
				{
					CComObject<COrder> * order = NULL;
					HRESULT hr = CComObject<COrder>::CreateInstance( &order );
					if( FAILED( hr ) )
					{
						m_pHandler->OnSystemStatus( _bstr_t("Cancel Reject status: Fail to create new order") );
						return;
					}
					hr = order->QueryInterface( &spOrder );
					if( FAILED( hr ) )
					{
						m_pHandler->OnSystemStatus( _bstr_t("Cancel Reject: Fail to query order interface") );
						return;
					}
					std::string szOID = org_cid;
					spOrder->put_ClientOrderID( _bstr_t(szOID.c_str()) );
				}
				else
					spOrder = (*it).second;
			}
			else
				spOrder = (*it).second;
		}
		try
		{
			std::string szText = reject.get(FIX::Text());
			spOrder->put_Text( _bstr_t(szText.c_str()) );
		}
		catch(...)
		{
		}
		spOrder->put_OrderModifier( eCancelReject );
		m_pHandler->OnOrderStatus( spOrder );
	}
	catch( exception & ex )
	{
		m_pHandler->OnSystemStatus( _bstr_t(ex.what() ) );
		return;
	}
	catch(...)
	{
		m_pHandler->OnSystemStatus( _bstr_t("Cancel Reject: Unknown exception") );
	}
}
