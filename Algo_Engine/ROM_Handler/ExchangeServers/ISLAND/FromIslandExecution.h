// FromIslandExecution.h: interface for the CFromIslandExecution class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FROMISLANDEXECUTION_H__99004E12_9034_11D2_8D44_00104B6DEAA9__INCLUDED_)
#define AFX_FROMISLANDEXECUTION_H__99004E12_9034_11D2_8D44_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Order.h"
#include "Execution.h"

class CFromIslandExecution : public CExecution  
{
		
public:
	CFromIslandExecution(const CString& Message);
	virtual ~CFromIslandExecution()	{	}

	CString GetGMTTransactionTime(CString& ts);
};

#endif // !defined(AFX_FROMISLANDEXECUTION_H__99004E12_9034_11D2_8D44_00104B6DEAA9__INCLUDED_)
