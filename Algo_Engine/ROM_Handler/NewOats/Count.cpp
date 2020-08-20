#include "StdAfx.h"
#include "Count.h"

CCount::CCount()
{
	InitializeCriticalSection(&m_Lock);
	m_Count = 0;
}

CCount::~CCount()
{
	DeleteCriticalSection(&m_Lock);
}

void CCount::Increment(long value)
{
	EnterCriticalSection(&m_Lock);
	m_Count += value;
	LeaveCriticalSection(&m_Lock);
}

void CCount::Decrement(long value)
{
	EnterCriticalSection(&m_Lock);
	m_Count -= value;
	LeaveCriticalSection(&m_Lock);
}

long CCount::Value()
{
	long rc = 0;

	EnterCriticalSection(&m_Lock);
	rc = m_Count;
	LeaveCriticalSection(&m_Lock);

	return rc;
}

CCounts::CCounts(int nThreads)
{
	CCount* pCount = NULL;

	for (int i = 0; i < nThreads; i++)
	{
		pCount = new CCount();
		m_Counts.push_back(pCount);
	}
}


CCounts::~CCounts()
{
	int size = m_Counts.size();

	for (int i = 0; i < size; i++)
	{
		delete m_Counts[i];
	}
}



void CCounts::Increment(int index, long value)
{
	if (index <= m_Counts.size())
	{
		(m_Counts[index])->Increment(value);
	}
}


long CCounts::Value()
{
	long rc = 0;

	for (int i = 0; i < m_Counts.size(); i++)
	{
		rc += (m_Counts[i])->Value();
	}

	return rc;
}
