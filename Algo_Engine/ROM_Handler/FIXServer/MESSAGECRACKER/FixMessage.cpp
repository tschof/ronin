// FixMessage.cpp: implementation of the FixMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OMMessageCracker.h"
#include "FixMessage.h"
#include <sstream>
#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FixTag::FixTag()
: m_nTagID(-1)
, m_szTagName("Custom")
, m_bRequired(false)
, m_szDesc("Not available")
{

}

FixTag::~FixTag()
{

}

FixTag::FixTag( const std::string & msg )
{
	std::string szMsg = msg;
	int nIndex = szMsg.find("<>");
	m_nTagID = atoi( szMsg.substr(0,nIndex).c_str() );
	szMsg = szMsg.substr( nIndex+2, -1 );
	nIndex = szMsg.find("<>");
	m_szTagName = szMsg.substr(0,nIndex);
	szMsg = szMsg.substr( nIndex+2, -1 );
	nIndex = szMsg.find("<>");
	m_bRequired = szMsg.substr(0,nIndex) == "Y" ? true:false;
	m_szDesc = szMsg.substr( nIndex+2, -1 );
}

FixTag::FixTag( const FixTag & tag )
{
	*this = tag;
}

FixTag & FixTag::operator=( const FixTag & tag )
{
	m_nTagID	= tag.m_nTagID;
	m_szTagName	= tag.m_szTagName;
	m_bRequired	= tag.m_bRequired;
	m_szDesc	= tag.m_szDesc;
	return *this;
}

FixMessage::FixMessage() : m_szType("")
{

}

FixMessage::~FixMessage()
{

}

void FixMessage::AddTag( FixTag & tag )
{
	m_TagMap.insert( std::make_pair<int,FixTag>( tag.getTagID(), tag ) );
}

FixTag FixMessage::FindTag( int nTagID )
{
	tTagMap::iterator it = m_TagMap.find( nTagID );
	if( it == m_TagMap.end() )
		throw exception("not found");
	return (*it).second;
}

FixMessage::FixMessage( const std::string & szMsg )
{
	int nIndex = szMsg.find("<M>");
	std::string msg = szMsg.substr(nIndex+3, -1);
	nIndex = msg.find("\n");
	m_szName = msg.substr( 0, nIndex );
	msg = msg.substr( nIndex+1, -1 );
	nIndex = m_szName.find("MsgType =");
	if( nIndex != -1 )
		m_szType = m_szName.substr( nIndex+10, 1 );

	nIndex = msg.find("<T>");
	m_szDesc = msg.substr( 0, nIndex );
	msg = msg.substr( nIndex+4, -1 );
	nIndex = msg.find("<Me>");
	msg = msg.substr( 0, nIndex );
	// line feed 13, enter 10
	while( ( nIndex = msg.find(13) ) != -1 )
	{
		FixTag tag(msg.substr(0,nIndex));
		AddTag(tag);
		msg = msg.substr(nIndex+1,-1);
	}
}
