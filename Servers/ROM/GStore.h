#ifndef _GSTORE_H__
#define _GSTORE_H__

#include <vector>
#include <stack>
#include "pthread.h"
#include "RomOrderNew.h"

class CGStore
{
public:
    CGStore() :
    m_step(0)
    {
        pthread_mutex_init(&m_lock, NULL);
        //Alloc();
    }

    ~CGStore()
    {
        CRomOrderNew* pObj = NULL;
        int size = m_store.size();

        for (int i = 0; i < size; i++)
        {
            pObj = m_store[i];
            delete pObj;
        }

        pthread_mutex_destroy(&m_lock);
    }

    static CGStore & getInstance()
    {
        static CGStore theStore;
        return theStore;
    }

    void init(unsigned int step)
    {
        m_step = step;
        Alloc();
    }

    CRomOrderNew* Get()
    {
        pthread_mutex_lock(&m_lock);

        if (m_freeList.size() == 0)
        {
            printf("%s<current allocated orders=%d>\n", "free list is empty", (int)(m_store.size()));
            Alloc();
        }

        CRomOrderNew* rt = m_freeList.top();
        m_freeList.pop();

        pthread_mutex_unlock(&m_lock);

        return rt;
    }

    void Return(CRomOrderNew* obj)
    {
        pthread_mutex_lock(&m_lock);
        //obj->Reset(false);
        obj->Reset();
        m_freeList.push(obj);
        pthread_mutex_unlock(&m_lock);
    }

private:
    void Alloc()
    {
        CRomOrderNew* obj = NULL;

        for (unsigned int i = 0; i < m_step; i++)
        {
            obj = new CRomOrderNew;
            m_freeList.push(obj);
            m_store.push_back(obj);
        }
    }

    std::vector<CRomOrderNew*> m_store;
    std::stack<CRomOrderNew*> m_freeList;
    unsigned int m_step;
    pthread_mutex_t m_lock;
};


#endif //_GSTORE_H__
