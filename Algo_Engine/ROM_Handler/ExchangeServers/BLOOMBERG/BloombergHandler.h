// BloombergHandler.h: interface for the CBloombergHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOOMBERGHANDLER_H__F46917C9_DFCC_11D3_A600_00500401F87A__INCLUDED_)
#define AFX_BLOOMBERGHANDLER_H__F46917C9_DFCC_11D3_A600_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OrderHandler.h"
#include "TagMapSet.h"
#include "memtagset.h"
#include "BloombergMemTagSet.h"


class CBloombergHandler : public COrderHandler  
{
public:
	// incoming
	virtual void ProcessMessage(COrder& Order);
	virtual void ProcessMessage(CCancel& Order);
	
	// outgoing
	virtual void ProcessMessage(const char *Status);
	virtual void ProcessMessage(CExecution& Report);
	virtual void ProcessMessage(CCancelReject& Reject);
	virtual void ProcessMessage(CReject& Reject);
	virtual void ProcessMessage(CReplace& Replace);

private:
	CBloombergMemTagSet *m_pTags;
	CString	m_DefaultAccount;

public:
	CBloombergHandler();
	virtual ~CBloombergHandler();

};

#endif // !defined(AFX_BLOOMBERGHANDLER_H__F46917C9_DFCC_11D3_A600_00500401F87A__INCLUDED_)
