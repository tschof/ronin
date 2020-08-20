// ActHost.h: interface for the CActHost class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTHOST_H__4649A163_EF11_11D5_A624_00500401F87A__INCLUDED_)
#define AFX_ACTHOST_H__4649A163_EF11_11D5_A624_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CActHost  
{
public:
	CActHost();
	virtual ~CActHost();
	CString GetHost();

private:
	CStringArray m_Hosts;
	int m_nHost;
};

#endif // !defined(AFX_ACTHOST_H__4649A163_EF11_11D5_A624_00500401F87A__INCLUDED_)
