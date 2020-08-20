// BOXView.h : interface of the CBOXView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOXVIEW_H__574E0801_0FC6_42B7_9E7E_DD98F208B210__INCLUDED_)
#define AFX_BOXVIEW_H__574E0801_0FC6_42B7_9E7E_DD98F208B210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBOXView : public CExchangeView
{
protected: // create from serialization only
	CBOXView();
	DECLARE_DYNCREATE(CBOXView)

public:
	//{{AFX_DATA(CISEView)
	enum { IDD = IDD_BOX_FORM };
	//}}AFX_DATA

// Attributes
public:
	CBOXDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBOXView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBOXView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CBOXView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BOXView.cpp
inline CBOXDoc* CBOXView::GetDocument()
   { return (CBOXDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOXVIEW_H__574E0801_0FC6_42B7_9E7E_DD98F208B210__INCLUDED_)
