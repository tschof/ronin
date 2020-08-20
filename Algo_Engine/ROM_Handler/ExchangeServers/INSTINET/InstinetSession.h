// InstinetSession.h: interface for the CInstinetSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETSESSION_H__E0817969_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_INSTINETSESSION_H__E0817969_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ExchangeSession.h"
#include "FixSequenceSet.h"
#include "OutgoingMessageSet.h"
#include "IncomingMessageSet.h"

class CInstinetSession : public CExchangeSession  
{
public:
	CFixSequenceSet		*GetSequenceSet() { return m_pSequenceSet; }
	COutgoingMessageSet	*GetOutgoingSet() { return m_pOutgoingSet; }
	CIncomingMessageSet	*GetIncomingSet() { return m_pIncomingSet; }

	void	ResetSequenceNumber();
	void	ProcessTimer();
	BOOL	SendMessage(const CFix&);
	CString	ProcessMessage(CString, CConnection*);

	// overridables
	virtual	BOOL StartLogon() { return m_InitiateLogon; }
	virtual	BOOL Logon();
	virtual	BOOL Logout();
	virtual	long ProcessEntry(COrder& Order);
	virtual	BOOL ProcessCancel(CCancel& Cancel);
	virtual	BOOL ProcessReplace(CReplace& Replace);
	virtual	BOOL ProcessStatusRequest(CRequest& Order);

private:
	int		m_IncomingHeartBeatInterval;
	BOOL	m_InitiateLogon;
	CTime	m_LastTestRequest;

	// data sets
	CFixSequenceSet		*m_pSequenceSet;
	COutgoingMessageSet	*m_pOutgoingSet;
	CIncomingMessageSet	*m_pIncomingSet;

private:
	// message processors
	void AddIncomingMessage(const CFixHeader& Header, const CString& Message) { m_pIncomingSet->AddMessage(Header, Message); }
	void AddOutgoingMessage(const CFixHeader& Header, const CString& Message) { m_pOutgoingSet->AddMessage(Header, Message); }
	void ProcessLogon(const CFixHeader& pHeader, CString& Message);
	void ProcessHeartBeat(const CFixHeader& pHeader, CString& Message);
	void ProcessTestRequest(const CFixHeader& pHeader, CString& Message);
	void ProcessSequenceReset(const CFixHeader& pHeader, CString& Message);
	void ProcessReject(const CFixHeader& pHeader, CString& Message);
	void ProcessLogout(const CFixHeader& pHeader, CString& Message);
	void ProcessResendRequest(const CFixHeader& pHeader, CString& Message);
	void ProcessExecutionReport(const CFixHeader& pHeader, CString& Message);
	void ProcessCancelReject(const CFixHeader& pHeader, CString& Message);

	void ProcessIOI(const CFixHeader& Header, CString& Message);

	// utilities
	BOOL	ProcessSequence(int);
	CString	StripMessage(CString *WholeMessage);
	BOOL	IsDuplicate(long SequenceNumber);
	BOOL	Connect();

public:
	CInstinetSession();
	virtual ~CInstinetSession();

};

#endif // !defined(AFX_INSTINETSESSION_H__E0817969_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
