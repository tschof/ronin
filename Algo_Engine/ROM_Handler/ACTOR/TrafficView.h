#if !defined(AFX_TRAFFICVIEW_H__FF7C9A32_27AC_11D2_8D12_006008CD4894__INCLUDED_)
#define AFX_TRAFFICVIEW_H__FF7C9A32_27AC_11D2_8D12_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TrafficView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrafficView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CTrafficView : public CFormView
{
protected:
	CTrafficView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTrafficView)

// Form Data
public:
	//{{AFX_DATA(CTrafficView)
	enum { IDD = IDD_LISTVIEW };
	CListBox	m_List;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void Add(const CString& Message);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrafficView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTrafficView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTrafficView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAFFICVIEW_H__FF7C9A32_27AC_11D2_8D12_006008CD4894__INCLUDED_)
