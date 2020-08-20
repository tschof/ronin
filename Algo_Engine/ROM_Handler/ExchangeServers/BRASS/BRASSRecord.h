// BRASSRecord.h: interface for the CBRASSRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRASSRECORD_H__0E236193_FF41_11D3_8CB8_005004DA7701__INCLUDED_)
#define AFX_BRASSRECORD_H__0E236193_FF41_11D3_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CBRASSRecord recordset

#include "IRecord.h"

class CBRASSRecord : public CIRecord
{
public:
	CBRASSRecord(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CBRASSRecord)

	const CString& GetCustomerID()		const	{ return m_CustomerID;		}
	const CString& GetTraderAcr()		const	{ return m_TraderAcr;		}
	const CString& GetLocalAcAcr()		const	{ return m_LocalAcAcr;		}
	const CString& GetClearingAcID()	const	{ return m_ClearingAcID;	}
	const CString& GetFirmAcr()			const	{ return m_FirmAcr;			}

	virtual BOOL CBRASSRecord::Open();

// Field/Param Data
	//{{AFX_FIELD(CBRASSRecord, CRecordset)
	CString	m_CustomerID;
	CString	m_TraderAcr;
	CString	m_LocalAcAcr;
	CString	m_ClearingAcID;
	CString	m_FirmAcr;
	BYTE	m_Flags;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBRASSRecord)
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

#endif // !defined(AFX_BRASSRECORD_H__0E236193_FF41_11D3_8CB8_005004DA7701__INCLUDED_)