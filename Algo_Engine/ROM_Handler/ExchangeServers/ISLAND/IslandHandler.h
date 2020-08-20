// IslandHandler.h: interface for the CIslandHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDHANDLER_H__AF6E8EB9_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDHANDLER_H__AF6E8EB9_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OrderHandler.h"
//#include "TagMapSet.h"
//#include "MemTagSet.h"
#include "IslandMemTagSet.h"
#include "OrderTypeValidator.h"

class CIslandHandler : public COrderHandler  
{
	CString	m_DefaultAccount;		// default clearing code

public:
	// incoming
	virtual void ProcessMessage(COrder& Order);
	virtual void ProcessMessage(CCancel& Order);
	virtual void ProcessMessage(CRequest& Order);

	// outgoing
	virtual void ProcessMessage(const char *Status);
	virtual void ProcessMessage(CExecution& Report);
	virtual void ProcessMessage(CCancelReject& Reject);
	virtual void ProcessMessage(CReject& Reject);

private:
    bool IsNASDSym(const char* symbol);

	CIslandMemTagSet *m_pTags;

	COrderTypeValidator*	m_orderTypeValidator;

public:
	CIslandHandler();
	virtual ~CIslandHandler();

};

#endif // !defined(AFX_ISLANDHANDLER_H__AF6E8EB9_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
