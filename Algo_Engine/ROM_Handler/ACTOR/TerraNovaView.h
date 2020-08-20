// TerraNovaView.h : interface of the CTerraNovaView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRANOVAVIEW_H__CB666D00_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_TERRANOVAVIEW_H__CB666D00_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ConnectionList.h"

#define ACTOR_CLIENT 1
#define ACTOR_HOST   2

class CTerraNovaView : public CFormView
{
protected: // create from serialization only
	CTerraNovaView();
	DECLARE_DYNCREATE(CTerraNovaView)

public:
	//{{AFX_DATA(CTerraNovaView)
	enum { IDD = IDD_TERRANOVA_FORM };
	CConnectionList m_List;
	//}}AFX_DATA

// Attributes
public:
	CTerraNovaDoc* GetDocument();
	int				m_TimerId;

// Operations
public:
	void CreateTimer(UINT Interval);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerraNovaView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerraNovaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTerraNovaView)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TerraNovaView.cpp
inline CTerraNovaDoc* CTerraNovaView::GetDocument()
   { return (CTerraNovaDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRANOVAVIEW_H__CB666D00_26ED_11D2_8D11_006008CD4894__INCLUDED_)
