#ifndef _HEAVYDUTY_H__
#define _HEAVYDUTY_H__

#include "Thread.h"
//#include "Csv.h"

typedef struct Duty
{
    void*(*theFunc)(void*);
    void* pArg;
    bool tmp;
} tDuty;

typedef std::deque<tDuty> tReqList;
typedef tReqList::iterator tReqListIt;

class CHeavyDuty : public CThread<tDuty>
{
public:

    static CHeavyDuty & getInstance()
	{
		static CHeavyDuty theDuty;
		return theDuty;
	}

    ~CHeavyDuty() {}

    virtual bool ProcessMessage(tReqList& reqlist);

private:
    CHeavyDuty() {}

};

#endif //_HEAVYDUTY_H__