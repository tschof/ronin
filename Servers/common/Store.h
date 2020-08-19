#ifndef _STORE_H__
#define _STORE_H__


#include <vector>
#include <stack>
#include <ext/hash_map>
#include "pthread.h"
#include "debug_logger.h"
#include "Utility.h"

using namespace __gnu_cxx;

int printf ( const char * format, ... );
int sprintf(char *str, const char *format, ...);

template<class tObj>
class CStore
{
public:
    typedef hash_map<long, bool> tCheckMap;
    
    ~CStore()
    {
        pthread_mutex_lock(&m_lock);

        tObj* pObj = NULL;
        size_t size = m_store.size();

        if (size < m_freeList.size())
        {
            printf("m_freeList is larger than m_store\n");
        }

        for (size_t i = 0; i < size; i++)
        {
            pObj = m_store[i];
            delete pObj;
        }

        pthread_mutex_unlock(&m_lock);

        pthread_mutex_destroy(&m_lock);
    }


    static CStore & getInstance()
    {
        static CStore<tObj> theStore;
        return theStore;
    }

    void init(unsigned long base, unsigned int step)
    {
        m_step = step;
        Alloc(base);
    }

    tObj* Get()
    {
        pthread_mutex_lock(&m_lock);

        if (m_freeList.size() == 0)
        {
            Alloc(m_step);
            printf("%s<current size=%d>\n", "CStore free list is empty", (int)(m_store.size()));
        }

        tObj* rt = m_freeList.top();
        m_freeList.pop();
        //m_checkMap.erase((long)rt);

        pthread_mutex_unlock(&m_lock);

        return rt;
    }

       
    void trick_Return(tObj* pObj, const char* file, int line, const char* caller)
    {
        pthread_mutex_lock(&m_lock);
        
        /*
        pair<tCheckMap::iterator, bool> p = m_checkMap.insert(tCheckMap::value_type((long)pObj, true));
        if (p.second == false)
        {
            //dup return!!!
            char err[516];
            sprintf(err, "Return() has been called with duplicated object by %s in %s at line %d", 
                    caller, file, line);
            send_debug_message(err);
            //send email
            std::string subj = "Duplicated objects has been put back to pool";
            CUtility::SendTechEMail(subj, err, strlen(err));
        }
        else
        {
            pObj->Reset();
            m_freeList.push(pObj);
        }
        */
        pObj->Reset();
        m_freeList.push(pObj);
        
        pthread_mutex_unlock(&m_lock);
    }
    

        
#define Return(pObj) trick_Return(pObj, __FILE__, __LINE__, __func__)
    
    void ReturnMany(std::vector<tObj*>& objs)
    {
        int sz = objs.size();

        //pthread_mutex_lock(&m_lock);

        for (int i = 0; i < sz; i++)
        {
            //objs[i]->Reset();
            //m_freeList.push(objs[i]);
            Return(objs[i]);
        }

        //pthread_mutex_unlock(&m_lock);

        objs.clear();
    }

    unsigned long FreeSize()
    {
        unsigned long rc = 0;
        pthread_mutex_lock(&m_lock);
        rc = m_freeList.size();
        pthread_mutex_unlock(&m_lock);
        return rc;
    }

    unsigned long StoreSize()
    {
        unsigned long rc = 0;
        pthread_mutex_lock(&m_lock);
        rc = m_store.size();
        pthread_mutex_unlock(&m_lock);
        return rc;
    }

private:
    CStore()
    {
        m_step = 500;
        pthread_mutex_init(&m_lock, NULL);
    }
    void Alloc(unsigned long step)
    {
        tObj* pObj = NULL;

        for (unsigned long i = 0; i < step; i++)
        {
            pObj = new tObj();
            m_freeList.push(pObj);
            //m_checkMap.insert(tCheckMap::value_type((long)pObj, true));
            m_store.push_back(pObj);
        }
    }

    std::vector<tObj*> m_store;
    std::stack<tObj*> m_freeList;
    //hash_map<long, bool> m_checkMap;
    unsigned int m_step;
    pthread_mutex_t m_lock;
};

#endif //_STORE_H__