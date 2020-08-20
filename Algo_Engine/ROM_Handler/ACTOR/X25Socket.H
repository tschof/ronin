#if !defined(X25_SOCKET)
#define X25_SOCKET

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
//#include <mem.h>
#include <dos.h>

#include "..\Sangoma X.25\WIN32\XAPI_M.H"    // include typedef's and function prototypes for XAPI
#include "..\Sangoma X.25\WIN32\XAPI32MC.H"  // 32-bit library

// XIP commands
#define ASY_PKT_RECIEVED		0x40	// XIP return code for an asynchronous
										// packet recieved

//#define SEND_MAILBOX_ADDRESS	0xD00016B0
//#define RECIEVE_MAILBOX_ADDRESS	0xD0001AD0

// for opp_flag use 
#define RESET					0x00
#define SET						0x01
#define ZERO					0x00

#define X25err_NOXFER			0x02
#define X25err_MODEM			0x08
#define X25err_NOCHANNEL		0x30
#define X25err_LONGARG			0x32
#define X25err_USEDBUF			0x33
#define X25err_DBIT				0x38
#define X25err_UNSUPPORTED		0x39
#define X25err_NOADDR			0x3A
#define X25err_ODD				0x3B
#define X25err_CHNLNOXFER		0x31
#define X25err_LONGDATA			0x32
#define X25err_USEDBUF			0x33

class CX25Socket
{
public:
	CX25Socket(unsigned char Card = 0, WORD SendChannel = 0, WORD ReceiveChannel = 0);
	~CX25Socket();

	PlaceCall();
	WaitForCallResults();
	CheckLink();
	Send(char *);
	Recieve(char *);
	Disconnect();
	GetLastError();
	GetLastCommand();
	SetSendChannel(WORD Channel);
	SetReceiveChannel(WORD Channel);
	WORD GetSendChannel()		{ return SEND_mailbox.logical_channel;	  }
	WORD GetReceieveChannel()	{ return RECEIVE_mailbox.logical_channel; }
	IsChannelActive(WORD Channel);
private:
	CheckActiveChannels();
	ExecuteCommand(CBLOCK*);
	void SetLastError(int);
	void SetLastCommand(int);
private:
	CBLOCK SEND_mailbox;
	CBLOCK RECEIVE_mailbox;
	static char call_arguments[];

	int m_LastError;
	int m_LastCommand;
	unsigned char m_Card;

	WORD m_SendChannel;
	WORD m_ReceiveChannel;
};

#endif // !defined(X25_SOCKET)