// SendCancelReject.cpp: implementation of the CSendCancelReject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SendCancelReject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSendCancelReject::CSendCancelReject(const CCancel& Cancel)	:
	CCancelReject()
{

	m_Reason			= "Unable To Send To Exchange";
	m_StaffordTag	= Cancel.StaffordTag();
	m_OrderTag		= Cancel.OrderTag();
	m_ExchangeTag	= Cancel.ExchangeTag();
	m_CancelTag		= Cancel.CancelTag();
	m_Symbol			= Cancel.Symbol();
	m_SourceId		= Cancel.SourceId();

	m_Debug.Format("Cancel Rej(%s) Order(%s) Exchange(%s) Symbol(%s) Reason(%s)",
		lpszStaffordTag(),
		lpszOrderTag(),
		lpszExchangeTag(), 
		m_Symbol,
		m_Reason);
	

}

CSendCancelReject::~CSendCancelReject()
{

}
