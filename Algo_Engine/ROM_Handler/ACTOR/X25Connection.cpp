// X25Connection.cpp: implementation of the CX25Connection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "X25Connection.h"
#include "Actor.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX25Connection::CX25Connection(const CString& Name, const CString& AppName, const CString& Path, unsigned char Card, WORD SendChannel, WORD ReceiveChannel)
	: CX25Socket(Card, SendChannel, ReceiveChannel)
{
	m_Name				= Name;
	m_LeftOver			= "";
	m_LoggedOn			= FALSE;
	m_Connected			= FALSE;
	m_ConnectionTime	= "Not Connected";
	m_Sent				= 0;
	m_Received			= 0;
}

CX25Connection::~CX25Connection()
{

}

BOOL CX25Connection::SendMessage(CString Message)
{
	if (m_Connected)
	{
		ProcessLogEntry("Sending: " + Message);
		if(Send(Message.GetBuffer(1024)) == 1)
		{
			PostError(GetLastCommand(), GetLastError());
			Message.ReleaseBuffer();
			return FALSE;
		}
		else
		{
			m_Sent++;
			//ProcessLogEntry("Sent: " + Message);
		}
	}
	else
	{
		ProcessError("Can't Send. Not Connected To Host");
		return FALSE;
	}
	Message.ReleaseBuffer();
	return TRUE;
}

/*void CX25Connection::Close()
{
	m_Connected = FALSE;
	m_LoggedOn = FALSE;

	ProcessError("Connection Lost");
}*/

void CX25Connection::SetLogonStatus(BOOL NewStatus, BOOL bReport)
{
	m_LoggedOn = NewStatus;

	if(bReport)
	{
		if (m_LoggedOn)
			ProcessStatus("Logged On To Host");
		else
			ProcessError("NOT Logged On To Host");
	}
}

void CX25Connection::ConnectToHost(BOOL bReport)
{
	BOOL bUpdate;
	if (CheckLink()==1)
	{	
		bUpdate = !m_Connected;
		SetLogonStatus(m_Connected = TRUE);
		m_ConnectionTime = (CTime::GetCurrentTime()).Format("%Y%m%d-%H:%M:%S");
		ProcessStatus("Connected To Host");
	}
	else	
	{
		bUpdate = m_Connected;
		SetLogonStatus(m_Connected = FALSE, bReport);
		if(bReport)
		{
			ProcessError("ERROR Connecting To Host");
			PostError(GetLastCommand(), GetLastError());
		}
	}
	if(bUpdate)
		((CTerraNovaApp *)AfxGetApp())->UpdateHost();
}

CString CX25Connection::GetErrorText(int nCommand, int nErrorCode)
{
	switch (nCommand)
	{
	case PLACE_CALL:
		switch(nErrorCode)
		{
		case X25err_NOXFER:
			return("The link is not currently in the ABM and no transfer of X.25 packets is possible.");
		case X25err_NOCHANNEL:
			return("The call packet was not transmitted as there was no logical channel available on which this outgoing call could be placed.");
		case X25err_LONGARG:
			return("The call arguments are too long.");
		case X25err_USEDBUF:
			return("The call was not placed as the asynchronous buffer was in use.");
		case X25err_DBIT:
			return("The D-bit was set in the Call packet, but D-bit pragmatics are not supported according to the X25_config_options configuration parameter.");
		case X25err_UNSUPPORTED:
			return("A facility included in the Call packet is not supported according to the gen_facilities_supported configuration parameter.");
		case X25err_NOADDR:
			return("The call arguments are erroneous as no destination address was included");
		case X25err_ODD:
			return("An odd number of bytes was included in the User Data Fields.  This field is reformatted in BCD, and therefore an even number of characters must be processed.");
		}
	case X25_WRITE:
	case X25_READ:
		switch(nErrorCode)
		{
		case X25err_NOXFER:
			return("The link is not currently in the ABM and data transfer is not possible");
		case X25err_NOCHANNEL:
			return("The selected logical channel is invalid.");
		case X25err_CHNLNOXFER:
			return("The selected logical channel is not in the data transfer state and therefore no data may be transfered.");
		case X25err_LONGDATA:
			return("The length of the data message to be transmitted exceeded the maximum permitted data length for that logical channel.  The data was not transmitted.");
		case X25err_USEDBUF:
			return("The data was not queued due to the fact that\n\tA: The packet window for the logical channel window is closed, or\n\tB: All the transmit buffers are currently occupied.");
		}
	default:
		return("Unknown X25 Error.");
	}
}

void CX25Connection::PostError(int Command, int Error)
{
	if(!Error)
		return;
	ProcessError(GetErrorText(Command, Error));
	m_Connected = FALSE;
}