#ifndef __ROMCOUNT_H__
#define __ROMCOUNT_H__

#include "pthread.h"

class CRomCount
{
public:
    CRomCount(unsigned long inicount, bool dosynch);
    ~CRomCount();

    unsigned long Increment(unsigned long mod = 1);
    unsigned long Decrement(unsigned long mod = 1);
    void Reset(unsigned long value);
    unsigned long GetCount();

private:
    //pthread_mutex_t m_mutex;
    pthread_rwlock_t m_rwMutex;
    bool m_doSynch;
    unsigned long m_count;
};

#endif //__ROMCOUNT_H__
