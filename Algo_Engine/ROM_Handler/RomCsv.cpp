//#include "StdAfx.h"
#include "RomCsv.h"

CRomCsv::CRomCsv(std::string& l_sub)
: RFIX::CRomMessage<CDummy>(l_sub)
{
}

CRomCsv::CRomCsv(const CRomCsv& csv)
: RFIX::CRomMessage<CDummy>(const_cast<CRomCsv&>(csv).Message())
{
}

CRomCsv::CRomCsv(void)
{
}

CRomCsv::~CRomCsv(void)
{
}

std::string CRomCsv::dump () const
{
  return std::string ();
}
