// OrderHandler.h: interface for the COrderHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDERHANDLER_H__50CABE53_EA63_11D1_8D04_006008CD4894__INCLUDED_)
#define AFX_ORDERHANDLER_H__50CABE53_EA63_11D1_8D04_006008CD4894__INCLUDED_

#include "Act.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class COrderHandler
{
public:

	virtual void ProcessMessage(CAct& Act)			= 0;
	//virtual void ProcessMessage(CCancel& Order)			= 0;
	//virtual void ProcessMessage(CReplace& Order)			{ };
	//virtual void ProcessMessage(CRequest& Order)			{ };
	virtual void ProcessMessage(const char *Status)		{ };
	
	virtual void ProcessMessage(CExecution& Report)		= 0;
	//virtual void ProcessMessage(CCancelReject& Reject)	= 0;
	//virtual void ProcessMessage(CReject& Reject)			= 0;

public:
	COrderHandler()				{	}
	virtual ~COrderHandler()	{	}
	
};
#endif // !defined(AFX_ORDERHANDLER_H__50CABE53_EA63_11D1_8D04_006008CD4894__INCLUDED_)
