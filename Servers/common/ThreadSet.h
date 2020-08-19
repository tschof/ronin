#ifndef _THREADSET_H__
#define _THREADSET_H__

#include "pthread.h"
#include <set>

template<class T>
class CThreadSet
{
public:
    CThreadSet(bool dolock = true)
    {
        m_dolock = dolock;
        if (m_dolock)
        {
            pthread_mutex_init(&m_Lock, NULL);
        }
    }

    ~CThreadSet()
    {
        if (m_dolock)
        {
            pthread_mutex_destroy(&m_Lock);
        }
    }

    unsigned long Add(const T& msg)
    {
        unsigned long rc = 0;

        if (m_dolock)
        {
            Lock();
        }

        m_set.insert(msg);
        rc = m_set.size();

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    unsigned long Add(std::set<T>& set)
    {
        unsigned long rc = 0;

        if (m_dolock)
        {
            Lock();
        }

        typename std::set<T>::iterator it = set.begin();
        while (it != set.end())
        {
            m_set.insert(*it);
            it++;
            rc++;
        }

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    void GetAll(std::set<T>& set)
    {
        if (m_dolock)
        {
            Lock();
        }

        typename std::set<T>::iterator it = m_set.begin();
        while (it != m_set.end())
        {
            set.insert(*it);
            it++;
        }

        if (m_dolock)
        {
            Unlock();
        }
    }

    bool Find(T& key)
    {
        bool rc = false;
        if (m_dolock)
        {
            Lock();
        }
        typename std::set<T>::iterator it = m_set.find(key);
        if (it != m_set.end())
        {
            rc = true;
        }
        if (m_dolock)
        {
            Unlock();
        }
        return rc;
    }
    
    bool Remove(T& msg)
    {
        bool rc = false;

        if (m_dolock)
        {
            Lock();
        }

        typename std::set<T>::iterator it = m_set.find(msg);
        if (it != m_set.end())
        {
            m_set.erase(it);
            rc = true;
        }

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    void Clear()
    {
        if (m_dolock)
        {
            Lock();
        }

        m_set.clear();

        if (m_dolock)
        {
            Unlock();
        }
    }

    void RemoveAll(std::set<T>& all)
    {
        if (m_dolock)
        {
            Lock();
        }
 
        all.swap(m_set);
        
        if (m_dolock)
        {
            Unlock();
        }
    }
    
private:
    std::set<T> m_set;
    pthread_mutex_t m_Lock;
    bool m_dolock;

    inline void Lock()
    {
        pthread_mutex_lock(&m_Lock);
    }

    inline void Unlock()
    {
        pthread_mutex_unlock(&m_Lock);
    }
};

#endif //_THREADSET_H__
