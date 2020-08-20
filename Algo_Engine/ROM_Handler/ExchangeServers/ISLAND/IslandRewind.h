// IslandRewind.h: interface for the CIslandRewind class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDREWIND_H__45276464_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDREWIND_H__45276464_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandMessage.h"

class CIslandRewind : public CIslandMessage  
{
private:
	char	m_Type;
	int		m_Sequence;

public:
	CIslandRewind(const int Expected)
	{
		m_Type		= 'W';
		m_Sequence	= Expected;

		m_Fields.AddField(10, m_Sequence);
		m_Fields.AddField(m_Type);
	}
	virtual ~CIslandRewind() {	}

};

#endif // !defined(AFX_ISLANDREWIND_H__45276464_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_)
