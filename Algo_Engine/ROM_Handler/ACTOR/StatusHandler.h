// StatusHandler.h: interface for the StatusHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSHANDLER_H__21667102_2B97_11D2_8D12_006008CD4894__INCLUDED_)
#define AFX_STATUSHANDLER_H__21667102_2B97_11D2_8D12_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CStatusHandler  
{
public:
	virtual BOOL	SetStatus(COrder&				Order)	= 0;
	virtual BOOL	SetStatus(CExecution&		Exec)		= 0;
	//virtual BOOL	SetStatus(CCancel&			Cancel)	= 0;
	//virtual BOOL	SetStatus(CReject&			Reject)	= 0;
	//virtual BOOL	SetStatus(CCancelReject&	Reject)	= 0;

public:
	CStatusHandler()				{	};
	virtual ~CStatusHandler()	{	};

};

#endif // !defined(AFX_STATUSHANDLER_H__21667102_2B97_11D2_8D12_006008CD4894__INCLUDED_)
