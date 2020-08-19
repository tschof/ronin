/*
 * File:   DBParm.h
 * Author: zxie
 *
 * Created on May 29, 2014, 2:44 PM
 */

#ifndef DBPARM_H
#define	DBPARM_H
#include "db_struct.h"
//#include "generic_db.h"
//#include "psql_db.h"
#include <string>
#include <vector>

class CDBParm
{
public:
    CDBParm();
    ~CDBParm();
    void Set(const std::string& name, int value);
    void Set(const std::string& name, double value);
    void Set(const std::string& name, const std::string& value);
    void Set(const std::string& name, const char* value);

    void Reset();
    gendb_parm* parm() { return &m_parm; }

private:
    unsigned int m_vbufsz;
    unsigned int m_namesz;
    gendb_parm m_parm;
    char* m_buf;  //override to the gendb_parm.value.g_un.un_array
                  //so that this obj can be reused
};

/*class CDBParms
{
public:
    CDBParms();
    ~CDBParms();

    void Add(CDBParm* pValue);
    CDBParm* Get(int index) const;
    int len();

private:
    std::vector<CDBParm*> m_parms;
};*/



#endif	/* DBPARM_H */

