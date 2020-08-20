// IslandView.h : interface of the CIslandView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDVIEW_H__AF6E8EB0_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDVIEW_H__AF6E8EB0_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ConnectionList.h"

class CIslandView : public CFormView
{
protected: // create from serialization only
	CIslandView();
	DECLARE_DYNCREATE(CIslandView)

public:
	//{{AFX_DATA(CIslandView)
	enum { IDD = IDD_ISLAND_FORM };
	CConnectionList	m_List;
	//}}AFX_DATA

// Attributes
public:
	CIslandDoc* GetDocument();
	int				m_TimerId;

// Operations
public:
	void CreateTimer(UINT Interval);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIslandView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIslandView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIslandView)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IslandView.cpp
inline CIslandDoc* CIslandView::GetDocument()
   { return (CIslandDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISLANDVIEW_H__AF6E8EB0_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
