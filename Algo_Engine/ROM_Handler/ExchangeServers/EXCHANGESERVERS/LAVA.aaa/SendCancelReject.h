// SendCancelReject.h: interface for the CSendCancelReject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SENDCANCELREJECT_H__F207C3B2_CA78_11D2_8D4E_00104B6DEAA9__INCLUDED_)
#define AFX_SENDCANCELREJECT_H__F207C3B2_CA78_11D2_8D4E_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Message.h"
#include "Cancel.h"
#include "CancelReject.h"

class CSendCancelReject : public CCancelReject  
{
public:
	CSendCancelReject(const CCancel& Cancel);
	virtual ~CSendCancelReject();

};

#endif // !defined(AFX_SENDCANCELREJECT_H__F207C3B2_CA78_11D2_8D4E_00104B6DEAA9__INCLUDED_)
