/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILLFixSession.h: interface for the CMERRILLFixSession class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#if !defined(AFX_MERRILLFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_)
#define AFX_MERRILLFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "fixmsg42\fixsession.h"
#include "fixmsg42\FixAppMsgs.h"
#include "fixmsg42\FixAdminMsgs.h"
#include "fixmsg42\Fix.h"

typedef std::map<TString, TString>	MapDestToLinkDest;
typedef std::map<char, TString>		MapOrderTypeStrings;
typedef std::map<char, TString>		MapTIFStrings;
typedef std::map<TString, TString>	MapOnBehalfOfCompID;

enum EFixFieldID_ML_Ext { FIX_ShortLender = 5700 };

class CMERRILLFixSession : public CFixSession
{
public:
	CMERRILLFixSession();
	virtual ~CMERRILLFixSession();
	virtual BOOL Logon();
	virtual void Process(const CFixLogout& a_Logout);
	virtual BOOL ProcessCancel(CCancel& a_Cancel);
	//virtual void Process(const CFixCancelReject& a_Reject);
	virtual long ProcessEntry(COrder& a_Order);
	virtual BOOL ProcessReplace(CReplace& a_Replace);
	virtual void Process(const CFixExecution& a_Exec);
	virtual BOOL ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec);
	virtual BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
	virtual BOOL OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	virtual BOOL ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
	void InitializeFixCustomTags();
private:
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixOrder& a_FixOrder, const COrder& a_Order);
	void ProcessCMTAGiveup(CString& GiveupAccount, CString& CMTAAccount, CFixReplace& a_FixReplace, const CReplace& a_Replace);
//	void ParseOnBehalfOfCompID( const std::string & str, std::vector<std::string> & fields, char token, char end = '\n' );
//	void ParseOnBehalfOfCompIDMap( );
//	std::string trim( const std::string & str );
private:
	MapOrderTypeStrings m_mapOrderTypeStrings;
	MapTIFStrings		m_mapTIFStrings;
	MapOnBehalfOfCompID	m_mapOnBehalfOfCompID;
	TString				m_execBroker;
	TString				m_defaultOnBehalfOfCompID;
};

#endif // !defined(AFX_MERRILLFIXSESSION_H__25E67653_44CE_4F69_B802_B0032E70A5C0__INCLUDED_)

