// InstinetHandler.h: interface for the CInstinetHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETHANDLER_H__E0817967_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_INSTINETHANDLER_H__E0817967_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OrderHandler.h"
#include "MemTagSet.h"
#include "InstinetMemTagSet.h"
#include "OrderTypeValidator.h"

//#include "ClearingAccount.h"
//#include "LocalAccount.h"
//#include "Definitions.h"
//#include "InstiData.h"
//#include "InstiInterface.h"
//#include "InstiRecord.h"

//#include "FromInstinetExecution.h"

class CInstinetHandler : public COrderHandler  
{
public:
	// incoming
	virtual void ProcessMessage(COrder& Order);
	virtual void ProcessMessage(CCancel& Cancel);
	virtual void ProcessMessage(CReplace& Replace);
	virtual void ProcessMessage(CRequest& Order);

	// outgoing
	virtual void ProcessMessage(const char *Status);
	virtual void ProcessMessage(CExecution& Report);
	virtual void ProcessMessage(CCancelReject& Reject);
	virtual void ProcessMessage(CReject& Reject);

private:
	//CCsv *BuildTerminalTrade(const CFromInstinetExecution& Exec, CInstiData& Terminal);

private:
	CInstinetMemTagSet *m_pTags;
	CString	m_DefaultAccount;

	COrderTypeValidator*	m_orderTypeValidator;
	// instinet terminal and trader information
	//CInstiInterface *m_pTerminals;

public:
	CInstinetHandler();
	virtual ~CInstinetHandler();

};

#endif // !defined(AFX_INSTINETHANDLER_H__E0817967_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
