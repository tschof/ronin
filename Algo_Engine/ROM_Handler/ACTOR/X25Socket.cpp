#include "stdafx.h"
#include "X25Socket.h"

char CX25Socket::call_arguments[] = "-d12345678 -s98765432 -f0101";

CX25Socket::CX25Socket(unsigned char Card, WORD SendChannel, WORD ReceiveChannel)
	: m_Card(Card)
{
	//SEND_mailbox_ptr	= (MAILBOX_INTERFACE_STRUCT*)SEND_MAILBOX_ADDRESS;
	//RECIEVE_mailbox_ptr = (MAILBOX_INTERFACE_STRUCT*)RECIEVE_MAILBOX_ADDRESS;
	//PlaceCall();
	CheckActiveChannels();
	SetSendChannel(SendChannel);
	SetReceiveChannel(ReceiveChannel);

}

CX25Socket::~CX25Socket()
{
	
}

CX25Socket::PlaceCall()
{
	int buffer_len = strlen(call_arguments);
	int rValue;

	SEND_mailbox.command = PLACE_CALL;
	SEND_mailbox.Q_D_M_bits = 0x00;
	memcpy(SEND_mailbox.data, call_arguments, SEND_mailbox.buffer_length = buffer_len);
	rValue = ExecuteCommand(&SEND_mailbox);
	
	if(!rValue)
		TRACE("X25Socket: Call placed on logical channel = %d\n", SEND_mailbox.logical_channel);
	return rValue;
}

CX25Socket::WaitForCallResults()
{
	do
	{
		SEND_mailbox.command = IS_DATA_AVAILABLE;
		SEND_mailbox.buffer_length = 0x00;

	} while(ExecuteCommand(&SEND_mailbox) != ASY_PKT_RECIEVED);

	TRACE("X25Socket: X.25 asynchronous packet recieved on logical channel %d\n", SEND_mailbox.logical_channel);
	TRACE("X25Socket: X.25 packet type (%x): ", SEND_mailbox.X25_packet_type & 0x7F);

	switch(SEND_mailbox.X25_packet_type & 0x7f)
	{
	case (0x02):
		TRACE("X25Socket: Clear Request");
		TRACE("X25Socket: X.25 cause field = %x", SEND_mailbox.cause);
		TRACE("X25Socket: X.25 diagnostic field = %x\n", SEND_mailbox.diagnostic);
		break;
	
	case (0x31):
		TRACE("X25Socket: Call Accept\n");
		break;

	default:
		TRACE("X25Socket: See X.25 manual\n");
	}

	if(SEND_mailbox.buffer_length)
	{
		TRACE("Data associated with this incomming packet: ");
		for(int i = 0; i < SEND_mailbox.buffer_length; i++)
			TRACE("%c", SEND_mailbox.data[i]);
		TRACE("\n");
	}
	return 1;
}

CX25Socket::CheckLink()
{
	int rValue;
	SEND_mailbox.command = LINK_STATUS;
	SEND_mailbox.buffer_length = 0;
	rValue = ExecuteCommand(&SEND_mailbox);
	return rValue;
}

CX25Socket::Disconnect()
{
	int rValue;
	SEND_mailbox.command = LINK_DISCONNECT;
	SEND_mailbox.buffer_length = 0x01;
	*SEND_mailbox.data = 0x01;
	rValue = ExecuteCommand(&SEND_mailbox);
	return rValue;
}

CX25Socket::CheckActiveChannels()
{
	int rValue;
	do
	{
		SEND_mailbox.command = READ_ACTIVE_CHANNELS;
		SEND_mailbox.buffer_length = 0x00;
	} while(rValue = ExecuteCommand(&SEND_mailbox));

	TRACE("%u logical channels are active\n", SEND_mailbox.buffer_length / 2);

	if(SEND_mailbox.buffer_length)
	{
		TRACE("These are: ");
		for(int i = 0; i < SEND_mailbox.buffer_length; i++)
			TRACE("%u ", *&SEND_mailbox.data[i++]);
		TRACE("\n");
	}
	return rValue;
}

BOOL CX25Socket::IsChannelActive(WORD Channel)
{
	do
	{
		SEND_mailbox.command = READ_ACTIVE_CHANNELS;
		SEND_mailbox.buffer_length = 0x00;
	} while(ExecuteCommand(&SEND_mailbox));

	if(SEND_mailbox.buffer_length)
	{
		for(int i = 0; i < SEND_mailbox.buffer_length; i++)
			if(*&SEND_mailbox.data[i++] == Channel)
				return TRUE;
	}
	return FALSE;
}

BOOL CX25Socket::SetSendChannel(WORD Channel)
{
	if(IsChannelActive(Channel))
	{
		m_SendChannel = SEND_mailbox.logical_channel = Channel;
		TRACE("Logical SEND Channel Set To %u\n", Channel);
		SEND_mailbox.command = RESET_LOGICAL_CHANNEL;
		SEND_mailbox.Q_D_M_bits = 0x00;
		ExecuteCommand(&SEND_mailbox);
		return TRUE;
	}
	TRACE("Unable To Set SEND Channel %u\n", Channel);
	return FALSE;
}

BOOL CX25Socket::SetReceiveChannel(WORD Channel)
{
	if(IsChannelActive(Channel))
	{
		m_ReceiveChannel = RECEIVE_mailbox.logical_channel = Channel;
		TRACE("Logical RECEIVE Channel Set To %u\n", Channel);
		RECEIVE_mailbox.command = RESET_LOGICAL_CHANNEL;
		RECEIVE_mailbox.Q_D_M_bits = 0x00;
		ExecuteCommand(&RECEIVE_mailbox);
		return TRUE;
	}
	TRACE("Unable To Set RECEIVE Channel %u\n", Channel);
	return FALSE;
}

CX25Socket::Send(char *send_data)
{
	int rValue;
	int buffer_len = strlen(send_data);

	//while(!XAPIqClearToXmit(m_SendChannel, m_Card));
	
	//CheckActiveChannels();
	SEND_mailbox.command = X25_WRITE;
	//SEND_mailbox.logical_channel = *(unsigned *)&SEND_mailbox.data[0x00];
	SEND_mailbox.Q_D_M_bits = 0x00;
	SEND_mailbox.logical_channel = m_SendChannel;
	memcpy(SEND_mailbox.data, send_data, buffer_len);
	SEND_mailbox.buffer_length = buffer_len;
	if((rValue = ExecuteCommand(&SEND_mailbox)))
		TRACE("Data packet not sent ( return code = %x)\n", rValue);
//	else
//		TRACE("Data successfully sent\n");
		
	return rValue;
}

CX25Socket::Recieve(char *recieve_data)
{
	int rValue;
	RECEIVE_mailbox.command = IS_DATA_AVAILABLE;
	RECEIVE_mailbox.buffer_length = 0x00;
	if(!(rValue = ExecuteCommand(&RECEIVE_mailbox)))
	{
		//TRACE("%d logical channels have incomming data vailable\n",
		//	RECEIVE_mailbox.buffer_length / 2);
		//TRACE("\nThese are: ");
		//for(int i = 0; i < RECEIVE_mailbox.buffer_length; i++)
		//	TRACE("%u ", *&RECEIVE_mailbox.data[i++]);
		//TRACE("\n");

		//unsigned logical_channel_in_use = *(unsigned *)&SEND_mailbox.data[0x00];
		
		RECEIVE_mailbox.command = X25_READ;
		RECEIVE_mailbox.buffer_length = 0x00;
		RECEIVE_mailbox.logical_channel = m_ReceiveChannel;
		//RECEIVE_mailbox.logical_channel = logical_channel_in_use;
		if(!(rValue = ExecuteCommand(&RECEIVE_mailbox)))
		{
			//TRACE("%d bytes of data recieved on logical channel %d\n", RECEIVE_mailbox.buffer_length, RECEIVE_mailbox.logical_channel);
			//TRACE("Data: ");
			for(int i = 0; i < RECEIVE_mailbox.buffer_length; i++)
				*(recieve_data + i) = RECEIVE_mailbox.data[i];
			//TRACE("\n");
		}
	}
	return rValue;
}

CX25Socket::ExecuteCommand(CBLOCK *mailbox_ptr)
{
	int rValue;
	SetLastCommand(mailbox_ptr->command);
	rValue = XAPIControl(mailbox_ptr, m_Card);
	SetLastError(rValue);
	return rValue;
}

void CX25Socket::SetLastError(int Error)
{
	m_LastError = Error;
}

int CX25Socket::GetLastError()
{
	return m_LastError;
}

void CX25Socket::SetLastCommand(int Command)
{
	m_LastCommand = Command;
}

int CX25Socket::GetLastCommand()
{
	return m_LastCommand;
}