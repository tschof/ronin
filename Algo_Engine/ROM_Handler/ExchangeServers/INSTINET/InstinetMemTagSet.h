// InstinetMemTagSet.h: interface for the CInstinetMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETMEMTAGSET_H__46D8FEA3_376B_11D6_A629_00500401F87A__INCLUDED_)
#define AFX_INSTINETMEMTAGSET_H__46D8FEA3_376B_11D6_A629_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct InstinetMemTagStruct : public MemTagStruct
{
	long m_Shares;
	long m_OriginalShares;
	double m_OriginalPrice;
	int m_nReplace;
	char m_TargetSubId[15];
};

class CInstinetMemTagSet : public CMemTagSet  
{
public:
	CInstinetMemTagSet();
	virtual ~CInstinetMemTagSet();

	const char *TargetSubId() { return ((InstinetMemTagStruct*)Current())->m_TargetSubId;	}
	BOOL SetTargetSubId(char *Id)
	{ 
		if(Current()) 
		{
			strcpy(((InstinetMemTagStruct*)Current())->m_TargetSubId, Id);
		}
		return (BOOL)Current();
	}

	const double OriginalPrice(){ return ((InstinetMemTagStruct*)Current())->m_OriginalPrice;	}
	const long Shares()	{ return ((InstinetMemTagStruct*)Current())->m_Shares;	}
	const long OriginalShares()	{ return ((InstinetMemTagStruct*)Current())->m_OriginalShares;	}
	
	BOOL SetShares(const long Shares)
	{ 
		if(Current()) ((InstinetMemTagStruct*)Current())->m_Shares = Shares;
		return (BOOL)Current();
	}
	BOOL SetOriginalShares(const long Shares)
	{ 
		if(Current()) ((InstinetMemTagStruct*)Current())->m_OriginalShares = Shares;
		return (BOOL)Current();
	}
	BOOL SetOriginalPrice(const double Price)
	{ 
		if(Current()) ((InstinetMemTagStruct*)Current())->m_OriginalPrice = Price;
		return (BOOL)Current();
	}
	void AddReplace()
	{ 
		if(Current())
		{
			((InstinetMemTagStruct*)Current())->m_nReplace += 1;
		}
	}
	void DeleteReplace()
	{
		if(Current())
		{
			((InstinetMemTagStruct*)Current())->m_nReplace -= 1;
		}
	}
	void InitReplace()
	{
		if(Current())
		{
			((InstinetMemTagStruct*)Current())->m_nReplace = 0;
		}
	}
	BOOL Replace() 
	{ 
		if (((InstinetMemTagStruct*)Current())->m_nReplace == 0)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
};

#endif // !defined(AFX_INSTINETMEMTAGSET_H__46D8FEA3_376B_11D6_A629_00500401F87A__INCLUDED_)
