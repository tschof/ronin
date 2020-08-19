#ifndef _IOVECSTORE_H__
#define _IOVECSTORE_H__

#include <vector>
#include <stack>
#include "pthread.h"
#include <sys/uio.h>

typedef struct
{
    struct iovec* vec;
    size_t size;
} tIovec;

class CIovecStore
{
public:
    CIovecStore(unsigned int iniSize = 500);
    ~CIovecStore();

    static CIovecStore & getInstance()
    {
        static CIovecStore theIovecStore;
        return theIovecStore;
    }

    tIovec* Get();
    void Return(tIovec* pIovec);

private:
    void Alloc();

    int m_AllocSize;
    int m_NumallocStep;
    std::vector<tIovec*> m_store;
    std::stack<tIovec*> m_freeList;
    pthread_mutex_t m_lock;
};

#endif //_IOVECSTORE_H__
