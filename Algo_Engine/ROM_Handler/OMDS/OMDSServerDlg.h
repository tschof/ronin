// OMDSServerDlg.h : header file
//

#if !defined(AFX_OMDSSERVERDLG_H__96C861F6_71D1_4253_B404_1C7FF0A84680__INCLUDED_)
#define AFX_OMDSSERVERDLG_H__96C861F6_71D1_4253_B404_1C7FF0A84680__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <list>
#include <set>
#include "ExceptionHandler.h"

/////////////////////////////////////////////////////////////////////////////
// COMDSServerDlg dialog
class CMain;
class COMDSServerDlg : public CDialog
{
// Construction
public:
	COMDSServerDlg(CWnd* pParent = NULL);	// standard constructor
	ExceptionHandler m_Handler;
// Dialog Data
	//{{AFX_DATA(COMDSServerDlg)
	enum { IDD = IDD_OMDSSERVER_DIALOG };
	CListBox	m_pWarning;
	CListBox	m_pError;
	UINT	m_nPort;
	CString	m_szNetwork;
	CString	m_szDaemon;
	CString	m_szService;
	UINT	m_nClients;
	UINT	m_nRequests;
	UINT	m_nResponses;
	UINT	m_nMsgIn;
	UINT	m_nMsgOut;
	UINT	m_nTibRate;
	UINT	m_nTibRatePeak;
	CString	m_szPath;
	UINT	m_nReqNum;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COMDSServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CMain	* m_pMain;

	bool	m_bTimer;
	std::vector<std::string>	m_ClientRequests;
	std::list<std::string>		m_OSPending;
	std::set<std::string>		m_OS;

	void GenerateRequests();
	void GenerateOptionQuoteRequest();
	void LoadRequests( const CString & szPath );

	// Generated message map functions
	//{{AFX_MSG(COMDSServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStart();
	afx_msg void OnDestroy();
	afx_msg void OnButtonStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonSimulate();
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	afx_msg LRESULT OnClientClose( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnClientUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnRequestUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnResponseUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnInTransUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnOutTransUpdate( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnError( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnWarning( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMDSSERVERDLG_H__96C861F6_71D1_4253_B404_1C7FF0A84680__INCLUDED_)
