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

#include "GlobexSequenceGenerator.h"

CGlobexOrderIdGenerator::CGlobexOrderIdGenerator(std::string& name, bool createNew,
                                                 std::string& path)
:m_hFile(NULL), m_hFileMap(NULL), m_pFileView(NULL),
m_time(), m_createNew(createNew), m_name(name), m_path(path), m_fileName()
{
    SYSTEMTIME systime;
    GetSystemTime(&systime);
    char currUTCTime[50];
    memset(currUTCTime, '\0', 50);
    wsprintf(currUTCTime, "%02u%02u",                
        systime.wMonth, systime.wDay);
    m_time = currUTCTime;
    m_fileName += path;
    m_fileName += name;
    m_fileName += "ORDID.map";
    m_name += "ORDID.map";
    AllocateMemoryMap();
}

CGlobexOrderIdGenerator::~CGlobexOrderIdGenerator()
{
    if(m_pFileView) {
        UnmapViewOfFile(m_pFileView);
    }
    if(m_hFileMap) {
        CloseHandle(m_hFileMap);
    }
    if(m_hFile) {
        CloseHandle(m_hFile);
    }
}

void CGlobexOrderIdGenerator::NextOrderId(std::string& orderTag) 
{
    GenerateOrderNum();
    orderTag += m_time;
    orderTag += m_sequenceStruct->m_uniqueSeq;
    ++m_sequenceStruct->m_seq;
}

void CGlobexOrderIdGenerator::GenerateOrderNum()
{
    int remainder = 0;
    memset(m_sequenceStruct->m_uniqueSeq, '0',
        GlobexOrderGen::SEQ_LEN - 1);
    int pos = 0;
    int value = m_sequenceStruct->m_seq;
    while (value > 0) {
        remainder = value % GlobexOrderGen::BASE;
        value = ((value - remainder)/ GlobexOrderGen::BASE);
        m_sequenceStruct->m_uniqueSeq[pos] = GlobexOrderGen::baseChars[remainder];
        ++pos;
    }
    strrev(m_sequenceStruct->m_uniqueSeq);
}

void CGlobexOrderIdGenerator::AllocateMemoryMap()
{
    if(m_pFileView) {
        UnmapViewOfFile(m_pFileView);
    }
    if(m_hFileMap) {
        CloseHandle(m_hFileMap);
    }
    if(m_hFile) {
        CloseHandle(m_hFile);
    }
    
    BOOL bNewFile;
    if(m_createNew) {
        DeleteFile(m_fileName.c_str());
    }
    m_hFile = CreateFile(m_fileName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,	
        NULL);
    bNewFile = (m_hFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;
    
    if(bNewFile == FALSE) {
        m_hFile = CreateFile(m_fileName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,	
            NULL
            );
    }
    
    m_hFileMap = CreateFileMapping((HANDLE)m_hFile, 
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(GlobexSequenceStruct),
        m_name.c_str());
    
    m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);
    if(m_pFileView) {
        m_sequenceStruct = (GlobexSequenceStruct*)m_pFileView;
        memset(m_sequenceStruct->m_uniqueSeq, '\0', GlobexOrderGen::SEQ_LEN);
        if(bNewFile) {
            m_sequenceStruct->m_seq = 1;
            
            GenerateOrderNum();
        }
    }
    
}