// TerraNovaSession.h: interface for the CTerraNovaSession class.
//
//////////////////////////////////////////////////////////////////////


#ifndef _TERRANOVA_FIXSESSION
#define _TERRANOVA_FIXSESSION

#include "FixSession.h"
#include "FixAppMsgs.h"
#include "Order.h"

class CTerraNovaSession : public CFixSession  
{

public:

	CTerraNovaSession() {}
	virtual ~CTerraNovaSession() {}
	long ProcessEntry(COrder& a_Order);

	virtual BOOL ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec);
	virtual BOOL ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
    virtual BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl); 
    

	void ProcessLowerSequenceNumber(const CFixMsg& fixMessage)
	{
		Logout();
	}

private:

	bool AddTradingSessions(CFixMsg& fixMessage, int OrderType);
};

#endif // !defined(AFX_TERRANOVASESSION_H__09702A63_A9FB_11D4_A614_00500401F87A__INCLUDED_)
