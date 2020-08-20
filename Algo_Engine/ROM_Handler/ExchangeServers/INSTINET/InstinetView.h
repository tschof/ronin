// InstinetView.h : interface of the CInstinetView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETVIEW_H__E0817960_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_INSTINETVIEW_H__E0817960_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ConnectionList.h"

class CInstinetView : public CFormView
{
protected: // create from serialization only
	CInstinetView();
	DECLARE_DYNCREATE(CInstinetView)

public:
	//{{AFX_DATA(CInstinetView)
	enum { IDD = IDD_INSTINET_FORM };
	CConnectionList	m_List;
	//}}AFX_DATA

// Attributes
public:
	CInstinetDoc* GetDocument();
	int				m_TimerId;

// Operations
public:
	void CreateTimer(UINT Interval);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstinetView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstinetView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CInstinetView)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in InstinetView.cpp
inline CInstinetDoc* CInstinetView::GetDocument()
   { return (CInstinetDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTINETVIEW_H__E0817960_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
