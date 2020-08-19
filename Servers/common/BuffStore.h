#ifndef _BUFFERSTORE_H__
#define _BUFFERSTORE_H__

#include <vector>
#include <stack>
#include "databuf.h"
#include "pthread.h"

class CBuffStore
{
public:
    //CBuffStore(int bufSize, int bufNum);
        
    ~CBuffStore();

    static CBuffStore & getInstance()
    {
        static CBuffStore theStoreBuffer;
        return theStoreBuffer;
    }

    databuf* Get();
#define ReturnB(pObj) trick_Return(pObj, __FILE__, __LINE__, __func__)
    
    void trick_Return(databuf* pBuffer, const char* file, int line, const char* caller);
    //void ReturnB(databuf* pBuffer);
    void SetBufAllocSize(int value);
    void SetBufNumAllocStep(int value);

private:
    CBuffStore();
    void Alloc();

    int m_bufAllocSize;
    int m_bufNumallocStep;
    std::vector<databuf*> m_store;
    std::stack<databuf*> m_freeList;
    pthread_mutex_t m_lock;
};

#endif //_BUFFERSTORE_H__
