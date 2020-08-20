// ChxFixSession.cpp: implementation of the CChxFixSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CHX.h"
#include "ChxFixSession.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CChxFixSession::ProcessEmail(CString& a_Text)
{
	CString l_MailRecipients = "";
	CString l_MailServer;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	GetPrivateProfileString("Fix Email", "Mail Recipients", "", l_MailRecipients.GetBuffer(128), 128, pApp->DatFile());
	GetPrivateProfileString("Fix Email", "Mail Server", "smkd-chi-et3b01.ronin.roncap.com", l_MailServer.GetBuffer(128), 128, pApp->DatFile());

	if (l_MailRecipients == "")
	{
		return;
	}

	CMailMessage l_MailMsg;
	CSMTP l_Smtp((LPCTSTR)l_MailServer);

	char l_Hostname[50];
	gethostname(l_Hostname,50);
	
	CString l_Subject = "Fix Email from CHX";

	l_MailMsg.m_sFrom = l_Hostname;
	l_MailMsg.AddMultipleRecipients((LPCTSTR)l_MailRecipients);
	l_MailMsg.m_sSubject = l_Subject;
	l_MailMsg.m_sBody = a_Text;

	l_Smtp.Connect();
	l_Smtp.SendMessage( &l_MailMsg );
	l_Smtp.Disconnect(); 
}

