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
#include "stdafx.h"
#include "iLinkGlobex.h"
#include "GlobexSequenceGenerator.h"

CGlobexOrderIdGenerator::CGlobexOrderIdGenerator()
:m_hFile(NULL), m_hFileMap(NULL), m_pFileView(NULL),
 m_time(CTime::GetCurrentTime().Format("%m%d"))
{
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

void CGlobexOrderIdGenerator::NextOrderId(CString& orderTag) 
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
    CMessageApp* pApp = (CMessageApp*)AfxGetApp();
    if(pApp->IsFirstRun() == TRUE) {
        DeleteFile("C:\\OrderManager\\" + pApp->Name() + "ORDID.map");
    }
    m_hFile = CreateFile("C:\\OrderManager\\" + pApp->Name() + "ORDID.map",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,	
        NULL);
    bNewFile = (m_hFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;
    
    if(bNewFile == FALSE) {
        m_hFile = CreateFile("C:\\OrderManager\\" + pApp->Name() + "ORDID.map",
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
        pApp->Name() + "_ORDID");
    
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