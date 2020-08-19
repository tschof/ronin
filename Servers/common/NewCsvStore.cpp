#include "NewCsvStore.h"
//#include "Csv.h"
CNewCsvStore::CNewCsvStore() :
m_step(0)
{
    pthread_mutex_init(&m_lock, NULL);
    //Alloc();
}

CNewCsvStore::~CNewCsvStore()
{
    pthread_mutex_lock(&m_lock);
    
    CNewCsv* pCsv = NULL;
    unsigned int size = m_store.size();

    if (size < m_freeList.size())
    {
        printf("m_freeList is larger than m_store\n");
    }
    
    for (unsigned int i = 0; i < size; i++)
    {
        pCsv = m_store[i];
        delete pCsv;
    }

    pthread_mutex_unlock(&m_lock);
    
    pthread_mutex_destroy(&m_lock);
}

void CNewCsvStore::init(unsigned int step)
{
    m_step = step;
    Alloc();
}

CNewCsv* CNewCsvStore::Get()
{
    pthread_mutex_lock(&m_lock);

    if (m_freeList.size() == 0)
    {
        Alloc();
        printf("%s<current size=%d>\n", "CNewCsvStore free list is empty", (int)(m_store.size()));
    }

    CNewCsv* rt = m_freeList.top();
    m_freeList.pop();

    pthread_mutex_unlock(&m_lock);

    return rt;
}

void CNewCsvStore::Return(CNewCsv* pCsv)
{
    pthread_mutex_lock(&m_lock);
    pCsv->Reset();
    m_freeList.push(pCsv);
    pthread_mutex_unlock(&m_lock);
}

void CNewCsvStore::Alloc()
{
    CNewCsv* pCsv = NULL;
        
    for (unsigned int i = 0; i < m_step; i++)
    {
        pCsv = new CNewCsv();
        m_freeList.push(pCsv);
        m_store.push_back(pCsv);
    }
}
