// ExchangeStatus.h: interface for the CExchangeStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCHANGESTATUS_H__3BFF7155_63DB_4E4A_A6F9_7DAF600B69A6__INCLUDED_)
#define AFX_EXCHANGESTATUS_H__3BFF7155_63DB_4E4A_A6F9_7DAF600B69A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExecutionMapSet.h"
#include "StatusHandler.h"

class CExchangeStatus : public CStatusHandler  
{
public:
	CExchangeStatus();
	virtual ~CExchangeStatus();

public:
	BOOL	SetStatus(COrder&			Order);
	BOOL	SetStatus(CExecution&		Exec);
	BOOL	SetStatus(CCancel&			Cancel);
	BOOL	SetStatus(CReject&			Reject);
	BOOL	SetStatus(CCancelReject&	Reject);
	BOOL	SetStatus(CReplaceReject&	Reject);
	BOOL	SetStatus(CReplace&			Replace);
	
private:
	int					m_nExchangeID;
};



#endif // !defined(AFX_EXCHANGESTATUS_H__3BFF7155_63DB_4E4A_A6F9_7DAF600B69A6__INCLUDED_)
