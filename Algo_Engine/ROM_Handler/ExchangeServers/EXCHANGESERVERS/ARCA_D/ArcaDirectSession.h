/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *  : Defines the class behaviors for the application.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *     Parnell     
 *
 *******************************************************************************/

#ifndef ARCADIRECTSESSION_H__
#define ARCADIRECTSESSION_H__
#include "stdafx.h"
#include <sstream>
#include <iostream>
#include "ExchangeSession.h"
#include "Execution.h"
#include "ArcaDirectSequenceSet.h"
#include "ArcaDirectMessage.h"
#include "ArcaDirectSequenceSet.h"

class ArcaDirectSession : public CExchangeSession {

private:
    CString m_Account;
    CString m_Password;
    char m_clientID[ArcaDirect::ID_STRING_LEN];
    char m_arcaID[ArcaDirect::ID_STRING_LEN];
    char m_reason[ArcaDirect::REJECT_REASON_LEN];
    int m_IncomingHeartBeatInterval;
    BOOL m_InitiateLogon;
    CArcaDirectSequenceSet m_sequenceSet;
    int m_loginSeqNum;
    void ProcessLogon();
    void ProcessLogonReject(std::stringstream& data);
    void ProcessTestRequest();
    void ProcessHeartBeat();
    void ProcessAckMessage(std::stringstream& data, 
                           int status);
    void ProcessExecutionReport(std::stringstream& data);
    void ProcessOrderCancelled(std::stringstream& data);
    void ProcessReject(std::stringstream& data);
    void ProcessOrderReplaced(std::stringstream& data);
    void ProcessBust(std::stringstream& data);

public:
    ArcaDirectSession();
    virtual ~ArcaDirectSession();

    void	ProcessTimer();
    BOOL	SendMessage(const char* message, int length);
    CString	ProcessMessage(CString, CBaseConnection *pC){ 
        
        return "";
    }
    int ProcessMessage(std::stringstream& data, int length, CBaseConnection *pC);

    // overridables
    virtual	BOOL StartLogon() { return m_InitiateLogon; }
    virtual BOOL Logon();
    virtual	BOOL Logout();
    virtual	long ProcessEntry(COrder& Order);
    virtual	BOOL ProcessCancel(CCancel& Cancel);
    virtual	BOOL ProcessReplace(CReplace& Replace);
    virtual	BOOL ProcessStatusRequest(CRequest& Order);

    int GetClientOrderId(char idStr[12]);
    void SetClientOrderID(int id);
    void SetNextIncomingSeqNum(int seq);
    void SetNextOutgoingSeqNum(int seq);
    int NextIncomingSeqNum();
    int NextOutgoingSeqNum();
    int GetNextIncomingSeqNum();
    int GetNextOutgoingSeqNum();
};

#endif //ARCADIRECTSESSION_H__
