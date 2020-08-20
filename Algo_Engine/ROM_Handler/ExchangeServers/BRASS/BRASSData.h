// BRASSData.h: interface for the CBRASSData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRASSDATA_H__0E236194_FF41_11D3_8CB8_005004DA7701__INCLUDED_)
#define AFX_BRASSDATA_H__0E236194_FF41_11D3_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IData.h"

class CBRASSData : public CIData  
{
public:
	CBRASSData();
	virtual ~CBRASSData();

	virtual CIData* Insert(int = 0);
	virtual CIData* Remove();
	virtual void SetDefault();

	const CString& GetCustomerID()		const	{ return m_CustomerID;		}
	const CString& GetTraderAcr()		const	{ return m_TraderAcr;		}
	const CString& GetLocalAcAcr()		const	{ return m_LocalAcAcr;		}
	const CString& GetClearingAcID()	const	{ return m_ClearingAcID;	}
	const CString& GetFirmAcr()			const	{ return m_FirmAcr;			}

	void SetCustomerID(CString& Value)		
	{ 
		m_CustomerID = Value;
		SetKey(m_CustomerID);
	}
	void SetTraderAcr(CString& Value)		{ m_TraderAcr = Value;		}
	void SetLocalAcAcr(CString& Value)		{ m_LocalAcAcr = Value;		}
	void SetClearingAcID(CString& Value)	{ m_ClearingAcID = Value;	}
	void SetFirmAcr(CString& Value)			{ m_FirmAcr = Value;		}

private:
	CString m_CustomerID;
	CString m_TraderAcr;
	CString m_LocalAcAcr;
	CString m_ClearingAcID;
	CString m_FirmAcr;

public:
	CBRASSData* GotoFirst()						{ return (CBRASSData*)CIData::GotoFirst();       }
	CBRASSData* GotoLast()						{ return (CBRASSData*)CIData::GotoLast();        }
	CBRASSData* Goto(int place)					{ return (CBRASSData*)CIData::Goto(place);       }
	CBRASSData* MoveNext()						{ return (CBRASSData*)CIData::MoveNext();        }
	CBRASSData* MovePrev()						{ return (CBRASSData*)CIData::MovePrev();        }
	CBRASSData* MoveAt(CString key)				{ return (CBRASSData*)CIData::MoveAt(key);       }
	CBRASSData* Insert(CIData* data, int pos)	{ return (CBRASSData*)CIData::Insert(data, pos);	}
	CBRASSData* Remove(CString key)				{ return (CBRASSData*)CIData::Remove(key);		}

};

#endif // !defined(AFX_BRASSDATA_H__0E236194_FF41_11D3_8CB8_005004DA7701__INCLUDED_)
