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
// IslandSequenceSet.cpp: implementation of the CIslandSequenceSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Island.h"
#include "IslandSequenceSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIslandSequenceSet::CIslandSequenceSet()
{
    m_pFileView = NULL;
    m_hFile = NULL;
    m_hFileMap = NULL;
    
    AllocateMemoryMap();
}

CIslandSequenceSet::~CIslandSequenceSet()
{
    if (m_pFileView)
        UnmapViewOfFile(m_pFileView);
    
    if (m_hFileMap)
        CloseHandle(m_hFileMap);
    
    if(m_hFile)
        CloseHandle(m_hFile);
}


void CIslandSequenceSet::AllocateMemoryMap()
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
        sizeof(SequneceSetStruct),
        pApp->Name() + "_SEQUENCE"
        );
    
    m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);
    
    if(m_pFileView)
    {
        m_SequenceSet = (SequneceSetStruct*)m_pFileView;
        CString sTime = CTime::GetCurrentTime().Format("%Y%m%d");
        
        if(bNewFile)
        {
            SetNext(1);
            SetExpected(1);
            SetDate((LPCTSTR)sTime);
        }
        else
        {
            if (strcpy(m_SequenceSet->m_Date, (LPCTSTR)sTime) != 0)
            {
                //	SetNext(1);
                //	SetExpected(1);
                SetDate((LPCTSTR)sTime);
            }
        }
    }
}


long CIslandSequenceSet::GetNext()
{
    m_SequenceSet->m_NextOut++;
    return(m_SequenceSet->m_NextOut - 1);
}

void CIslandSequenceSet::SetNext(long NextSequence)
{
    m_SequenceSet->m_NextOut = NextSequence;
}

long CIslandSequenceSet::GetExpected()
{
    m_SequenceSet->m_NextIn++;
    return(m_SequenceSet->m_NextIn - 1);
}

void CIslandSequenceSet::SetExpected(long ExpectedSequence)
{
    if (ExpectedSequence < 1)
    {
        ExpectedSequence = 1;
    }
    
    m_SequenceSet->m_NextIn = ExpectedSequence;
}

void CIslandSequenceSet::SetDate(const char * pDate)
{
    strcpy(m_SequenceSet->m_Date, pDate);
}
