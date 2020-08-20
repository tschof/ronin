#ifndef _ROMCSV_H__
#define _ROMCSV_H__

#pragma once

#include "COMMON\ROMFIXENGINE\FixEngine\Fix.h"
#include "COMMON\ROMFIXENGINE\FixEngine\ROMMessage.h"
#include <algorithm>
#include <sstream>


//namespace ROMMSG
//{
class CDummy
{
public:
  void Validate(std::vector<std::string>&) {}
  bool ShouldSend() { return true; }
  void ParseSecDefChildren(std::string&, std::vector<RFIX::CRCsv*>&) {}
};

class CRomCsv : public RFIX::CRomMessage<CDummy>
{
public:
  CRomCsv(std::string& l_sub);
  CRomCsv(const CRomCsv& csv);
  CRomCsv(void);
  ~CRomCsv(void);

  std::string dump () const;

  inline void SetAt(int field, const std::string& value)
  {
    RFIX::CRomMessage<CDummy>::SetAt(field, value);
  }

  //inline void SetAt(int field, const char* value)
  //{
  //    RFIX::CRomMessage<CDummy>::SetAt(field, value);
  //}

  inline void SetAt(int field, double value)
  {
    char buffer[20];
    sprintf(buffer, "%f", value);
    RFIX::CRomMessage<CDummy>::SetAt(field, buffer);
  }

  inline void SetAt(int field, long value)
  {
    char buffer[20];
    sprintf(buffer, "%ld", value);
    RFIX::CRomMessage<CDummy>::SetAt(field, buffer);
  }

  inline void SetAt(int field, unsigned long value)
  {
    char buffer[20];
    sprintf(buffer, "%u", value);
    RFIX::CRomMessage<CDummy>::SetAt(field, buffer);
  }

  inline void SetAt(int field, char value)
  {
    if (0 != value)
    {
      char buffer[2];
      sprintf(buffer, "%c", value);
      RFIX::CRomMessage<CDummy>::SetAt(field, buffer);
    }
  }

  inline void SetAt(int field, short value)
  {
    char buffer[20];
    sprintf(buffer, "%d", value);
    RFIX::CRomMessage<CDummy>::SetAt(field, buffer);
  }

  inline void SetAt(int field, int value)
  {
    char buffer[20];
    sprintf(buffer, "%d", value);
    RFIX::CRomMessage<CDummy>::SetAt(field, buffer);
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

  inline const char* Get(int field)
  {
    std::string& s = RFIX::CRomMessage<CDummy>::Get(field);
    return s.c_str();
  }

  inline bool IsFieldEmpty(int field) const
  {
    std::string s = GetAt(field);
    return s.empty();
  }
};
//}

#endif //_ROMCSV_H__
