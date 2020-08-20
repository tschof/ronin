// IslandLogin.h: interface for the CIslandLogin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDLOGIN_H__AF6E8EBB_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDLOGIN_H__AF6E8EBB_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandMessage.h"

class CIslandLogin  : public CIslandMessage
{
private:
	char	m_Type;
	CString	m_Account;
	CString	m_Password;

public:
	CIslandLogin(const CString& Account, const CString& Password)
	{
		m_Type		= 'L';
		m_Account	= Account;
		m_Password	= Password;

		m_Fields.AddField(10, m_Password);
		m_Fields.AddField(6, m_Account);
		m_Fields.AddField(m_Type);
	}

	virtual ~CIslandLogin()	{	}

};

#endif // !defined(AFX_ISLANDLOGIN_H__AF6E8EBB_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
