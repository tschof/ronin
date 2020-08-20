//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"


//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CILinkGlobexApp::ProcessClientCancel(CBaseConnection& pC, CCsv& csv)
{
	CString sourceId;

	CCancel cancel(csv);

	cancel.SetOwner(pC.m_Name);
	
	sourceId.Format("%s,%s", pC.m_Name, cancel.Trader());
	
	cancel.SetSourceId(sourceId);

	Handler().ProcessMessage(cancel);

	Audit(csv, "SERVER GLOBEX", "FROM SERVER OM",
        true, cancel.CancelTag());

	return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CILinkGlobexApp::ProcessClientLogon(CBaseConnection &baseConnection, CCsv& csv)
{
	CLogin Login;

	baseConnection.m_Name = csv.Get(CSV_TRADER);

	// If the connection already exists, kick it out
	CBaseConnection* pExist = Clients().FindClient((LPCSTR) (baseConnection.m_Name));

	if(pExist)
	{
		CString msg = pExist->GetConnector() + " replaced by " + baseConnection.GetConnector();
		ProcessLogEntry(baseConnection.m_Name,msg);
		ProcessClientLogout(*pExist, csv);
	}

	baseConnection.SetLogonStatus(TRUE);

	Clients().SetClient(&baseConnection, baseConnection.m_Name);

	Login.SetTrader(m_Name);
	baseConnection.SendMessage(Login.Message());

	Clients().Dump(baseConnection.Name());

	Audit(csv, "SERVER GLOBEX", "FROM SERVER OM",
        true, csv.Get(CSV_STAFFORD_TAG));

	return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CILinkGlobexApp::ProcessClientLogout(CBaseConnection &pC, CCsv& csv)
{
    
	Audit(csv, "SERVER GLOBEX", "FROM SERVER OM",
        true, csv.Get(CSV_STAFFORD_TAG));

	return FALSE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CILinkGlobexApp::ProcessClientOrder(CBaseConnection& baseConnection, CCsv& csv)
{
	CString sourceId;

	COrder order(csv);

	order.SetOwner(baseConnection.m_Name);
	
	sourceId.Format("%s,%s", baseConnection.m_Name, order.Trader());
	
	order.SetSourceId(sourceId);

	Handler().ProcessMessage(order);

	Audit(csv, "SERVER GLOBEX", "FROM SERVER OM", true,
        order.OrderTag());

	return TRUE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CILinkGlobexApp::ProcessClientReplace(CBaseConnection& pC, CCsv& csv)
{
    CReplace replace(csv);
    replace.SetOwner(pC.m_Name);
    CString sourceId;
    sourceId.Format("%s,%s", pC.m_Name, replace.Trader());
    //replace.SetSourceId(replace.Trader());
    replace.SetSourceId(sourceId);
    Handler().ProcessMessage(replace);
	Audit(csv, "SERVER GLOBEX", "FROM SERVER OM",
        true, replace.CancelTag());

	return FALSE;
}

//**************************************************************************
// 
//	Method:
//	
//
//	Parameters:
//
//
//	Returns
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//	History
//
//****************************************************************************
BOOL CILinkGlobexApp::ProcessClientStatus(CBaseConnection& baseConnection, CCsv& csv)
{
	CString sourceId;

	CRequest request(csv);
	
	request.SetOwner(baseConnection.m_Name);

	CString Dest = csv.Get(CSV_TEXT);

	Audit(csv, "SERVER OM", Dest.GetBuffer(0),
        FALSE, csv.Get(CSV_STAFFORD_TAG));

	sourceId.Format("%s,%s", baseConnection.m_Name, request.Trader());
	request.SetSourceId(sourceId);
	Handler().ProcessMessage(request);

	
	return TRUE;
}