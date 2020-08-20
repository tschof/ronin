// CHXMemTagSet.h: interface for the CCHXMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHXMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_CHXMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct CHXMemTagStruct : public MemTagStruct
{
	BOOL m_bCancel;
	long m_Shares;
	long m_OriginalShares;
	double m_OriginalPrice;
};

class CCHXMemTagSet : public CMemTagSet  
{
public:
	CCHXMemTagSet();
	virtual ~CCHXMemTagSet();

	const double OriginalPrice(){ return ((CHXMemTagStruct*)Current())->m_OriginalPrice;	}
	const long Shares()	{ return ((CHXMemTagStruct*)Current())->m_Shares;	}
	const long OriginalShares()	{ return ((CHXMemTagStruct*)Current())->m_OriginalShares;	}
	const BOOL Cancel() { return ((CHXMemTagStruct*)Current())->m_bCancel; }

	BOOL SetCancel(const BOOL bCancel)
	{ 
		if(Current()) ((CHXMemTagStruct*)Current())->m_bCancel = bCancel;
		return (BOOL)Current();
	}

	BOOL SetShares(const long Shares)
	{ 
		if(Current()) ((CHXMemTagStruct*)Current())->m_Shares = Shares;
		return (BOOL)Current();
	}
	BOOL SetOriginalShares(const long Shares)
	{ 
		if(Current()) ((CHXMemTagStruct*)Current())->m_OriginalShares = Shares;
		return (BOOL)Current();
	}
	BOOL SetOriginalPrice(const double Price)
	{ 
		if(Current()) ((CHXMemTagStruct*)Current())->m_OriginalPrice = Price;
		return (BOOL)Current();
	}

protected:
	virtual int SizeOfStruct() { return sizeof(CHXMemTagStruct); }
};

#endif // !defined(AFX_CHXMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
