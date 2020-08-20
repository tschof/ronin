// ISEFixSession.h: interface for the CISEFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISEFIXSESSION_H__FD37FA29_D3CB_4C00_AFFD_6A3268CE2167__INCLUDED_)
#define AFX_ISEFIXSESSION_H__FD37FA29_D3CB_4C00_AFFD_6A3268CE2167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptFixSession.h"

class CISEFixSession : public COptFixSession  
{
public:
	CISEFixSession();
	virtual ~CISEFixSession();
	long ProcessEntry(COrder& a_Order);
	BOOL ProcessReplace(CReplace& a_Replace);
	BOOL ProcessCancel(CCancel& a_Cancel);
	BOOL ProcessSpread(OrderVector& orderVector);
protected:
	BOOL OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
	BOOL ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
	void Process(const CFixExecution& a_Exec);
	void Process(const CFixNews & a_News);
	void Process(const CFixCancelReject& a_Reject);
private:
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order);
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace);
private:
	bool m_bIgnoreAwayMarketForTNTO;
	bool m_bIgnoreNewsMsg;
};

#endif // !defined(AFX_ISEFIXSESSION_H__FD37FA29_D3CB_4C00_AFFD_6A3268CE2167__INCLUDED_)
