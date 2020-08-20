// FromIslandReject.h: interface for the CFromIslandReject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FROMISLANDREJECT_H__99004E13_9034_11D2_8D44_00104B6DEAA9__INCLUDED_)
#define AFX_FROMISLANDREJECT_H__99004E13_9034_11D2_8D44_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Reject.h"

class CFromIslandReject : public CReject  
{
public:
	CFromIslandReject(const CString& Message);
	virtual ~CFromIslandReject()	{	};

};

#endif // !defined(AFX_FROMISLANDREJECT_H__99004E13_9034_11D2_8D44_00104B6DEAA9__INCLUDED_)
