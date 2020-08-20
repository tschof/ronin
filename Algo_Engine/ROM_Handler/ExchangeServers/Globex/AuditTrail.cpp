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

#include "AuditTrail.h"


CAuditLogger* CAuditLogger::pInstance_ = 0;

CAuditLogger* CAuditLogger::Instance() 
{
    if(!pInstance_) {
        Lock lock;
        if(!pInstance_) {
            pInstance_ = new CAuditLogger();
        }
    }
    return pInstance_;
}

CAuditLogger::CAuditLogger()
: m_hMMFile(NULL), m_hMMFileMap(NULL), m_pMMFileView(NULL){
    DWORD dwCreateDisp = OPEN_ALWAYS;
    
    char pszPath[MAX_PATH];
    GetCurrentDirectory( sizeof(pszPath), pszPath );
    strcat( pszPath, "\\" ); 
    std::string auditFileName(pszPath);
    auditFileName += "CMEAudit";
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    memset(m_currUTCTime, '\0', 50);
    wsprintf(m_currUTCTime, "%02u-%02u-%02u",                
        systime.wYear, systime.wMonth, systime.wDay);
    auditFileName += m_currUTCTime;
    auditFileName += ".log";
    DWORD dwFileAccess = GENERIC_READ | GENERIC_WRITE;
    DWORD dwShareMode = FILE_SHARE_WRITE | FILE_SHARE_READ;
       
    m_hFile = CreateFile(auditFileName.c_str(), dwFileAccess, dwShareMode,
        NULL, dwCreateDisp, FILE_ATTRIBUTE_NORMAL, NULL);
   
    DWORD fileSize = GetFileSize(m_hFile, 0);
    bool createNew = fileSize > 0 ? false : true;
    SetFilePointer(m_hFile, fileSize, NULL, FILE_BEGIN);
    

    std::string mmFileName(pszPath);
    mmFileName.append("AuditNum.map");
    std::string mmName("AuditNum.map");
    AllocateMemoryMap(createNew, mmFileName, mmName);
}

void CAuditLogger::Log(std::string& message)
{
    
    DWORD nBytesWritten = 0;
    
    int msgSize = message.size();
    WriteFile(m_hFile, message.c_str(), msgSize,
        &nBytesWritten, NULL);  
}




void CAuditLogger::ParseDate(const std::string& Symbol, std::string& retStr)
{
    char time[50];
    if(Symbol.length() <= 0) return;
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    memset(time, '\0', 50);
    wsprintf(time, "%02u",                
        systime.wYear);
    char buffer[10];
    
    std::string strCurrentYearFirst;
    strCurrentYearFirst.append(time);
    std::string strCurrentYearLast = strCurrentYearFirst.substr(strCurrentYearFirst.length() - 1);
    strCurrentYearFirst = strCurrentYearFirst.substr(0, 3);
    
    
    std::string Date = Symbol.substr(Symbol.length() - 2);
    char Month = Date[0];
    strCurrentYearFirst.append(1, Date[1]);
    int FullExpYear = atol(strCurrentYearFirst.c_str());
    int FullYear = atoi(time);//CTime::GetCurrentTime().GetYear();
    
    if(FullExpYear < FullYear)
    {
        FullExpYear += 10;
    }
    
    switch(Month)
    {
    case 'F':
        Date = "01";
        break;
    case 'G':
        Date = "02";
        break;
    case 'H':
        Date = "03";
        break;
    case 'J':
        Date = "04";
        break;
    case 'K':
        Date = "05";
        break;
    case 'M':
        Date = "06";
        break;
    case 'N':
        Date = "07";
        break;
    case 'Q':
        Date = "08";
        break;
    case 'U':
        Date = "09";
        break;
    case 'V':
        Date = "10";
        break;
    case 'X':
        Date = "11";
        break;
    case 'Z':
        Date = "12";
    }
    retStr.append((ltoa(FullExpYear, buffer, 10)));
    retStr.append("-");
    retStr.append(Date);
}


void CAuditLogger::SetSeqNum()
{
    long temp = atoi(m_sequenceStruct->m_uniqueSeq);
    memset(m_sequenceStruct->m_uniqueSeq, '\0', 50);  
    ++temp;
    ltoa(temp, m_sequenceStruct->m_uniqueSeq, 10);
}

void CAuditLogger::Audit(RFIX::CRCsv& auditCSV, char *source, char *direction,
                         bool bTime, const std::string& orderTag, std::string& auditFirm)
{
    char time[50];
    try
    {
        RFIX::CRomMessage<RFIX::CRomAdmin> writeCSV;
        std::string string;
        
        int testNum;
        char testChar;
        
        writeCSV.Set(0, source);
        std::string messNum(m_sequenceStruct->m_uniqueSeq);

        writeCSV.SetAt(1, messNum);
        SetSeqNum();
        time;
        SYSTEMTIME systime;
        GetSystemTime(&systime);
        memset(time, '\0', 50);
        wsprintf(time, "%02u-%02u-%02u",                
            systime.wYear, systime.wMonth, systime.wDay);
        std::string timeStr(time);
        writeCSV.SetAt(2, timeStr);
        memset(time, '\0', 50);
        wsprintf(time, "%02u:%02u:%02u",                
            systime.wHour, systime.wMinute, systime.wSecond);
        writeCSV.Set(3, time);
        writeCSV.Set(4, "CME");
        writeCSV.Set(5, direction);
        
        testNum = atoi(auditCSV.GetAt(RFIX::CSV_STATUS).c_str());
        
        if(testNum == 8 || testNum == 14)
        {
            writeCSV.SetAt(6, "REJECT");
        }
        else
        {
            writeCSV.SetAt(6, "OK");
        }
        
        writeCSV.SetAt(7, orderTag);//auditCSV.Get(CSV_STAFFORD_TAG));
        writeCSV.SetAt(8, auditCSV.GetAt(RFIX::CSV_STAFFORD_TAG));
        std::string commandStr = auditCSV.GetAt(RFIX::CSV_COMMAND);
        
        testChar = commandStr[0];
        
        switch(testChar)
        {
        case 'C':
            writeCSV.Set(9, "CANCEL");
            break;
            
        case 'E':
            writeCSV.Set(9, "ENTER");
            break;
            
        case 'R':
            writeCSV.Set(9, "MODIFY");
            break;
            
        case 'L':
            writeCSV.Set(9, "LOGIN");
            CAuditLogger::Instance()->Log(writeCSV.PureMessage());
            return;
            break;
            
        case 'O':
            writeCSV.Set(9, "LOGOFF");
            CAuditLogger::Instance()->Log(writeCSV.PureMessage());
            return;
            break;
            
        case 'S':
            switch(testNum)
            {
            case 2:
            case 1:
                writeCSV.Set(9, "EXECUTION");
                writeCSV.SetAt(19, auditCSV.GetAt(RFIX::CSV_PRICE));
                writeCSV.SetAt(22, auditCSV.GetAt(RFIX::CSV_CONTRA));
                break;
                
            case 10:
            case 13:
            case 8:
                writeCSV.Set(9, "ENTER");
                break;
                
            case 6:
            case 4:
            case 14:
            case 20:
                writeCSV.Set(9, "CANCEL");
                break;

            case 21:
            case 5:
                writeCSV.Set(9, "MODIFY");
                break;
            }
        };
        
        testNum = atoi(auditCSV.GetAt(RFIX::CSV_SIDE).c_str());
        
        if(testNum == 1)
        {
            writeCSV.SetAt(10, "B");
        }
        else if(testNum == 2)
        {
            writeCSV.SetAt(10, "S");
        }
        
        writeCSV.SetAt(11, auditCSV.GetAt(RFIX::CSV_SHARES));
        writeCSV.SetAt(12, auditCSV.GetAt(RFIX::CSV_SYMBOL));
        std::string symbolString = auditCSV.GetAt(RFIX::CSV_SYMBOL);
        writeCSV.SetAt(13, symbolString.substr(0, (symbolString.length() - 2)));
        std::string dateStr;
        ParseDate(symbolString, dateStr);
        writeCSV.SetAt(14, dateStr);
        writeCSV.Set(15, "");
        if(auditCSV.GetAt(RFIX::CSV_SECURITY_TYPE) == "O") {
            writeCSV.SetAt(16, auditCSV.GetAt(RFIX::CSV_STRIKEPRICE));
        }
        writeCSV.SetAt(18, auditCSV.GetAt(RFIX::CSV_STOPPRICE));
        testNum = atoi(auditCSV.GetAt(RFIX::CSV_TYPE).c_str());
        
        switch(testNum)
        {
        case 1:
            writeCSV.Set(20, "MKT");
            break;
        case 2:
            writeCSV.Set(20, "LMT");
            if(writeCSV.GetAt(19).length() <= 0) {
                writeCSV.SetAt(17, auditCSV.GetAt(RFIX::CSV_PRICE));
            }
            break;
        case 3:
            writeCSV.Set(20, "STP");
            break;
        case 4:
            writeCSV.Set(20, "STL");
            break;
        case 13:
            writeCSV.Set(20, "MKT");
            break;
        };
        
        writeCSV.Set(21, "DAY");
        writeCSV.SetAt(23, auditCSV.GetAt(RFIX::CSV_TRADER));
        writeCSV.Set(24, "4");
        writeCSV.Set(25, "1");
        writeCSV.SetAt(26, auditCSV.GetAt(RFIX::CSV_CLEARINGACCOUNT));
        writeCSV.SetAt(27, "");
        writeCSV.Set(28, "N");
        writeCSV.SetAt(29, "");
        writeCSV.SetAt(30, auditCSV.GetAt(RFIX::CSV_TEXT));
        writeCSV.SetAt(31, auditFirm);//m_AuditFirm);
        Log(writeCSV.PureMessage());
    }
    catch(...)
    {
    }
}


void CAuditLogger::AllocateMemoryMap(bool createNew, std::string& fileName, std::string& name)
{
    if(m_pMMFileView) {
        UnmapViewOfFile(m_pMMFileView);
    }
    if(m_hMMFileMap) {
        CloseHandle(m_hMMFileMap);
    }
    if(m_hMMFile) {
        CloseHandle(m_hMMFile);
    }
    
    BOOL bNewFile;
    if(createNew) {
        DeleteFile(fileName.c_str());
    }
    m_hMMFile = CreateFile(fileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,	
        NULL);
    bNewFile = (m_hMMFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;
    
    if(bNewFile == FALSE) {
        m_hMMFile = CreateFile(fileName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,	
            NULL
            );
    }
    
    m_hMMFileMap = CreateFileMapping((HANDLE)m_hMMFile, 
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(AuditMessageNum),
        name.c_str());
    
    m_pMMFileView = MapViewOfFile(m_hMMFileMap, FILE_MAP_WRITE, 0, 0, 0);
    if(m_pMMFileView) {
        m_sequenceStruct = (AuditMessageNum*)m_pMMFileView;
        if(bNewFile) {
            memset(m_sequenceStruct->m_uniqueSeq, '\0', 50);
            itoa(1, m_sequenceStruct->m_uniqueSeq, 10);
        }
    }
    
}