// TerraMemTagSet.h: interface for the CTerraMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAMEMTAGSET_H__F1158D23_3D7D_11D5_A61F_00500401F87A__INCLUDED_)
#define AFX_TERRAMEMTAGSET_H__F1158D23_3D7D_11D5_A61F_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TMemoryMapInitializer.h"

struct TerraMemTagStruct : public MemTagStruct
{
	BOOL m_bCancel;
	BOOL m_bReplace;
	int  m_nCancelReplaceCnt;
};

class CTerraMemTagSet : public CMemTagSet, public TArcaMemoryMapInitializer<CTerraMemTagSet>
{

public:

	CTerraMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload) : TArcaMemoryMapInitializer<CTerraMemTagSet>(lDatabaseSettingsAutoPtr)
	{
		bool fileCreated = true;
		
		InitMap(fileCreated, forceReload);
		
		TagSet(this);
		
		TArcaMemoryMapInitializer<CTerraMemTagSet>::Initialize(fileCreated, forceReload);
	}

	virtual ~CTerraMemTagSet()
	{
	}

	const BOOL Cancel()
	{
		return ((TerraMemTagStruct*)Current())->m_bCancel;
	}

	const BOOL Replace()
	{
		return ((TerraMemTagStruct*)Current())->m_bReplace;
	}

	const int CancelReplaceCnt()
	{
		return ((TerraMemTagStruct*)Current())->m_nCancelReplaceCnt;
	}

	BOOL SetCancel(const BOOL bCancel)
	{ 
		if(Current())
		{
			((TerraMemTagStruct*)Current())->m_bCancel = bCancel;
			return (BOOL)Current();
		}

		return FALSE;
	}

	BOOL SetReplace(const BOOL bReplace)
	{ 
		if(Current())
		{
			((TerraMemTagStruct*)Current())->m_bReplace = bReplace;
			return (BOOL)Current();
		}

		return FALSE;
	}

	BOOL SetCancelReplaceCnt(const int nCount)
	{ 
		if(Current())
		{
			((TerraMemTagStruct*)Current())->m_nCancelReplaceCnt = nCount;
			return (BOOL)Current();
		}

		return FALSE;
	}

protected:
	
	virtual int SizeOfStruct() { return sizeof(TerraMemTagStruct); }
};

#endif // !defined(AFX_TERRAMEMTAGSET_H__F1158D23_3D7D_11D5_A61F_00500401F87A__INCLUDED_)
