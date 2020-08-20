// OneChicagoFixSession.h: interface for the COneChicagoFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OneChicagoFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_)
#define AFX_OneChicagoFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptFixSession.h"
#include "OneChicagoMemTagSet.h"

typedef std::map<TString, TString>	MapDestToLinkDest;
typedef std::map<int, TString>		MapLinkageRejReasons;

class COneChicagoFixSession : public COptFixSession  
{
public:
	COneChicagoFixSession();
	virtual ~COneChicagoFixSession();
	virtual BOOL Logon();
	virtual void Process(const CFixLogout& a_Logout);
	virtual BOOL ProcessCancel(CCancel& a_Cancel);	
	virtual void Process(const CFixCancelReject& a_Reject);
	virtual long ProcessEntry(COrder& a_Order);
	virtual BOOL ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec);
	virtual BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
	virtual BOOL OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	virtual BOOL ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
	virtual BOOL SendRejectMsg(const long& a_RefSeqNo, const char* a_pText);
	virtual BOOL ProcessReplace(CReplace& a_Replace);
	
private:
	TString					m_SessionString;
	TString					m_UserNamePassword;
	TString					m_execBroker;
	TString					m_inCrowdExecBroker;
	TString					m_linkageExecBroker;
	MapDestToLinkDest		m_mapDestToLinkDest;
	MapLinkageRejReasons	m_mapLinkageRejReasons;

	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order)
	{
		if(GiveupAccount != "")
			a_FixOrder.ExecBroker(GiveupAccount);

		if(CMTAAccount != "")
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}

	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace)
	{
		if(GiveupAccount != "")
			a_FixReplace.ExecBroker(GiveupAccount);

		if(CMTAAccount != "")
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)439)->Value(CMTAAccount);
	}

	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixCancel& a_FixCancel, const CCancel& a_Cancel)
	{
		if(GiveupAccount != "")
			a_FixCancel.ExecBroker(GiveupAccount);
	}

};

#endif // !defined(AFX_OneChicagoFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_)
