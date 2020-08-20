// TradeClientDlg.h : header file
//

#if !defined(AFX_TRADECLIENTDLG_H__76FC8156_BAB7_4D61_A10E_118456D8F934__INCLUDED_)
#define AFX_TRADECLIENTDLG_H__76FC8156_BAB7_4D61_A10E_118456D8F934__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTradeClientDlg dialog
#include "Application.h"
#include "MessageCracker.h"
#include "OMOrder.h"
#include <map>
#include <deque>

#define WM_REPORT WM_USER+10
#define WM_REJECT WM_USER+20

class CTradeClientDlg : public CDialog, public FIX::Application, public FIX::MessageCracker
{
// Construction
public:
	CTradeClientDlg(CWnd* pParent = NULL);	// standard constructor

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



// Dialog Data
	//{{AFX_DATA(CTradeClientDlg)
	enum { IDD = IDD_TRADECLIENT_DIALOG };
	CComboBox	m_ExecInst;
	CListBox	m_lstReport;
	CComboBox	m_SecType;
	CComboBox	m_CallPut;
	CComboBox	m_OpenClose;
	CListCtrl	m_lstOrder;
	CComboBox	m_Type;
	CComboBox	m_TIF;
	CComboBox	m_Side;
	CString	m_szSymbol;
	UINT	m_nQty;
	double	m_dPrice;
	UINT	m_nDest;
	CString	m_szAccount;
	CString	m_szExpire;
	float	m_fStrike;
	CString	m_szStatus;
	float	m_fStopPrice;
	UINT	m_nRate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTradeClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	enum 
	{
		ORDERID,
		SYMBOL,
		STATUS,
		QTY,
		PRICE,
		LEAVE,
		CUM,
		TYPE,
		TIF,
		NUMFIELDS
	};

	static void ThreadProc		( void * pParam );
	bool		Start			();
	std::string CreateOID		();
	void		InitListCtrl	();
	void		BuildColumns	();
	void		InsertOrder		( FIX::ClOrdID & cid, FIX42::OMOrder *pOrder );
	void		SendOrder		();

	HICON	m_hIcon;
	HANDLE	m_hKill;
	HANDLE	m_hKilled;
	CFont	m_font;
	std::string m_szSenderCompID;
	std::string m_szTargetCompID;
	long	m_nWidth;
	CDC *	m_pDC;
	bool	m_bTimer;
	FIX::Mutex	m_mutex;

	typedef std::map<FIX::ClOrdID,FIX42::OMOrder *>	tOrders;
	tOrders	m_Orders;

	std::deque<FIX42::ExecutionReport>	m_Reports;
	std::deque<FIX42::OrderCancelReject> m_Rejects;

	FIX::Session * m_pSession;
	// Generated message map functions
	//{{AFX_MSG(CTradeClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSubmit();
	afx_msg void OnButtonCancel();
	afx_msg void OnButtonModify();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonBurstOrder();
	afx_msg void OnButtonStop();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnExecutionReport( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnCancelReject( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADECLIENTDLG_H__76FC8156_BAB7_4D61_A10E_118456D8F934__INCLUDED_)
