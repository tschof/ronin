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

#include <iostream>
#include "TestClassLevelLock.h"

LockTester* LockTester::pInstance_ = 0;
int LockTester::versions_ = 0;
CRITICAL_SECTION LockTester::sLock_;

LockTester* LockTester::Instance()
{
    if(!pInstance_) {
        Lock lock;
        if(!pInstance_) {
            ++versions_;
            ::InitializeCriticalSection(&sLock_);
            pInstance_ = new LockTester();
        }
    }
    return pInstance_;
}

LockTester::LockTester()
:nIncrementor_(90), nLoop_(0)
{

}

int LockTester::IncrementerTest()
{
    Lock lock;
    int goo();

    //::EnterCriticalSection(&sLock_);
    int local = 0;
    nIncrementor_ = 80;
    nIncrementor_ =2;
    std::cerr<<nIncrementor_;
    ++nLoop_;
    if((nLoop_ % 10) == 0) {
        std::cerr<<"\n";
    }
    local = nIncrementor_;
   // ::LeaveCriticalSection(&sLock_);
    return local;
}

void LockTester::PrintDone(const char* message)
{
    Lock lock;
   //::EnterCriticalSection(&sLock_);
   std::cerr<<message<<std::endl;
   //::LeaveCriticalSection(&sLock_);
}