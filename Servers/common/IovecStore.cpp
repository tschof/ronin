#include <stdlib.h>
#include "IovecStore.h"
#include "romfielddefs.h"


CIovecStore::CIovecStore(unsigned int iniSize) :
    m_NumallocStep(iniSize),
    m_store(iniSize)
{
    pthread_mutex_init(&m_lock, NULL);
    Alloc();
}

CIovecStore::~CIovecStore()
{
    int size = m_store.size();

    for (int i = 0; i < size; i++)
    {
        free(m_store[i]);
    }

    pthread_mutex_destroy(&m_lock);
}


tIovec* CIovecStore::Get()
{
    pthread_mutex_lock(&m_lock);

    if (m_freeList.size() == 0)
    {
        Alloc();
    }

    tIovec* rt = m_freeList.top();
    m_freeList.pop();

    pthread_mutex_unlock(&m_lock);

    return rt;
}

void CIovecStore::Return(tIovec* pIovec)
{
    pthread_mutex_lock(&m_lock);
    m_freeList.push(pIovec);
    pthread_mutex_unlock(&m_lock);
}

void CIovecStore::Alloc()
{
    tIovec* pIovec = NULL;
    unsigned int size = m_store.size() + m_NumallocStep;
    m_store.resize(size);

    for (int i = 0; i < m_NumallocStep; i++)
    {
        pIovec = (tIovec*)calloc(ROMFIELDS::MAXFIELDS, sizeof(tIovec));
        m_freeList.push(pIovec);
        m_store.push_back(pIovec);
    }
}

