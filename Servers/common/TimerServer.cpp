#include "TimerServer.h"

#ifdef _WIN32
#include "Windows.h"
#else
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#endif

CTimerServer::CTimerServer() : m_Running(false)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_mutex_init(&m_runningmutex, NULL);
}

CTimerServer::~CTimerServer()
{
    pthread_mutex_destroy(&m_mutex);
    pthread_mutex_destroy(&m_runningmutex);
    std::set<tTimerRequest*, MyLess >::iterator it = m_reqList.begin();
    while (it != m_reqList.end())
    {
        delete *it;
        it++;
    }
}

void* CTimerServer::Run(void* arg)
{
    return reinterpret_cast<CTimerServer*>(arg)->theRun();
}

void* CTimerServer::theRun()
{
    SetRunning(true);
    unsigned int delt = 0;
        
    while (Running())
    {
        if (delt < 30)
        {
            CUtility::Sleep(30 - delt);
        }
        delt = ProcessRequests();
    }

    return NULL;
}

time_t CTimerServer::ProcessRequests()
{
    time_t ltime_before = 0, ltime_after = 0;
    
    pthread_mutex_lock(&m_mutex);

    time(&ltime_before);
    
    tTimerRequest* pReq = NULL;
    std::set<tTimerRequest*, MyLess >::iterator it = m_reqList.begin();
    std::set<tTimerRequest*, MyLess >::iterator it2;
    while (it != m_reqList.end())
    {
        pReq = *it;
        if (pReq->timerhandler(ltime_before, pReq->anyobj) == true &&
            pReq->oneshot)
        {
            delete pReq;
            it2 = it;
            it++;
            m_reqList.erase(it2); 
        }
        else
        {
            it++;
        }
    }  
    
    time(&ltime_after);
    
    pthread_mutex_unlock(&m_mutex);
    
    return ltime_after - ltime_before;
}

bool CTimerServer::Start()
{
    bool rc = true;
    if (pthread_create(&m_ID, NULL, CTimerServer::Run, (void*)(this)))
    {
        perror("pthread_create failed");
        rc = false;
    }
    return rc;
}

void CTimerServer::Stop()
{
    SetRunning(false);
    void* dummy = NULL;
    pthread_join(m_ID, &dummy);
}

void CTimerServer::AddRequest(tTimerRequest* pReq)
{
    pthread_mutex_lock(&m_mutex);

    m_reqList.insert(pReq);

    pthread_mutex_unlock(&m_mutex);
}

bool CTimerServer::Running()
{
    bool rc = false;
    pthread_mutex_lock(&m_runningmutex);
    rc = m_Running;
    pthread_mutex_unlock(&m_runningmutex);
    return rc;
}

void CTimerServer::SetRunning(bool value)
{
    pthread_mutex_lock(&m_runningmutex);
    m_Running = value;
    pthread_mutex_unlock(&m_runningmutex);
}
