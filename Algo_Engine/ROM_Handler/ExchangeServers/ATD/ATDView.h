// ATDView.h : interface of the CATDView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATDVIEW_H__574E0801_0FC6_42B7_9E7E_DD98F208B210__INCLUDED_)
#define AFX_ATDVIEW_H__574E0801_0FC6_42B7_9E7E_DD98F208B210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CATDView : public CExchangeView
{
protected: // create from serialization only
	CATDView();
	DECLARE_DYNCREATE(CATDView)

public:
	//{{AFX_DATA(CISEView)
	enum { IDD = IDD_ATD_FORM };
	//}}AFX_DATA

// Attributes
public:
	CATDDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATDView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CATDView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CATDView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ATDView.cpp
inline CATDDoc* CATDView::GetDocument()
   { return (CATDDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATDVIEW_H__574E0801_0FC6_42B7_9E7E_DD98F208B210__INCLUDED_)
