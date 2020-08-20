//**************************************************************************
// 
//	File:
//	
//	iLinkGlobexView.h
//
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#if !defined(AFX_ILINKGLOBEXVIEW_H__0822A0C5_EDE6_4912_838A_1A3110951C13__INCLUDED_)
#define AFX_ILINKGLOBEXVIEW_H__0822A0C5_EDE6_4912_838A_1A3110951C13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConnectionList.h"

class CILinkGlobexView : public CFormView
{
protected: // create from serialization only
	CILinkGlobexView();
	DECLARE_DYNCREATE(CILinkGlobexView)

public:
	//{{AFX_DATA(CILinkGlobexView)
	enum { IDD = IDD_ILINK_FORM };
	CConnectionList	m_List;
	//}}AFX_DATA

// Attributes
public:
	CILinkGlobexDoc* GetDocument();
	int	m_TimerId;

// Operations
public:
	void CreateTimer(UINT Interval);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CILinkGlobexView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CILinkGlobexView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CILinkGlobexView)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in iLinkGlobexView.cpp
inline CILinkGlobexDoc* CILinkGlobexView::GetDocument()
   { return (CILinkGlobexDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILINKGLOBEXVIEW_H__0822A0C5_EDE6_4912_838A_1A3110951C13__INCLUDED_)
