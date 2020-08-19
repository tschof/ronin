#ifndef _CSVSTORE_H__
#define _CSVSTORE_H__

#include <vector>
#include <stack>
#include "Csv.h"
#include "pthread.h"

class CCsvStore
{
public:
    CCsvStore();
    ~CCsvStore();

    static CCsvStore & getInstance()
    {
        static CCsvStore theStore;
        return theStore;
    }

    void init(unsigned int step);
    
    CCsv* Get();
    void Return(CCsv* pCsv);

private:
    void Alloc();

    std::vector<CCsv*> m_store;
    std::stack<CCsv*> m_freeList;
    unsigned int m_step;
    pthread_mutex_t m_lock;
};

#endif //_CSVSTORE_H__
