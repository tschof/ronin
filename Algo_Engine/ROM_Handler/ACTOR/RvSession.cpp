#include "stdafx.h"
#include "rvcpp.h"
#include "rvevmwin.h"
#include "RvSession.h"
#include "MessageApp.h"

#pragma warning(disable:4786)

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// session management

CRvSession::CRvSession()
{
	rv_Error			RvError;
	CMessageApp		*pApp = (CMessageApp *)AfxGetApp();
	CRvHandler		*pHandler;
	CString			FileName;

	// init
		m_pSession	= NULL;
		m_pSender	= NULL;
		FileName = pApp->Path() + pApp->Name() + ".DAT";


	// load in subjects
		GetPrivateProfileString("Rv Information", "Order Subject",				"",	m_OrderSubject.GetBuffer(128),			128, FileName); m_OrderSubject.ReleaseBuffer();
		GetPrivateProfileString("Rv Information", "Cancel Subject",				"",	m_CancelSubject.GetBuffer(128),			128, FileName); m_CancelSubject.ReleaseBuffer();
		GetPrivateProfileString("Rv Information", "Status Request Subject",	"",	m_StatusRequestSubject.GetBuffer(128), 128, FileName); m_StatusRequestSubject.ReleaseBuffer();
		GetPrivateProfileString("Rv Information", "Order Status Subject",		"",	m_StatusSubject.GetBuffer(128),			128, FileName); m_StatusSubject.ReleaseBuffer();
		GetPrivateProfileString("Rv Information", "Server",						"",	m_Server,										128, FileName);
		GetPrivateProfileString("Rv Information", "Port",							"",	m_Port,											128, FileName);
				
	// Initialize Rendezvous
      if ((RvError = rv_InitWin(&m_RvEmbeddedSession, (pApp->GetMainWnd())->GetSafeHwnd(), pApp->m_hInstance, m_Port, NULL, m_Server)) == RV_OK)
			{
			if (m_pSession = new RvSession(m_RvEmbeddedSession))
				{
				if (m_pSession->status() == RV_OK)
					{
					// create sender
						m_pSender = m_pSession->newSender(m_StatusSubject);

					// create order entry data handler
						pHandler = new CRvHandler();
						pHandler->m_pHandler = new CRvOrderCallBack(this);
						pHandler->m_pListener = m_pSession->newListener(m_OrderSubject, pHandler->m_pHandler, "Order");
						m_Handlers.AddHead(pHandler);

					// create cancel data handler
					/*	pHandler = new CRvHandler();
						pHandler->m_pHandler = new CRvCancelCallBack(this);
						pHandler->m_pListener = m_pSession->newListener(m_CancelSubject, pHandler->m_pHandler, "Cancel");
						m_Handlers.AddHead(pHandler);

					// create status request handler
						pHandler = new CRvHandler();
						pHandler->m_pHandler = new CRvStatusCallBack(this);
						pHandler->m_pListener = m_pSession->newListener(m_StatusRequestSubject, pHandler->m_pHandler, "Status Request");
						m_Handlers.AddHead(pHandler);*/
					}
				else
					((CMessageApp *)AfxGetApp())->ProcessError("RV", "Failed To Initialize RvSession");
				}
			else
				((CMessageApp *)AfxGetApp())->ProcessError("RV", "Failed To Create RvSession");
			}
		else
			((CMessageApp *)AfxGetApp())->ProcessError("RV", "rv_InitWin Failed");
}

COrderHandler *CRvSession::Handler()
{ 
return ((CMessageApp *)AfxGetApp())->Handler(); 
}

CRvSession::~CRvSession()
{
	CRvHandler	*pHandler;

	// cycle through handler list and delete each message subject handler
		while (!(m_Handlers.IsEmpty()))
			{
			if ((pHandler = (CRvHandler *)m_Handlers.RemoveHead()))
				delete pHandler;
			}

	// delete the sender
		if (m_pSender)
			delete m_pSender;

	// delete the session which will in turn close down listeners and senders
		if (m_pSession)
			delete m_pSession;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order message handling

void CRvOrderCallBack::onData(const char *Subject, RvSender *Sender, const RvDatum &Data, RvListener *Invoker)
{
	/*CString	Message;
	CString	SourceId;
	RvMsg		RvMessage;

	RvMessage = Data.data();
	RvMessage.get("ORDER", Message.GetBuffer(256), 256); Message.ReleaseBuffer();

	if (Message!= "")
		{
		((CMessageApp *)AfxGetApp())->ProcessLogEntry("RECV CLIENT", Message);
		CMessage	Message(CMessage::Rv, Message);
		COrder	Order(Message.Message());
		Order.SetOwner(Order.Trader());
		SourceId.Format("Rv,%s", Order.Trader());
		Order.SetSourceId(SourceId);
					
		Handler()->ProcessMessage(Order);
		}
	else
		((CMessageApp *)AfxGetApp())->ProcessError("RV", "Received Blank Rv Message In rvOrder::onData");*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// cancel message handling

/*void CRvCancelCallBack::onData(const char *Subject, RvSender *Sender, const RvDatum &Data, RvListener *Invoker)
{
	CString	Message;
	RvMsg		RvMessage;
	CString	SourceId;

	RvMessage = Data.data();
	RvMessage.get("ORDER", Message.GetBuffer(256), 256); Message.ReleaseBuffer();

	if (Message != "")
		{
		((CMessageApp *)AfxGetApp())->ProcessLogEntry("RECV CLIENT", Message);
		CMessage	Message(CMessage::Rv, Message);
		CCancel	Cancel(Message.Message());

		Cancel.SetOwner(Cancel.Trader());
		SourceId.Format("Rv,%s", Cancel.Trader());
		Cancel.SetSourceId(SourceId);
					
		Handler()->ProcessMessage(Cancel);
		}
	else 
		((CMessageApp *)AfxGetApp())->ProcessError("RV", "Received Blank Rv Message In rvCancel::onData");
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// status message handling

/*void CRvStatusCallBack::onData(const char *Subject, RvSender *Sender, const RvDatum &Data, RvListener *Invoker)
{
	CString Message;
	RvMsg		RvMessage;
	CString	SourceId;

	RvMessage = Data.data();
	RvMessage.get("ORDER", Message.GetBuffer(256), 256); Message.ReleaseBuffer();

	if (Message != "")
		{
		((CMessageApp *)AfxGetApp())->ProcessLogEntry("RECV CLIENT", Message);
		CRequest Request(Message);

		Request.SetOwner(Request.Trader());
		SourceId.Format("Rv,%s", Request.Trader());
		Request.SetSourceId(SourceId);

		Handler()->ProcessMessage(Request);
		}
	else
		((CMessageApp *)AfxGetApp())->ProcessError("RV", "Received Blank Rv Message In rvStatusRequest::onData");
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sending functions

int CRvSession::ProcessStatusReport(const char *Status, const CString& StaffordTag)
{
	CString	Error;
	rv_Error Result;
	CString	Subject;

	if (StaffordTag != "")
		 Subject = (m_StatusSubject + "." + StaffordTag);
	else 
		Subject = (m_StatusSubject);
	
	if ((Result = rv_Send(m_RvEmbeddedSession, Subject.GetBuffer(Subject.GetLength()), RVMSG_STRING, 0, (void *)Status)) != RV_OK)
		{
		Error.Format("Error Sending To RV (%d)", Result);
		((CMessageApp *)AfxGetApp())->ProcessError("RV SEND", Error);
		}

	Subject.ReleaseBuffer();

return(0);
}

int CRvSession::ProcessHeartBeat(const char *Status)
{
	CString	Error;
	rv_Error Result;
	
	CString	Subject = (m_StatusSubject);
	
	if ((Result = rv_Send(m_RvEmbeddedSession, Subject.GetBuffer(Subject.GetLength()), RVMSG_STRING, 0, (void *)Status)) != RV_OK)
		{
		Error.Format("Error Sending To RV (%d)", Result);
		((CMessageApp *)AfxGetApp())->ProcessError("RV SEND", Error);
		}

	Subject.ReleaseBuffer();

return(0);
}

