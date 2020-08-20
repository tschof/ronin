// Msg.cpp: implementation of the CMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor.h"
#include "Msg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char* CMsg::m_pTail;
BYTE CMsg::m_InChannel[64];
BYTE CMsg::m_OutChannel[64];
char CMsg::m_LogonID[11];
char CMsg::m_Version[3];

#define LOGONCMD			"LGQ"
#define LOGONRCMD			"LGR"
#define HEARTBEATCMD		"HBQ"
#define HEARTBEATRCMD		"HBR"
#define FLOWCONTROLCMD		"FLO"
#define CHANELLSTATECMD		"LCQ"
#define CHANELLSTATERCMD	"LCR"
#define ACTCMD				"CMS"

CMsg::CMsg()
{

}


CMsg::CMsg(u_short Type, BYTE Channel)
{
	switch (Type)
	{
	case LOGON:
		strcpy(m_Type, LOGONCMD);
		SetLogon();
		break;
	case HEARTBEAT:
		strcpy(m_Type, HEARTBEATCMD);
		SetHeartBeat();
		break;
	case CHANNELSTATERESPONSE:
		strcpy(m_Type, CHANELLSTATERCMD);
		SetChannelStateResponse(Channel);
		break;
	default:
		break;
	}
}


CMsg::CMsg(const char* pActBody)
{
	strcpy(m_Type, ACTCMD);
	//CAct Act(pActBody);
	//CString s = Act.Send();
	SetAct(pActBody, strlen(pActBody));
}

CMsg::~CMsg()
{

}

void CMsg::Init(char* pVersion, char* pLogonID)
{
	m_pTail = "UU";

	//memset(m_LogonID, '\0', 10);
	strcpy(m_LogonID, pLogonID);
	strcpy(m_Version, pVersion);

	for (int i = 0; i < 64; i++)
	{
		if (i == 0 || i == 1)
		{
			m_InChannel[i] = TRUE;
		}
		else
		{
			m_InChannel[i] = FALSE;
		}

		m_OutChannel[i] = FALSE;
	}
}

/*
u_short CMsg::GetType()
{
	if (strcmp(m_Type, LOGONCMD) == 0)
	{
		return LOGON;
	}
	else if (strcmp(m_Type, LOGONRCMD) == 0)
	{
		return LOGONRESPONSE;
	}
	else if (strcmp(m_Type, HEARTBEATCMD) == 0)
	{
		return HEARTBEAT;
	}
	else if (strcmp(m_Type, HEARTBEATRCMD) == 0)
	{
		return HEARTBEATRESPONSE;
	}
	else if (strcmp(m_Type, FLOWCONTROLCMD) == 0)
	{
		return FLOWCONTROL;
	}
	else if (strcmp(m_Type, CHANELLSTATECMD) == 0)
	{
		return CHANNELSTATE;
	}
	else if (strcmp(m_Type, CHANELLSTATERCMD) == 0)
	{
		return CHANNELSTATERESPONSE;
	}
	else
	{
		return ACT;
	}
}
*/
void CMsg::GetCurrentTimeStamp(char* pTimeStamp)
{
	CTime tm = CTime::GetCurrentTime( );
	strcpy(pTimeStamp, tm.Format("%H%M%S"));
	strcat(pTimeStamp, "00");
}

BYTE CMsg::GetActiveOutChannel() 
{
	BYTE rc = 0;

	for ( int i = 1; i < 64; i++)
	{
		if (m_OutChannel[i] == 1)
		{
			rc = i;
			break;
		}
	}

	return rc;
} 

char* CMsg::Message()
{
	char TimeStamp[TIMESTAMP_LEN + 1];
	GetCurrentTimeStamp(TimeStamp);

	BYTE OutChannel = 0;
	if (strcmp(m_Type, ACTCMD) == 0)
	{
		OutChannel = GetActiveOutChannel();
	}

	char* p = m_Message + LENGHTH_LEN + VERSION_LEN;
	memcpy(p, TimeStamp, TIMESTAMP_LEN);  p += TIMESTAMP_LEN;
	memcpy(p, &OutChannel, CHANNEL_LEN);
	return m_Message;
}

char* CMsg::ASCIIMessage()
{
	return NULL;
}


void CMsg::SetLogon()
{
	m_Size = LOGIN_LEN;
	strcpy(m_Command, LOGONCMD);
	char *p = m_Message;
	
	memcpy(p, &m_Size, LENGHTH_LEN);	p += LENGHTH_LEN;
	memcpy(p, m_Version, VERSION_LEN); p += VERSION_LEN + TIMESTAMP_LEN + CHANNEL_LEN;
	memcpy(p, m_Command, COMMAND_LEN); p += COMMAND_LEN;
	memcpy(p, m_LogonID, LOGINID_LEN); p += LOGINID_LEN;

	for (int i = 0; i < 64; i++)
	{
		memcpy(p, &(m_InChannel[i]), 1);
		p++;
	}

	//memcpy(p, &(m_InChannel[0]), CHANNELS_LEN); p += CHANNELS_LEN;
	memcpy(p, m_pTail, TAIL_LEN);
}

void CMsg::SetHeartBeat()
{
	m_Size = HEARTBEAT_LEN;
	strcpy(m_Command, HEARTBEATCMD);
	char *p = m_Message;
	char Dumy[11];
	
	memset(Dumy, '\0', 10);
	memcpy(p, &m_Size, LENGHTH_LEN);	p += LENGHTH_LEN;
	memcpy(p, m_Version, VERSION_LEN); p += VERSION_LEN + TIMESTAMP_LEN + CHANNEL_LEN;
	memcpy(p, m_Command, COMMAND_LEN); p += COMMAND_LEN;
	memcpy(p, Dumy, 10); p += 10;
	memcpy(p, m_pTail, TAIL_LEN);
}

void CMsg::SetChannelStateQuery()
{
}

void CMsg::GetAct(char* pAct, const char* pMessage, u_short Size)
{
	int BodyOffset = LENGHTH_LEN + 
					 VERSION_LEN + 
					 TIMESTAMP_LEN + 
					 CHANNEL_LEN + 
					 COMMAND_LEN;
	const char* p = pMessage + BodyOffset;

	memcpy(pAct, p, Size - BodyOffset - 2);
	pAct[Size - BodyOffset - 2] = '\0';
}

void CMsg::SetChannelStateResponse(BYTE Channel)
{
	m_Size = CHANNELQUERY_LEN;
	strcpy(m_Command, CHANELLSTATERCMD);
	char *p = m_Message;
	char Dumy[11];
	
	memset(Dumy, '\0', 10);
	memcpy(p, &m_Size, LENGHTH_LEN);	p += LENGHTH_LEN;
	memcpy(p, m_Version, VERSION_LEN); p += VERSION_LEN + TIMESTAMP_LEN + CHANNEL_LEN;
	memcpy(p, m_Command, COMMAND_LEN); p += COMMAND_LEN;
	memcpy(p, &Channel, CHANNEL_LEN); p += CHANNEL_LEN;
	memcpy(p, &(m_InChannel[Channel]), CHANNEL_LEN); p += CHANNEL_LEN;
	memcpy(p, Dumy, 8); p += 8;
	memcpy(p, m_pTail, TAIL_LEN);
}

void CMsg::SetAct(const char *pActBody, u_short ActSize)
{
	m_Size = 18 + ActSize;
	strcpy(m_Command, ACTCMD);
	char *p = m_Message;

	memcpy(p, &m_Size, LENGHTH_LEN);	p += LENGHTH_LEN;
	memcpy(p, m_Version, VERSION_LEN); p += VERSION_LEN + TIMESTAMP_LEN + CHANNEL_LEN;
	memcpy(p, m_Command, COMMAND_LEN); p += COMMAND_LEN;
	memcpy(p, pActBody, ActSize); p += ActSize;
	memcpy(p, m_pTail, TAIL_LEN);
}



BYTE CMsg::GetChannel(const char *pSource)
{
	const char* p =	pSource + 
				LENGHTH_LEN + 
				VERSION_LEN + 
				TIMESTAMP_LEN + 
				CHANNEL_LEN + 
				COMMAND_LEN;

	BYTE Channel;
	memcpy(&Channel, p, CHANNEL_LEN);

	return Channel;
}


int CMsg::GetType(const char *pSource)
{
	const char* p = pSource + LENGHTH_LEN + VERSION_LEN + TIMESTAMP_LEN + CHANNEL_LEN;

	if (strncmp(p, HEARTBEATCMD, COMMAND_LEN) == 0)
	{
		return HEARTBEAT;
	}
	else if (strncmp(p, LOGONRCMD, COMMAND_LEN) == 0)
	{
		p += COMMAND_LEN;
		memcpy(&(m_OutChannel[0]), p, 64);
		return LOGONRESPONSE;
	}
	else if (strncmp(p, HEARTBEATRCMD, COMMAND_LEN) == 0)
	{
		return HEARTBEATRESPONSE;
	}
	else if (strncmp(p, CHANELLSTATECMD, COMMAND_LEN) == 0)
	{
		return CHANNELSTATE;
	}
	else if (strncmp(p, CHANELLSTATERCMD, COMMAND_LEN) == 0)
	{
		return CHANNELSTATERESPONSE;
	}
	else if (strncmp(p, FLOWCONTROLCMD, COMMAND_LEN) == 0)
	{
		BYTE Channel, State;
		p += COMMAND_LEN;

		memcpy(&Channel, p, CHANNEL_LEN); p += CHANNEL_LEN;
		memcpy(&State, p, CHANNEL_LEN);
		m_OutChannel[Channel] = State;

		return FLOWCONTROL;
	}
	else
	{
		return ACT;
	}
}


void CMsg::GetActMsg(char* pSource, char* pMessage)
{
	u_short Size;
	char* p =	pSource + 
				LENGHTH_LEN + 
				VERSION_LEN + 
				TIMESTAMP_LEN + 
				CHANNEL_LEN + 
				COMMAND_LEN;

	memcpy(&Size, pSource, LENGHTH_LEN);
	memcpy(pMessage, p, Size);
}


void CMsg::GetActBody(CString *Body)
{
	char ActBody[1024];
	int Offset = LENGHTH_LEN + 
				 VERSION_LEN + 
				 TIMESTAMP_LEN + 
				 CHANNEL_LEN + 
				 COMMAND_LEN;
	char *p = m_Message + Offset;
	int size = m_Size - Offset - 2;

	strncpy(ActBody, p, size);
	ActBody[size] = '\0';
	*Body = ActBody;
}

void CMsg::FlowControlMsg(const char *Msg, CString &rMsg)
{
	BYTE Channel, State;
	const char* p = Msg + LENGHTH_LEN + VERSION_LEN + TIMESTAMP_LEN + CHANNEL_LEN + COMMAND_LEN;

	memcpy(&Channel, p, CHANNEL_LEN); p += CHANNEL_LEN;
	memcpy(&State, p, CHANNEL_LEN);

	int iState = (int)State;
	int iChannel = (int)Channel;

	if (iState == 1)
	{
		rMsg.Format("Channel %s is unblocked", iChannel);
	}
	else
	{
		rMsg.Format("Channel %s is blocked", iChannel);
	}
}