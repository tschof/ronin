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
#include "GlobexHandler.h"

#include "GlobexServer.h"
#include "Mailer.h"
#include "AuditTrail.h"
#include "GlobexLogon.h"
#include "ExchangeServer.h"

#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"


CGlobexServer::CGlobexServer(std::string& dataFile)
:m_password(), m_userName(), m_romHost(),
m_senderCompId(), m_targetCompId(),m_serverHost(), m_fixPort(0), m_romPort(0),
m_isFixServer(true), m_fixLogPath(), m_lastRunDate(), m_datFile(), m_cancelChoice(false),
m_exchangeDestId(), m_dbUser(), m_dbPassword(), m_dbServer(), m_appName("Globex"),
m_devGroup(), m_bizGroup(), m_emailServer(), m_logFilePath()
{
    
    char pszPath[MAX_PATH];
    GetCurrentDirectory( sizeof(pszPath), pszPath );
    strcat( pszPath, "\\" );
    m_fixLogPath += pszPath;
    strcat( pszPath, dataFile.c_str() );
    m_datFile = pszPath;
    std::string buffer;
    buffer.reserve(129);
    
    
    char szName[1024];
    GetModuleFileName( NULL, szName, sizeof(szName) );
    std::string consoleTitle(szName);
    GetPrivateProfileString("Runtime Information",
        "Server Version", "", (char*) buffer.c_str(), 128,
        pszPath);
    
    consoleTitle += buffer.c_str();
    SetConsoleTitle(consoleTitle.c_str());//(szName); 
    
    
    
    int isServer = GetPrivateProfileInt("Fix Information",
        "ConnectionType", 1, pszPath);
    m_isFixServer = isServer == 0 ? true : false;
    
    
    int cancel = GetPrivateProfileInt("Fix Information",
        "Cancel Choice", 0, pszPath);
    m_cancelChoice = cancel == 0 ? false : true;
    
    
    m_fixPort = GetPrivateProfileInt("Fix Information",
        "Fix Port", m_fixPort, pszPath);
    
    m_romPort = GetPrivateProfileInt("Rom Information",
        "Rom Port", m_romPort, pszPath);
    
    
    GetPrivateProfileString("Globex Information",
        "Password", "", (char*)buffer.c_str(), 128,
        pszPath);
    m_password = buffer.c_str();
    
    GetPrivateProfileString("Rom Information",
        "Rom User", "", (char*)buffer.c_str(), 128,
        pszPath);
    m_userName = buffer.c_str();
    
    GetPrivateProfileString("Rom Information",
        "Rom Host", "", (char*)buffer.c_str(), 128,
        pszPath);
    m_romHost = buffer.c_str();
    
    
    GetPrivateProfileString("Fix Information",
        "SenderCompId", "PARNELL", (char*)buffer.c_str(), 128,
        pszPath);
    m_senderCompId = buffer.c_str();
    
    GetPrivateProfileString("Fix Information",
        "TargetCompId", "", (char*)buffer.c_str(), 128,
        pszPath);
    m_targetCompId = buffer.c_str();
    
    
    GetPrivateProfileString("Fix Information",
        "Host", "", (char*)buffer.c_str(), 128,
        pszPath);
    m_serverHost = buffer.c_str();
    
    
    m_recycleDaily = GetPrivateProfileInt("Fix Information",
        "DailySequenceRecycle", 1, pszPath);
    
    
    GetPrivateProfileString("Runtime Information",
        "Last Run Date", "", (char*) buffer.c_str(), 128,
        pszPath);
    
    m_lastRunDate = buffer.c_str();
    
    m_endTime = static_cast<unsigned short>(GetPrivateProfileInt("Runtime Information",
        "End Time", 22, pszPath));
    
    
    GetPrivateProfileString("Email Recipients", "Developer Group", "", (char*) buffer.c_str(),
        128, pszPath);
    m_devGroup = buffer.c_str();
    
    GetPrivateProfileString("Email Recipients", "Business Group", "", (char*) buffer.c_str(),
        128, pszPath);
    m_bizGroup = buffer.c_str();
    
    GetPrivateProfileString("Email Recipients", "Mail Server", "", (char*) buffer.c_str(),
        128, pszPath);
    m_emailServer = buffer.c_str();
    
    GetPrivateProfileString("Globex Server",
        "Exchange Destination Id",
        "",
        (char*) buffer.c_str(),
        128,
        pszPath);
    m_exchangeDestId = buffer.c_str();


    GetPrivateProfileString("Globex Server",
        "Log File Path",
        "",
        (char*) buffer.c_str(),
        128,
        pszPath);
    m_logFilePath = buffer.c_str();
    
    GetPrivateProfileString("Order Database Information",
        "User",
        "",
        (char*) buffer.c_str(),
        128,
        pszPath);
    m_dbUser = buffer.c_str();
    
    GetPrivateProfileString("Order Database Information",
        "Password",
        "",
        (char*) buffer.c_str(),
        128,
        pszPath);
    m_dbPassword = buffer.c_str();
    
    GetPrivateProfileString("Order Database Information",
        "Server",
        "",
        (char*) buffer.c_str(),
        128,
        pszPath);
    m_dbServer = buffer.c_str();


    GetPrivateProfileString("Globex Information","FeeBilling","",(char*) buffer.c_str(),128,pszPath);
    m_feeBilling = buffer.c_str();
    GetPrivateProfileString("Globex Information","Firm ID","",(char*) buffer.c_str(),128,pszPath);
    m_firmId = buffer.c_str();
    GetPrivateProfileString("Globex Information","SenderSubId","",(char*) buffer.c_str(),128,pszPath);
    m_senderSubId = buffer.c_str();
    GetPrivateProfileString("Globex Information","targetSubId","",(char*) buffer.c_str(),128,pszPath);
    m_targetSubId = buffer.c_str();
    GetPrivateProfileString("Globex Information","SenderLocationID","",(char*) buffer.c_str(),128,pszPath);
    m_senderLocId = buffer.c_str();
    GetPrivateProfileString("Globex Information","sessionId","",(char*) buffer.c_str(),128,pszPath);
    m_sessionId = buffer.c_str();
    GetPrivateProfileString("Globex Information","CustomerTypeCode","",(char*) buffer.c_str(),128,pszPath);
    m_customerTypeCode = buffer.c_str();
    GetPrivateProfileString("Globex Information","CustomerOrFirm","",(char*) buffer.c_str(),128,pszPath);
    m_customerOrFirm = buffer.c_str();
    GetPrivateProfileString("Globex Information","FTI","",(char*) buffer.c_str(),128,pszPath);
    m_fti = buffer.c_str();




    m_numRecords = GetPrivateProfileInt("MemoryMapSettings", "Allocated Orders", m_numRecords, pszPath);
    if(m_numRecords < 1000 || m_numRecords > 20000) {
        m_numRecords = 5000;
    }
    m_maxFileSize = GetPrivateProfileInt("Runtime Information", "Max Log File Size", RFIX::MAX_FILE_SIZE, pszPath);
    if(m_maxFileSize <= 0) {
        m_maxFileSize = RFIX::MAX_FILE_SIZE;
    }
    int forceReload = 0;
    m_forceReload = GetPrivateProfileInt("MemoryMapSettings", "ForceReload",
        forceReload, pszPath) == 0 ? false : true;

    int recycleDay = 1;
    m_recycleDay = GetPrivateProfileInt("Fix Information",
        "Recycle Day", recycleDay, pszPath);
    
    
}

CGlobexServer::~CGlobexServer()
{
    delete m_pTags;
}

void CGlobexServer::Run()
{
    RASL::CRSocket fixSock;
    RASL::CRSocket romSock;
    romSock.SetPort(m_romPort);
    
    SMTP::CMailer mailer(m_emailServer, m_bizGroup, m_devGroup);
    
    romSock.SetHostName(m_romHost);
    std::string incomingFile = m_fixLogPath;
    incomingFile += "IncomingMsgSeq.log";
    std::string outgoingFile = m_fixLogPath;
    outgoingFile += "OutgoingMsgSeq.log";
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    memset(m_time, '\0', 50);
    wsprintf(m_time, "%04u%02u%02u",                
        systime.wYear, systime.wMonth, systime.wDay);
    std::string curDate = m_time;
    bool createNew = ShouldRecycle(curDate);//false;
    

    std::string logFile(m_logFilePath);//m_fixLogPath;
    std::string modName("Globex");
    logFile += modName;
    logFile += "Log";
    logFile += curDate;
    
    RFIX::CMsgSequence inSeq(incomingFile.c_str(), createNew);
    RFIX::CMsgSequence outSeq(outgoingFile.c_str(), createNew);

    bool createNewGenerator = curDate != m_lastRunDate ? true : false;
    CGlobexOrderIdGenerator generator(modName, createNewGenerator, m_fixLogPath);
    
    RFIX::CLogger logger(logFile.c_str(), createNew, m_maxFileSize);
    
    WritePrivateProfileString("Runtime Information",
        "Last Run Date", m_time, m_datFile.c_str());
    
    
    int resetIncoming = GetPrivateProfileInt("Fix Information",
        "Reset Incoming", 0, m_datFile.c_str());
    
    int resetOutgoing = GetPrivateProfileInt("Fix Information",
        "Reset Outgoing", 0, m_datFile.c_str());
    
    if(resetIncoming > 0) {
        inSeq.SetNextMsgSeqNum(resetIncoming);
        WritePrivateProfileString("Fix Information", 
            "Reset Incoming","0", m_datFile.c_str());
    }
    
    if(resetOutgoing > 0) {
        outSeq.SetNextMsgSeqNum(resetOutgoing);
        WritePrivateProfileString("Fix Information",
            "Reset Outgoing", "0", m_datFile.c_str());
    }
    
    RFIX::CRomSender<RASL::CRSocket, RFIX::CLogger> romSender(&romSock, romSock.Send, &logger, logger.Log);
    RFIX::CFixSender<RASL::CRSocket, RFIX::CLogger> fixSender(&fixSock, fixSock.Send, &logger, logger.Log);
    
    typedef CGlobexHandler<RFIX::CFixSender<RASL::CRSocket,RFIX::CLogger>,
        RFIX::CRomSender<RASL::CRSocket, RFIX::CLogger>,
        RFIX::CRomFixTranslator<RFIX::CFixRomMapping> > TGlobexHandler;
    
    typedef CGlobexFixSession<RFIX::CFixSender<RASL::CRSocket,RFIX::CLogger>,
        TGlobexHandler, RFIX::CRomFixTranslator<RFIX::CFixRomMapping>, CGlobexLogon, RFIX::CFixFourTwo,
        RFIX::CFixExecution, CGlobexSequenceReset> TFixSession;
    
    typedef RFIX::CFixResendRequest<TGlobexHandler,
        RFIX::CRomFixTranslator<RFIX::CFixRomMapping>,RFIX::CFixFourTwo,
        RFIX::CFixExecution, CGlobexSequenceReset > TResender;
    
    typedef RFIX::CFixMsgCreator<RFIX::CFixFourTwo,RFIX::CFixMsgType, RFIX::CFixLogout,
        RFIX::CFixHeartbeat, CGlobexTestRequest, CGlobexSequenceReset,
        TResender, RFIX::CFixCancel, RFIX::CFixOrder, RFIX::CFixReplace, RFIX::CFixExecution,
        RFIX::CRomStatus,RFIX::CRomOrder, RFIX::CRomCancel, RFIX::CRomReplace, RFIX::CFixReject,
        TFixSession, TFixSession, TFixSession> TFixCreator;
    
    
    bool reloadMemMap = false;
    if(createNew || m_forceReload) {
        reloadMemMap = true;
    }
    InitializeMemoryMap(reloadMemMap);

    CAuditLogger::Instance();
    TGlobexHandler handler(m_pTags, &mailer,
        &fixSender, fixSender.Send, &romSender, romSender.Send,
        &outSeq, m_senderCompId, m_targetCompId, m_fti, m_targetSubId, m_sessionId,
        m_senderSubId, m_senderLocId, m_feeBilling, m_customerOrFirm,
        m_customerTypeCode, &generator,m_firmId);
    
    
    TFixSession fixSession(&fixSender, fixSender.Send, &handler, handler.FromGlobex,
        &inSeq, &outSeq, m_senderCompId, m_targetCompId, m_fti, m_targetSubId, m_sessionId,
        m_senderSubId, m_senderLocId, m_feeBilling, m_customerOrFirm, m_customerTypeCode,
        m_password, m_firmId);
    
    RFIX::CRomHandler<TGlobexHandler, RFIX::CLogger> romHandler(&handler, handler.FromRom, &logger, logger.Log);
    RASL::CRecvCallback<RFIX::CRomHandler<TGlobexHandler, RFIX::CLogger> > romCallback(&romHandler, romHandler.Notify);
    romSock.SetListener(&romCallback);        
    
    TResender resender(&handler, handler.FromGlobex);
    TFixCreator fixCreator(&fixSession, fixSession.ProcessSessionMsg,
        &fixSession, fixSession.ProcessApplicationMsg, &fixSession, fixSession.IsLoggedOn,
        &resender);
    
    RFIX::CFixHandler<TFixCreator, RFIX::CLogger> fixHandler(&fixCreator, fixCreator.Process, &logger, logger.Log);
    RASL::CRecvCallback<RFIX::CFixHandler<TFixCreator, RFIX::CLogger> > fixCallback(&fixHandler, fixHandler.Notify);
    
    
    fixSock.SetListener(&fixCallback);
    Loop<RASL::CRSocket, RASL::CRSocket, TFixSession, TGlobexHandler>
        (&fixSock, &romSock, &fixSession, &handler, m_fixPort, m_serverHost, 27000, m_endTime, 0);
   /* bool clientDown = true;
    fixSock.SetPort(m_fixPort); 
    fixSock.SetHostName(m_serverHost);
    BOOL clientConnect = fixSock.Connect();
    if(clientConnect) {
        m_wsaEvents[0] = fixSock.CreateDisconnectEvent();
        fixSession.Logon();
        clientDown = false;
        std::cerr<<"Connected to Globex sent logon "<<std::endl;
    } else {
        m_wsaEvents[0] = WSACreateEvent();
        handler.SetConnected(false);
    }
    
    BOOL bound = romSock.Bind();
    if(bound) {
        m_wsaEvents[1] = WSACreateEvent();//romSock.CreateDisconnectEvent();
        m_wsaEvents[2] = romSock.CreateAcceptEvent();
    } else {
        m_wsaEvents[1] = WSACreateEvent();
        m_wsaEvents[2] = WSACreateEvent();
    }
    
    DWORD trueStart = GetTickCount();
    bool l_bRun = true;
    DWORD dwStart = GetTickCount();
    bool fSocketDown = clientDown;
    while(l_bRun) {
        DWORD waitResult;
        waitResult = WSAWaitForMultipleEvents(3, m_wsaEvents, FALSE, WAKEUP_INT, TRUE);
        
        switch(waitResult) {
            
        case WAIT_IO_COMPLETION:
            {
                break;
            }
        case WSA_WAIT_TIMEOUT:
            {
                DWORD timeElapsed = (GetTickCount() - dwStart); 
                
                if(timeElapsed >= HEARTBEAT_INT && fixSession.LoggedOn()) {
                    
                    if(!clientDown) {
                        bool connected = fixSession.SendHeartBeat();
                        if(!connected) {
                            clientDown = true;
                        }
                        dwStart = GetTickCount();
                    } 
                    GetSystemTime(&systime);
                    WORD curHour = systime.wHour;                   
                    if(curHour == m_endTime) {
                        romSock.Close();
                        fixSock.Close();
                        l_bRun = false;
                    }  
                } 
                if(timeElapsed >= HEARTBEAT_INT && clientDown) {
                    if(fSocketDown) {
                        std::cerr<<"ATTEMPTING TO RECONNECT Globex!!!"<<std::endl;
                        BOOL connected = fixSock.Connect();
                        
                        if(connected) {
                            std::cerr<<"RECONNECTED!"<<std::endl;
                            handler.SetConnected(true);
                            m_wsaEvents[0] = fixSock.CreateDisconnectEvent();
                            fixSession.Logon();
                            clientDown = false;
                            fSocketDown = false;
                        } 
                    } else {
                        fixSession.Logon();
                        clientDown = false;
                        dwStart = GetTickCount();
                    }
                }
                break;
            }
        default:
            {
                DWORD result = waitResult - WSA_WAIT_EVENT_0;
                switch(result) {
                case 0:
                    {
                        std::cerr<<"FIX SOCK DYIN!!!!!!!!"<<std::endl;
                        fixSock.Reset();
                        handler.SetConnected(false);
                        clientDown = true;
                        fSocketDown = true;
                        break;
                    }
                case 1:
                    {   
                        std::cerr<<"RESETTING******"<<std::endl;
                        romSock.Reset();
                        break;
                    }
                case 2:
                    {
                        std::cerr<<"ACCEPTING!!! "<<std::endl;
                        romSock.Accept();
                        m_wsaEvents[1] = romSock.CreateDisconnectEvent();
                        //m_wsaEvents[2] = romSock.CreateAcceptEvent();
                        break;
                    }
                };
                WSAResetEvent(m_wsaEvents[waitResult - WSA_WAIT_EVENT_0]);
                break;
            }
        };
    }
    */
}


bool CGlobexServer::ShouldRecycle(std::string& curDate)
{
    bool createNew = false;
    if(m_recycleDaily == 1 && curDate != m_lastRunDate) {
        createNew = true;
    } else {
        SYSTEMTIME systime;
        GetSystemTime(&systime);

        if(systime.wDayOfWeek == m_recycleDay && curDate != m_lastRunDate) {
            createNew = true;
        }
    }

    return createNew;

}

void CGlobexServer::GetCurrTimeString(char time[50]) 
{
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    memset(time, '\0', 50);
    wsprintf(time, "%02u%02u%02u",                
        systime.wHour, systime.wMinute, systime.wSecond);
    
}

void CGlobexServer::InitializeMemoryMap(bool forceReload)
{       
    DatabaseSettings::TIDatabaseSettingsAutoPtr lDatabaseSettings = 
        DatabaseSettings::CDatabaseSettingsFactory::CreateDatabaseSettings();
    lDatabaseSettings->AppName(m_appName.c_str());
    lDatabaseSettings->DestinationId(m_exchangeDestId.c_str());
    lDatabaseSettings->Server(m_dbServer.c_str());
    lDatabaseSettings->User(m_dbUser.c_str());
    lDatabaseSettings->Password(m_dbPassword.c_str());
    m_pTags = new CGlobexMemTagSet(lDatabaseSettings, forceReload,
        m_fixLogPath.c_str(), m_appName.c_str(), m_numRecords);

}