// ChxFixSession.h: interface for the CChxFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHXFIXSESSION_H__F1F3AB83_6A4D_11D5_A620_00500401F87A__INCLUDED_)
#define AFX_CHXFIXSESSION_H__F1F3AB83_6A4D_11D5_A620_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FixSession.h"

class CChxFixSession : public CFixSession
{
public:
	CChxFixSession() {}
	virtual ~CChxFixSession() {}

	virtual void ProcessEmail(CString& a_Text);
};

#endif // !defined(AFX_CHXFIXSESSION_H__F1F3AB83_6A4D_11D5_A620_00500401F87A__INCLUDED_)
