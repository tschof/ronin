#ifndef _CNEWSVSTORE_H__
#define _CNEWSVSTORE_H__

#include <vector>
#include <stack>
#include "NewCsv.h"
#include "pthread.h"

class CNewCsvStore
{
public:
    CNewCsvStore();
    ~CNewCsvStore();

    static CNewCsvStore & getInstance()
    {
        static CNewCsvStore theStore;
        return theStore;
    }

    void init(unsigned int step);
    
    CNewCsv* Get();
    void Return(CNewCsv* pCsv);

private:
    void Alloc();

    std::vector<CNewCsv*> m_store;
    std::stack<CNewCsv*> m_freeList;
    unsigned int m_step;
    pthread_mutex_t m_lock;
};

#endif //_CNEWSVSTORE_H__
