// TerraNovaDoc.h : interface of the CTerraNovaDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRANOVADOC_H__CB666CFE_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_TERRANOVADOC_H__CB666CFE_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CTerraNovaDoc : public CDocument
{
protected: // create from serialization only
	CTerraNovaDoc();
	DECLARE_DYNCREATE(CTerraNovaDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerraNovaDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTerraNovaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTerraNovaDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRANOVADOC_H__CB666CFE_26ED_11D2_8D11_006008CD4894__INCLUDED_)
