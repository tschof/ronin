// Entitlement.cpp: implementation of the CEntitlement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Entitlement.h"
#include "dbo.h"
#include "Locker.h"
#include "Log.h"
#include "Main.h"
#include <sstream>

#pragma comment( lib, "libcs.lib")
#pragma comment( lib, "libct.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEntitlement::CEntitlement()
{

}

CEntitlement::~CEntitlement()
{

}

bool CEntitlement::Load( const std::string & szUser, const std::string & szToken )
{
#ifdef ENABLE_ENTITLEMENT
	DboConnection db( m_szServer.c_str(), m_szDBUser.c_str(), m_szDBPass.c_str(), "OMDS" );
	DboCommand cmd( &db, "rom.dbo.OMDS_authUser" );
	cmd.paramIn( "@username", (char *)szUser.c_str() );
	cmd.paramIn( "@token", (char *)szToken.c_str() );
	cmd.exec();
	if( cmd.status() != DBO_SUCCESS )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, cmd.statusText() );
		return false;
	}
	Locker locker(&m_Lock);
	DboResults * pRes = NULL;
	m_UserMap.erase( szUser );
	for( int i = 0; i < cmd.resultsCount(); i++ )
	{
		pRes = cmd.results(i);
		for( int j = 0; j < pRes->rowCount(); j++ )
		{
			try
			{
				pRes->rowSet( j );
				char szBuff[128];
				//pRes->colValue( 0, szBuff, sizeof(szBuff) );
				//std::string user = szBuff;
				pRes->colValue( 1, szBuff, sizeof(szBuff) );
				std::string szEx = szBuff;
				int nEnabled = 0;
				pRes->colValue( 2, &nEnabled );

				tUserMap::iterator it = m_UserMap.find( szUser );
				if( it == m_UserMap.end() )
				{
					tExMap exchanges;
					exchanges.insert( tExMap::value_type( szEx, nEnabled == 0 ? false:true ) );
					m_UserMap.insert( tUserMap::value_type( szUser, exchanges ) );

				}
				else
				{
					(*it).second.insert( tExMap::value_type( szEx, nEnabled == 0 ? false:true ) );
				}
			}
			catch(...)
			{
				continue;
			}
		}
	}
	return true;
#else
	return true;
#endif
}

bool CEntitlement::IsEntitled( const std::string & szUser, const std::string & szEx, const std::string & szSymbol )
{
#ifdef ENABLE_ENTITLEMENT
	if( CMain::getInstance()->m_bEnableSecurity == false )
		return true;
	Locker locker(&m_Lock);
	tUserMap::iterator it = m_UserMap.find( szUser );
	if( it == m_UserMap.end() )
	{
		std::stringstream stm;
		stm << "CEntitlement::IsEntitled user not found: " << szUser;
		CMain::getInstance()->m_log.write(LOG_ERROR, stm.str() );
		return false;
	}
	tExMap::iterator ite = (*it).second.find( szEx );
	if( ite == (*it).second.end() )
	{
		std::stringstream stm;
		stm << "CEntitlement::IsEntitled exchange " << szEx << " not found for user " << szUser;
		CMain::getInstance()->m_log.write(LOG_ERROR, stm.str() );
		return false;
	}
	return (*ite).second;
#else
	return true;
#endif
}

bool CEntitlement::IsValidUser( const std::string & szUser, const std::string & szToken )
{
	Locker locker(&m_Lock);
	tUserMap::iterator it = m_UserMap.find( szUser );
	if( !Load( szUser, szToken ) )
		return false;
	return true;
}

