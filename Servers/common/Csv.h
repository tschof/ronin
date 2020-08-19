#ifndef _CSV_H__
#define _CSV_H__

#include <vector>
#include <sstream>
#include "romfielddefs.h"
#include "romvaluedefs.h"
#include "databuf.h"

#ifndef _WIN32
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/uio.h>
#endif


static const unsigned int CSV_BUFFER_SIZE = 400;
static const int CSV_FIELD_SIZE = 50;


class CCsv
{
public:

    typedef struct //TFIELDSTRUCT
    {
        bool updated;
        bool reallocated;
        char* offset;
        char buffer[CSV_FIELD_SIZE];
        unsigned int value_len;
    } tFieldStruct;

    void Set(const std::string& msg);
    void Set(const char* msg, unsigned int len);
    void Set(const CCsv& csv);

    CCsv();
    CCsv(const std::string& msg);
    CCsv(const char* msg, unsigned int len);
    CCsv(const CCsv& csv);
    ~CCsv(void);

    //Gets
    bool GetAt(unsigned int id, char* field, unsigned int len) const;
    bool GetAt(unsigned int id, std::string& field) const;
    bool GetAt(unsigned int id, char& field) const;
    bool GetAt(unsigned int id, int& field) const;
    bool GetAt(unsigned int id, double& field) const;
    bool GetAt(unsigned int id, long& field) const;
    bool GetAt(unsigned int id, unsigned long& field) const;
    bool GetAt(unsigned int id, short& field) const;

    //Sets
    void SetAt(unsigned int id, int value);
    void SetAt(unsigned int id, const char* value);
    inline void SetAt(unsigned int id, char value)
    {
        char buffer[2];
        buffer[0] = value;
        buffer[1] = '\0';
        SetAt(id, buffer);
    }
    inline void SetAt(unsigned int id, const std::string& value)
    {
        SetAt(id, value.c_str());
    }
    void SetAt(unsigned int id, double value, int precision);
    void SetAt(unsigned int id, unsigned long value);
    void SetAt(unsigned int id, long value);

    inline bool IsFieldEmpty(unsigned int id) const
    {
        bool rc = true;

        if (ROMFIELDS::MAXFIELDS > id &&
            m_fields[id].value_len > 0)
        {
            rc = false;
        }

        return rc;
    }

    inline void GetMsg(std::string& msg)
    {
        getMsg(msg);
        msg.append(1, ROMFIELDS::ENDER);
    }

    inline void GetMsgWithSize(std::string& msg)
    {
        getMsg(msg);
        char sz[12];
        sprintf(sz, "%10lu%c", msg.length(), ROMFIELDS::ENDER);
        msg.append(sz);
    }

    inline void GetMsgNewWithSize(databuf_t* pBuffer)
    {
        getMsgNew(pBuffer);
        char sz[12];
        sprintf(sz, "%10lu%c", databuf_unread(pBuffer), ROMFIELDS::ENDER);
        databuf_memcpy(pBuffer, sz, strlen(sz));
    }

    void GetMsgNew(databuf_t* pBuffer);
    void getMsgNew(databuf_t* pBuffer);
    void Reset();

private:

    void BuildCsv(const char* msg, unsigned int len);
    void Init();


    void getMsg(std::string& msg);
    void CreateBuffer(const char* msg, unsigned int len);
    void BuildFields();

    tFieldStruct m_fields[ROMFIELDS::MAXFIELDS];
    char* m_pBuffer;
    int m_bufferLen;
    unsigned int m_currBond;
    bool m_updated;
    char m_Buffer[CSV_BUFFER_SIZE];
};

#endif //_CSV_H__
