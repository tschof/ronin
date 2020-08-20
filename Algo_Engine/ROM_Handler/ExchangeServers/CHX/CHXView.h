// CHXView.h : interface of the CCHXView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHXVIEW_H__CB666D00_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_CHXVIEW_H__CB666D00_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ConnectionList.h"

class CCHXView : public CFormView
{
protected: // create from serialization only
	CCHXView();
	DECLARE_DYNCREATE(CCHXView)

public:
	//{{AFX_DATA(CCHXView)
	enum { IDD = IDD_CHX_FORM };
	CConnectionList	m_List;
	//}}AFX_DATA

// Attributes
public:
	CCHXDoc*	GetDocument();
	int			m_TimerId;

// Operations
public:
	void CreateTimer(UINT Interval);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCHXView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCHXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCHXView)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CHXView.cpp
inline CCHXDoc* CCHXView::GetDocument()
   { return (CCHXDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHXVIEW_H__CB666D00_26ED_11D2_8D11_006008CD4894__INCLUDED_)
