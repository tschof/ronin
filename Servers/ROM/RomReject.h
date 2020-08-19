#ifndef _ROMREJECT_H__
#define _ROMREJECT_H__

#pragma once

#include "Csv.h"
#include "romvaluedefs.h"

class CRomReject : public CCsv
{
public:
	CRomReject(const CCsv& msg, std::string& reason);
	~CRomReject(void);
		
private:
};

#endif //_ROMREJECT_H__