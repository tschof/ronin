// ISEView.h : interface of the CISEView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISEVIEW_H__804DF4E8_8507_47AF_9373_0FEBE3CBD541__INCLUDED_)
#define AFX_ISEVIEW_H__804DF4E8_8507_47AF_9373_0FEBE3CBD541__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CISEView : public CExchangeView
{
protected: // create from serialization only
	CISEView();
	DECLARE_DYNCREATE(CISEView)

public:
	//{{AFX_DATA(CInstinetView)
	enum { IDD = IDD_ISE_FORM };
	//}}AFX_DATA

// Attributes
public:
	CISEDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISEView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CISEView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CISEView)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ISEView.cpp
inline CISEDoc* CISEView::GetDocument()
   { return (CISEDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISEVIEW_H__804DF4E8_8507_47AF_9373_0FEBE3CBD541__INCLUDED_)
