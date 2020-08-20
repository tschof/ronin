// CMSHandler.h: interface for the CCMSHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMSHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
#define AFX_CMSHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Exchangehandler.h"
#include "CMSSession.h"

class CCMSHandler : public CExchangeHandler  
{
public:
	CCMSHandler();
	virtual ~CCMSHandler()
	{
		delete m_pTags;
	}

protected:
	virtual inline BOOL OnProcessOrder(COrder& Order)
	{
		Order.SetSecurityType(COrder::Option);
		return m_pApp->Exchange().ProcessEntry(Order);
	}

	virtual inline BOOL OnProcessCancel(CCancel& Cancel)
	{
		Cancel.SetLeavesQty(m_pTags->LeavesShares());
		return m_pApp->Exchange().ProcessCancel(Cancel);
	}

	virtual inline BOOL OnProcessReplace(CReplace& Replace)
	{
		//do not do replace
		return false;
	}

	virtual inline BOOL OnProcessExecution(CExecution& Execution)
	{
		return TRUE;
	}

	inline BOOL OnProcessCancelReject(CCancelReject& Reject)
	{
		return TRUE;
	}
	inline BOOL OnProcessReplaceReject(CReplaceReject& Reject)
	{
		return TRUE;
	}
	inline BOOL OnProcessReject(CReject& Reject)
	{
		return TRUE;
	}

	virtual inline CString& GetNewOrdTagString(long SeqNum)
	{
		m_OrdTag.Format("%s %04d",((CCMSSession*) &(m_pApp->Exchange()))->GetBranchCode(),SeqNum);
		return m_OrdTag;
	}

protected:
	CString m_OrdTag;
};

#endif // !defined(AFX_CMSHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
