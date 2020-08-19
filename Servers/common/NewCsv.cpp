#include <stdio.h>
#include <stdlib.h>
#include "BuffStore.h"
#include "Utility.h"
#include "NewCsv.h"

void CNewCsv::Init()
{
    memset(this, 0, sizeof(CNewCsv));
    m_pOriginalFields = CBuffStore::getInstance().Get();
    m_pModifiedFields = CBuffStore::getInstance().Get();
}

CNewCsv::CNewCsv()
{
    Init();
    //BuildEmpty();
}


CNewCsv::CNewCsv(CNewCsv* pSource)
{
    Init();
    Set(pSource);
}

CNewCsv::CNewCsv(char* msg, unsigned int len)
{
    Init();
    if (msg[len - 1] == ROMVALUE::MESSAGE_END)
    {
        if (msg[len - 2] != ROMFIELDS::DELIMITER)
        {
            msg[len - 1] = ROMFIELDS::DELIMITER;
        }
        else
        {
            len--;
        }
    }
 
    BuildCsv(msg, len);
}

void CNewCsv::Set(const std::string& msg)
{
    Set(msg.c_str(), msg.length());
}

void CNewCsv::Set(const char* msg, unsigned int len)
{
    BuildCsv(msg, len);
}

void CNewCsv::Set(databuf_t* pBuf)
{
    m_pOriginalFields = pBuf;
    buildCsv();
}

void CNewCsv::Set(CNewCsv* pSource)
{
    databuf_t* pOriginal = m_pOriginalFields;
    databuf_t* pModified = m_pModifiedFields;
    
    memcpy(this, pSource, sizeof(CNewCsv));
    m_pOriginalFields = pOriginal;
    m_pModifiedFields = pModified;
    
    unsigned int ol = databuf_unread(pSource->OriginalFields());
    unsigned int ml = databuf_unread(pSource->ModifiedFields());
    m_bufferLen = ol;
    databuf_memcpy(m_pOriginalFields, pSource->OriginalFields(), ol);
    databuf_memcpy(m_pModifiedFields, pSource->ModifiedFields(), ml);
    
    for (unsigned int i = 0; i < m_currBond; i++)
    {
        if (m_fields[i].updated)
        {
            m_fields[i].pBuf = m_pModifiedFields;
        }
        else
        {
            m_fields[i].pBuf = m_pOriginalFields;
        }
    }
}
   
void CNewCsv::buildCsv()
{
    char* pCur = m_pOriginalFields->buffer;
    unsigned len = databuf_unread(m_pOriginalFields);
    unsigned int count = 0;
    unsigned int offset = 0;
    void* pTmp = NULL;
    long bytestosearch = len;
    m_bufferLen = len;
    
    while ( bytestosearch > 0 &&
            (pTmp = memchr(pCur, ROMFIELDS::DELIMITER, bytestosearch)) != NULL &&
            count < ROMFIELDS::MAXFIELDS)
    {
        len = (char*)pTmp - pCur + 1;
        m_fields[count].pBuf = m_pOriginalFields;
        m_fields[count].offset = offset;
        m_fields[count].value_len = len;
        offset += len;
        bytestosearch -= len;
        count++;
        pCur = (char*)pTmp + 1;
    }

    if (bytestosearch > 0 &&
        count < ROMFIELDS::MAXFIELDS)
    {
        m_fields[count].pBuf = m_pOriginalFields;
        m_fields[count].offset = offset;
        m_fields[count].value_len = bytestosearch;
        count++;
    }

    m_currBond = count;
}

void CNewCsv::BuildEmpty()
{
    unsigned int count = 0;
    unsigned int offset = 0;
       
    while (count < ROMFIELDS::MAXFIELDS)
    {
        m_fields[count].pBuf = m_pOriginalFields;
        m_fields[count].offset = offset;
        m_fields[count].value_len = 1;
        char* pPtr = &(m_pOriginalFields->buffer[offset]);
        *pPtr = ROMFIELDS::DELIMITER;
        
        offset++;
        count++;
    }

    m_currBond = count;
}

void CNewCsv::BuildCsv(const char* msg, unsigned int len)
{
    databuf_memcpy(m_pOriginalFields, msg, len);
    buildCsv();
}

CNewCsv::~CNewCsv(void)
{
    CBuffStore::getInstance().Return(m_pOriginalFields);
    CBuffStore::getInstance().Return(m_pModifiedFields);
}

bool CNewCsv::GetWriteV(tWriteV& writev)
{
    int len = 0;
    char* curSrcOffset = m_pOriginalFields->buffer;
    unsigned int vecIndex = 0;
    
    if (m_updated == false &&
        curSrcOffset)
    {
        writev.iov[0].iov_base = curSrcOffset;
        writev.iov[0].iov_len = m_bufferLen;
        writev.iovcnt = 1;
        vecIndex = 1;
    }
    else
    {
        unsigned int index = 0;
                
        while (index < m_currBond)
        {
            tFieldStruct& fld = m_fields[index];
                       
            if (!len)
            {
                curSrcOffset = fld.pBuf->buffer + fld.offset;
            }

            if (fld.updated)
            {
                if (len) //read out the content whenever a updated field is hit!!
                {
                    writev.iov[vecIndex].iov_base = curSrcOffset;
                    writev.iov[vecIndex].iov_len = len;
                    writev.iovcnt++;
                    vecIndex++;
                }

                writev.iov[vecIndex].iov_base = fld.pBuf->buffer + fld.offset;
                writev.iov[vecIndex].iov_len = fld.value_len;
                writev.iovcnt++;
                vecIndex++;
                
                if (index < m_currBond - 1)
                {
                    curSrcOffset = m_fields[index + 1].pBuf->buffer + m_fields[index + 1].offset;
                }

                len = 0;
            }
            else
            {
                len += fld.value_len;
            }
            index++;
        }

        if (len)
        {
            writev.iov[vecIndex].iov_base = curSrcOffset;
            writev.iov[vecIndex].iov_len = len;
            writev.iovcnt++;
            vecIndex++;
        }
    }
    
    writev.iov[vecIndex].iov_base = const_cast<char*>(&(ROMVALUE::MESSAGE_END));
    writev.iov[vecIndex].iov_len = 1;
    writev.iovcnt++;
    
    return true;
}

void CNewCsv::GetMsg(databuf_t* pBuffer)
{
    int len = 0;
    char* curSrcOffset = m_pOriginalFields->buffer;

    if (m_updated == false &&
        curSrcOffset)
    {
        databuf_memcpy(pBuffer, curSrcOffset, m_bufferLen);
    }
    else
    {
        unsigned int index = 0;
                
        while (index < m_currBond)
        {
            tFieldStruct& fld = m_fields[index];
                       
            if (!len)
            {
                curSrcOffset = fld.pBuf->buffer + fld.offset;
            }

            if (fld.updated)
            {
                if (len) //read out the content whenever a updated field is hit!!
                {
                    databuf_memcpy(pBuffer, curSrcOffset, len);
                }

                databuf_memcpy(pBuffer, fld.pBuf->buffer + fld.offset, fld.value_len);
                
                if (index < m_currBond - 1)
                {
                    curSrcOffset = m_fields[index + 1].pBuf->buffer + m_fields[index + 1].offset;
                }

                len = 0;
            }
            else
            {
                len += fld.value_len;
            }
            index++;
        }

        if (len)
        {
            databuf_memcpy(pBuffer, curSrcOffset, len);
        }
    }
    
    char end = pBuffer->buffer[databuf_unread(pBuffer) - 1];
    if (end != ROMFIELDS::ENDER)
    {
        databuf_memcpy(pBuffer, &(ROMFIELDS::ENDER), 1);
    }
}

void CNewCsv::GetMsg(std::string& msg)
{
    int len = 0;
    char* curSrcOffset = m_pOriginalFields->buffer;
    msg = "";
    
    if (m_updated == false &&
        curSrcOffset)
    {
        msg.append(curSrcOffset, m_bufferLen);
    }
    else
    {
        unsigned int index = 0;
                
        while (index < m_currBond)
        {
            tFieldStruct& fld = m_fields[index];
                       
            if (!len)
            {
                curSrcOffset = fld.pBuf->buffer + fld.offset;
            }

            if (fld.updated)
            {
                if (len) //read out the content whenever a updated field is hit!!
                {
                    msg.append(curSrcOffset, len);
                }

                msg.append(fld.pBuf->buffer + fld.offset, fld.value_len);
                
                if (index < m_currBond - 1)
                {
                    curSrcOffset = m_fields[index + 1].pBuf->buffer + m_fields[index + 1].offset;
                }

                len = 0;
            }
            else
            {
                len += fld.value_len;
            }
            index++;
        }

        if (len)
        {
            msg.append(curSrcOffset, len);
        }
    }
    
    len = msg.length();
    if (len != 0 &&
        msg[len - 1] != ROMFIELDS::ENDER)
    {
        msg.append(&(ROMFIELDS::ENDER), 1);
    }
}
 
bool CNewCsv::GetAt(unsigned int id, char* field, unsigned int len) const
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        unsigned int l_len = m_fields[id].value_len - 1;
        if (l_len > len - 1)
        {
            l_len = len - 1;
        }

        unsigned int offset = m_fields[id].offset;
        char* pBuf = &((m_fields[id].pBuf)->buffer[offset]);
        strncpy(field, pBuf, l_len);
        field[l_len] = '\0';
        rc = true;
    }

    return rc;
}

bool CNewCsv::GetAt(unsigned int id, std::string& field) const
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        unsigned int l_len = m_fields[id].value_len - 1;
        unsigned int offset = m_fields[id].offset;
        char* pBuf = &((m_fields[id].pBuf)->buffer[offset]);
        field.assign(pBuf, l_len);
        rc = true;
    }

    return rc;
}

bool CNewCsv::GetAt(unsigned int id, char& field) const
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        unsigned int offset = m_fields[id].offset;
        char* pBuf = &((m_fields[id].pBuf)->buffer[offset]);
        
        field = pBuf[0];
        rc = true;
    }

    return rc;
}

bool CNewCsv::GetAt(unsigned int id, int& field) const
{
    bool rc = false;

    std::string buffer;
    if (GetAt(id, buffer))
    {
        field = atoi(buffer.c_str());
        rc = true;
    }

    return rc;
}

bool CNewCsv::GetAt(unsigned int id, double& field) const
{
    bool rc = false;

    std::string buffer;
    if (GetAt(id, buffer))
    {
        field = atof(buffer.c_str());
        rc = true;
    }

    return rc;
}

bool CNewCsv::GetAt(unsigned int id, long& field) const
{
    bool rc = false;

    std::string buffer;
    if (GetAt(id, buffer))
    {
        field = atol(buffer.c_str());
        rc = true;
    }

    return rc;
}

bool CNewCsv::GetAt(unsigned int id, short& field) const
{
    int nbuf = 0;
    bool rc = false;

    if (GetAt(id, nbuf) &&
        nbuf <= SHRT_MAX)
    {
        field = nbuf;
        rc = true;
    }

    return rc;
}


//Sets


void CNewCsv::SetAt(unsigned int id, const char* value)
{
    unsigned int len = strlen(value);
    SetAt(id, value, len);
}

void CNewCsv::SetAt(unsigned int id, const char* value, unsigned int len)
{
    if (ROMFIELDS::MAXFIELDS > id)
    {
        if (m_currBond <= id)
        {
            int oldBond = m_currBond;
            m_currBond = id + 1;
                        
            for (int i = oldBond; i < m_currBond - 1; i++)
            {
                tFieldStruct& fld = m_fields[i];
                fld.value_len = 1;
                fld.updated = true;
                fld.pBuf = m_pModifiedFields;
                fld.offset = databuf_unread(m_pModifiedFields);
                databuf_memcpy(m_pModifiedFields, &(ROMFIELDS::DELIMITER), 1);
            }
        }
        m_updated = true;

        tFieldStruct& fld = m_fields[id];
        if (fld.value_len >= len + 1 &&
            fld.updated == true)         //Don't waste databuf space!!!
        {
            fld.value_len = len + 1;
            unsigned int offset = fld.offset;
            char* pPtr = &(m_pModifiedFields->buffer[offset]);
            memcpy(pPtr, value, len);
            
            //if (id == m_currBond - 1)
            //{
            //    pPtr[len] = ROMFIELDS::ENDER;
            //}
            //else
            //{
                pPtr[len] = ROMFIELDS::DELIMITER;
            //}
        }
        else
        {
            fld.value_len = len + 1;
            fld.updated = true;
            fld.pBuf = m_pModifiedFields;
            fld.offset = databuf_unread(m_pModifiedFields);
            databuf_memcpy(m_pModifiedFields, value, len);

            //if (id == m_currBond - 1)
            ////{
            //    databuf_write(m_pModifiedFields, "%c", ROMFIELDS::ENDER);
            //}
            //else
            //{
                databuf_write(m_pModifiedFields, "%c", ROMFIELDS::DELIMITER);
            //}
        }
    }
}

void CNewCsv::SetAt(unsigned int id, double value)
{
    char buffer[312];
    CUtility::ftoA(value, 7, buffer, 312);
    SetAt(id, buffer);
}
void CNewCsv::SetAt(unsigned int id, unsigned long value)
{
    char buffer[30];
    CUtility::itoA(value, buffer, 30);
    SetAt(id, buffer);
}

void CNewCsv::SetAt(unsigned int id, long value)
{
    char buffer[30];
    CUtility::itoA(value, buffer, 30);
    SetAt(id, buffer);
}

void CNewCsv::SetAt(unsigned int id, int value)
{
    char buffer[20];
    CUtility::itoA(value, buffer, 20);
    SetAt(id, buffer);
}

void CNewCsv::Reset()
{
    memset(&(m_fields[0]), 0, ROMFIELDS::MAXFIELDS * sizeof(tFieldStruct));
    databuf_reset(m_pOriginalFields);
    databuf_reset(m_pModifiedFields);
}


bool CNewCsv::CompareGetUpdate(unsigned int id, char* value, unsigned int len, bool compareOnly)
{ 
    bool rc = false;
    if (m_currBond > id)
    {
        tFieldStruct& fld = m_fields[id];
        unsigned int vlen = strlen(value);
        if (!(fld.value_len == vlen + 1 &&
              memcmp(&(fld.pBuf->buffer[fld.offset]), value, vlen) == 0))
        {
            if (false == compareOnly)
            {
                vlen = fld.value_len - 1;
                if (vlen > len - 1)
                {
                    vlen = len - 1;
                }
                memcpy(value, &(fld.pBuf->buffer[fld.offset]), vlen);
                value[vlen] = '\0';
            }
            rc = true;
        }
        else if (fld.value_len == 1) //the field is empty
        {
            SetAt(id, value);            
        }
    }
    
    return rc;
}

bool CNewCsv::CompareGetUpdate(unsigned int id, char& value, bool compareOnly)
{
    bool rc = false;
    char target;
    if (GetAt(id, target))
    {
        if (target != value)
        {
            if (false == compareOnly)
            {
                value = target;
            }
            rc = true;
        }
    }
    else
    {
        SetAt(id, value);
    }
    return rc;
}

bool CNewCsv::CompareGetUpdate(unsigned int id, short& value, bool compareOnly)
{
    bool rc = false;
    short target;
    if (GetAt(id, target))
    {
        if (target != value)
        {
            if (false == compareOnly)
            {
                value = target;
            }
            rc = true;
        }
    }
    else
    {
        SetAt(id, value);
    }
    return rc;
}

bool CNewCsv::CompareGetUpdate(unsigned int id, long& value, bool compareOnly)
{
    bool rc = false;
    long target;
    if (GetAt(id, target))
    {
        if (target != value)
        {
            if (false == compareOnly)
            {
                value = target;
            }
            rc = true;
        }
    }
    else
    {
        SetAt(id, value);
    }
    return rc;
}

bool CNewCsv::CompareGetUpdate(unsigned int id, double& value, bool compareOnly)
{
    bool rc = false;
    double target;
    if (GetAt(id, target))
    {
        if (target != value)
        {
            if (false == compareOnly)
            {
                value = target;
            }
            rc = true;
        }
    }
    else
    {
        SetAt(id, value);
    }
    return rc;
}

bool CNewCsv::GetField(unsigned int id, tField& field)
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        unsigned int offset = m_fields[id].offset;
        field.field = &((m_fields[id].pBuf)->buffer[offset]);
        field.field_l = m_fields[id].value_len - 1;
        rc = true;
    }

    return rc;
}