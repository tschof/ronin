// BloombergStatus.h: interface for the CBloombergStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOOMBERGSTATUS_H__F46917CA_DFCC_11D3_A600_00500401F87A__INCLUDED_)
#define AFX_BLOOMBERGSTATUS_H__F46917CA_DFCC_11D3_A600_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatusHandler.h"
#include "ExecutionMapSet.h"

class CBloombergStatus : public CStatusHandler  
{
private:
	CExecutionMapSet *m_pExecMap;
	BOOL ProcessTrade(CExecution& Exec);

public:
	BOOL SetStatus(COrder&			Order);
	BOOL SetStatus(CExecution&		Exec);
	BOOL SetStatus(CCancel&			Cancel);
	BOOL SetStatus(CReject&			Reject);
	BOOL SetStatus(CCancelReject&	Reject);
	BOOL SetStatus(CReplaceReject&  Reject);

public:
	CBloombergStatus();
	virtual ~CBloombergStatus();

};

#endif // !defined(AFX_BLOOMBERGSTATUS_H__F46917CA_DFCC_11D3_A600_00500401F87A__INCLUDED_)
