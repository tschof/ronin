// RecorderView.h : interface of the CRecorderView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECORDERVIEW_H__E2E8B3D0_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_)
#define AFX_RECORDERVIEW_H__E2E8B3D0_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRecorderDoc;
#include "ConnectionList.h"

class CRecorderView : public CFormView
{
protected: // create from serialization only
	CRecorderView();
	DECLARE_DYNCREATE(CRecorderView)

public:
	//{{AFX_DATA(CRecorderView)
	enum { IDD = IDD_DBRECORDER_FORM };
	CConnectionList	m_List;
	//}}AFX_DATA

private:
	int	m_Timer;

// Attributes
public:
	CRecorderDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecorderView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRecorderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRecorderView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RecorderView.cpp
inline CRecorderDoc* CRecorderView::GetDocument()
   { return (CRecorderDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECORDERVIEW_H__E2E8B3D0_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_)
