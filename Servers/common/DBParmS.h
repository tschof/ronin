/*
 * File:   DBParm.h
 * Author: zxie
 *
 * Created on May 29, 2014, 2:44 PM
 */

#ifndef DBPARMS_H
#define	DBPARMS_H

//#include "generic_db.h"
#include <string>
#include <vector>

class CDBParmS
{
public:
    CDBParmS();
    ~CDBParmS();
    void Set(const std::string& name, int value);
    void Set(const std::string& name, double value);
    void Set(const std::string& name, const std::string& value);
    void Set(const std::string& name, const char* value);

    void Reset();
    gendb_parm* parm() { return &m_parm; }

private:
    unsigned int m_vbufsz;
    gendb_parm m_parm;
    char* m_buf;  //override to the gendb_parm.value.g_un.un_array
                  //so that this obj can be reused
};

#endif	/* DBPARMS_H */

