#ifndef _ROMTYPEDEF_H__
#define _ROMTYPEDEF_H__

//#include "ace/Message_Block.h"
#include "Csv.h"
#include <string>
#include <list>
//#include "generic_db.h"
//#include "psql_db.h"

namespace ROMTYPEDEF
{
//static const short NUM_PARMS = 74;

typedef struct TDESTSTRUCT
{
    bool newFlag;   //0:replace IP/Port of the existing connection
    //1:new connection
    bool block;
    unsigned short ID;
    char IP[20];
    unsigned short Port;
} tDestStruct;

typedef struct TLOGINSTRUCT
{
    std::string login;
    std::string password;
    int resetday;
    time_t lastLogonTime;
    int msgrateLimit[4];
} tLoginStruct;

typedef struct TCONNECTIONSTATUS
{
    bool connected;
    bool loggedon;
    bool blocked;
    std::string id;
    std::string description;
} tConnectionStatus;

//typedef struct OATSDBPARM
//{
//    int len;
//    gendb_parm parms[NUM_PARMS];
//    void Reset() { memset(this, 0, sizeof(struct OATSDBPARM)); }
//} tOatsDbParm;

typedef std::list<tDestStruct> tDestStructList;
typedef tDestStructList::iterator tDestStructListIt;
typedef std::list<tLoginStruct> tLoginList;
typedef tLoginList::iterator tLoginListIt;

template<class tType>
class CNumType
{
private:
    bool m_valid;
    tType m_value;

public:

    CNumType()
    {
        m_valid = false;
    }

    CNumType(tType value)
    {
        m_valid = value;
        m_value = true;
    }

    ~CNumType()
    {

    }

    void Set(tType v)
    {
        m_value = v;
        m_valid = true;
    }

    bool Get(tType& lvalue) const
    {
        if (m_valid)
        {
            lvalue = m_value;
            return true;
        }
        else
        {
            return false;
        }
    }

    void Reset()
    {
        m_valid = false;
    }

};

/*
typedef struct
{
    struct iovec* pIovec;
    size_t iovecSize;
} tMsgStruct;
*/
};


#endif //_ROMTYPEDEF_H__
