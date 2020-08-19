#include "DBParmS.h"
#include <stdlib.h>
#include <string.h>

CDBParmS::CDBParmS()
{
    memset(this, 0, sizeof(CDBParmS));
}

CDBParmS::~CDBParmS()
{
    if (m_vbufsz)
    {
        free(m_buf);
    }
}

void CDBParmS::Set(const std::string& name, int value)
{
    m_parm.name = const_cast<char*>(name.c_str());
    m_parm.value.type_in_union = DB_INT;
    m_parm.value.g_un.un_int = value;
    m_parm.value.arr_len = 0;
    if (m_vbufsz)
    {
        free(m_buf);
        m_buf = NULL;
        m_vbufsz = 0;
    }
}


void CDBParmS::Set(const std::string& name, double value)
{
    m_parm.name = const_cast<char*>(name.c_str());
    m_parm.value.type_in_union = DB_DOUBLE;
    m_parm.value.g_un.un_double = value;
    m_parm.value.arr_len = 0;
    if (m_vbufsz)
    {
        free(m_buf);
        m_buf = NULL;
        m_vbufsz = 0;
    }
}


void CDBParmS::Set(const std::string& name, const char* value)
{
    m_parm.name = const_cast<char*>(name.c_str());

    m_parm.value.type_in_union = DB_ARRAY;
    size_t sz = strlen(value);

    if (sz >= m_vbufsz)
    {
        if (m_vbufsz)
        {
            free(m_buf);
        }
        m_vbufsz = sz + 1;
        m_buf = (char*)malloc(m_vbufsz);
    }

    strcpy(m_buf, value);
    m_parm.value.g_un.un_array = m_buf;
    m_parm.value.arr_len = sz;
}

void CDBParmS::Set(const std::string& name, const std::string& value)
{
    Set(name, value.c_str());
}


void CDBParmS::Reset()
{
    m_parm.name = NULL;
}


