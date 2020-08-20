#include "stdafx.h"
#include "Message.h"
#include "Cancel.h"
#include "Order.h"
#include "Replace.h"
#include "BrassReject.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

CBrassReject::CBrassReject(const CString& Reason, const CBrassReplace& Replace)
{
	m_Reason		= Reason;
	m_StaffordTag	= Replace.StaffordTag();
	m_OrderTag		= Replace.OrderTag();
	m_ExchangeTag	= Replace.ExchangeTag();
	m_CancelTag		= Replace.CancelTag();
	m_Symbol		= Replace.Symbol();
	m_SourceId		= Replace.SourceId();

	m_Debug.Format("Rej(%s) Replace(%s) Exchange(%s) Symbol(%s) Reason(%s)",
		lpszStaffordTag(),
		lpszOrderTag(),
		lpszExchangeTag(), 
		m_Symbol,
		m_Reason);
}
