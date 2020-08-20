#if !defined(AFX_CONNECTIONLIST_H__CDA9BBF2_2161_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_CONNECTIONLIST_H__CDA9BBF2_2161_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConnectionList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectionList window

#include "Connection.h"

class CConnectionList : public CListBox
{
// Construction
public:
	CConnectionList();

// Attributes
public:

// Operations
public:
	void Add(CConnection *pConnection);
	void Delete(CConnection *pConnection);
	void Update()										{ Invalidate(FALSE); UpdateWindow(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionList)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CConnectionList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CConnectionList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTIONLIST_H__CDA9BBF2_2161_11D2_8D11_006008CD4894__INCLUDED_)
