#ifndef _THREADVALUE_H__
#define _THREADVALUE_H__

#include "pthread.h"

template<class T>
class CThreadValue
{
public:
    CThreadValue(T value)
    {
        pthread_mutex_init(&m_Lock, NULL);
        m_value = value;
    }
    
    ~CThreadValue()
    {
        pthread_mutex_destroy(&m_Lock);
    }
    
    void Set(T value)
    {
        pthread_mutex_lock(&m_Lock);
        m_value = value;
        pthread_mutex_unlock(&m_Lock);
    }
    
    T Get()
    {
        T rc;
        pthread_mutex_lock(&m_Lock);
        rc = m_value;
        pthread_mutex_unlock(&m_Lock);
        return rc;
    }
    
private:
    T m_value;
    pthread_mutex_t m_Lock;
};

#endif //_THREADVALUE_H__
