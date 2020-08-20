#if !defined(AFX_TRAFFICDOC_H__FF7C9A33_27AC_11D2_8D12_006008CD4894__INCLUDED_)
#define AFX_TRAFFICDOC_H__FF7C9A33_27AC_11D2_8D12_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TrafficDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrafficDoc document

class CTrafficDoc : public CDocument
{
protected:
	CTrafficDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTrafficDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrafficDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTrafficDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrafficDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAFFICDOC_H__FF7C9A33_27AC_11D2_8D12_006008CD4894__INCLUDED_)
