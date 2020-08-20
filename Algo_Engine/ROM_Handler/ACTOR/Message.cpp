// Message.cpp: implementation of the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Message.h"
//#include "Order.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessage::CMessage(int Source, CString& Message)
{
	// set source
		m_Source = Source;
		m_pMessage = NULL;

	switch (Message.GetAt(0))
		{
		case Logon:
			m_Type = Logon;
			m_pMessage = new CLogin(Message);
			break;
		case Logout:
			m_Type = Logout;
			m_pMessage = new CLogout(Message);
			break;
		case Order:
			m_Type = Order;
			m_pMessage = new CCsv(Message);
			break;

		default:
			break;
		}

	
}

CMessage::CMessage(int Source)
{
	// set source
		m_Source = Source;
		m_pMessage = NULL;
}

CMessage::~CMessage()
{
	if (m_pMessage) delete m_pMessage;
}
