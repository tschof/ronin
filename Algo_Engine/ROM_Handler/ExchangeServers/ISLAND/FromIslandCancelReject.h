// FromIslandCancelReject.h: interface for the CFromIslandCancelReject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FROMISLANDCANCELREJECT_H__99004E14_9034_11D2_8D44_00104B6DEAA9__INCLUDED_)
#define AFX_FROMISLANDCANCELREJECT_H__99004E14_9034_11D2_8D44_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CancelReject.h"

class CFromIslandCancelReject : public CCancelReject  
{
public:
	CFromIslandCancelReject(const CString& Message);
	virtual ~CFromIslandCancelReject()	{	}

};

#endif // !defined(AFX_FROMISLANDCANCELREJECT_H__99004E14_9034_11D2_8D44_00104B6DEAA9__INCLUDED_)
