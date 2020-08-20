// IslandHeartbeat.h: interface for the CIslandHeartbeat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDHEARTBEAT_H__28527F62_8EA2_11D2_8D43_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDHEARTBEAT_H__28527F62_8EA2_11D2_8D43_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandMessage.h"

class CIslandHeartbeat : public CIslandMessage  
{
private:
	char m_Type;

public:
	CIslandHeartbeat()
	{
		m_Type = 'I';
		m_Fields.AddField(m_Type);
	}
	virtual ~CIslandHeartbeat()	{	}

};

#endif // !defined(AFX_ISLANDHEARTBEAT_H__28527F62_8EA2_11D2_8D43_00104B6DEAA9__INCLUDED_)
