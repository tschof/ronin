// 1CMemTagSet.h: interface for the C1CMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_1CMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_1CMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "OptionMemTagSet.h"

struct OneChicagoMemTagStruct: public OptionMemTagStruct
{
	char			m_ClearingFm[32];
	char			m_ClearingAccount[36];
	char			m_Symbol[18];
};

class COneChicagoMemTagSet : public COptionMemTagSet  
{
  public:
	COneChicagoMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings, bool forceReload, const char* storProcName = "");
	virtual ~COneChicagoMemTagSet();

	const char* ClearingAccount()	
	{
		return ((OneChicagoMemTagStruct*)m_pCurrent)->m_ClearingAccount;	
	}
	
	BOOL SetClearingAccount(const CString& ClearingAccount)	
	{ 
		if(Current()) 
		{
			strncpy(((OneChicagoMemTagStruct*)m_pCurrent)->m_ClearingAccount , ((CString&)ClearingAccount).GetBuffer(0), 35);
			return TRUE;
		}
		return FALSE;
	}

	const char* Symbol()	
	{
		return ((OneChicagoMemTagStruct*)m_pCurrent)->m_Symbol;	
	}
	
	BOOL SetSymbol(const CString& sym)	
	{ 
		if(Current()) 
		{
			strncpy(((OneChicagoMemTagStruct*)m_pCurrent)->m_Symbol, ((CString&)sym).GetBuffer(0), 17);
			return TRUE;
		}
		return FALSE;
	}


  protected:
	virtual int SizeOfStruct() { return sizeof(OneChicagoMemTagStruct); }
};




#endif // !defined(AFX_1CMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)

