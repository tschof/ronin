#if !defined(CEXCHANGESESSION_H)
#define CEXCHANGESESSION_H

#include "Act.h"

class CExchangeSession
{
public:
	virtual void		ProcessTimer()									{ }
	virtual long		ProcessEntry(CCsv*)				= 0;
	//virtual BOOL		ProcessCancel(CCancel& Cancel)			= 0;
	//virtual BOOL		ProcessReplace(CReplace& Replace)		{ return FALSE; }
	//virtual BOOL		ProcessStatusRequest(CRequest& Order)	{ return FALSE; }
	virtual CString	ProcessMessage(CString)						= 0;
	virtual BOOL		Logon()											{ return FALSE; }
	virtual BOOL		Logout()											{ return FALSE; }
	virtual BOOL		StartLogon()									{ return TRUE;  }
	virtual void		ResetSequenceNumber()						{ }

public:
	CExchangeSession()				{ }
	virtual ~CExchangeSession()	{ }
};

#endif