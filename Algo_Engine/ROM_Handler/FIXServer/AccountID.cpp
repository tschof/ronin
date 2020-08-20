// AccountID.cpp: implementation of the AccountID class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AccountID.h"
#include <TCHAR.H>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AccountID::AccountID(const std::string & szTraderAcr, const std::string & szLocalAcr, const std::string & szClearingAcID, const std::string & szFirm, const std::string & szType )
: m_szTraderAcr(szTraderAcr), m_szLocalAcAcr(szLocalAcr),m_szClearingAcID(szClearingAcID),m_szFirmAcr(szFirm),m_szType(szType)
{
 	m_szKey = m_szTraderAcr+" "+m_szClearingAcID+" "+m_szFirmAcr+" "+m_szLocalAcAcr+" "+m_szType;
}

AccountID::AccountID( const std::string & szKey, const std::string & szType ) 
: m_szTraderAcr(""),m_szLocalAcAcr(""),m_szClearingAcID(""),m_szFirmAcr(""), m_szType("")
{
	m_szKey = szKey + " " + szType;	
}

AccountID::~AccountID()
{

}

AccountID::AccountID( const AccountID & id )
{
	*this = id;
}

AccountID & AccountID::operator=( const AccountID & id )
{
	m_szTraderAcr		= id.m_szTraderAcr;
	m_szLocalAcAcr		= id.m_szLocalAcAcr;
	m_szClearingAcID	= id.m_szClearingAcID;
	m_szFirmAcr			= id.m_szFirmAcr;
	m_szType			= id.m_szType;
	m_szKey				= id.m_szKey;
	return *this;
}

bool AccountID::operator<( const AccountID & id ) const
{
	return m_szKey < id.m_szKey;
}

bool AccountID::operator==( const AccountID & id ) const
{
	if( m_szKey == id.m_szKey )
		return true;
	return false;
}

void AccountID::TrimRight( std::string & aString )
{
	char szBuf[256];
	strcpy( szBuf, aString.c_str() );
	LPTSTR lpsz = szBuf;
	LPTSTR lpszLast = NULL;

	while (*lpsz != '\0')
	{
		if (_istspace(*lpsz))
		{
			if (lpszLast == NULL)
				lpszLast = lpsz;
		}
		else
			lpszLast = NULL;
		lpsz = _tcsinc(lpsz);
	}

	if (lpszLast != NULL)
	{
		// truncate at trailing space start
		*lpszLast = '\0';
	}
	aString = szBuf;
}