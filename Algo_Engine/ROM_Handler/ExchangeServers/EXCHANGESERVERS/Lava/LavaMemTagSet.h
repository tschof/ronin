// LAVAMemTagSet.h: interface for the CLAVAMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAVAMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_LAVAMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "OptionMemTagSet.h"

struct LavaMemTagStruct: public OptionMemTagStruct
{
	char			m_ClearingFm[32];
	char			m_ClearingAccount[36];
	char			m_Symbol[18];
};

class CLavaMemTagSet : public COptionMemTagSet  
{
  public:
	CLavaMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings, bool forceReload, const char* storProcName = "");
	virtual ~CLavaMemTagSet();

	const char* ClearingAccount()	
	{
		return ((LavaMemTagStruct*)m_pCurrent)->m_ClearingAccount;	
	}
	
	BOOL SetClearingAccount(const CString& ClearingAccount)	
	{ 
		if(Current()) 
		{
			strncpy(((LavaMemTagStruct*)m_pCurrent)->m_ClearingAccount , ((CString&)ClearingAccount).GetBuffer(0), 35);
			return TRUE;
		}
		return FALSE;
	}

	const char* Symbol()	
	{
		return ((LavaMemTagStruct*)m_pCurrent)->m_Symbol;	
	}
	
	BOOL SetSymbol(const CString& sym)	
	{ 
		if(Current()) 
		{
			strncpy(((LavaMemTagStruct*)m_pCurrent)->m_Symbol, ((CString&)sym).GetBuffer(0), 17);
			return TRUE;
		}
		return FALSE;
	}


  protected:
	virtual int SizeOfStruct() { return sizeof(LavaMemTagStruct); }
};




#endif // !defined(AFX_LAVAMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)

