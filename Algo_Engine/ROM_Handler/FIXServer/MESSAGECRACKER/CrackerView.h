// CrackerView.h : interface of the CCrackerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRACKERVIEW_H__FF27639D_8D1F_4E11_A89C_AAA5A891459C__INCLUDED_)
#define AFX_CRACKERVIEW_H__FF27639D_8D1F_4E11_A89C_AAA5A891459C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Application.h"
#include "MessageCracker.h"
#include <vector>
#include <string>
#include <algorithm>
#include "IOMApplication.h"

class COmListener;

class CCrackerView : public CFormView, public FIX::Application, public FIX::MessageCracker, public IOMApplication
{
protected: // create from serialization only
	CCrackerView();
	DECLARE_DYNCREATE(CCrackerView)

public:
	virtual void onCreate( const FIX::SessionID& sessionID );
	virtual void onLogon( const FIX::SessionID& sessionID );
	virtual void onLogout( const FIX::SessionID& sessionID );
	virtual void toAdmin( FIX::Message& message, const FIX::SessionID& sessionID );
	virtual void toApp( FIX::Message& message, const FIX::SessionID& sessionID )
		throw(FIX::DoNotSend&);
    virtual void fromAdmin( const FIX::Message& message, const FIX::SessionID& )
		throw(FIX::FieldNotFound&, FIX::RejectLogon&);
	virtual void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
		throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&);
	void onRun();

	virtual void onMessage( const FIX42::ExecutionReport& report, const FIX::SessionID& sessionID ); 
	virtual void onMessage( const FIX42::OrderCancelReject& reject, const FIX::SessionID& sessionID ); 

	virtual void	OnMessage	( std::string & szMsg );
	virtual int		Send		( std::string & szMsg );

	//{{AFX_DATA(CCrackerView)
	enum { IDD = IDD_MESSAGECRACKER_FORM };
	CListBox	m_lstResponse;
	CProgressCtrl	m_Progress;
	CListCtrl	m_lstMsg;
	CListCtrl	m_lstLog;
	CString	m_szKey;
	CString	m_szSendKey;
	CString	m_szRecvKey;
	//}}AFX_DATA

// Attributes
public:

	tMessages	m_Results;
	bool		m_bHasResults;
	int			m_nMsgWidth;
	CFont		m_font;
	std::string	m_szSenderCompID;
	std::string	m_szTargetCompID;
	HANDLE		m_hKill;
	HANDLE		m_hKilled;
	COmListener * m_pListener;

	CString		GetSelected( CCrackerApp::tOmFieldMap & fields );
	bool		Start();
	static void ThreadProc( void * pParam );

	std::string &	GetMessage( int nIndex );
	void			Reset();
	void			CrackFixMsg( CString & szMsg );
	void			Search();
// Operations
public:
	CCrackerDoc* GetDocument();
	void AddLogMsg();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackerView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCrackerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void InitListControl();
// Generated message map functions
protected:
	//{{AFX_MSG(CCrackerView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonSearch();
	afx_msg void OnButtonShowall();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClickListLog(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnOdstatechangedListLog(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnButtonBegin();
//	afx_msg void OnButtonAfter();
	afx_msg void OnButtonProcess();
	afx_msg void OnButtonOm();
	afx_msg void OnButtonFix();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void GetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCacheHint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOdfinditem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawListMsg( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CrackerView.cpp
inline CCrackerDoc* CCrackerView::GetDocument()
   { return (CCrackerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRACKERVIEW_H__FF27639D_8D1F_4E11_A89C_AAA5A891459C__INCLUDED_)
