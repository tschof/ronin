#if !defined(AFX_SEQDLG_H__74F427D3_AE3A_11D3_8C9D_005004DA7701__INCLUDED_)
#define AFX_SEQDLG_H__74F427D3_AE3A_11D3_8C9D_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SeqDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSeqDlg dialog

class CSeqDlg : public CDialog
{
// Construction
public:
	CSeqDlg(BOOL bNextOutgoing, CWnd* pParent = NULL);   // standard constructor
	long SeqNum() { return m_SeqNum; }

// Dialog Data
	//{{AFX_DATA(CSeqDlg)
	enum { IDD = IDD_SEQDLG };
	long	m_SeqNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeqDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSeqDlg)
		// NOTE: the ClassWizard will add member functions here
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bOutgoing;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEQDLG_H__74F427D3_AE3A_11D3_8C9D_005004DA7701__INCLUDED_)
