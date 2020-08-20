#if !defined(AFX_ACTDIALOG_H__CFA2BFB1_A00E_11D2_9F4F_00105AA84069__INCLUDED_)
#define AFX_ACTDIALOG_H__CFA2BFB1_A00E_11D2_9F4F_00105AA84069__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ActDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CActDialog dialog

class CActDialog : public CDialog
{
// Construction
public:
	CActDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CActDialog)
	enum { IDD = IDD_ACTDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CActDialog)
	afx_msg void OnSelchangeComboFunction();
	afx_msg void OnSelchangeComboAsof();
	afx_msg void OnSelchangeComboSeccls();
	afx_msg void OnSelchangeComboBsx();
	afx_msg void OnKillfocusEditRefnum();
	afx_msg void OnKillfocusEditVolume();
	afx_msg void OnKillfocusEditPricedollar();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboSymbol();
	afx_msg void OnKillfocusEditPricenum();
	afx_msg void OnSelchangeComboDen();
	afx_msg void OnSelchangeComboTrademod();
	afx_msg void OnPriceoveride();
	afx_msg void OnKillfocusEditOeid();
	afx_msg void OnKillfocusEditOegu();
	afx_msg void OnKillfocusEditOeclearnum();
	afx_msg void OnKillfocusEditMmid();
	afx_msg void OnKillfocusEditMmgu();
	afx_msg void OnKillfocusEditMmclearnum();
	afx_msg void OnSelchangeComboMmpa();
	afx_msg void OnSelchangeComboTraderepflag();
	afx_msg void OnSelchangeComboSpecialindicator();
	afx_msg void OnReportflag();
	afx_msg void OnKillfocusEditMemo();
	afx_msg void OnKillfocusEditTradepriceexp();
	afx_msg void OnKillfocusEditTradepriceexp2();
	afx_msg void OnSelchangeComboClearflag();
	afx_msg void OnSendact();
	afx_msg void OnNcLButtonDblClk( UINT nHitTest, CPoint point );
	afx_msg void OnChangeEdit1();
	afx_msg void OnChangeSymbolEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CAct m_ActMessage;
	CRect m_Size;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTDIALOG_H__CFA2BFB1_A00E_11D2_9F4F_00105AA84069__INCLUDED_)
