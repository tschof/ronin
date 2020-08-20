#if !defined(AFX_TRADETABLESET_H__F1BCD352_015E_11D2_8D0F_006008CD4894__INCLUDED_)
#define AFX_TRADETABLESET_H__F1BCD352_015E_11D2_8D0F_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TradeTableSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTradeTableSet recordset

#define TRADE_ADD_QUERY			0	
#define TRADE_UPDATE_QUERY		1
#define TRADE_REMOVE_QUERY		2

class CTradeTableSet : public CRecordset
{
public:
	void	SetType(int Type);
	void	SetProperties(const CCsv& Exec, const COrderSet& Order);
	long	AddTrade(const CCsv& Exec, const COrderSet& Order);
	long	RemoveTrade(const CCsv& Exec, const COrderSet& Order);
	long	UpdateTrade(const CCsv& Exec, const COrderSet& Order);

	void	SetProperties(const CCsv& Exec);
	long	AddTrade(const CCsv& Exec);

	CTradeTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CTradeTableSet)

	int				m_Type;
// Field/Param Data
	//{{AFX_FIELD(CSTEPTradeSet, CRecordset)
	long		m_param_trade_id;
	long		m_param_trade_status;
	CString	m_param_uid;
	CString	m_param_trader;
	CString	m_param_local_ac;
	CString	m_param_clearing_ac_acr;
	CString	m_param_clearing_firm;
	CString	m_param_contra;
	CString	m_param_contra_firm;
	CString	m_param_purpose;
	CString	m_param_base;
	CString	m_param_sec_types;
	CString	m_param_cp;
	double	m_param_low_strike;
	double	m_param_high_strike;
	CString	m_param_bs;
	long		m_param_low_qty;
	long		m_param_high_qty;
	double	m_param_low_price;
	double	m_param_high_price;
	CString	m_param_start_exp_date;
	CString	m_param_end_exp_date;
	CString	m_param_start_trade_date;
	CString	m_param_end_trade_date;
	CString	m_param_start_entry_date;
	CString	m_param_end_entry_date;

	long		m_trade_id;
	CString	m_trader;
	CString	m_local_ac;
	CString	m_clearing_ac;
	CString	m_clearing_ac_acr;
	CString	m_clearing_firm;
	CString	m_clearing_firm_id;
	CString	m_contra;
	CString	m_contra_firm;
	CString	m_exchange;
	CString	m_base_cusip;
	CString	m_symbol;
	CString	m_sec_type;
	CString	m_TypeStr;
	CTime		m_exp_date;
	CString	m_cp;
	double	m_strike;
	long		m_qty;
	double	m_price;
	long		m_spread_id;
	CString	m_spread_type;
	CTime		m_trade_date;
	CTime		m_entry_date;
	CTime		m_change_date;
	CString	m_uid;
	long		m_trade_status;
	int		m_active;
	CString	m_source;
	CString	m_transno;
	CString	m_transdt;
	int		m_msgseq;
	CString	m_trade_stat;
	CString m_purpose;
	//}}AFX_FIELD

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTEPTradeSet)
	public:
	virtual CString GetDefaultConnect();	// Default connection string
	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
private:
	CString GetTimeString( CTime& time, CString format="%m/%d/%Y %H:%M:%S" );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRADETABLESET_H__F1BCD352_015E_11D2_8D0F_006008CD4894__INCLUDED_)
