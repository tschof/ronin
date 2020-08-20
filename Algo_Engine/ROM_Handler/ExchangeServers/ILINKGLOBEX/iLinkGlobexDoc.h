//**************************************************************************
// 
//	File:
//	
//	iLinkGlobexDoc.h
//
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#if !defined(AFX_ILINKGLOBEXDOC_H__EB484AD1_E767_4EB0_B581_4BDCCCB3DA1B__INCLUDED_)
#define AFX_ILINKGLOBEXDOC_H__EB484AD1_E767_4EB0_B581_4BDCCCB3DA1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CILinkGlobexDoc : public CDocument
{
protected: // create from serialization only
	CILinkGlobexDoc();
	DECLARE_DYNCREATE(CILinkGlobexDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CILinkGlobexDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CILinkGlobexDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CILinkGlobexDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILINKGLOBEXDOC_H__EB484AD1_E767_4EB0_B581_4BDCCCB3DA1B__INCLUDED_)
