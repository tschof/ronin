// OptFixSession.h: interface for the COptFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTFIXSESSION_H__35E92E74_9EF5_4A04_8D76_C603F9C65A02__INCLUDED_)
#define AFX_OPTFIXSESSION_H__35E92E74_9EF5_4A04_8D76_C603F9C65A02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _FIX_42_OPTIONS
	#include "fixsession\FixAppMsgs.h"
	#include "fixsession\fixsession.h"
#else
	#include "fixmsg42\FixAppMsgs.h"
	#include "fixmsg42\fixsession.h"
#endif

#include "messageapp.h"

#define	FromAMEX "LKAA"
#define	FromCBOE "LKAC"
#define	FromISE "LKAI"
#define	FromPCX	"LKAP"
#define	FromPHLX "LKAX"

typedef std::map<char, TString>		MapOrderTypeStrings;
typedef std::map<char, TString>		MapTIFStrings;

class COptFixSession : public CFixSession  
{
public:
	COptFixSession();
	virtual ~COptFixSession();

private:
	inline virtual void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order) {}
	inline virtual void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace) {}
	inline virtual void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixCancel& a_FixCancel, const CCancel& a_Cancel) {}
public:
	void COptFixSession::ProcessEmail(CString& a_Subject, CString& a_Text);

protected:
	BOOL	OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	BOOL	CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
	BOOL	ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
	BOOL	ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec);
	const	char const* GetLinkString(int a_HomeEx);

protected:
	MapOrderTypeStrings m_mapOrderTypeStrings;
	MapTIFStrings		m_mapTIFStrings;
};

#endif // !defined(AFX_OPTFIXSESSION_H__35E92E74_9EF5_4A04_8D76_C603F9C65A02__INCLUDED_)
