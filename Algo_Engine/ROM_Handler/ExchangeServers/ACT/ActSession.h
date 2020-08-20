// ActSession.h: interface for the CFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXSESSION_H__E69D7592_E5AA_11D1_8D02_006008CD4894__INCLUDED_)
#define AFX_FIXSESSION_H__E69D7592_E5AA_11D1_8D02_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Act.h"


class CActSession //: public CExchangeSession
{
public:

	CActSession();
	virtual ~CActSession();

public:
	
	virtual void		ProcessTimer();
	long				ProcessActEntry(CCsv*);
	virtual long		ProcessEntry(COrder& Order) { return 0;}
	void				ProcessMessage(const char *Msg, int Size);

private:
	CString m_Date;
	CPtrList m_ActMsgList;
	CTime m_LastSendTime;	
	int m_HeartbtInt;

private:

	void ProcessLogonResponse();
	//void ProcessHeartBeatQuery();
	void ProcessHeartBeatResponse();
	//void ProcessFlowControl(const char *Msg);
	void ProcessChannelStateQuery(const char *Msg);
	//void ProcessChannelStateResponse(const char *Msg);
	void ProcessActResponse(const char* Msg, u_short Size);
	//void ProcessAct(CString Msg, int Size);
	//void ProcessLogon(const char *Msg);
	//void ProcessHeartBeat();

	BOOL SendLogin();
	BOOL SendAct(const char *Act);
	BOOL SendActMsgs();

	BOOL SendChannelState(BYTE Channel);
	BOOL SendHeartBeat();

	void ConnectionCheck();

	void TrimLeadingZeros(CString &s);

	//void SetEnvelope(char * MsgBuffer, int BodySize);
};

#endif // !defined(AFX_FIXSESSION_H__E69D7592_E5AA_11D1_8D02_006008CD4894__INCLUDED_)
