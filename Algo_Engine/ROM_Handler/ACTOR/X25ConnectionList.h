#if !defined(AFX_X25ConnectionList_H__CDA9BBF2_2161_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_X25ConnectionList_H__CDA9BBF2_2161_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// X25ConnectionList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CX25ConnectionList window

#include "X25Connection.h"

class CX25ConnectionList : public CListBox
{
// Construction
public:
	CX25ConnectionList();

// Attributes
public:

// Operations
public:
	void Add(CX25Connection *pConnection);
	void Delete(CX25Connection *pConnection);
	void Update()										{ Invalidate(FALSE); UpdateWindow(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CX25ConnectionList)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CX25ConnectionList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CX25ConnectionList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_X25ConnectionList_H__CDA9BBF2_2161_11D2_8D11_006008CD4894__INCLUDED_)