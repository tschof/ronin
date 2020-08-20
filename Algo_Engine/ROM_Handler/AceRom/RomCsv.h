#ifndef _ROMCSV_H__
#define _ROMCSV_H__

#pragma once

#include "ROMMessage.h"
#include <algorithm>
#include <sstream>


//namespace ROMMSG
//{
    class CRomCsv : public CRomMessage
    {
    public:
        CRomCsv(std::string& l_sub);
		CRomCsv(const char* pMsg, int size);
        CRomCsv(const CRomCsv& csv);
        CRomCsv(void);
        ~CRomCsv(void);

        inline void SetAt(int field, const std::string& value)
        {
            CRomMessage::SetAt(field, value);
        }
 
		//inline void SetAt(int field, const char* value)
        //{
        //    RFIX::CRomMessage<CDummy>::SetAt(field, value);
        //}

        inline void SetAt(int field, double value)
        {
            std::stringstream stm;
            stm << value;
            CRomMessage::SetAt(field, stm.str());
        }

        inline void SetAt(int field, long value)
        {
            std::stringstream stm;
            stm << value;
            CRomMessage::SetAt(field, stm.str());
        }

		inline void SetAt(int field, unsigned long value)
        {
  			char buffer[20];
			sprintf(buffer, "%u", value);
            CRomMessage::SetAt(field, buffer);
        }

		inline void SetAt(int field, char value)
        {
			if (0 != value)
			{
				std::stringstream stm;
				stm << value;
				CRomMessage::SetAt(field, stm.str());
			}
        }

		inline void SetAt(int field, short value)
        {
            std::stringstream stm;
            stm << value;
            CRomMessage::SetAt(field, stm.str());
        }

		inline void SetAt(int field, int value)
        {
  			char buffer[20];
			sprintf(buffer, "%d", value);
            CRomMessage::SetAt(field, buffer);
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

		inline int cGet(int field)const
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

		inline const char* Get(int field)
        {
			std::string& s = CRomMessage::Get(field);
			return s.c_str();
        }

        inline bool IsFieldEmpty(int field)
        {
            std::string s = GetAt(field);
            return s.empty();
        }
    };
//}

#endif //_ROMCSV_H__
