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
#ifndef _GLOBEX_SERVER_H__
#define _GLOBEX_SERVER_H__

#pragma warning(disable: 4786)
#pragma warning(disable: 4503)

#include <process.h>
#include <winsock2.h>


#include <iostream>
#include <fstream>
#include <string>


#include "FixMsgSequence.h"
#include "FixMsg.h"
#include "RomSession.h"

#include "RomValidators.h"
#include "FixValidators.h"
#include "FixResendRequest.h"
#include "FixSender.h"
#include "RomSender.h"
#include "RomMessageCreator.h"
#include "FixMsgCreator.h"
#include "FixRomMapping.h"
#include "RomFixTranslator.h"
#include "RomHandler.h"
#include "FixHandler.h"
#include "Socket.h"
#include "Logger.h"
#include "GlobexMemTagSet.h"
#include "GlobexFixSession.h"

static const DWORD WAKEUP_INT = 15000;
static const DWORD HEARTBEAT_INT = 27000;
static const DWORD LEN_DAY_RUN = 43200000;

class CGlobexServer {
    
public:
    explicit CGlobexServer(std::string& dataFile);
    ~CGlobexServer();
    void Run();
private:
    
    void GetCurrTimeString(char time[50]);
    void InitializeMemoryMap(bool forceReload);

    bool ShouldRecycle(std::string& curDate);
    std::string m_password;
    std::string m_userName;
    std::string m_romHost;
    std::string m_senderCompId;
    std::string m_targetCompId;
    std::string m_serverHost;
    
    short m_fixPort;
    short m_romPort;
    bool m_isFixServer;
    std::string m_fixLogPath;
    int m_recycleDaily;
    char m_time[50];
    WORD m_endTime;
    std::string m_lastRunDate;
    std::string m_datFile;
    bool m_cancelChoice;
    std::string m_exchangeDestId;
    std::string m_dbUser;
    std::string m_dbPassword;
    std::string m_dbServer;
    std::string m_appName;
    std::string m_devGroup;
    std::string m_bizGroup;
    std::string m_emailServer;
    std::string m_logFilePath;
    bool m_forceReload;
    int m_numRecords;
    int m_maxFileSize;
    int m_recycleDay;

    //Globex specific "constants";
    std::string m_feeBilling;
    std::string m_customerOrFirm;
    std::string m_firmId;
    std::string m_fti;
    std::string m_senderSubId;
    std::string m_targetSubId;
    std::string m_senderLocId;
    std::string m_sessionId;
    std::string m_customerTypeCode;

    CGlobexMemTagSet* m_pTags;
    WSAEVENT m_wsaEvents[3];
    


    
};

#endif //_GLOBEX_SERVER_H__