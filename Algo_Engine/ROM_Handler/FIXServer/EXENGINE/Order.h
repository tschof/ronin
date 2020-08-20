// Order.h : Declaration of the COrder

#ifndef __ORDER_H_
#define __ORDER_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// COrder

class ATL_NO_VTABLE COrder : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COrder, &CLSID_Order>,
	public IDispatchImpl<IOrder, &IID_IOrder, &LIBID_EXENGINELib>
{
public:
	COrder()
	{
		m_dPrice			= 0.0;
		m_dAvgPrice			= 0.0;
		m_nCumQty			= 0;
		m_nLeaveQty			= 0;
		m_nOrgQty			= 0;
		m_szSymbol			= "";
		m_szClientOrderID	= "";
		m_eState			= eNew;
		m_eMod				= ePending;
		m_eType				= eMarket;
		m_eTIF				= eDay;
		m_eSecType			= eEquity;
		m_eSide				= eBuy;
		m_szAccount			= "0551 USER0001 WAG USER0001";
		m_nDestination		= 1;
		m_nLastShares		= 0;
		m_dExecPrice		= 0.0;
		m_eOpenClose		= eOpenClose_Open;
		m_eCallPut			= eCallPut_Call;
		m_dStrike			= 0.0;
		m_szExpire			= "";
		m_szText			= "";
		m_szExecID			= "";
		m_szCMTA			= "";
		m_szGiveUp			= "";
		m_eExecInst			= eExecInst_None;
		m_szShortLender		= "";
		m_szMMAccount		= "";
		m_szHomeExchange	= "";
		m_eCapacity			= eCapacity_Agent;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ORDER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(COrder)
	COM_INTERFACE_ENTRY(IOrder)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IOrder
public:
	STDMETHOD(get_OpenClose)(/*[out, retval]*/ enum OpenClose *pVal);
	STDMETHOD(put_OpenClose)(/*[in]*/ enum OpenClose newVal);
	STDMETHOD(get_CallPut)(/*[out, retval]*/ enum CallPut *pVal);
	STDMETHOD(put_CallPut)(/*[in]*/ enum CallPut newVal);
	STDMETHOD(get_Expiration)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Expiration)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Strike)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Strike)(/*[in]*/ double newVal);
	STDMETHOD(get_ExecPrice)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_ExecPrice)(/*[in]*/ double newVal);
	STDMETHOD(get_LastShares)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_LastShares)(/*[in]*/ long newVal);
	STDMETHOD(get_AvgPrice)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_AvgPrice)(/*[in]*/ double newVal);
	STDMETHOD(get_Price)(/*[out, retval]*/ double *pVal);
	STDMETHOD(put_Price)(/*[in]*/ double newVal);
	STDMETHOD(get_CumQty)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_CumQty)(/*[in]*/ long newVal);
	STDMETHOD(get_LeaveQty)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_LeaveQty)(/*[in]*/ long newVal);
	STDMETHOD(get_OrgQty)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_OrgQty)(/*[in]*/ long newVal);
	STDMETHOD(get_Symbol)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Symbol)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ClientOrderID)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ClientOrderID)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_SecType)(/*[out, retval]*/ enum SecType *pVal);
	STDMETHOD(put_SecType)(/*[in]*/ enum SecType newVal);
	STDMETHOD(get_OrderState)(/*[out, retval]*/ enum OrderState *pVal);
	STDMETHOD(put_OrderState)(/*[in]*/ enum OrderState newVal);
	STDMETHOD(get_OrderModifier)(/*[out, retval]*/ enum OrderModifier *pVal);
	STDMETHOD(put_OrderModifier)(/*[in]*/ enum OrderModifier newVal);
	STDMETHOD(get_OrderType)(/*[out, retval]*/ enum OrderType *pVal);
	STDMETHOD(put_OrderType)(/*[in]*/ enum OrderType newVal);
	STDMETHOD(get_TimeInForce)(/*[out, retval]*/ enum TimeInForce *pVal);
	STDMETHOD(put_TimeInForce)(/*[in]*/ enum TimeInForce newVal);
	STDMETHOD(get_OrderSide)(/*[out, retval]*/ enum OrderSide *pVal);
	STDMETHOD(put_OrderSide)(/*[in]*/ enum OrderSide newVal);
	STDMETHOD(get_Destination)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_Destination)(/*[in]*/ long newVal);
	STDMETHOD(get_Account)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Account)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_Text)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_Text)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ExecID)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ExecID)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_CMTA)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_CMTA)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_GiveUp)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_GiveUp)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ShortLender)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ShortLender)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ExecInst)(/*[out, retval]*/ enum ExecInst *pVal);
	STDMETHOD(put_ExecInst)(/*[in]*/ enum ExecInst newVal);
	STDMETHOD(get_Capacity)(/*[out, retval]*/ enum Capacity *pVal);
	STDMETHOD(put_Capacity)(/*[in]*/ enum Capacity newVal);
	STDMETHOD(get_MMAccount)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_MMAccount)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_HomeExchange)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_HomeExchange)(/*[in]*/ BSTR newVal);

private:
	double				m_dPrice;
	double				m_dAvgPrice;
	long				m_nCumQty;
	long				m_nLeaveQty;
	long				m_nOrgQty;
	_bstr_t				m_szSymbol;
	_bstr_t				m_szClientOrderID;
	enum OrderState		m_eState;
	enum OrderModifier	m_eMod;
	enum OrderType		m_eType;
	enum TimeInForce	m_eTIF;
	enum SecType		m_eSecType;
	enum OrderSide		m_eSide;
	enum ExecInst		m_eExecInst;
	_bstr_t				m_szAccount;
	long				m_nDestination;
	long				m_nLastShares;
	double				m_dExecPrice;
	double				m_dStrike;
	_bstr_t				m_szExpire;
	enum OpenClose		m_eOpenClose;
	enum CallPut		m_eCallPut;
	_bstr_t				m_szText;
	_bstr_t				m_szExecID;
	_bstr_t				m_szCMTA;
	_bstr_t				m_szGiveUp;
	_bstr_t				m_szShortLender;
	_bstr_t				m_szMMAccount;
	_bstr_t				m_szHomeExchange;
	enum Capacity		m_eCapacity;
};

#endif //__ORDER_H_
