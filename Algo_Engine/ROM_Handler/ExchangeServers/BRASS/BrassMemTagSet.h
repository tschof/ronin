// BrassMemTagSet.h: interface for the CBrassMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRASSMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_BRASSMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000]

#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

struct BrassMemTagStruct : public MemTagStruct
{
	long	m_Shares;
	long	m_RemainingShares;
	double	m_Price;
	char	m_Type;
	long	m_NewShares;
	double	m_NewPrice;
	double  m_NewStopPrice;
	char	m_NewTIF;
	char	m_NewExecInst;
	char	m_NewType;
	int		m_Side;
	char	m_originalOrderTime[20];
	BOOL	m_bReplacePending;
};

class CBrassMemTagSet : public CMemTagSet, public TBrassMemoryMapInitializer<CBrassMemTagSet>
{
public:

	CBrassMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload);
	virtual ~CBrassMemTagSet();

	const long Shares()
	{
		return ((BrassMemTagStruct*)Current())->m_Shares;
	}
	
	const int Side()
	{
		return ((BrassMemTagStruct*)Current())->m_Side;
	}

	const long RemainingShares()
	{
		return ((BrassMemTagStruct*)Current())->m_RemainingShares;
	}

	const double Price()
	{
		return ((BrassMemTagStruct*)Current())->m_Price;
	}

	const char Type()
	{
		return ((BrassMemTagStruct*)Current())->m_Type;
	}

	const double NewPrice()
	{
		return ((BrassMemTagStruct*)Current())->m_NewPrice;
	}

	const double NewStopPrice()
	{
		return ((BrassMemTagStruct*)Current())->m_NewStopPrice;
	}

	const long NewShares()
	{
		return ((BrassMemTagStruct*)Current())->m_NewShares;
	}

	const char NewType()
	{
		return ((BrassMemTagStruct*)Current())->m_NewType;
	}

	const char NewTIF()
	{
		return ((BrassMemTagStruct*)Current())->m_NewTIF;
	}

	const char NewExecInst()
	{
		return ((BrassMemTagStruct*)Current())->m_NewExecInst;
	}

	const CString OriginalOrderTime()
	{
		return ((BrassMemTagStruct*)Current())->m_originalOrderTime;
	}

	const BOOL ReplacePending() 
	{ 
		return ((BrassMemTagStruct*)Current())->m_bReplacePending;
	}

	BOOL SetRemainingShares(long Shares)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_RemainingShares = Shares;
		}

		return (BOOL)Current();
	}
	
	BOOL SetShares(const long Shares)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_Shares = Shares;
		}

		return (BOOL)Current();
	}

	BOOL SetSide(const int Side)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_Side = Side;
		}

		return (BOOL)Current();
	}

	BOOL SetPrice(const double Price)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_Price = Price;
		}

		return (BOOL)Current();
	}

	BOOL SetType(const char Type)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_Type = Type;
		}

		return (BOOL)Current();
	}

	BOOL SetNewPrice(const double Price)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_NewPrice = Price;
		}

		return (BOOL)Current();
	}

	BOOL SetNewShares(const long Shares)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_NewShares = Shares;
		}

		return (BOOL)Current();
	}

	BOOL SetNewType(const char Type)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_NewType = Type;
		}

		return (BOOL)Current();
	}

	BOOL SetNewStopPrice(const double StopPrice)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_NewStopPrice = StopPrice;
		}

		return (BOOL)Current();
	}

	BOOL SetNewTIF(const char TIF)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_NewTIF = TIF;
		}

		return (BOOL)Current();
	}

	BOOL SetNewExecInsts(const char ExecInst)
	{ 
		if(Current())
		{
			((BrassMemTagStruct*)Current())->m_NewExecInst = ExecInst;
		}

		return (BOOL)Current();
	}

	void SetOriginalOrderTime(const CString& originalOrderTime)
	{
		strcpy(((BrassMemTagStruct*)Current())->m_originalOrderTime,originalOrderTime);
	}

	void SetReplacePending(const BOOL& ReplacePending)
	{
		((BrassMemTagStruct*)Current())->m_bReplacePending = ReplacePending;
	}

protected:

	virtual int SizeOfStruct()
	{
		return sizeof(BrassMemTagStruct);
	}

};

#endif // !defined(AFX_BRASSMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
