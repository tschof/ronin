// IslandLogout.h: interface for the CIslandLogout class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDLOGOUT_H__AF6E8EBD_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDLOGOUT_H__AF6E8EBD_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandMessage.h"

class CIslandLogout : public CIslandMessage  
{
private:
	char m_Type;

public:
	CIslandLogout()
	{
		m_Type = 'F';
		m_Fields.AddField(m_Type);
	}

	virtual ~CIslandLogout()	{	}

};

#endif // !defined(AFX_ISLANDLOGOUT_H__AF6E8EBD_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
