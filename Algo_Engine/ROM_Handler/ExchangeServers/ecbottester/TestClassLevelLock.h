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
#ifndef _TEST_CLASS_LEVEL_LOCK_H__
#define _TEST_CLASS_LEVEL_LOCK_H__

#include "Threads.h"


class LockTester : public ECBOT::ClassLevelLockable<LockTester> {
public:
    static LockTester* Instance();
    static int versions_;
    int IncrementerTest();
    void PrintDone(const char* message);
private:
    LockTester();
    ~LockTester() { ::DeleteCriticalSection(&sLock_); }
    LockTester(const LockTester&);
    LockTester& operator =(const LockTester&);
    int nIncrementor_;
    int nLoop_;
    
    static LockTester* pInstance_;
    static CRITICAL_SECTION sLock_;
};


#endif //_TEST_CLASS_LEVEL_LOCK_H__
