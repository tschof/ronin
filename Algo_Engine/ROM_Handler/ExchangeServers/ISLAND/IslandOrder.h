// IslandOrder.h: interface for the CIslandOrder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDORDER_H__45276462_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDORDER_H__45276462_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandMessage.h"
#include "Order.h"

class CIslandOrder : public CIslandMessage  
{
public:
	CIslandOrder(const COrder& Order);
	virtual ~CIslandOrder()	{	}

};

#endif // !defined(AFX_ISLANDORDER_H__45276462_8F6B_11D2_8D44_00104B6DEAA9__INCLUDED_)
