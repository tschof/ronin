/*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : The Handler class takes the place of the RomMessage createtor.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/

#ifndef _GLOBEX_LOGON_H__
#define _GLOBEX_LOGON_H__

#include "FixMsgSequence.h"
#include "FixMsg.h"
#include "Fix.h"
#include "FixValidators.h"

class CGlobexAdminMsg {
public:
    virtual ~CGlobexAdminMsg() {}
    inline void Validate(RFIX::TFixMsg& msg) {
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderSubID], m_senderSubId));
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetSubID], m_targetSubId));
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderLocationID], m_senderLocId));
        DoValidate(msg);
    }
    inline void Process(RFIX::CMsgSequence* in, RFIX::CMsgSequence* out, 
        std::string& output, RFIX::TFixMsg& msg) {
        DoProcess(in, out, output, msg);
    }
protected:
    explicit CGlobexAdminMsg(const char* datFile, const char* value) 
        : m_value(value){
        char pszPath[MAX_PATH];
        GetCurrentDirectory( sizeof(pszPath), pszPath );
        strcat( pszPath, "\\" );
        strcat( pszPath, datFile);
        std::string buffer;
        buffer.reserve(129);
        
        GetPrivateProfileString("Globex Information","SenderSubId","",(char*) buffer.c_str(),128,pszPath);
        m_senderSubId = buffer.c_str();
        GetPrivateProfileString("Globex Information","targetSubId","",(char*) buffer.c_str(),128,pszPath);
        m_targetSubId = buffer.c_str();
        GetPrivateProfileString("Globex Information","SenderLocationID","",(char*) buffer.c_str(),128,pszPath);
        m_senderLocId = buffer.c_str();
    }
    
    virtual void DoValidate(RFIX::TFixMsg& msg) = 0;
    virtual void DoProcess(RFIX::CMsgSequence* in, RFIX::CMsgSequence* out, 
        std::string& output, RFIX::TFixMsg& msg) = 0;
    std::string m_value;
    std::string m_senderSubId;
    std::string m_targetSubId;
    std::string m_senderLocId;
    
};
class CGlobexSequenceReset : public CGlobexAdminMsg{
public:
    CGlobexSequenceReset(): CGlobexAdminMsg("Globex.dat", "4") {}
    virtual ~CGlobexSequenceReset() {}
protected:
    
    inline void DoValidate(RFIX::TFixMsg& msg) {
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgType], m_value));
        RFIX::TFixMapIter iter =  msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderSubID]);
        if(iter == msg.end()) {
            msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderSubID], m_senderSubId));
        }
        iter =  msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetSubID]);
        if(iter == msg.end()) {
            msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetSubID], m_targetSubId));
        }
        iter =  msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderLocationID]);
        if(iter == msg.end()) {
            msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderLocationID], m_senderLocId));
        }
        char currUTCTime[50];
        SYSTEMTIME systime;
        GetSystemTime(&systime);
        memset(currUTCTime, '\0', 50);
        wsprintf(currUTCTime, "%04u%02u%02u-%02u:%02u:%02u.%03u",                
            systime.wYear, systime.wMonth, systime.wDay,
            systime.wHour, systime.wMinute, systime.wSecond,
            systime.wMilliseconds);
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_OrigSendingTime], currUTCTime));
    }
    inline void DoProcess(RFIX::CMsgSequence* in, RFIX::CMsgSequence* out, 
        std::string& output, RFIX::TFixMsg& msg) {
        std::cerr<<"FIX SEQ RESET!!!!!"<<std::endl;
        long seq = 0;
        
        RFIX::TFixMapIter end = msg.end();
        RFIX::TFixMapIter iter =  msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_GapFillFlag]);
        if(iter != end && iter->second == RFIX::GAP_FILL_FLAG) {           
            iter =  msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_NewSeqNo]);
            if(iter != end) {
                std::string seqStr = iter->second;
                seq = atoi(seqStr.c_str());
                std::cerr<<"Reseting Incoming Seq to: "<<seq<<std::endl;
                in->SetNextMsgSeqNum(seq);
            }
            
        } else {
            iter =  msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgSeqNum]);
            if(iter != end) {
                std::string seqStr = iter->second;
                seq = atoi(seqStr.c_str());
                in->SetNextMsgSeqNum(seq);
                std::cerr<<"Reseting Incoming Seq to: "<<seq<<std::endl;
            }
            
        }
        
    }
};

class CGlobexLogon {
public:
    CGlobexLogon():m_value("A"), m_resetSeq("N") {
        
    }
    virtual ~CGlobexLogon() {}
    
    void Validate(RFIX::TFixMsg& msg) {
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgType], m_value));
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_ResetSeqNumFlag], m_resetSeq));
        std::string encryptMethod("0");
        std::string heartbeatInt("30");
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_HeartBtInt], heartbeatInt));
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_EncryptMethod], encryptMethod));
    }
    void Process(RFIX::CMsgSequence* in, RFIX::CMsgSequence* out, 
        std::string& output, RFIX::TFixMsg& msg) {
    }
    
private:
    std::string m_value;
    std::string m_resetSeq;
    
};

class CGlobexTestRequest : public CGlobexAdminMsg {
public:
    CGlobexTestRequest() : CGlobexAdminMsg("Globex.dat", "1") {}
    ~CGlobexTestRequest() {}
protected:
    void DoValidate(RFIX::TFixMsg& msg) {
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgType], m_value));
        char time[50];
        SYSTEMTIME systime;
        GetSystemTime(&systime);
        memset(time, '\0', 50);
        wsprintf(time, "%02u%02u%02u",                
            systime.wHour, systime.wMinute, systime.wSecond);
        std::string timeStr(time);
        msg.insert(RFIX::TFixPair(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TestReqID], timeStr));
    }
    void DoProcess(RFIX::CMsgSequence* in, RFIX::CMsgSequence* out, 
        std::string& output, RFIX::TFixMsg& msg) {
        long seq = out->NextMsgSeqNum();
        RFIX::CTypedFixMsg<RFIX::CFixFourTwo, RFIX::CFixHeartbeat> heartbeat;
        
        RFIX::TFixMapIter finder = msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TestReqID]);
        heartbeat.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TestReqID], finder->second);
        finder = msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID]);
        heartbeat.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID], finder->second);
        finder = msg.find(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID]);
        heartbeat.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID], finder->second);
        heartbeat.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderSubID], m_senderSubId);
        heartbeat.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetSubID], m_targetSubId);
        heartbeat.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderLocationID], m_senderLocId);
        heartbeat.SetMsgSeqNum(seq);
        
        output = heartbeat.GetFormattedMsg();
        out->AddMsg(output, seq);
    }
    
    
    
};



#endif //_GLOBEX_LOGON_H__
