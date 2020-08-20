// IslandStatus.h: interface for the CIslandStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDSTATUS_H__AF6E8EBA_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDSTATUS_H__AF6E8EBA_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StatusHandler.h"

class CIslandStatus : public CStatusHandler  
{
public:
	BOOL	SetStatus(COrder&				Order);
	BOOL	SetStatus(CExecution&		Exec);
	BOOL	SetStatus(CCancel&			Cancel);
	BOOL	SetStatus(CReject&			Reject);
	BOOL	SetStatus(CCancelReject&	Reject);

	BOOL	ProcessTrade(CExecution& Exec);

public:
	CIslandStatus();
	virtual ~CIslandStatus();

};

#endif // !defined(AFX_ISLANDSTATUS_H__AF6E8EBA_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
