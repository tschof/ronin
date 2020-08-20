// Msg.h: interface for the CMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSG_H__D17B3D96_E37B_11D5_A623_00500401F87A__INCLUDED_)
#define AFX_MSG_H__D17B3D96_E37B_11D5_A623_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LOGONCMD			"LGQ"
#define LOGONRCMD			"LGR"
#define HEARTBEATCMD		"HBQ"
#define HEARTBEATRCMD		"HBR"
#define FLOWCONTROLCMD		"FLO"
#define CHANELLSTATECMD		"LCQ"
#define CHANELLSTATERCMD	"LCR"
#define ACTCMD				"CMS"

class CMsg  
{
public:

	enum MsgType
	{
		LOGON,
		LOGONRESPONSE,
		HEARTBEAT,
		HEARTBEATRESPONSE,
		FLOWCONTROL,
		CHANNELSTATE,
		CHANNELSTATERESPONSE,
		ACT
	};

	enum MSG_INFO
	{
		OTHER_LEN = 13,
		LOGIN_LEN = 92,
		HEARTBEAT_LEN = 28,
		FLOWCONTROL_LEN = 28,
		CHANNELQUERY_LEN = 28,
		LOGINR_LEN = 82,
		COMMAND_LEN = 3,
		TIMESTAMP_LEN = 8,
		VERSION_LEN = 2,
		LENGHTH_LEN = 2,
		CHANNEL_LEN = 1,
		LOGINID_LEN = 10,
		CHANNELS_LEN = 64,
		TAIL_LEN = 2
	};

	CMsg();
	CMsg(const char *pActBody);
	//CMsg(CCsv* pActBody);
	CMsg(u_short Type, BYTE Channel = 0);

	virtual ~CMsg();
	
	//u_short GetType();
	u_short Size() { return m_Size; }
	char* Message();
	char* ASCIIMessage();
	
	void GetActBody(CString *Body);

	static BYTE GetActiveOutChannel(); 
	static void UpdateInChannels(BYTE *pChannels);
	static int GetType(const char *pSource);
	static BYTE GetChannel(const char *pSource);
	static void GetActMsg(char* pSource, char* pMessage);
	static void GetAct(char* pAct, const char* pMessage, u_short Size);
	static void Init(char *pVersion, char* pLogonID);
	static void FlowControlMsg(const char *Msg, CString &rMsg);

private:
	u_short m_Size;
	//char m_TimeStamp[9];
	char m_Type[4];
	char m_Command[4];
	char m_Message[1050];

	static char *m_pTail;
	static BYTE m_InChannel[64];  // our channel used by Nasdaq
	static BYTE m_OutChannel[64]; // Nasdaq's channel used by us
	static char m_LogonID[11];
	static char m_Version[3];

	void SetLogon();
	void SetHeartBeat();
	void SetChannelStateQuery();
	void SetChannelStateResponse(BYTE Channel);
	void SetAct(const char *pActBody, u_short ActSize);

	void ProcessLogonResponse(char* Message);

	void GetCurrentTimeStamp(char *pTimeStamp);
	void AssembleMessage();
};

#endif // !defined(AFX_MSG_H__D17B3D96_E37B_11D5_A623_00500401F87A__INCLUDED_)
