#ifndef _ROMMAPN_H__
#define _ROMMAPN_H__

#include <string>
#include <list>
#include <set>
#include "pthread.h"

#ifndef _WIN32
#include <ext/hash_map>
using namespace __gnu_cxx;
namespace __gnu_cxx
{
template<>
struct hash<std::string>
{
    size_t operator() (std::string const& s) const
    {
        return __stl_hash_string (s.c_str());
    }
};
}
//typedef hash_map<std::string, void*, hash<std::string> > tMap;
#else
#include <hash_map>
using namespace stdext;
//typedef hash_map<std::string, > tMap;
#endif

//typedef tMap::iterator tMapIt;

template<class VALUE>
class CRomMap
{
public:

#ifndef _WIN32
    typedef hash_map<std::string, VALUE, hash<std::string> > tMap;
#else
    typedef hash_map<std::string, VALUE> tMap;
#endif

    typedef typename tMap::iterator tMapIt;

    typedef struct TKEYVALUEPAIR
    {
        std::string key;
        VALUE value;
    } tKeyValuePair;

    //CRomMap() {}

    ~CRomMap(void)
    {
        if (m_doSynch)
        {
            pthread_rwlock_destroy(&m_rwMutex);
        }
    }

    bool DoSynch() const
    {
        return m_doSynch;
    }

    CRomMap(bool doSynch = false)
    {
        m_doSynch = doSynch;

        if (m_doSynch)
        {
            pthread_rwlock_init(&m_rwMutex, NULL);
        }
    }

    bool Add(const std::string& key, const VALUE& value)
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        bool rc = false;
        tMapIt it = m_Map.find(key);

        if (it == m_Map.end())
        {
            m_Map.insert(typename tMap::value_type(key, value));
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
        return rc;
    }

    bool Add(const char* pKey, const VALUE& value)
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }
        std::string key = pKey;
        bool rc = false;
        tMapIt it = m_Map.find(key);

        if (it == m_Map.end())
        {
            m_Map.insert(typename tMap::value_type(key, value));
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
        return rc;
    }

    VALUE AddOrReplace(const std::string& key, const VALUE& value)
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        tMapIt it = m_Map.find(key);

        VALUE lvalue;

        if (it != m_Map.end())
        {
            lvalue = it->second;
            m_Map.erase(it);
            m_Map.insert(typename tMap::value_type(key, value));
        }
        else
        {
            lvalue = value;
            m_Map.insert(typename tMap::value_type(key, value));
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }

        return lvalue;
    }

    void RemoveAll()
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        m_Map.clear();

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
    }

    bool ReplaceKey(const std::string& oldkey, const std::string& newkey)
    {
        bool rc = false;

        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        tMapIt it = m_Map.find(oldkey);

        VALUE value;
        if (it != m_Map.end())
        {
            value = it->second;
            m_Map.erase(it);
            m_Map.insert(typename tMap::value_type(newkey, value));
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }

        return rc;
    }

    void* RemoveNext()
    {
        void* rc = NULL;

        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        tMapIt it = m_Map.begin();

        if (it != m_Map.end())
        {
            rc = it->second;
            m_Map.erase(it);
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }

        return rc;
    }

    bool Remove(const std::string& key, VALUE& value)
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        bool rc = false;

        tMapIt it = m_Map.find(key);

        if (it != m_Map.end())
        {
            value = it->second;
            m_Map.erase(it);
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
        return rc;
    }

    VALUE Update(const std::string& key, const VALUE& value)
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        VALUE rc;

        tMapIt it = m_Map.find(key);

        if (it != m_Map.end())
        {
            rc = it->second;
            it->second = value;
        }
        else
        {
            rc = value;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
        return rc;
    }

    bool Find(const std::string& key)
    {
        bool rc = false;

        if (m_doSynch)
        {
            pthread_rwlock_rdlock(&m_rwMutex);
        }

        tMapIt it = m_Map.find(key);

        if (it != m_Map.end())
        {
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }

        return rc;
    }

    bool Get(const std::string& key, VALUE& value)
    {
        if (m_doSynch)
        {
            pthread_rwlock_rdlock(&m_rwMutex);
        }

        bool rc = false;

        tMapIt it = m_Map.find(key);

        if (it != m_Map.end())
        {
            value = it->second;
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
        return rc;
    }

    bool GetAll(std::list<VALUE>& all)
    {
        if (m_doSynch)
        {
            pthread_rwlock_rdlock(&m_rwMutex);
        }

        bool rc = false;

        tMapIt it = m_Map.begin();

        while (it != m_Map.end())
        {
            all.push_back(it->second);
            it++;
            rc = true;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
        return rc;
    }

    void GetAllKeys(std::set<std::string>& keys)
    {
        if (m_doSynch)
        {
            pthread_rwlock_rdlock(&m_rwMutex);
        }

        tMapIt it = m_Map.begin();

        while (it != m_Map.end())
        {
            keys.insert(it->first);
            it++;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
    }

    void GetKeyValuePairs(std::list<tKeyValuePair>& rKeyValues)
    {
        if (m_doSynch)
        {
            pthread_rwlock_rdlock(&m_rwMutex);
        }

        tKeyValuePair keyvalue;

        tMapIt it = m_Map.begin();

        while (it != m_Map.end())
        {
            keyvalue.key = it->first;
            keyvalue.value = it->second;
            rKeyValues.push_back(keyvalue);
            it++;
        }

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
    }

    void Swap(CRomMap& newMap)
    {
        if (m_doSynch)
        {
            pthread_rwlock_wrlock(&m_rwMutex);
        }

        m_Map.swap((newMap.theMap()));

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }
    }

    unsigned long Size()
    {
        if (m_doSynch)
        {
            pthread_rwlock_rdlock(&m_rwMutex);
        }

        unsigned long rc = m_Map.size();

        if (m_doSynch)
        {
            pthread_rwlock_unlock(&m_rwMutex);
        }

        return rc;
    }

    //hash_map<KEY, VALUE>& theMap() { return m_Map; }
    tMap& theMap()
    {
        return m_Map;
    }//const{ return const_cast<tMap*>(&m_Map); }

private:
    //hash_map<KEY, VALUE> m_Map;
    tMap m_Map;
    pthread_rwlock_t m_rwMutex;
    bool m_doSynch;
};

#endif //_ROMMAPN_H__
