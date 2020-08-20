#ifndef _ROMCANCEL_H__
#define _ROMCANCEL_H__

#pragma once

#include "RomCsv.h"
#include "RomOrder.h"

class CRomCancel : public CRomCsv
{
public:
	CRomCancel::CRomCancel(CRomOrder& order);
	~CRomCancel(void);

private:

};

#endif //_ROMCANCEL_H__