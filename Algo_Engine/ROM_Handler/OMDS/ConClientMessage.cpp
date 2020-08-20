// ConClientMessage.cpp: implementation of the CConClientMessage class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	The std::string messages are wrapped in this object.  the CConClientMessage
//	object also contains the clients Id, so messages to and from a client
//	can be identified and properly routed

#include "stdafx.h"
#include "ConClientMessage.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConClientMessage::CConClientMessage()
{

}

CConClientMessage::~CConClientMessage()
{

}

CConClientMessage::CConClientMessage( const CConClientMessage & msg )
{
	*this = msg;
}

CConClientMessage & CConClientMessage::operator=( const CConClientMessage & msg )
{
	m_clientId = msg.m_clientId;
	assert( !msg.message.empty() );
	message = msg.message;
	message.begin(); // force copy on write
	return *this;
}
