#ifndef _COUNT_H__
#define _COUNT_H__

#include "afxmt.h"

class CCount
{
public:
	CCount();
	void Increment(long value);
	long Value();

private:
	long m_Count;
	CCriticalSection m_Lock;
};


#endif // _COUNT_H__
