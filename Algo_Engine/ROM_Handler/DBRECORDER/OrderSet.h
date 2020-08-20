#if !defined(AFX_ORDERSET_H__CB0653E2_3B9C_11D2_8D1C_00104B6DEAA9__INCLUDED_)
#define AFX_ORDERSET_H__CB0653E2_3B9C_11D2_8D1C_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OrderSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COrderSet recordset

#include "ClientConnection.h"
#include "Clients.h"

class COrderSet : public CRecordset
{
private:
	CString	m_SQLStatement;

public:
	BOOL	New(const CCsv& Order, const CString& Owner);
	BOOL	Status(const CCsv& Status);
	BOOL	Zap(const CCsv& Zap);
	BOOL	TradeID(const CCsv& Exec, int TradeID);
	BOOL	Query(const CString& Client, CClients& ClientList);
	BOOL	GetOrder(const CCsv& Order);

public:
	COrderSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(COrderSet)

// Field/Param Data
	//{{AFX_FIELD(COrderSet, CRecordset)
	int		m_Exchange;
	int		m_Status;
	int		m_Type;
	int		m_Capacity;
	int		m_TimeInForce;
	int		m_Side;
	CString	m_ClearingAccount;
	CString	m_ClearingId;
	CString	m_LocalAccount;
	CString	m_Firm;
	CString	m_Instructions;
	CString	m_Symbol;
	CString	m_Trader;
	double	m_Price;
	CString	m_Owner;
	CString	m_SourceId;
	long	m_Floor;
	long	m_MinQty;
	long	m_PriceCheck;
	long	m_Shares;
	long	m_OriginalShares;
	double	m_ReplacementPrice;
	long	m_PendingReplace;
	double	m_StopPrice;
	BYTE	m_Discretionary;
	BYTE	m_Solicited;
	CString	m_ClientTag;
	CString	m_OMTag;
	CTime	m_TimeStamp;
	CString	m_Client;
	CString	m_TradeFor;
	int		m_TradeID;
	CString m_ExDestination;
	CString m_ExpDate;
	CString m_CallOrPut;
	double m_StrikePrice;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COrderSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORDERSET_H__CB0653E2_3B9C_11D2_8D1C_00104B6DEAA9__INCLUDED_)
