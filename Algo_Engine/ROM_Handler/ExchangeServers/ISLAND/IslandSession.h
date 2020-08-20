// IslandSession.h: interface for the IslandSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDSESSION_H__AF6E8EB8_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDSESSION_H__AF6E8EB8_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ExchangeSession.h"
//#include "FixSequenceSet.h"
#include "IslandSequenceSet.h"
#include "IslandLogin.h"
#include "IslandLogout.h"
#include "IslandHeartbeat.h"

class CIslandSession : public CExchangeSession  
{
private:
	CString	m_Account;
	CString	m_Password;

public:
	//CIslandSequenceSet	*GetSequenceSet() { return m_pSequenceSet; }

	void	ProcessTimer();
	BOOL	SendMessage(CIslandMessage&);
	CString	ProcessMessage(CString, CBaseConnection *pC);

	// overridables
	virtual	BOOL StartLogon() { return m_InitiateLogon; }
	virtual BOOL Logon();
	virtual	BOOL Logout();
	virtual	long ProcessEntry(COrder& Order);
	virtual	BOOL ProcessCancel(CCancel& Cancel);
	virtual	BOOL ProcessReplace(CReplace& Replace);
	virtual	BOOL ProcessStatusRequest(CRequest& Order);

private:
	int		m_IncomingHeartBeatInterval;
	BOOL	m_InitiateLogon;

	// data sets
	CIslandSequenceSet m_sequenceSet;

private:
	// message processors
	void ProcessLogon();
	void ProcessHeartBeat(const CIslandHeader& Header, const CString& Message);
	void ProcessReject(const CString& Message);
	void ProcessLogout(const CIslandHeader& pHeader);
	void ProcessExecutionReport(const CIslandHeader& Header, const CString& Message);
	void ProcessCancelReject(const CString& Message);
	void ProcessText(const CString& Message);
	void ProcessEvent(const CString& Message);

	// utilities
	BOOL ProcessSequence(int Received, int Expected);

public:
	CIslandSession();
	virtual ~CIslandSession();
	
	void SetNextIncomingSeqNum(int seq);
	void SetNextOutgoingSeqNum(int seq);
	int NextIncomingSeqNum()	{ return m_sequenceSet.NextIn(); }
	int NextOutgoingSeqNum() { return m_sequenceSet.NextOut(); }

	int GetNextIncomingSeqNum();
	int GetNextOutgoingSeqNum();
};

#endif // !defined(AFX_ISLANDSESSION_H__AF6E8EB8_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
