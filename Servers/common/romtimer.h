#ifndef __ROMTIMER_H_
#define __ROMTIMER_H_

#include <signal.h>
#include <time.h>
#include <set>
#include "pthread.h"

class CRomTimer
{
public:
    static void Callback(union sigval arg)
    {
        CRomTimer* pTheTimer = (CRomTimer*)arg.sival_ptr;
        pTheTimer->CallFunc();
    }
    
    CRomTimer(time_t period, time_t start_wait, void(* handler)(std::set<void*>&), std::set<void*> argQ);
    ~CRomTimer();
    void CallFunc();
    bool AddArg(void*);
    bool RemoveArg(const void*);
    
private:
       
    timer_t m_id;
    bool m_dead;
    pthread_mutex_t m_lock;
    std::set<void*> m_argQ;
    void(* m_func)(std::set<void*>&);
};



#endif //__ROMTIMER_H_
