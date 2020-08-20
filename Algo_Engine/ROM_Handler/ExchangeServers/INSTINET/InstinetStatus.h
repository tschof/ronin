// InstinetStatus.h: interface for the CInstinetStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETSTATUS_H__E0817968_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_INSTINETSTATUS_H__E0817968_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StatusHandler.h"
#include "ExecutionMapSet.h"

class CInstinetStatus : public CStatusHandler  
{
private:
	CExecutionMapSet *m_pExecMap;

public:
	BOOL SetStatus(COrder&			Order);
	BOOL SetStatus(CExecution&		Exec);
	BOOL SetStatus(CCancel&			Cancel);
	BOOL SetStatus(CReject&			Reject);
	BOOL SetStatus(CCancelReject&	Reject);
	BOOL ProcessTrade(CExecution&	Exec);

public:
	CInstinetStatus();
	virtual ~CInstinetStatus();

};

#endif // !defined(AFX_INSTINETSTATUS_H__E0817968_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
