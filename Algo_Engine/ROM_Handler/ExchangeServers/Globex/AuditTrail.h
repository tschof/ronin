/*******************************************************************************
*
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

#ifndef _AUDIT_TRAIL_H__
#define _AUDIT_TRAIL_H__

#include <winsock2.h>
#include <string>
#include <iostream>

#include "GenMutex.h"
#include "Fix.h"
#include "ROMMessage.h"
#include "RomValidators.h"



typedef struct AuditMessageNum {
    char m_uniqueSeq[50];
} AuditMessageNum;



class CAuditLogger : public ClassLevelLockable<CAuditLogger> {
public:
    
    static CAuditLogger* Instance();
    //explicit CAuditLogger(const char* fileName, bool createNew, unsigned long maxSize = MAX_FILE_SIZE);    
    
    void Audit(RFIX::CRCsv& auditCSV, char *source, char *direction,
        bool bTime, const std::string& orderTag, std::string& auditFirm);
private:
    
    void Log(std::string& msg);
    void ParseDate(const std::string& Symbol, std::string& retStr);
    void AllocateMemoryMap(bool createNew, std::string& fileName, std::string& name);
    void SetSeqNum();
    
    
    CAuditLogger();
    virtual ~CAuditLogger() {
        CloseHandle(m_hFile);
        if(m_pMMFileView) {
            UnmapViewOfFile(m_pMMFileView);
        }
        if(m_hMMFileMap) {
            CloseHandle(m_hMMFileMap);
        }
        if(m_hMMFile) {
            CloseHandle(m_hMMFile);
        }
    }
    //No copying!
    CAuditLogger(const CAuditLogger&);
    CAuditLogger& operator=(const CAuditLogger&);
    static CAuditLogger* pInstance_;
    /*We do not allow copying of Loggers.*/
    HANDLE m_hFile;
    char m_currUTCTime[50];
    
    HANDLE m_hMMFile;
    HANDLE m_hMMFileMap;
    HANDLE m_pMMFileView;
    AuditMessageNum* m_sequenceStruct;
};






#endif //_AUDIT_TRAIL_H__