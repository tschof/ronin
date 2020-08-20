// BloombergView.h : interface of the CBloombergView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOOMBERGVIEW_H__F46917BF_DFCC_11D3_A600_00500401F87A__INCLUDED_)
#define AFX_BLOOMBERGVIEW_H__F46917BF_DFCC_11D3_A600_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ConnectionList.h"
#include "BloombergDoc.h"

class CBloombergView : public CFormView
{
protected: // create from serialization only
	CBloombergView();
	DECLARE_DYNCREATE(CBloombergView)

public:
	//{{AFX_DATA(CBloombergView)
	enum{ IDD = IDD_BLOOMBERG_FORM };
	CConnectionList	m_List;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	void CreateTimer(UINT Interval);
	void CreateConnectionCheckTimer(UINT Interval);
	CBloombergDoc* GetDocument();

private:
	int m_TimerId;
	int m_TimerConnectionCheckID;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBloombergView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBloombergView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBloombergView)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BloombergView.cpp
inline CBloombergDoc* CBloombergView::GetDocument()
   { return (CBloombergDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOOMBERGVIEW_H__F46917BF_DFCC_11D3_A600_00500401F87A__INCLUDED_)
