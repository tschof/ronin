//**************************************************************************
// 
//	File:
//	
//	GlobexFixSession.h
//
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#if !defined(_GLOBEXSTATUSHANDLER_H_)
#define _GLOBEXSTATUSHANDLER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExecutionMapSet.h"

class CGlobexStatusHandler : public CStatusHandler  
{
public:
	
	CGlobexStatusHandler();
	virtual ~CGlobexStatusHandler();

	BOOL SetStatus(CCancel& cancel);
	BOOL SetStatus(CCancelReject& cancelReject);
    BOOL SetStatus(CReplaceReject& replaceReject);
	BOOL SetStatus(CExecution& execution);
	BOOL SetStatus(COrder& order);
	BOOL SetStatus(CReject& reject);
	BOOL SetStatus(CReplace& replace);
	
private:
	CExecutionMapSet *m_pExecMap;
};


#endif // !defined(_GLOBEXSTATUSHANDLER_H_)
