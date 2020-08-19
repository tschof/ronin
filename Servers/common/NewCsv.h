#ifndef _NEWCSV_H__
#define _NEWCSV_H__

#include <vector>
#include <sstream>
#include "romfielddefs.h"
#include "romvaluedefs.h"
#include "databuf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/uio.h>


typedef struct
{
    struct iovec iov[ROMFIELDS::MAXFIELDS];
    unsigned short iovcnt;
} tWriteV;

typedef struct
{
    char* field;
    unsigned int field_l;
} tField;

class CNewCsv
{
public:

    typedef struct //TFIELDSTRUCT
    {
        bool updated;
        databuf_t* pBuf;
        unsigned int offset;
        unsigned int value_len;
    } tFieldStruct;

    void Set(const std::string& msg);
    void Set(const char* msg, unsigned int len);
    void Set(CNewCsv* pSource);
    void Set(databuf_t* pBuf);
    
    CNewCsv();
    CNewCsv(char* msg, unsigned int len);
    CNewCsv(CNewCsv* pSource);
    ~CNewCsv(void);
    void BuildEmpty();
    
    //Gets
    bool GetAt(unsigned int id, char* field, unsigned int len) const;
    bool GetAt(unsigned int id, std::string& field) const;
    bool GetAt(unsigned int id, char& field) const;
    bool GetAt(unsigned int id, int& field) const;
    bool GetAt(unsigned int id, double& field) const;
    bool GetAt(unsigned int id, long& field) const;
    bool GetAt(unsigned int id, short& field) const;
     
    //Sets
    void SetAt(unsigned int id, int value);
    void SetAt(unsigned int id, const char* value);
    void SetAt(unsigned int id, const char* value, unsigned int len);
    
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
    void SetAt(unsigned int id, double value);
    void SetAt(unsigned int id, unsigned long value);
    void SetAt(unsigned int id, long value);

    bool CompareGetUpdate(unsigned int id, char* value, unsigned int len, bool compareOnly = false);
    bool CompareGetUpdate(unsigned int id, char& value, bool compareOnly = false);
    bool CompareGetUpdate(unsigned int id, short& value, bool compareOnly = false);
    bool CompareGetUpdate(unsigned int id, long& value, bool compareOnly = false);
    bool CompareGetUpdate(unsigned int id, double& value, bool compareOnly = false);
    
    bool GetField(unsigned int id, tField& field);
    
    inline bool IsFieldEmpty(unsigned int id) const
    {
        bool rc = true;

        if (ROMFIELDS::MAXFIELDS > id &&
            m_fields[id].value_len > 1)
        {
            rc = false;
        }

        return rc;
    }
    
    void GetMsg(databuf_t* pBuffer);
    void GetMsg(std::string& msg);
    bool GetWriteV(tWriteV& writev);
    void Reset();
    
    databuf_t* OriginalFields() { return m_pOriginalFields; }
    databuf_t* ModifiedFields() { return m_pModifiedFields; }
    
private:

    void BuildCsv(const char* msg, unsigned int len);
    void buildCsv();
    //void BuildEmpty();
    void Init();

    tFieldStruct m_fields[ROMFIELDS::MAXFIELDS];
    int m_bufferLen;
    unsigned int m_currBond;
    bool m_updated;
    
    databuf_t* m_pOriginalFields;
    databuf_t* m_pModifiedFields;
};

#endif //_NEWCSV_H__
