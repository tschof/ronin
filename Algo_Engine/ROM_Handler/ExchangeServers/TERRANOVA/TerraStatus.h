// TerraStatus.h: interface for the CArcaStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRASTATUS_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_)
#define AFX_TERRASTATUS_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ExecutionMapSet.h"

class CArcaStatus : public CStatusHandler  
{

public:
	CArcaStatus();
	virtual ~CArcaStatus();

	BOOL	SetStatus(CCancel&				cancel);
	BOOL	SetStatus(CCancelReject&		cancelReplace);
	BOOL	SetStatus(CExecution&			execution);
	BOOL	SetStatus(COrder&				order);
	BOOL	SetStatus(CReject&				reject);
	BOOL	SetStatus(CReplace&				replace);
	BOOL	SetStatus(CReplaceReject&		replaceReject);
};

#endif // !defined(AFX_TERRASTATUS_H__21667103_2B97_11D2_8D12_006008CD4894__INCLUDED_)
