// ActSession.h: interface for the CFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXSESSION_H__E69D7592_E5AA_11D1_8D02_006008CD4894__INCLUDED_)
#define AFX_FIXSESSION_H__E69D7592_E5AA_11D1_8D02_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Act.h"

class CActSession : public CExchangeSession
{
public:
	CActSession();
	virtual ~CActSession();

public:
	
	virtual void			ProcessTimer();
	virtual long			ProcessEntry(CCsv*);
	CString					ProcessMessage(CString);

private:
	CString m_Date;

private:
};

#endif // !defined(AFX_FIXSESSION_H__E69D7592_E5AA_11D1_8D02_006008CD4894__INCLUDED_)
