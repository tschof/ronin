#include "romtimer.h"
#include "stdio.h"

CRomTimer::CRomTimer(time_t period, time_t start_wait, void(* handler)(std::set<void*>&), std::set<void*> argQ)
{
    pthread_mutex_init(&m_lock, NULL);
    m_func = handler;
    m_argQ = argQ;
    struct sigevent ev;
    ev.sigev_notify = SIGEV_THREAD;
    ev.sigev_notify_function = &Callback;
    ev.sigev_notify_attributes = NULL;
    ev.sigev_value.sival_ptr = this;
    
    struct itimerspec spec;
    spec.it_interval.tv_sec = period;
    spec.it_interval.tv_nsec = 0;
    spec.it_value.tv_sec = start_wait;
    spec.it_value.tv_nsec = 0;
       
    if (timer_create(CLOCK_REALTIME, &ev, &m_id) != 0)
    {
        printf("timer_create failed\n");
    }
    else
    {
        if (timer_settime(m_id, 0, &spec, NULL) == -1)
        {
            printf("timer_settime failed\n");
        }
    }
}

CRomTimer::~CRomTimer()
{
    pthread_mutex_destroy(&m_lock);
    timer_delete(m_id);
}

void CRomTimer::CallFunc()
{
    pthread_mutex_lock(&m_lock);
    
    //if (m_argQ.empty() == false)
    //{
        m_func(m_argQ);
    //}
    
    pthread_mutex_unlock(&m_lock);
}

bool CRomTimer::AddArg(void* pArg)
{
    pthread_mutex_lock(&m_lock);
    
    bool rc = false;
    std::pair<std::set<void*>::iterator, bool> p = m_argQ.insert(pArg);
    rc = p.second;
    pthread_mutex_unlock(&m_lock);
    return rc;
}

bool CRomTimer::RemoveArg(const void* pArg)
{
    pthread_mutex_lock(&m_lock);
    bool rc = (m_argQ.erase(const_cast<void*>(pArg)) == 1)? true : false;
    pthread_mutex_unlock(&m_lock);
    return rc;
}

