#include "StdAfx.h"
#include "Count.h"



CCount::CCount()
{
	m_Count = 0;
}


void CCount::Increment(long value)
{
	m_Lock.Lock();
	m_Count += value;
	m_Lock.Unlock();
}


long CCount::Value()
{
	long rc = 0;
	m_Lock.Lock();
	rc = m_Count;
	m_Lock.Unlock();
	return rc;
}
