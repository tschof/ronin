#ifndef _ROMCANCEL_H__
#define _ROMCANCEL_H__

#pragma once

#include "Csv.h"
#include "RomOrderNew.h"

class CRomCancel : public CCsv
{
public:
	CRomCancel(CRomOrderNew& order);
  CRomCancel(const std::string& destID);
	~CRomCancel(void);

private:

};

#endif //_ROMCANCEL_H__