#include "Csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "BuffStore.h"
#include "Utility.h"

void CCsv::Init()
{
    /*m_pBuffer = NULL;
    m_currBond = 0;
    m_updated = false;

    tFieldStruct dummy = {false, false, NULL, "\0", 0};

    for (unsigned int i = 0; i < ROMFIELDS::MAXFIELDS; i++)
    {
        m_fields[i] = dummy;
    }*/

    memset(this, 0, sizeof(CCsv));
    m_pBuffer = &(m_Buffer[0]);
}

CCsv::CCsv()
{
    Init();
}

CCsv::CCsv(const CCsv& csv)
{
    Init();
    std::string s;
    (const_cast<CCsv&>(csv)).getMsg(s);
    BuildCsv(s.c_str(), s.length());
}


CCsv::CCsv(const std::string& msg)
{
    Init();

    unsigned int len = msg.length();
    if (msg[len - 1] == '\n');
    {
        len--;
    }

    BuildCsv(msg.c_str(), len);
}

CCsv::CCsv(const char* msg, unsigned int len)
{
    Init();
    Set(msg, len);
}

void CCsv::Set(const std::string& msg)
{
    Set(msg.c_str(), msg.length());
}

void CCsv::Set(const char* msg, unsigned int len)
{
    if (len > 0)
    {
        if (msg[len - 1] == '\n')
        {
            len--;
        }
        BuildCsv(msg, len);
    }
}


void CCsv::Set(const CCsv& csv)
{
    std::string s;
    (const_cast<CCsv&>(csv)).getMsg(s);
    BuildCsv(s.c_str(), s.length());
}


void CCsv::BuildCsv(const char* msg, unsigned int len)
{
    CreateBuffer(msg, len);
    BuildFields();
}

CCsv::~CCsv(void)
{
    Reset();
}

void CCsv::GetMsgNew(databuf_t* pBuffer)
{
    getMsgNew(pBuffer);
    databuf_write(pBuffer, "%c", ROMFIELDS::ENDER);
}


void CCsv::getMsgNew(databuf_t* pBuffer)
{
    int len = 0;
    char* curSrcOffset = m_pBuffer;

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
                curSrcOffset = fld.offset;
            }

            if (fld.updated)
            {
                if (len) //read out the content whenever a updated field is hit!!
                {
                    databuf_memcpy(pBuffer, curSrcOffset, len);
                }

                databuf_memcpy(pBuffer, fld.offset, fld.value_len);
                databuf_memcpy(pBuffer, ROMVALUE::DELIMITER.c_str(), ROMVALUE::DELIMITER.size());

                if (index < m_currBond - 1)
                {
                    curSrcOffset = m_fields[index + 1].offset;
                }

                len = 0;
            }
            else
            {
                if (0 == m_fields[index].offset)
                {
                    if (len)
                    {
                        databuf_memcpy(pBuffer, curSrcOffset, len);
                    }

                    databuf_memcpy(pBuffer, ROMVALUE::DELIMITER.c_str(), ROMVALUE::DELIMITER.size());
                    len = 0;
                }
                else
                {
                    len += fld.value_len + 1;
                }
            }
            index++;
        }

        if (len)
        {
            databuf_memcpy(pBuffer, curSrcOffset, len);
        }
    }
}

bool CCsv::GetAt(unsigned int id, char* field, unsigned int len) const
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        unsigned int l_len = m_fields[id].value_len;
        if (l_len > len - 1)
        {
            l_len = len - 1;
        }

        strncpy(field, m_fields[id].offset, l_len);
        field[l_len] = '\0';
        rc = true;
    }

    return rc;
}

bool CCsv::GetAt(unsigned int id, std::string& field) const
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        field.assign(m_fields[id].offset, m_fields[id].value_len);
        rc = true;
    }

    return rc;
}

bool CCsv::GetAt(unsigned int id, char& field) const
{
    bool rc = false;

    if (IsFieldEmpty(id) == false)
    {
        field = (m_fields[id].offset)[0];
        rc = true;
    }

    return rc;
}

bool CCsv::GetAt(unsigned int id, int& field) const
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

bool CCsv::GetAt(unsigned int id, double& field) const
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

bool CCsv::GetAt(unsigned int id, long& field) const
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

bool CCsv::GetAt(unsigned int id, unsigned long& field) const
{
    bool rc = false;

    std::string buffer;
    if (GetAt(id, buffer))
    {
        field = strtoul(buffer.c_str(), NULL, 0);
        rc = true;
    }

    return rc;
}

bool CCsv::GetAt(unsigned int id, short& field) const
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


void CCsv::SetAt(unsigned int id, const char* value)
{
    if (ROMFIELDS::MAXFIELDS > id)
    {
        if (m_currBond <= id)
        {
            m_currBond = id + 1;
        }
        m_updated = true;

        tFieldStruct& fld = m_fields[id];
        unsigned int len = strlen(value);

        if (len > sizeof(fld.buffer) - 1)
        {
            if (fld.reallocated)
            {
                free (fld.offset);
            }
            else
            {
                fld.reallocated = true;
            }
            fld.offset = (char*)calloc(len + 1, 1);
        }
        else
        {
            if (fld.reallocated)
            {
                free (fld.offset);
                fld.offset = NULL;
                fld.reallocated = false;
            }
            fld.offset = fld.buffer;
        }

        strncpy(fld.offset, value, len);
        (fld.offset)[len] = '\0';
        fld.value_len = len;
        fld.updated = true;
    }
}


void CCsv::getMsg(std::string& msg)
{
    msg.clear();

    //for testing only
    msg.reserve(400);
    //end of testing only
    if (m_updated == false &&
            m_pBuffer)
    {
        msg.assign(m_pBuffer);
    }
    else
    {
        unsigned int len = 0;
        unsigned int index = 0;
        char* currOffset = m_pBuffer;

        while (index < m_currBond)
        {
            tFieldStruct& fld = m_fields[index];
            if (!len)
            {
                currOffset = fld.offset;
            }

            if (fld.updated)
            {
                if (len) //read out the content whenever a updated field is hit!!
                {
                    msg.append(currOffset, len);
                }

                msg.append(fld.offset, fld.value_len);
                msg.append(1, ROMFIELDS::DELIMITER);

                if (index < m_currBond - 1)
                {
                    currOffset = m_fields[index + 1].offset;
                }

                len = 0;
            }
            else
            {
                if (0 == m_fields[index].offset)
                {
                    if (len)
                    {
                        msg.append(currOffset, len);
                    }

                    msg.append(1, ROMFIELDS::DELIMITER);
                    len = 0;
                }
                else
                {
                    len += fld.value_len + 1;
                }
            }
            index++;
        }

        if (len)
        {
            msg.append(currOffset, len);
        }
    }
}

void CCsv::CreateBuffer(const char* msg, unsigned int len)
{
    if (!len)
    {
        return;
    }

    if (len >= CSV_BUFFER_SIZE)
    {
        if (msg[len - 1] != ROMFIELDS::DELIMITER)
        {
            m_pBuffer = (char*)malloc(len + 2);
            *(m_pBuffer + len) = ROMFIELDS::DELIMITER;
            *(m_pBuffer + len + 1) = '\0';
        }
        else
        {
            m_pBuffer = (char*)malloc(len + 1);
            *(m_pBuffer + len) = '\0';
        }
    }
    else
    {
        if (msg[len - 1] != ROMFIELDS::DELIMITER)
        {
            m_pBuffer = (char*)malloc(len + 2);
            *(m_pBuffer + len) = ROMFIELDS::DELIMITER;
            *(m_pBuffer + len + 1) = '\0';
        }
        else
        {
            m_pBuffer = &(m_Buffer[0]);
            *(m_pBuffer + len) = '\0';
        }
    }

    memcpy(m_pBuffer, msg, len);
    m_bufferLen = len;
}

void CCsv::BuildFields()
{
    char* pCur = m_pBuffer;
    unsigned int count = 0;
    void* pTmp = NULL;
    long bytestosearch = m_bufferLen;
    size_t len = 0;

    while ( bytestosearch > 0 &&
            (pTmp = memchr(pCur, ROMFIELDS::DELIMITER, bytestosearch)) != NULL &&
            count < ROMFIELDS::MAXFIELDS)
    {
        len = (char*)pTmp - pCur;
        if (len > 0)
        {
            m_fields[count].offset = pCur;
            m_fields[count].value_len = len;
        }

        bytestosearch -= len + 1;
        count++;
        pCur = (char*)pTmp + 1;
    }

    if (bytestosearch > 0 &&
        count < ROMFIELDS::MAXFIELDS &&
        *pCur != '\0')
    {
        m_fields[count].offset = pCur;
        m_fields[count].value_len = bytestosearch;
        count++;
    }

    m_currBond = count;

/*
    char* pCur = m_pBuffer;
    unsigned int count = 0;
    char* pTmp = NULL;

    while ( pCur - m_pBuffer < m_bufferLen &&
            (pTmp = strchr(pCur, ROMFIELDS::DELIMITER)) != NULL &&
            count < ROMFIELDS::MAXFIELDS)
    {
        if (pTmp - m_pBuffer >= m_bufferLen)
        {
            break;
        }

        m_fields[count].offset = pCur;
        m_fields[count].value_len = pTmp - pCur;
        count++;
        pCur = pTmp + 1;
    }

    if (pCur - m_pBuffer < m_bufferLen &&
            count < ROMFIELDS::MAXFIELDS)
    {
        m_fields[count].offset = pCur;
        m_fields[count].value_len = m_bufferLen - (pCur - m_pBuffer);
        count++;
    }

    m_currBond = count;
*/
}

void CCsv::SetAt(unsigned int id, double value, int precision)
{
    if (value > 0)
    {
        value += (1/pow(10, precision + 1));
    }
    else if (value < 0)
    {
        value -= (1/pow(10, precision + 1));
    }
    char buffer[128];
    CUtility::ftoA(value, precision, buffer, 128);
    SetAt(id, buffer);
}
void CCsv::SetAt(unsigned int id, unsigned long value)
{
    char buffer[30];
    //sprintf(buffer, "%ld", value);
    CUtility::itoA(value, buffer, 30);
    SetAt(id, buffer);
}

void CCsv::SetAt(unsigned int id, long value)
{
    char buffer[30];
    //sprintf(buffer, "%ld", value);
    CUtility::itoA(value, buffer, 30);
    SetAt(id, buffer);
}

void CCsv::SetAt(unsigned int id, int value)
{
    char buffer[20];
    //sprintf(buffer, "%d", value);
    CUtility::itoA(value, buffer, 20);
    SetAt(id, buffer);
}

void CCsv::Reset()
{
    if (&(m_Buffer[0]) != m_pBuffer)
    {
        free(m_pBuffer);
    }

    for (unsigned int i = 0; i < ROMFIELDS::MAXFIELDS; i++)
    {
        if (m_fields[i].reallocated)
        {
            free(m_fields[i].offset);
            m_fields[i].offset = NULL;
            m_fields[i].reallocated = false;
        }
    }

    memset(this, 0, sizeof(CCsv));
    m_pBuffer = &(m_Buffer[0]);
}

