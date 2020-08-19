#include "DBParm.h"
#include <stdlib.h>
#include <string.h>

CDBParm::CDBParm()
{
    memset(this, 0, sizeof(CDBParm));
}

CDBParm::~CDBParm()
{
    if (m_vbufsz)
    {
        free(m_buf);
    }

    if (m_namesz)
    {
        free(m_parm.name);
    }
}

void CDBParm::Set(const std::string& name, int value)
{
   if (name.length() >= m_namesz)
    {
        if (m_namesz)
        {
            free (m_parm.name);
        }
        m_namesz = name.length() + 1;
        m_parm.name = (char*)malloc(m_namesz);
    }

    strcpy(m_parm.name, name.c_str());
    m_parm.value.type_in_union = DB_INT;
    m_parm.value.g_un.un_int = value;
    m_parm.value.arr_len = 0;
}


void CDBParm::Set(const std::string& name, double value)
{
    if (name.length() >= m_namesz)
    {
        if (m_namesz)
        {
            free (m_parm.name);
        }
        m_namesz = name.length() + 1;
        m_parm.name = (char*)malloc(m_namesz);
    }
    
    strcpy(m_parm.name, name.c_str());;
    m_parm.value.type_in_union = DB_DOUBLE;
    m_parm.value.g_un.un_double = value;
    m_parm.value.arr_len = 0;
}


void CDBParm::Set(const std::string& name, const char* value)
{
    if (name.length() >= m_namesz)
    {
        if (m_namesz)
        {
            free (m_parm.name);
        }
        m_namesz = name.length() + 1;
        m_parm.name = (char*)malloc(m_namesz);
    }

    strcpy(m_parm.name, name.c_str());;
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

void CDBParm::Set(const std::string& name, const std::string& value)
{
    Set(name, value.c_str());
}


void CDBParm::Reset()
{
    /*
    if (m_parm.value.type_in_union == DB_ARRAY &&
        m_parm.value.arr_len)
    {
        free(m_parm.value.g_un.un_array);
    }
    memset(this, 0, sizeof(CDBParm));
    */
}


