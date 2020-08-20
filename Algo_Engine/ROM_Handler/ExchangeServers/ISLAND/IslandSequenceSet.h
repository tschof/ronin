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

// IslandSequenceSet.h: interface for the CIslandSequenceSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDSEQUENCESET_H__68C1F616_4A15_11D5_A61F_00500401F87A__INCLUDED_)
#define AFX_ISLANDSEQUENCESET_H__68C1F616_4A15_11D5_A61F_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct SequneceSetStruct
{
    long	m_NextOut;
    long	m_NextIn;
    char	m_Date[20];
} SequneceSetStruct;

class CIslandSequenceSet  
{
public:
    CIslandSequenceSet();
    virtual ~CIslandSequenceSet();
    
    long GetNext();				// increments seq num and returns seq num - 1
    void SetNext(long NextSequence);			// set the next sequence number to recover from errors
    long GetExpected();			// increments the incoming sequence number and returns the expected
    void SetExpected(long ExpectedSequence);		// sets number. used after receiving a sequence reset
    void SetDate(const char * pDate);  // Set today's date
    
    int	NextIn()	{ return m_SequenceSet->m_NextIn;  }
    int	NextOut()	{ return m_SequenceSet->m_NextOut; }
private:
    HANDLE m_hFile;
    HANDLE m_hFileMap;
    LPVOID m_pFileView;
    SequneceSetStruct *m_SequenceSet;
    
    void AllocateMemoryMap();
    
};

#endif // !defined(AFX_ISLANDSEQUENCESET_H__68C1F616_4A15_11D5_A61F_00500401F87A__INCLUDED_)
