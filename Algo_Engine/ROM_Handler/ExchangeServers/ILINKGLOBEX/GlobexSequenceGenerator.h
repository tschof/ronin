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

#ifndef GLOBEXSEQUENCEGENERATOR_H__
#define GLOBEXSEQUENCEGENERATOR_H__

namespace GlobexOrderGen {
    static const char baseChars[36] =
    {
     '0','1','2','3','4','5','6','7',
     '8','9','A','B','C','D','E','F',
     'G','H','I','J','K','L','M','N',
     'O','P','Q','R','S','T','U','V',
     'W','X','Y','Z'
    };
    static const int SEQ_LEN = 5;
    static const int BASE = 36;
}

typedef struct GlobexSequenceStruct {
    long m_seq;
    char m_uniqueSeq[5];
} GlobexSequenceStruct;

class CGlobexOrderIdGenerator {

public:
    CGlobexOrderIdGenerator();
    virtual ~CGlobexOrderIdGenerator();

    void NextOrderId(CString& orderTag);
private:
    HANDLE m_hFile;
    HANDLE m_hFileMap;
    HANDLE m_pFileView;
    GlobexSequenceStruct* m_sequenceStruct;
    CString m_time;

    void AllocateMemoryMap();
    void GenerateOrderNum();


};

#endif //GLOBEXSEQUENCEGENERATOR_H__