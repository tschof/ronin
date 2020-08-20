// BRASSInterface.h: interface for the CBRASSInterface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRASSINTERFACE_H__0E236195_FF41_11D3_8CB8_005004DA7701__INCLUDED_)
#define AFX_BRASSINTERFACE_H__0E236195_FF41_11D3_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BRASSData.h"
#include "BRASSRecord.h"
#include "Interface.h"

class CBRASSInterface : public CInterface  
{
public:
	CBRASSInterface(CDatabase*, BOOL DoQuery = TRUE);
	
	virtual BOOL Query();
	virtual BOOL Update() { return FALSE; }

public:
	CBRASSData    *GetMemoryLink()			{ return (CBRASSData*)CInterface::GetMemoryLink();    }
	CBRASSRecord  *GetRecordLink()			{ return (CBRASSRecord*)CInterface::GetRecordLink();  }
	CBRASSData    *GetMemoryAt(CString Key)  { return (CBRASSData*)CInterface::GetMemoryAt(Key);   }
	CBRASSRecord  *GetRecordAt(CString Key)  { return (CBRASSRecord*)CInterface::GetRecordAt(Key); }
};

#endif // !defined(AFX_BRASSINTERFACE_H__0E236195_FF41_11D3_8CB8_005004DA7701__INCLUDED_)
