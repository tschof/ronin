#ifndef _THREADLIST_H__
#define _THREADLIST_H__

#include "pthread.h"
#include <list>

template<class T>
class CThreadList
{
public:
    CThreadList(bool dolock = true)
    {
        m_dolock = dolock;
        if (m_dolock)
        {
            pthread_mutex_init(&m_Lock, NULL);
        }
    }

    ~CThreadList()
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

        m_List.push_back(msg);
        rc = m_List.size();

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    unsigned long Add(std::list<T>& list)
    {
        unsigned long rc = 0;

        if (m_dolock)
        {
            Lock();
        }

        typename std::list<T>::iterator it = list.begin();
        while (it != list.end())
        {
            m_List.push_back(*it);
            it++;
            rc++;
        }

        if (m_dolock)
        {
            Unlock();
        }

        return rc;
    }

    void GetAll(std::list<T>& list)
    {
        if (m_dolock)
        {
            Lock();
        }

        typename std::list<T>::iterator it = m_List.begin();
        while (it != list.end())
        {
            list.push_back(*it);
            it++;
        }

        if (m_dolock)
        {
            Unlock();
        }
    }

    /*bool Get(T& msg)
    {
    	bool rc = false;

    if (m_dolock)
    {
    	  Lock();
    }

    	if (m_List.empty() == false)
    	{
    		msg = m_List.front();
    		m_List.pop_front();
    		rc = true;
    	}
    	else
    	{
    		msg = "";
    	}

    if (m_dolock)
    {
    	  Unlock();
    }

    	return rc;
    }*/

    void Clear()
    {
        if (m_dolock)
        {
            Lock();
        }

        m_List.clear();

        if (m_dolock)
        {
            Unlock();
        }
    }

private:
    std::list<T> m_List;
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


#endif //_THREADLIST_H__
