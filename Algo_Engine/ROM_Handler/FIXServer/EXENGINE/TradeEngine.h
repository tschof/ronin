// TradeEngine.h : Declaration of the CTradeEngine

#ifndef __TRADEENGINE_H_
#define __TRADEENGINE_H_

#include "resource.h"       // main symbols
#include "Application.h"
#include "MessageCracker.h"
#include <map>

/////////////////////////////////////////////////////////////////////////////
// CTradeEngine
class ATL_NO_VTABLE CTradeEngine : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTradeEngine, &CLSID_TradeEngine>,
	public ISupportErrorInfo,
	public IDispatchImpl<ITradeEngine, &IID_ITradeEngine, &LIBID_EXENGINELib>,
	public FIX::Application, 
	public FIX::MessageCracker
{
public:
	CTradeEngine() : m_bInit(false)
	{
		m_hKill = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		m_hKilled = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	}

	virtual ~CTradeEngine()
	{
		CloseHandle( m_hKill );
		CloseHandle( m_hKilled );
	}

DECLARE_REGISTRY_RESOURCEID(IDR_TRADEENGINE)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CTradeEngine)
	COM_INTERFACE_ENTRY(ITradeEngine)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ITradeEngine
public:
	STDMETHOD(Shutdown)();
	STDMETHOD(Init)(/*[in]*/ BSTR szPath,/*[in]*/ IStatusHandler * pHandler );
	STDMETHOD(ReplaceOrder)(/*[in]*/ IOrder * pOrder,/*[in]*/ BSTR szNewClientID, /*[in]*/ long nNewQty, /*[in]*/ double dNewPrice);
	STDMETHOD(CancelOrder)(/*[in]*/ IOrder * pOrder);
	STDMETHOD(SubmitOrder)(/*[in]*/ IOrder * pOrder);
	STDMETHOD(CreateNewOrder)(/*[out]*/ IOrder ** ppOrder);

	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

public:
	virtual void onCreate( const FIX::SessionID& sessionID );
	virtual void onLogon( const FIX::SessionID& sessionID );
	virtual void onLogout( const FIX::SessionID& sessionID );
	virtual void toAdmin( FIX::Message& message, const FIX::SessionID& sessionID );
	virtual void toApp( FIX::Message& message, const FIX::SessionID& sessionID )
		throw(FIX::DoNotSend&);
    virtual void fromAdmin( const FIX::Message& message, const FIX::SessionID& )
		throw(FIX::FieldNotFound&, FIX::RejectLogon&);
	virtual void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&);
	void onRun();

	virtual void onMessage( const FIX42::ExecutionReport& report, const FIX::SessionID& sessionID ); 
	virtual void onMessage( const FIX42::OrderCancelReject& reject, const FIX::SessionID& sessionID ); 

	void SetSenderCompID( const std::string & sender ) { m_szSenderCompID = sender; }
	void SetTargetCompID( const std::string & target ) { m_szTargetCompID = target; }

private:
	static void ThreadProc( void * pParam );

	typedef std::map<std::string,CAdapt< CComPtr<IOrder> > > tOrders;
	typedef std::map<std::string,CAdapt< CComPtr<IOrder> > >::iterator tOrderIterator;

	bool				m_bInit;
	IStatusHandler *	m_pHandler;
	HANDLE				m_hKill;
	HANDLE				m_hKilled;

	static _bstr_t		m_szPath;

	std::string			m_szSenderCompID;
	std::string			m_szTargetCompID;

	LPSTREAM			m_pStream;
	tOrders				m_Orders;
	FIX::Mutex			m_OrderMutex;

};

#endif //__TRADEENGINE_H_
