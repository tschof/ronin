// BrassMemTagSet.h: interface for the CBrassMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRASSMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_BRASSMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemTagSet.h"

struct BloombergMemTagStruct : public MemTagStruct
{
	long m_Shares;
	long m_OriginalShares;
	double m_OriginalPrice;
};

class CBloombergMemTagSet : public CMemTagSet  
{
public:
	CBloombergMemTagSet();
	virtual ~CBloombergMemTagSet();

	const double OriginalPrice(){ return ((BloombergMemTagStruct*)Current())->m_OriginalPrice;	}
	const long Shares()	{ return ((BloombergMemTagStruct*)Current())->m_Shares;	}
	const long OriginalShares()	{ return ((BloombergMemTagStruct*)Current())->m_OriginalShares;	}

	BOOL SetShares(const long Shares)
	{ 
		if(Current()) ((BloombergMemTagStruct*)Current())->m_Shares = Shares;
		return (BOOL)Current();
	}
	BOOL SetOriginalShares(const long Shares)
	{ 
		if(Current()) ((BloombergMemTagStruct*)Current())->m_OriginalShares = Shares;
		return (BOOL)Current();
	}
	BOOL SetOriginalPrice(const double Price)
	{ 
		if(Current()) ((BloombergMemTagStruct*)Current())->m_OriginalPrice = Price;
		return (BOOL)Current();
	}
	/*BOOL SetOriginalPrice(const double Price)
	{ 
		if(Current()) ((BloombergMemTagStruct*)Current())->m_OriginalPrice = Price;
		return (BOOL)Current();
	}*/
	

protected:
	virtual int SizeOfStruct() { return sizeof(BloombergMemTagStruct); }
};

#endif // !defined(AFX_BRASSMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
