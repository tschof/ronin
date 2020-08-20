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

#ifndef ARCADIRECTSEQUENCESET_H__
#define ARCADIRECTSEQUENCESET_H__

typedef struct ArcaDirectSequenceSetStruct
{
    long m_NextOut;
    long m_NextIn;
    long m_NextMsg;
    char m_Date[20];
} ArcaDirectSequenceSetStruct;

class CArcaDirectSequenceSet {
public:
    CArcaDirectSequenceSet();
    virtual ~CArcaDirectSequenceSet();

    long GetNextOut();
    void SetNextOut(long NextSequence);
    long GetNextIn();
    void SetNextIn(long ExpectedSequence);
    void SetDate(const char* pDate);

    long GetNextMsgNum();
    void SetNextMsgNum(long msgNum);

    int NextIn() { return m_SequenceSet->m_NextIn; }
    int NextOut() { return m_SequenceSet->m_NextOut; }
    int NextMsgNum() { return m_SequenceSet->m_NextMsg; }

private:
    HANDLE m_hFile;
    HANDLE m_hFileMap;
    LPVOID m_pFileView;
    ArcaDirectSequenceSetStruct* m_SequenceSet;

    void AllocateMemoryMap();
    
};

#endif //ARCADIRECTSEQUENCESET_H__