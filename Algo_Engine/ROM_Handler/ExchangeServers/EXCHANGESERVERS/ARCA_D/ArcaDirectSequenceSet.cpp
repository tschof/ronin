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
#include "ArcaDirect.h"
#include "ArcaDirectSequenceSet.h"




CArcaDirectSequenceSet::CArcaDirectSequenceSet()
{
    m_pFileView = NULL;
    m_hFile = NULL;
    m_hFileMap = NULL;
    AllocateMemoryMap();
    
}
CArcaDirectSequenceSet::~CArcaDirectSequenceSet()
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

long CArcaDirectSequenceSet::GetNextOut()
{
    m_SequenceSet->m_NextOut++;
    return(m_SequenceSet->m_NextOut - 1);
}
void CArcaDirectSequenceSet::SetNextOut(long NextSequence)
{
    m_SequenceSet->m_NextOut = NextSequence;
}
long CArcaDirectSequenceSet::GetNextIn()
{
    m_SequenceSet->m_NextIn++;
    return(m_SequenceSet->m_NextIn - 1);
}
void CArcaDirectSequenceSet::SetNextIn(long ExpectedSequence)
{
    if(ExpectedSequence < 1) {
        ExpectedSequence = 1;
    }
    m_SequenceSet->m_NextIn = ExpectedSequence;
}
long CArcaDirectSequenceSet::GetNextMsgNum()
{
    long retVal = m_SequenceSet->m_NextMsg;
    ++m_SequenceSet->m_NextMsg;
    return retVal;
}
void CArcaDirectSequenceSet::SetNextMsgNum(long msgNum)
{
    if(msgNum < 1) {
        msgNum = 1;
    }
    m_SequenceSet->m_NextMsg = msgNum;
}

void CArcaDirectSequenceSet::SetDate(const char* pDate)
{
    strcpy(m_SequenceSet->m_Date, pDate);
}
void CArcaDirectSequenceSet::AllocateMemoryMap()
{
    if(m_pFileView)
        UnmapViewOfFile(m_pFileView);
    
    if(m_hFileMap)
        CloseHandle(m_hFileMap);
    
    if(m_hFile)
        CloseHandle(m_hFile);
    
    BOOL bNewFile;
    CMessageApp* pApp = (CMessageApp*)AfxGetApp();
    if (pApp->IsFirstRun() == TRUE) {
        DeleteFile("C:\\OrderManager\\" + pApp->Name() + "Sequence.map");
    }
    
    
    m_hFile = CreateFile("C:\\OrderManager\\" + pApp->Name() + "Sequence.map",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,	
        NULL
        );
    
    bNewFile = (m_hFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;
    
    if (bNewFile == FALSE)
    {
        m_hFile = CreateFile("C:\\OrderManager\\" + pApp->Name() + "Sequence.map",
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,	
            NULL
            );
    }
    
    m_hFileMap = CreateFileMapping(	(HANDLE)m_hFile,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(ArcaDirectSequenceSetStruct),
        pApp->Name() + "_SEQUENCE"
        );
    
    m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);
    if(m_pFileView)
    {
        m_SequenceSet = (ArcaDirectSequenceSetStruct*)m_pFileView;
        CString sTime = CTime::GetCurrentTime().Format("%Y%m%d");
        
        if(bNewFile)
        {
            SetNextOut(1);
            SetNextIn(0);
            SetNextMsgNum(1);
            SetDate((LPCTSTR)sTime);
        }
        else
        {
            if (strcpy(m_SequenceSet->m_Date, (LPCTSTR)sTime) != 0)
            {
                //SetNext(1);
                //SetExpected(1);
                SetDate((LPCTSTR)sTime);
            }
        }
    }
}