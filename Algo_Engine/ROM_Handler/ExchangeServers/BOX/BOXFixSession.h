// BOXFixSession.h: interface for the CBOXFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOXFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_)
#define AFX_BOXFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptFixSession.h"

typedef std::map<TString, TString>	MapDestToLinkDest;

class CBOXFixSession : public COptFixSession
{
public:
	CBOXFixSession();
	virtual ~CBOXFixSession();
	virtual BOOL Logon();
	virtual BOOL Logout();
	virtual void Process(const CFixLogout& a_Logout);
	virtual BOOL ProcessCancel(CCancel& a_Cancel);
	virtual void Process(const CFixCancelReject& a_Reject);
	virtual long ProcessEntry(COrder& a_Order);
	virtual BOOL ProcessReplace(CReplace& a_Replace);
	virtual void Process(const CFixExecution& a_Exec);
	virtual BOOL ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec);
	virtual BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
	virtual BOOL OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	virtual BOOL ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
	virtual void Process(const CFixResendRequest& a_Request);
	virtual bool AddIncomingMsgToSequence(const CFixMsg& a_Msg);
	
	void	ReloadFirmCombinedOrderType();

private:
	MapDestToLinkDest	m_mapDestToLinkDest;
	int					m_nCombinedOrderType;

private:
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order);
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace);
};

#endif // !defined(AFX_BOXFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_)
