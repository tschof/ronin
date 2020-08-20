#ifndef _ROMREJECT_H__
#define _ROMREJECT_H__

#pragma once

#include "RomCsv.h"
#include "RomValueDefs.h"

class CRomReject : public CRomCsv
{
public:
	CRomReject(const CRomCsv& msg, std::string& reason);
	~CRomReject(void);
	//virtual std::string& Message();
	
private:
	//ROMMSG::CRomMessage* m_pCsv;
};

#endif //_ROMREJECT_H__