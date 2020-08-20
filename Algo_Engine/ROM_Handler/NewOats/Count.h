#ifndef _COUNT_H__
#define _COUNT_H__

//#include "afxmt.h"
#include <vector>

class CCount
{
public:
	CCount();
	~CCount();

	void Increment(long value);
	void Decrement(long value);
	long Value();

private:
	CRITICAL_SECTION m_Lock;
	long m_Count;
};

class CCounts
{
public:
	CCounts(int nThreads);
	~CCounts();

	void Increment(int index, long value);
	long Value();

private:
	std::vector<CCount*> m_Counts;
};


#endif // _COUNT_H__
