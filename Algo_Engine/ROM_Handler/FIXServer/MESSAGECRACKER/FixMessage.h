// FixMessage.h: interface for the FixMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIXMESSAGE_H__AB3C9F80_F4C8_4376_AE08_D3A3BCBE48DD__INCLUDED_)
#define AFX_FIXMESSAGE_H__AB3C9F80_F4C8_4376_AE08_D3A3BCBE48DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>

class FixTag
{
public:
	FixTag();
	virtual ~FixTag();
	
	FixTag( const std::string & msg );
	FixTag( const FixTag & tag );
	FixTag & operator=( const FixTag & tag );

	void setTagID	( int nTagID )						{ m_nTagID = nTagID; }
	void setTagName	( const std::string & szTagName )	{ m_szTagName = szTagName; }
	void setRequired( bool bRequired )					{ m_bRequired = bRequired; }
	void setDesc	( const std::string & szDesc )		{ m_szDesc = szDesc; }
	
	int			getTagID	()		{ return m_nTagID; }
	std::string	getTagName	()		{ return m_szTagName; }
	bool		getRequired	()		{ return m_bRequired; }
	std::string getDesc		()		{ return m_szDesc; }

private:
	int			m_nTagID;
	std::string m_szTagName;
	bool		m_bRequired;
	std::string m_szDesc;
};

class FixMessage  
{
public:
	typedef std::map<int,FixTag> tTagMap;

	FixMessage();
	FixMessage( const std::string & szMsg );
	virtual ~FixMessage();

	void setName( const std::string & szName ) { m_szName = szName; }
	void setDesc( const std::string & szDesc ) { m_szDesc = szDesc; }

	std::string getName() { return m_szName; }
	std::string getDesc() { return m_szDesc; }
	std::string getType() { return m_szType; }

	void	AddTag	( FixTag & tag );
	FixTag	FindTag	( int nTagID );

private:
	std::string m_szName;
	std::string m_szDesc;
	tTagMap		m_TagMap;
	std::string	m_szType;

};

#endif // !defined(AFX_FIXMESSAGE_H__AB3C9F80_F4C8_4376_AE08_D3A3BCBE48DD__INCLUDED_)
