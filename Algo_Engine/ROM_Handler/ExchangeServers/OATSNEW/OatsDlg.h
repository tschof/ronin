// oatsDlg.h : header file
//

#if !defined(AFX_OATSDLG_H__8A627926_2B35_11D3_A8D7_006008CD4894__INCLUDED_)
#define AFX_OATSDLG_H__8A627926_2B35_11D3_A8D7_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// COatsDlg dialog

#include "OatsCallback.h"
#include "resource.h"
#include <vector>
#include <string>
class CDictionaryCallBack;

class COatsDlg : public CDialog
{
// Construction
public:
	COatsDlg(CWnd* pParent = NULL);	// standard constructor

	static COatsCallback * GetCallback() { return & m_OatsCallback; }
	void UpdateStatus();
	CString Received() { return m_szCount;}
	CString Recorded() { return m_szQueued;}

	void SetAdminCount(CString& sCount) { m_szAdminCount = sCount; }
// Dialog Data
	//{{AFX_DATA(COatsDlg)
	enum { IDD = IDD_OATS_DIALOG };
	CProgressCtrl	m_Progress;
	CString	m_szCount;
	CString	m_szQueued;
	//BOOL	m_bConnected;
	CString	m_szAdminCount;
	CTime	m_Begin;
	CTime	m_End;
	CString	m_szFile;
	CString m_szRomCount;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COatsDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	

// Implementation
protected:
	HICON	m_hIcon;
	bool	m_bPause;
	CBrush	m_brGreen;
	CBrush	m_brRed;

	// Generated message map functions
	//{{AFX_MSG(COatsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonPause();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	afx_msg void OnButtonUpdate();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonFile();
	afx_msg void OnButtonProcess();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
	// void UpdateStatus();
//	bool IsValidMessage( const CString & szDate, const CString & szTime );
	bool IsValidMessage(const CString & szTime );
	bool Parse( std::vector<std::string> & l, const char * pszMsg, char c );
	static COatsCallback	m_OatsCallback;
	CTime					m_LastHB;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OATSDLG_H__8A627926_2B35_11D3_A8D7_006008CD4894__INCLUDED_)
