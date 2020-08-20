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


#include <process.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#include <iostream>

#include "TestClassLevelLock.h"

static unsigned __stdcall RunOne(void* pArgs);
static unsigned __stdcall RunTwo(void* pArgs);
static unsigned __stdcall RunThree(void* pArgs);

static int ALL_DEAD = 0;

int main(int argc, char* argv[])
{
    try {
        unsigned int thIDOne, thIDTwo, thIDThree;
        HANDLE hServer = NULL;
        HANDLE hFix = NULL;
        HANDLE hClient = NULL;
        HANDLE lpHandles[3];
        lpHandles[0] = hServer;
        lpHandles[1] = hClient;
        lpHandles[2] = hFix;
        
        hServer = (HANDLE)_beginthreadex(NULL, 0,
            &RunOne, NULL, 0, &thIDOne);
        
        hFix = (HANDLE)_beginthreadex(NULL, 0,
            &RunTwo, NULL, 0, &thIDTwo);
        
        hClient = (HANDLE)_beginthreadex(NULL, 0,
            &RunThree, NULL, 0, &thIDThree);
        
        while(ALL_DEAD < 3) {
            Sleep(1000);
        }
        std::cerr<<"WAIT HAS RETURNED "<<std::endl;
        CloseHandle(hClient);
        CloseHandle(hFix);
        CloseHandle(hServer);
    } catch(...) {
        std::cerr<<"WE DIED "<<std::endl;
    }
    return 0;
    
}

static unsigned __stdcall RunOne(void* pArgs) {
    for(int i = 0; i < 5000; ++i) {
        if(LockTester::Instance()->IncrementerTest() != 2) {
            std::cerr<<"FAILED INCREMENTER TEST"<<std::endl;
            if(LockTester::versions_ > 1) {
                std::cerr<<"FAILED MULT VERSIONS!!!"<<std::endl;
            }
        }
    }
    ++ALL_DEAD;
    LockTester::Instance()->PrintDone("THREAD ONE DEAD ");
    return 0;
}
static unsigned __stdcall RunTwo(void* pArgs) {
    for(int i = 0; i < 5000; ++i) {
        if(LockTester::Instance()->IncrementerTest() != 2) {
            std::cerr<<"FAILED INCREMENTER TEST"<<std::endl;
            if(LockTester::versions_ > 1) {
                std::cerr<<"FAILED MULT VERSIONS!!!"<<std::endl;
            }
        }
    }
    ++ALL_DEAD;
    LockTester::Instance()->PrintDone("THREAD TWO DEAD ");
    return 0;
}
static unsigned __stdcall RunThree(void* pArgs) {
    for(int i = 0; i < 5000; ++i) {
        if(LockTester::Instance()->IncrementerTest() != 2) {
            std::cerr<<"FAILED INCREMENTER TEST"<<std::endl;
            if(LockTester::versions_ > 1) {
                std::cerr<<"FAILED MULT VERSIONS!!!"<<std::endl;
            }
        }
    }
    ++ALL_DEAD;
    LockTester::Instance()->PrintDone("THREAD THREE DEAD ");
    return 0;
}