/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : Defines the class behaviors for the application.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/
#ifndef _ROMMESSAGE_H__
#define _ROMMESSAGE_H__

#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>
#include "ROMFieldDefs.h"

using namespace ROMFIELDS;

    
class CRCsv {
public:
    virtual ~CRCsv() {}
    virtual void SetAt(int field, std::string& value) = 0;
    virtual void Set(int field, const char* val) = 0;
    virtual const std::string& GetAt(int field) const= 0;
	//virtual std::string& Get(const int field) = 0;
    virtual void Erase(int field) = 0;
    virtual void Clear() = 0;
    virtual std::string& Message() = 0;
	virtual std::string& ChildMessage() = 0;
    virtual bool HasChildren() = 0;
	virtual std::vector<CRCsv*>& GetChildren() = 0;
};

struct deleter
{
	void operator ()(CRCsv* ptr) {delete ptr;}
};

static const int ROMMSG_INIT_LEN = 128;
namespace {
    static const char DELIMITER = ',';
}

class CRomMessage : public CRCsv {
public:
    typedef std::vector<CRCsv*> tChild;
    typedef tChild::iterator tChildIterator;
    CRomMessage()
        :m_fields(), m_emptyField(""),
        m_timeStr() {
        m_fields.reserve(CSV_NUM_FIELDS);
        for(int i = 0; i < CSV_NUM_FIELDS; ++i) {
            m_fields.push_back(m_emptyField);
        }
        m_message.reserve(ROMMSG_INIT_LEN);
    }
    
    explicit CRomMessage(std::string& l_sub)
        :m_fields(), m_emptyField(""),
        m_timeStr() {
        m_fields.reserve(CSV_NUM_FIELDS);
        for(int i = 0; i < CSV_NUM_FIELDS; ++i) {
            m_fields.push_back(m_emptyField);
        }
        m_message.reserve(ROMMSG_INIT_LEN);
		
        ParseMessage(l_sub);
    }

	explicit CRomMessage(const char* pBuf, int len)
            :m_fields(), m_emptyField(""), 
            m_timeStr() 
	{
        m_fields.reserve(CSV_NUM_FIELDS);
        for(int i = 0; i < CSV_NUM_FIELDS; ++i) 
		{
            m_fields.push_back(m_emptyField);
        }
        m_message.reserve(ROMMSG_INIT_LEN);
		
        ParseMessage(pBuf, len);
    }

	explicit CRomMessage(const CRomMessage& other) 
		:m_fields(), m_emptyField(""),
		m_timeStr() 
	{
		m_fields.reserve(CSV_NUM_FIELDS);
		for(int i = 0; i < CSV_NUM_FIELDS; ++i) 
		{
			m_fields[i] = other.GetAt(i);
			//m_fields.push_back(m_emptyField);
		}
		m_message.reserve(ROMMSG_INIT_LEN);

		//ParseMessage(const_cast<CRomMessage&>(other).Message());
	}

    ~CRomMessage() {
		std::for_each(m_children.begin(), m_children.end(),
		deleter());
	}
    
    inline void SetAt(int field, std::string& value) {
        if(field >= 0 && field < CSV_NUM_FIELDS) {
			ValidateField(value);
            m_fields[field] = value;
        }
    }
    inline void SetAt(int field, const std::string& value) {
        if(field >= 0 && field < CSV_NUM_FIELDS) {
			std::string cpy(value);
			ValidateField(cpy);
            m_fields[field] = cpy;
        }
    }

    inline void Set(int field, const char* val) {
        if(field >= 0 && field < CSV_NUM_FIELDS) {
			std::string cpy(val);
			ValidateField(cpy);
            m_fields[field] = val;
        }
    }

    inline const std::string& GetAt(int field) const{
        if(field >= 0 && field < CSV_NUM_FIELDS) {
            return m_fields[field];
        } else {
            return m_emptyField;
        }
    }

	inline std::string& Get(const int field) {
        if(field >= 0 && field < CSV_NUM_FIELDS) {
            return m_fields[field];
        } else {
            return m_emptyField;
        }
    }

	inline void SetAt(int field, double value)
    {
        char buffer[20];
		sprintf(buffer, "%f", value);
        SetAt(field, buffer);
    }

    inline void SetAt(int field, long value)
    {
		char buffer[20];
		sprintf(buffer, "%ld", value);
        SetAt(field, buffer);
    }

	inline void SetAt(int field, unsigned long value)
    {
		char buffer[20];
		sprintf(buffer, "%u", value);
        SetAt(field, buffer);
    }

	inline void SetAt(int field, char value)
    {
		if (0 != value)
		{
			char buffer[2];
			sprintf(buffer, "%c", value);
			SetAt(field, buffer);
		}
    }

	inline void SetAt(int field, short value)
    {
		char buffer[20];
		sprintf(buffer, "%d", value);
        SetAt(field, buffer);
    }
	
	inline void SetAt(int field, int value)
    {
		char buffer[20];
		sprintf(buffer, "%d", value);
        SetAt(field, buffer);
    }

    inline const int nGet(int field)const
    {
        int rc = 0;
        std::string s = GetAt(field);

        if (s.empty() == false)
        {
            rc = atoi(s.c_str());
        }
        
        return rc;
    }

	inline const long lGet(int field)const
    {
        long rc = 0;

        std::string s = GetAt(field);

        if (s.empty() == false)
        {
            rc = atol(s.c_str());
        }
        
        return rc;
    }

	inline char cGet(int field)const
    {
        char rc = 0;
        std::string s = GetAt(field);

        if (s.empty() == false)
        {
            rc = s[0];
        }
        
        return rc;
    }

    inline double dGet(int field)const
    {
        double rc = 0;
        std::string s = GetAt(field);

        if (s.empty() == false)
        {
            rc = atof(s.c_str());
        }
        
        return rc;
    }

	//inline const char* Get(int field)
    //{
	//	std::string& s = Get(field);
	//	return s.c_str();
    //}

    inline bool IsFieldEmpty(int field) const
    {
        std::string s = GetAt(field);
        return s.empty();
    }

    inline void Erase(int field) {
        m_fields[field] = m_emptyField;
    }

    inline std::string& Message() {
       // SetTime();
        m_message.erase(0, m_message.size());
        int end = m_fields.size();
        for(int i = 0; i < end; ++i) {
            m_message += m_fields[i];
            m_message += ",";
        }
		if(HasChildren()) {
			m_message += "!#!";
			for(unsigned int i = 0; i < m_children.size(); ++i) {
				
				m_message += m_children[i]->ChildMessage();
				m_message += "!#!";
			}
		}
        m_message += "\n";
        return m_message;
        
    }
	inline std::string& ChildMessage() {
        if(m_fields[0] == "G") {
			m_fields[0] = "E";
		}
        //SetTime();
        m_message.erase(0, m_message.size());
        int end = m_fields.size();
        for(int i = 0; i < end; ++i) {
            m_message += m_fields[i];
            m_message += ",";
        }
		if(HasChildren()) {
			m_message += "!#!";
			for(unsigned int i = 0; i < m_children.size(); ++i) {					
				m_message += m_children[i]->Message();
				m_message += "!#!";
			}
		}
        return m_message;
        
    }
    inline std::string& PureMessage() {
        m_message.erase(0, m_message.size());
        int end = m_fields.size();
        for(int i = 0; i < end; ++i) {
            m_message += m_fields[i];
            m_message += ",";
        }
		if(HasChildren()) {
			m_message += "!#!";
			for(unsigned int i = 0; i < m_children.size(); ++i) {					
				m_message += m_children[i]->ChildMessage();
				m_message += "!#!";
			}
		}
       m_message += "\n";
        return m_message;
    }
    
    inline void Clear() {
        m_message.erase(0, m_message.size());
        m_fields.clear();
    }
    inline void SetTime() {
        SYSTEMTIME systime;
        GetSystemTime(&systime);
        memset(m_time, '\0', 50);
        sprintf(m_time, "%02u%02u%02u",                
            systime.wHour, systime.wMinute, systime.wSecond);
        m_timeStr = m_time;
        m_fields[1] = m_timeStr;
    }
	virtual bool HasChildren() {
		return m_children.size() > 0;
	}
	virtual std::vector<CRCsv*>& GetChildren() {
		return m_children;
	}
private:

    inline void ParseMessage(std::string& l_sub) {
		std::string::size_type idx = l_sub.find("!#!");
		std::string::size_type length = l_sub.size();
		
		if(idx != std::string::npos) {
			std::string childStr = l_sub.substr(idx + 3);
			l_sub = l_sub.substr(0, idx);
			while( (idx = childStr.find("!#!")) != std::string::npos) {
				std::string subChild = childStr.substr(0, idx);
				std::string::size_type newStart = (idx + 3) > length ? length : idx + 3;
				CRCsv* pCsv = new CRomMessage(subChild);
				m_children.push_back(pCsv);
				childStr = childStr.substr(newStart);
			}
		}

		std::string::size_type subOff = 0;
        int count = 0;
        //int length = l_sub.size();
		std::string::size_type delimPos = l_sub.find(DELIMITER, subOff);
		while (subOff < length && count < (ROMMSG_INIT_LEN -1) && delimPos != std::string::npos) {
            std::string sub = l_sub.substr(subOff, (delimPos - subOff));
            SetAt(count, sub);
            ++count;
            subOff = delimPos + 1;
            delimPos = l_sub.find(DELIMITER, subOff);
        }
		if(subOff < length) {
			std::string sub = l_sub.substr(subOff);
			SetAt(count, sub);
		}
    }

	
	inline void ParseMessage(const char* pBuf, int len) 
	{
		char* pCur = strstr(const_cast<char*>(pBuf), "!#!");
			int length = len;
			if(pCur != NULL) 
			{
				length = pCur - pBuf;
				std::string l_sub(pBuf, len);
				std::string::size_type idx = l_sub.find("!#!");
				std::string childStr = l_sub.substr(idx + 3);
				l_sub = l_sub.substr(0, idx);
				while( (idx = childStr.find("!#!")) != std::string::npos) {
					std::string subChild = childStr.substr(0, idx);
					std::string::size_type newStart = (idx + 3) > len ? len : idx + 3;
					CRCsv* pCsv = new CRomMessage(subChild);
					m_children.push_back(pCsv);
					childStr = childStr.substr(newStart);
				}
			}

			pCur = const_cast<char*>(pBuf);
			int count = 0;
			char* pTmp = NULL;

			while ( pCur - pBuf < length &&
					(pTmp = strchr(pCur, DELIMITER)) != NULL &&
					count < CSV_NUM_FIELDS)
			{
				if (pTmp - pBuf >= length)
				{
					break;
				}

				m_fields[count].assign(pCur, pTmp - pCur);
				count++;
				pCur = pTmp + 1;
			}

			if (pCur - pBuf < length &&
				count < CSV_NUM_FIELDS)
			{
				m_fields[count].assign(pCur, length - (pCur - pBuf));
			}
    }
	
	inline void ValidateField(std::string& str) {
		std::string::size_type loc = str.find(",");
		if(loc != std::string::npos) {
			while (loc != std::string::npos) {
				str.erase(loc, 1);
				loc = str.find(",");
			}
		}
	}
    
    std::vector<std::string> m_fields;
    std::string m_message;
    std::string m_emptyField;
    tChild m_children;
    char m_time[50];
    std::string m_timeStr;
};

#endif //_ROMMESSAGE_H__