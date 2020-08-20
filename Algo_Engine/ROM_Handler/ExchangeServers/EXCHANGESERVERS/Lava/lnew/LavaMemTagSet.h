// LavaMemTagSet.h: interface for the CLavaMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAVAMEMTAGSET_H__F1158D23_3D7D_11D5_A61F_00500401F87A__INCLUDED_)
#define AFX_LAVAMEMTAGSET_H__F1158D23_3D7D_11D5_A61F_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

struct LavaMemTagStruct : public MemTagStruct
{
    CString   m_Symbol; 
	BOOL m_bCancel;
	BOOL m_bReplace;
	int  m_nCancelReplaceCnt;
};

class CLavaMemTagSet : public CMemTagSet, public TArcaMemoryMapInitializer<CLavaMemTagSet>
{

public:

	CLavaMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload) : TArcaMemoryMapInitializer<CLavaMemTagSet>(lDatabaseSettingsAutoPtr)
	{
		bool fileCreated = true;
		
		InitMap(fileCreated, forceReload);
		
		TagSet(this);
		
		TArcaMemoryMapInitializer<CLavaMemTagSet>::Initialize(fileCreated, forceReload);
	}

	virtual ~CLavaMemTagSet()
	{
	}

	const CString Symbol()
	{
		return ((LavaMemTagStruct*)Current())->m_Symbol;
	}

	const BOOL Cancel()
	{
		return ((LavaMemTagStruct*)Current())->m_bCancel;
	}

	const BOOL Replace()
	{
		return ((LavaMemTagStruct*)Current())->m_bReplace;
	}

	const int CancelReplaceCnt()
	{
		return ((LavaMemTagStruct*)Current())->m_nCancelReplaceCnt;
	}

	BOOL SetSymbol(const CString& sym)
	{ 
		if(Current())
		{
			((LavaMemTagStruct*)Current())->m_Symbol = sym;
			return (BOOL)Current();
		}

		return FALSE;
	}

	BOOL SetCancel(const BOOL bCancel)
	{ 
		if(Current())
		{
			((LavaMemTagStruct*)Current())->m_bCancel = bCancel;
			return (BOOL)Current();
		}

		return FALSE;
	}

	BOOL SetReplace(const BOOL bReplace)
	{ 
		if(Current())
		{
			((LavaMemTagStruct*)Current())->m_bReplace = bReplace;
			return (BOOL)Current();
		}

		return FALSE;
	}

	BOOL SetCancelReplaceCnt(const int nCount)
	{ 
		if(Current())
		{
			((LavaMemTagStruct*)Current())->m_nCancelReplaceCnt = nCount;
			return (BOOL)Current();
		}

		return FALSE;
	}

protected:
	
	virtual int SizeOfStruct() { return sizeof(LavaMemTagStruct); }
};

#endif // !defined(AFX_LAVAMEMTAGSET_H__F1158D23_3D7D_11D5_A61F_00500401F87A__INCLUDED_)
