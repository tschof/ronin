#ifndef _CTIMERSERVER_H__
#define _CTIMERSERVER_H__

#include "Thread.h"
#include "pthread.h"
#include "ROMTypedef.h"
#include "Utility.h"
#include <set>


typedef struct
{
    bool (*timerhandler)(time_t now, void* obj);
    bool oneshot;
    void* anyobj;
    //unsigned short laps; //if the laps is zero the timerhandler will be called every 30 seconds!!
} tTimerRequest;

struct MyLess
{
    bool operator()(const tTimerRequest* lhs, const tTimerRequest* rhs)
    {
        return (unsigned long)lhs < (unsigned long)rhs;
    }
};

class CTimerServer
{
public:

    CTimerServer();
    ~CTimerServer();

    static CTimerServer & getInstance()
    {
        static CTimerServer theObj;
        return theObj;
    }

    static void* Run(void* arg);

    void* theRun();
    bool Start();

    void Stop();

    void AddRequest(tTimerRequest* pReq);
    time_t ProcessRequests();
    bool Running();
    void SetRunning(bool value);

private:
    bool m_Running;
    std::set<tTimerRequest*, MyLess > m_reqList;
    pthread_mutex_t m_mutex;
    pthread_mutex_t m_runningmutex;
    pthread_t m_ID;
};

#endif //_CTIMERSERVER_H__
