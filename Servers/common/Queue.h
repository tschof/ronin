#ifndef _THREADQUEUE_H__
#define _THREADQUEUE_H__

#include <queue>
#include "pthread.h"

template<class T>
class CThreadQueue
{
public:
    CThreadQueue(bool dolock = true)
    {
        m_dolock = dolock;
        if (m_dolock)
        {
            pthread_mutex_init(&m_Lock, NULL);
        }
    }

    ~CThreadQueue()
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

        m_Queue.push_back(msg);
        rc = m_Queue.size();

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    unsigned long Add(std::deque<T>& list)
    {
        unsigned long rc = 0;

        if (m_dolock)
        {
            Lock();
        }

        typename std::deque<T>::iterator it = list.begin();
        while (it != list.end())
        {
            m_Queue.push_back(*it);
            it++;
            rc++;
        }

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    void GetAll(std::deque<T>& list)
    {
        if (m_dolock)
        {
            Lock();
        }

        if (m_Queue.empty() == false)
        {
            m_Queue.swap(list);
        }

        if (m_dolock)
        {
            Unlock();
        }
    }

    bool Get(T& msg)
    {
        bool rc = false;

        if (m_dolock)
        {
            Lock();
        }

        if (m_Queue.empty() == false)
        {
            msg = m_Queue.front();
            m_Queue.pop_front();
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

        m_Queue.clear();

        if (m_dolock)
        {
            Unlock();
        }
    }

    bool IsEmpty()
    {
        bool rc = false;

        if (m_dolock)
        {
            Lock();
        }

        rc = m_Queue.empty();

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    int Size()
    {
        int rc = 0;

        if (m_dolock)
        {
            Lock();
        }

        rc = m_Queue.size();

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

private:
    std::deque<T> m_Queue;
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

#endif //_THREADQUEUE_H__