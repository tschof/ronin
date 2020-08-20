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

#ifndef _GLOBEX_FIXSESSION_H__
#define _GLOBEX_FIXSESSION_H__

#include	<Winsock2.h>
#pragma  comment(lib, "Ws2_32.lib")

#include <iostream>
#include <map>

#include "Fix.h"
#include "FixMsgSequence.h"
#include "ROMMessage.h"
#include "FixMsg.h"



template <class T, class P, class R,
class FixLogon, class FixVersion,
class FixExecution, class FixSequenceReset >
class CGlobexFixSession {
public:
    explicit CGlobexFixSession(T* t, void(T::*_func)(std::string&),
        P* p, void(P::*_pfunc)(RFIX::CRCsv&),
        RFIX::CMsgSequence* incomming, RFIX::CMsgSequence* outgoing, 
        std::string& senderCompID, std::string& targetCompID, 
        std::string& fti, std::string& targetSubId, std::string& sessionId,
        std::string& senderSubId, std::string& senderLocId, std::string& fee,
        std::string& custormerOrFirm, std::string& customerTypeCode,
        std::string& password, std::string& firmId)
        
        :m_T(t), func(_func),m_P(p),pfunc(_pfunc),m_incoming(incomming),
        m_outgoing(outgoing), m_senderCompID(senderCompID),
        m_targetCompID(targetCompID),m_isLoggedOn(false), m_R(), m_cTestReq(0),
        m_fti(fti), m_targetSubId(targetSubId), m_sessionId(sessionId),
        m_senderSubId(senderSubId), m_senderLocId(senderLocId), 
        m_feeBilling(fee), m_customerOrFirm(custormerOrFirm),
        m_customerTypeCode(customerTypeCode), m_password(password),
        m_firmId(firmId), m_rejectResend(false){
        
        m_senderCompID += m_firmId;
        m_senderCompID += m_fti;
    }
    virtual ~CGlobexFixSession() {}
    
    inline void ProcessSessionMsg(RFIX::CFixMsg& sessionMsg) {
        m_dwLastMess = GetTickCount();
        long seqNum = sessionMsg.GetSequenceNum();
        RFIX::CMsgSequence::EAddMsgReturnCode code = m_incoming->AddMsg(sessionMsg.GetRawMessage(), seqNum); 

        if(code != RFIX::CMsgSequence::EAddMsgReturnCode::ADD_MSG_NO_ERROR) {
            std::cerr<<"SESSION FAILED TO ADD THIS SEQ "<<seqNum<<" error "<<code<<std::endl;
            if(code == RFIX::CMsgSequence::EAddMsgReturnCode::ADD_MSG_SEQ_NUM_GAP) {
                SetGlobexHeaderFields(sessionMsg);
                sessionMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID], m_senderCompID);
                sessionMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID], m_targetCompID);
                (m_T->*func)(sessionMsg.Process(m_incoming, m_outgoing));
                SendResendRequest(sessionMsg.GetValueForTag(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgSeqNum]));
                return;
            } else {
                SetGlobexHeaderFields(sessionMsg);
                sessionMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID], m_senderCompID);
                sessionMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID], m_targetCompID);          
                (m_T->*func)(sessionMsg.Process(m_incoming, m_outgoing));
            }
            
        }
        if((sessionMsg.GetValueForTag(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgType]) == "3")) {
            std::string rejectstr = sessionMsg.Process(m_incoming, m_outgoing);
            sessionMsg.AbsorbeMessage(rejectstr);
            RFIX::CRomMessage<RFIX::CRomStatus> romMsg;
            m_R.FixToRom(romMsg, sessionMsg);
            std::string exe("8");
            romMsg.SetAt(RFIX::CSV_ECHO, exe);
            romMsg.SetAt(RFIX::CSV_STATUS, RFIX::ROM_REJECT);
            (m_P->*pfunc)(romMsg);
            return;
        } else {
            SetGlobexHeaderFields(sessionMsg);
            sessionMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID], m_senderCompID);
            sessionMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID], m_targetCompID);       
            (m_T->*func)(sessionMsg.Process(m_incoming, m_outgoing));
        }
    }
    
    inline void ProcessApplicationMsg(RFIX::CFixMsg& fixMsg, RFIX::CRCsv& romMsg) {
        m_dwLastMess = GetTickCount();
        long seqNum = fixMsg.GetSequenceNum();
        RFIX::CMsgSequence::EAddMsgReturnCode code = m_incoming->AddMsg(fixMsg.GetRawMessage(), seqNum);
        if(code != RFIX::CMsgSequence::EAddMsgReturnCode::ADD_MSG_NO_ERROR) {
            std::cerr<<"APP FAILED TO ADD THIS SEQ "<<seqNum<<" error "<<code<<std::endl;
            if(code == RFIX::CMsgSequence::EAddMsgReturnCode::ADD_MSG_SEQ_NUM_GAP) {
                SetGlobexHeaderFields(fixMsg);
                SendResendRequest(fixMsg.GetValueForTag(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgSeqNum]));
            } else {
                
            }
            return;
        }
        m_R.FixToRom(romMsg, fixMsg);
        (m_P->*pfunc)(romMsg);
    }
    
    inline void Logon() {
        RFIX::CTypedFixMsg<FixVersion, FixLogon> msg;
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_RawData], m_password);
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_RawDataLength], "3");
        SetGlobexHeaderFields(msg);
        SetSeqAndSendToFix(msg);
    }
    
    inline void Logon(RFIX::CFixMsg& msg) {
        SetGlobexHeaderFields(msg);
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_RawData], m_password));
        SetSeqAndSendToFix(msg);
    }
    
    inline void Logout() {
        RFIX::CTypedFixMsg<FixVersion, CFixLogout> msg;
        long seq = m_outgoing->NextMsgSeqNum();
        char nextExp[50];
        memset(nextExp, '\0', 50);
        itoa(seq,nextExp,  10);
        msg.SetAt(RFIX::FIX_TAG_STRINGS[789],nextExp); 
        SetGlobexHeaderFields(msg);
        SetSeqAndSendToFix(msg);
    }
    
    
    inline bool SendHeartBeat() {
        
        if(m_isLoggedOn) {
            DWORD interval = GetTickCount() - m_dwLastMess;
            if(interval <= RFIX::HB_INTERVAL) {
                RFIX::CTypedFixMsg<FixVersion, RFIX::CFixHeartbeat> msg;
                SetGlobexHeaderFields(msg);
                SetSeqAndSendToFix(msg);
                m_cTestReq = 0;
                return true;
            } else if(interval > RFIX::HB_INTERVAL && interval <= RFIX::TR_INTERVAL) {
                SendTestReq();
                m_cTestReq += 1;
                return true;
            } else if(m_cTestReq <= 1) {
                SendTestReq();
                m_cTestReq += 1;
                return true;
            } else {
                std::cerr<<"TIMEOUT "<<interval<<std::endl;
                m_isLoggedOn = false;
                return false;
            }           
        } else {
            return false;
        }
    }
    
    
    inline void IsLoggedOn(bool isLoggedOn) {
        m_isLoggedOn = isLoggedOn;
    }
    
    inline bool LoggedOn() {
        return m_isLoggedOn;
    }
    
    
private:
    
    inline void SendTestReq() {
        RFIX::CTypedFixMsg<FixVersion, RFIX::CFixTestRequest<FixVersion> > msg;
        SetGlobexHeaderFields(msg);
        SetSeqAndSendToFix(msg);
    }
    
    inline void SendResendRequest(std::string& seqNum) {
        RFIX::CTypedFixMsg<FixVersion, RFIX::CFixResendRequest<P,R, FixVersion,
            FixExecution, FixSequenceReset> > msg;
        SetGlobexHeaderFields(msg);
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_EndSeqNo], seqNum);
        char begSeqNum[50];
        memset(begSeqNum, '\0', 50);
        itoa(m_incoming->NextMsgSeqNum(),begSeqNum,  10);
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_BeginSeqNo], begSeqNum);
        SetSeqAndSendToFix(msg);
    }
    
    inline void SetGlobexHeaderFields(RFIX::CFixMsg& msg) {       
        msg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID], m_senderCompID);
        msg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID], m_targetCompID);
        msg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderSubID], m_senderSubId);
        msg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetSubID], m_targetSubId);
        msg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderLocationID], m_senderLocId);
    }
    
    inline void SetSeqAndSendToFix(RFIX::CFixMsg& msg) {
        long seq = m_outgoing->NextMsgSeqNum();
        msg.SetMsgSeqNum(seq);
        std::string str = msg.GetFormattedMsg();
        m_outgoing->AddMsg(str, seq);
        (m_T->*func)(str);
    }
    
    
    void (T::*func)(std::string&);
    T* m_T;
    void (P::*pfunc)(RFIX::CRCsv&);
    P* m_P;
    char m_seqNum[50];
    RFIX::CMsgSequence* m_incoming;
    RFIX::CMsgSequence* m_outgoing;
    std::string m_senderCompID;
    std::string m_targetCompID;
    bool m_isLoggedOn;
    DWORD m_dwLastMess;
    int m_cTestReq;
    R m_R;
    
    std::string m_feeBilling;
    std::string m_customerOrFirm;
    std::string m_firmId;
    std::string m_fti;
    std::string m_senderSubId;
    std::string m_targetSubId;
    std::string m_senderLocId;
    std::string m_sessionId;
    std::string m_customerTypeCode;
    std::string m_password;
    bool m_rejectResend;
    
};



#endif //_GLOBEX_FIXSESSION_H__