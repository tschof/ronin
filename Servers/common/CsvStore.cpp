#include "CsvStore.h"

CCsvStore::CCsvStore() :
m_step(0)
{
    pthread_mutex_init(&m_lock, NULL);
    //Alloc();
}

CCsvStore::~CCsvStore()
{
    pthread_mutex_lock(&m_lock);
    
    CCsv* pCsv = NULL;
    int size = m_store.size();

    if (size < m_freeList.size())
    {
        printf("m_freeList is larger than m_store\n");
    }
    
    for (int i = 0; i < size; i++)
    {
        pCsv = m_store[i];
        delete pCsv;
    }

    pthread_mutex_unlock(&m_lock);
    
    pthread_mutex_destroy(&m_lock);
}

void CCsvStore::init(unsigned int step)
{
    m_step = step;
    Alloc();
}

CCsv* CCsvStore::Get()
{
    pthread_mutex_lock(&m_lock);

    if (m_freeList.size() == 0)
    {
        Alloc();
        printf("%s<current size=%d>\n", "CCsvStore free list is empty", (int)(m_store.size()));
    }

    CCsv* rt = m_freeList.top();
    m_freeList.pop();

    pthread_mutex_unlock(&m_lock);

    return rt;
}

void CCsvStore::Return(CCsv* pCsv)
{
    pthread_mutex_lock(&m_lock);
    pCsv->Reset();
    m_freeList.push(pCsv);
    pthread_mutex_unlock(&m_lock);
}

void CCsvStore::Alloc()
{
    CCsv* pCsv = NULL;

    for (int i = 0; i < m_step; i++)
    {
        pCsv = new CCsv();
        m_freeList.push(pCsv);
        m_store.push_back(pCsv);
    }
}

