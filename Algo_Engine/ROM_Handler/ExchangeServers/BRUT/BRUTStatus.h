/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *   
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 11-11-2005   Parnell    Added the overloaded function SetStatus(CReplace). 
 *******************************************************************************/

// BRUTStatus.h: interface for the CBRUTStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRUTStatus_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_)
#define AFX_BRUTStatus_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ExecutionMapSet.h"

class CBRUTStatus : public CStatusHandler  
{
private:
	CExecutionMapSet *m_pExecMap;

public:
	BOOL	SetStatus(COrder&				Order);
	BOOL	SetStatus(CExecution&		Exec);
	BOOL	SetStatus(CCancel&			Cancel);
    BOOL    SetStatus(CReplace&         Replace);
	BOOL	SetStatus(CReject&			Reject);
	BOOL	SetStatus(CCancelReject&	Reject);
	
private:
	BOOL ProcessTrade(CExecution& Exec);

public:
	CBRUTStatus();
	virtual ~CBRUTStatus();

};

#endif // !defined(AFX_BRUTStatus_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_)
