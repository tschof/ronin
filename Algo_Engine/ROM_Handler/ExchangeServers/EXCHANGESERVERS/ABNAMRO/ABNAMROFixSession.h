// ABNAMROFixSession.h: interface for the CABNAMROFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABNAMROFIXSESSION_H__FD37FA29_D3CB_4C00_AFFD_6A3268CE2167__INCLUDED_)
#define AFX_ABNAMROFIXSESSION_H__FD37FA29_D3CB_4C00_AFFD_6A3268CE2167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptFixSession.h"

typedef std::map<TString, TString>	MapExDestToABNAMRODest;

class CABNAMROFixSession : public COptFixSession  
{
public:
	CABNAMROFixSession();
	virtual ~CABNAMROFixSession();
	long ProcessEntry(COrder& a_Order);
	BOOL ProcessCancel(CCancel& a_Cancel);

protected:
	BOOL OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
	void Process(const CFixExecution& a_Exec);
	void Process(const CFixNews & a_News);
	void Process(const CFixResendRequest& a_Request);
private:
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order);
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace);

protected:
	MapExDestToABNAMRODest	m_mapExDestToABNAMRODest;
	CString					m_strDefaultCMTA;
};

#endif // !defined(AFX_ABNAMROFIXSESSION_H__FD37FA29_D3CB_4C00_AFFD_6A3268CE2167__INCLUDED_)
