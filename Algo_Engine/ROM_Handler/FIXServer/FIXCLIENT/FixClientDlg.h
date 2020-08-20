// FixClientDlg.h : header file
//

#if !defined(AFX_FIXCLIENTDLG_H__FAB8BC26_78A6_4A6C_9317_628A1B37D8A8__INCLUDED_)
#define AFX_FIXCLIENTDLG_H__FAB8BC26_78A6_4A6C_9317_628A1B37D8A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFixClientDlg dialog
#include "ClientApplication.h"
#include <fstream>
class CFixClientDlg : public CDialog, ClientApplication
{
// Construction
public:
	CFixClientDlg(CWnd* pParent = NULL);	// standard constructor

	virtual void onLogon( const FIX::SessionID& sessionID );
	virtual void onLogout( const FIX::SessionID& sessionID );
	virtual void onMessage( const FIX42::ExecutionReport&, const FIX::SessionID& );

	FIX42::Message CreateNewOrder();

	long m_nMaxLen;
	int	m_nWidth;
// Dialog Data
	//{{AFX_DATA(CFixClientDlg)
	enum { IDD = IDD_FIXCLIENT_DIALOG };
	CListBox	m_lstReport;
	CString	m_szCount;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	static void ThreadProc( void * pParam );
	// Generated message map functions
	//{{AFX_MSG(CFixClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonLogon();
	afx_msg void OnDestroy();
	afx_msg void OnButtonBuy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	std::ofstream	m_Log;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXCLIENTDLG_H__FAB8BC26_78A6_4A6C_9317_628A1B37D8A8__INCLUDED_)
