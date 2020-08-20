#if !defined(AFX_ConnectionFrame_H__CDA9BBF3_2161_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_ConnectionFrame_H__CDA9BBF3_2161_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConnectionFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectionFrame frame

#include "ChildFrm.h"

class CConnectionFrame : public CChildFrame
{
	DECLARE_DYNCREATE(CConnectionFrame)
protected:
	CConnectionFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectionFrame)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CConnectionFrame();

	// Generated message map functions
	//{{AFX_MSG(CConnectionFrame)
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ConnectionFrame_H__CDA9BBF3_2161_11D2_8D11_006008CD4894__INCLUDED_)
