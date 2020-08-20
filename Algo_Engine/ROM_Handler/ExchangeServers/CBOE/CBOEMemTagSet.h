// CBOEMemTagSet.h: interface for the CCBOEMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBOEMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_CBOEMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptionMemTagSet.h"

struct CBOEMemTagStruct: public OptionMemTagStruct
{
	char			m_ExecBroker[36];
	char			m_ClearingAccount[36];
};

class CCBOEMemTagSet : public COptionMemTagSet  
{
  public:
	CCBOEMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings, bool forceReload, const char* storProcName = "");
	virtual ~CCBOEMemTagSet();

	const char* ClearingAccount()	
	{
		return ((CBOEMemTagStruct*)m_pCurrent)->m_ClearingAccount;	
	}
	
	BOOL SetClearingAccount(const CString& ClearingAccount)	
	{ 
		if(Current()) 
		{
			strncpy(((CBOEMemTagStruct*)m_pCurrent)->m_ClearingAccount , ((CString&)ClearingAccount).GetBuffer(0), 35);
			return TRUE;
		}
		return FALSE;
	}

	const char* ExecBroker()	
	{
		return ((CBOEMemTagStruct*)m_pCurrent)->m_ExecBroker;	
	}
	
	BOOL SetExecBroker(const CString& ExecBroker)	
	{ 
		if(Current()) 
		{
			strncpy(((CBOEMemTagStruct*)m_pCurrent)->m_ExecBroker, ((CString&)ExecBroker).GetBuffer(0), 35);
			return TRUE;
		}
		return FALSE;
	}

  protected:
	virtual int SizeOfStruct() { return sizeof(CBOEMemTagStruct); }
};




#endif // !defined(AFX_CBOEMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)

