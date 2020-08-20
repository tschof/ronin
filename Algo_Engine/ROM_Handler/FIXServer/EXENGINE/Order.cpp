// Order.cpp : Implementation of COrder
#include "stdafx.h"
#include "ExEngine.h"
#include "Order.h"

/////////////////////////////////////////////////////////////////////////////
// COrder


STDMETHODIMP COrder::get_ClientOrderID(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szClientOrderID.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_ClientOrderID(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szClientOrderID = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Symbol(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szSymbol.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_Symbol(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szSymbol = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_OrgQty(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_nOrgQty;
	return S_OK;
}

STDMETHODIMP COrder::put_OrgQty(long newVal)
{
	// TODO: Add your implementation code here
	m_nOrgQty = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_LeaveQty(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_nLeaveQty;
	return S_OK;
}

STDMETHODIMP COrder::put_LeaveQty(long newVal)
{
	// TODO: Add your implementation code here
	m_nLeaveQty = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_CumQty(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_nCumQty;
	return S_OK;
}

STDMETHODIMP COrder::put_CumQty(long newVal)
{
	// TODO: Add your implementation code here
	m_nCumQty = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Price(double *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_dPrice;
	return S_OK;
}

STDMETHODIMP COrder::put_Price(double newVal)
{
	// TODO: Add your implementation code here
	m_dPrice = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_AvgPrice(double *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_dAvgPrice;
	return S_OK;
}

STDMETHODIMP COrder::put_AvgPrice(double newVal)
{
	// TODO: Add your implementation code here
	m_dAvgPrice = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_SecType(enum SecType *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eSecType;
	return S_OK;
}

STDMETHODIMP COrder::put_SecType(enum SecType newVal)
{
	// TODO: Add your implementation code here
	m_eSecType = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_OrderState(enum OrderState *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eState;
	return S_OK;
}

STDMETHODIMP COrder::put_OrderState(enum OrderState newVal)
{
	// TODO: Add your implementation code here
	m_eState = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_OrderModifier(enum OrderModifier *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eMod;
	return S_OK;
}

STDMETHODIMP COrder::put_OrderModifier(enum OrderModifier newVal)
{
	// TODO: Add your implementation code here
	m_eMod = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_OrderType(enum OrderType *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eType;
	return S_OK;
}

STDMETHODIMP COrder::put_OrderType(enum OrderType newVal)
{
	// TODO: Add your implementation code here
	m_eType = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_TimeInForce(enum TimeInForce *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eTIF;
	return S_OK;
}

STDMETHODIMP COrder::put_TimeInForce(enum TimeInForce newVal)
{
	// TODO: Add your implementation code here
	m_eTIF = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_OrderSide(enum OrderSide *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eSide;
	return S_OK;
}

STDMETHODIMP COrder::put_OrderSide(enum OrderSide newVal)
{
	// TODO: Add your implementation code here
	m_eSide = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Account(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szAccount.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_Account(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szAccount = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Destination(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_nDestination;
	return S_OK;
}

STDMETHODIMP COrder::put_Destination(long newVal)
{
	// TODO: Add your implementation code here
	m_nDestination = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_LastShares(long *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_nLastShares;
	return S_OK;
}

STDMETHODIMP COrder::put_LastShares(long newVal)
{
	// TODO: Add your implementation code here
	m_nLastShares = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_ExecPrice(double *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_dExecPrice;
	return S_OK;
}

STDMETHODIMP COrder::put_ExecPrice(double newVal)
{
	// TODO: Add your implementation code here
	m_dExecPrice = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Strike(double *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_dStrike;
	return S_OK;
}

STDMETHODIMP COrder::put_Strike(double newVal)
{
	// TODO: Add your implementation code here
	m_dStrike = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Expiration(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szExpire.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_Expiration(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szExpire = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_CallPut(enum CallPut *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eCallPut;
	return S_OK;
}

STDMETHODIMP COrder::put_CallPut(enum CallPut newVal)
{
	// TODO: Add your implementation code here
	m_eCallPut = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_OpenClose(enum OpenClose *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eOpenClose;
	return S_OK;
}

STDMETHODIMP COrder::put_OpenClose(enum OpenClose newVal)
{
	// TODO: Add your implementation code here
	m_eOpenClose = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Text(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szText.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_Text(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szText = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_ExecID(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szExecID.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_ExecID(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szExecID = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_CMTA(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szCMTA.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_CMTA(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szCMTA = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_GiveUp(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szGiveUp.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_GiveUp(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szGiveUp = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_ShortLender(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szShortLender.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_ShortLender(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szShortLender = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_ExecInst(enum ExecInst *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eExecInst;
	return S_OK;
}

STDMETHODIMP COrder::put_ExecInst(enum ExecInst newVal)
{
	// TODO: Add your implementation code here
	m_eExecInst = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_Capacity(enum Capacity *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_eCapacity;
	return S_OK;
}

STDMETHODIMP COrder::put_Capacity(enum Capacity newVal)
{
	// TODO: Add your implementation code here
	m_eCapacity = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_MMAccount(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szMMAccount.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_MMAccount(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szMMAccount = newVal;
	return S_OK;
}

STDMETHODIMP COrder::get_HomeExchange(BSTR *pVal)
{
	// TODO: Add your implementation code here
	*pVal = m_szHomeExchange.copy();
	return S_OK;
}

STDMETHODIMP COrder::put_HomeExchange(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_szHomeExchange = newVal;
	return S_OK;
}