// TerraStatus.h: interface for the CTerraStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRASTATUS_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_)
#define AFX_TERRASTATUS_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "ExecutionMapSet.h"

class CTerraStatus : public CStatusHandler  
{
private:
	//CExecutionMapSet *m_pExecMap;

public:
	BOOL	SetStatus(COrder&				Order);
	BOOL	SetStatus(CExecution&		Exec);
	//BOOL	SetStatus(CCancel&			Cancel);
	//BOOL	SetStatus(CReject&			Reject);
	//BOOL	SetStatus(CCancelReject&	Reject);
	
private:
	BOOL ProcessTrade(CExecution& Exec);

public:
	CTerraStatus();
	virtual ~CTerraStatus();

};

#endif // !defined(AFX_TERRASTATUS_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_)
