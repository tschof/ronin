#include "RomCount.h"

CRomCount::CRomCount(unsigned long inicount, bool dosynch) :
    m_doSynch(dosynch),
    m_count(inicount)
{
    if (m_doSynch)
    {
        pthread_rwlock_init(&m_rwMutex, NULL);
    }
}

CRomCount::~CRomCount()
{
    if (m_doSynch)
    {
        pthread_rwlock_destroy(&m_rwMutex);
    }
}

void CRomCount::Reset(unsigned long value)
{
    if (m_doSynch)
    {
        pthread_rwlock_wrlock(&m_rwMutex);
    }

    m_count = value;

    if (m_doSynch)
    {
        pthread_rwlock_unlock(&m_rwMutex);
    }
}

unsigned long CRomCount::Increment(unsigned long mod)
{
    unsigned long rc = 0;

    if (m_doSynch)
    {
        pthread_rwlock_wrlock(&m_rwMutex);
    }

    m_count += mod;
    rc = m_count;

    if (m_doSynch)
    {
        pthread_rwlock_unlock(&m_rwMutex);
    }

    return rc;
}

unsigned long CRomCount::Decrement(unsigned long mod)
{
    unsigned long rc = 0;

    if (m_doSynch)
    {
        pthread_rwlock_wrlock(&m_rwMutex);
    }

    if (m_count > mod)
    {
        m_count -= mod;
        rc = m_count;
    }
    else
    {
        m_count = 0;
    }

    rc = m_count;

    if (m_doSynch)
    {
        pthread_rwlock_unlock(&m_rwMutex);
    }

    return rc;
}

unsigned long CRomCount::GetCount()
{
    unsigned long rc = 0;

    if (m_doSynch)
    {
        pthread_rwlock_rdlock(&m_rwMutex);
    }

    rc = m_count;

    if (m_doSynch)
    {
        pthread_rwlock_unlock(&m_rwMutex);
    }

    return rc;
}
