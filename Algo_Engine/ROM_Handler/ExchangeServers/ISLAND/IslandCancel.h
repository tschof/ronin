// IslandCancel.h: interface for the CIslandCancel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDCANCEL_H__45276463_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDCANCEL_H__45276463_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandMessage.h"
#include "Message.h"
#include "Cancel.h"

class CIslandCancel : public CIslandMessage  
{
public:
	CIslandCancel(const CCancel& Cancel);
	virtual ~CIslandCancel()					{	}

};

#endif // !defined(AFX_ISLANDCANCEL_H__45276463_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_)
