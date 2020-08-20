// ABNAMROView.h : interface of the CABNAMROView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABNAMROVIEW_H__804DF4E8_8507_47AF_9373_0FEBE3CBD541__INCLUDED_)
#define AFX_ABNAMROVIEW_H__804DF4E8_8507_47AF_9373_0FEBE3CBD541__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CABNAMROView : public CExchangeView
{
protected: // create from serialization only
	CABNAMROView();
	DECLARE_DYNCREATE(CABNAMROView)

public:
	//{{AFX_DATA(CInstinetView)
	enum { IDD = IDD_ABNAMRO_FORM };
	//}}AFX_DATA

// Attributes
public:
	CABNAMRODoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CABNAMROView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CABNAMROView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CABNAMROView)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ABNAMROView.cpp
inline CABNAMRODoc* CABNAMROView::GetDocument()
   { return (CABNAMRODoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABNAMROVIEW_H__804DF4E8_8507_47AF_9373_0FEBE3CBD541__INCLUDED_)
