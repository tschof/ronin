#include "BuffStore.h"
#include <stdio.h>
#include "debug_logger.h"
#include "Utility.h"

//CBuffStore::CBuffStore(int bufSize, int bufNum) :
//m_bufAllocSize(bufSize),
//m_bufNumallocStep(bufNum),
//m_store(bufNum)
CBuffStore::CBuffStore() :
    m_bufAllocSize(400),
    m_bufNumallocStep(1000),
    m_store(200)
{
    pthread_mutex_init(&m_lock, NULL);
    Alloc();
}

CBuffStore::~CBuffStore()
{
    databuf* pBuffer = NULL;
    int size = m_store.size();

    for (int i = 0; i < size; i++)
    {
        pBuffer = m_store[i];
        databuf_destroy(pBuffer);
    }

    pthread_mutex_destroy(&m_lock);
}


databuf* CBuffStore::Get()
{
    pthread_mutex_lock(&m_lock);

    if (m_freeList.size() == 0)
    {
        Alloc();
    }

    databuf* rt = m_freeList.top();
    m_freeList.pop();
    
    pthread_mutex_unlock(&m_lock);

    return rt;
}

void CBuffStore::trick_Return(databuf* pBuffer, const char* file, int line, const char* caller)
//void CBuffStore::ReturnB(databuf* pBuffer)
{
    pthread_mutex_lock(&m_lock);
    
    databuf_reset(pBuffer);
    m_freeList.push(pBuffer);
    
    /*
    pair<tCheckMap::iterator, bool> p = m_checkMap.insert(tCheckMap::value_type((long)pBuffer, true));
    if (p.second == false)
    {
        //dup return!!!
        char err[516];
        sprintf(err, "ReturnB() has been called with duplicated object by % in %s at line %d\n", 
                caller, file, line);
        send_debug_message(err);
        //send email
        std::string subj = "Duplicated objects has been put back to pool";
        CUtility::SendTechEMail(subj, err, strlen(err));
    }
    else
    {
        databuf_reset(pBuffer);
        m_freeList.push(pBuffer);
    }
    */
    pthread_mutex_unlock(&m_lock);
}

void CBuffStore::Alloc()
{
    databuf* pBuffer = NULL;
    unsigned int size = m_store.size() + m_bufNumallocStep;
    m_store.reserve(size);

    for (int i = 0; i < m_bufNumallocStep; i++)
    {
        pBuffer = databuf_alloc(m_bufAllocSize);
        m_freeList.push(pBuffer);
        m_store.push_back(pBuffer);
    }
}

void CBuffStore::SetBufAllocSize(int value)
{
    pthread_mutex_lock(&m_lock);
    m_bufAllocSize =  value;
    pthread_mutex_unlock(&m_lock);
}

void CBuffStore::SetBufNumAllocStep(int value)
{
    pthread_mutex_lock(&m_lock);
    m_bufNumallocStep = value;
    pthread_mutex_unlock(&m_lock);
}

