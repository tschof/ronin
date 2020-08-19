#ifndef _THREAD_H__
#define _THREAD_H__

#include "Queue.h"
#include "pthread.h"
#include "signal.h"
#include <deque>

#ifndef _WIN32
#include <stdlib.h>
#include <stdio.h>
#else
#include <time.h>
#endif

template<class QType>
class CThread
{
public:
    typedef std::deque<QType> tMsgList;

    CThread():m_msgReady(false),m_Running(false), m_Queue(false)
    {
        pthread_cond_init(&m_Cond, NULL);
        pthread_mutex_init(&m_Mutex, NULL);
        pthread_mutex_init(&m_RunningMutex, NULL);
    }

    ~CThread()
    {
        pthread_cond_destroy(&m_Cond);
        pthread_mutex_destroy(&m_Mutex);
        pthread_mutex_destroy(&m_RunningMutex);
    }

    virtual void Init() {}

    static void* Run(void* arg)
    {
        return reinterpret_cast<CThread*>(arg)->theRun();
    }

    virtual bool Stop()
    {
        SetRunning(false);
        m_msgReady = false;
        bool rc = true;
        void* ptr = NULL;

        pthread_mutex_lock(&m_Mutex);
        pthread_cond_signal(&m_Cond);
        pthread_mutex_unlock(&m_Mutex);

        if (pthread_join(m_ID, &ptr))
        {
            perror("pthread_join failed");
            rc = false;
        }

        return rc;
    }

    virtual void* theRun()
    {
        void* rc = NULL;
        SetRunning(true);

        Init();

        while (Running())
        {
            pthread_mutex_lock(&m_Mutex);

            while(m_msgReady == false &&
                  Running() == true)
            {
                pthread_cond_wait(&m_Cond, &m_Mutex);
            }

            tMsgList msgs;
            m_Queue.GetAll(msgs);
            m_msgReady = false;

            pthread_mutex_unlock(&m_Mutex);

            ProcessMessage(msgs);
        }

        return rc;
    }

    bool Start()
    {
        bool rc = true;
        if (pthread_create(&m_ID, NULL, CThread::Run, reinterpret_cast<void*>(this)))
        {
            perror("pthread_create failed");
            rc = false;
        }
        return rc;
    }

    virtual bool ProcessMessage(tMsgList& msg) = 0;

    unsigned long AddMessage(QType msg)
    {
        pthread_mutex_lock(&m_Mutex);

        unsigned long rc = m_Queue.Add(msg);

        if (m_msgReady != true)
        {
            m_msgReady = true;
            pthread_cond_signal(&m_Cond);
        }

        pthread_mutex_unlock(&m_Mutex);
        return rc;
    }

    unsigned long AddMessages(tMsgList& msgs)
    {
        pthread_mutex_lock(&m_Mutex);

        unsigned long rc = m_Queue.Add(msgs);

        if (m_msgReady != true)
        {
            m_msgReady = true;
            pthread_cond_signal(&m_Cond);
        }

        pthread_mutex_unlock(&m_Mutex);
        return rc;
    }

    virtual bool Running()
    {
        bool rc = false;
        pthread_mutex_lock(&m_RunningMutex);
        rc = m_Running;
        pthread_mutex_unlock(&m_RunningMutex);
        return rc;
    }

    void SetRunning(bool value)
    {
        pthread_mutex_lock(&m_RunningMutex);
        m_Running = value;
        pthread_mutex_unlock(&m_RunningMutex);
    }

    int qSize()
    {
        int rc = 0;

        pthread_mutex_lock(&m_Mutex);
        rc = m_Queue.Size();
        pthread_mutex_unlock(&m_Mutex);

        return rc;
    }


private:
    pthread_t m_ID;
    pthread_cond_t m_Cond;
    pthread_mutex_t m_Mutex;
    pthread_mutex_t m_RunningMutex;
    bool m_msgReady;
    bool m_Running;
    CThreadQueue<QType> m_Queue;
};



#endif //_THREAD_H__
