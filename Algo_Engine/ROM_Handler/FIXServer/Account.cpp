// Account.cpp: implementation of the Account class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Account.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OMAccount::OMAccount( const AccountID & id, const std::string & szRetail, const std::string & szOMAcct )
: m_AcctID( id ), m_szRetailAcct( szRetail ), m_szOMAcct( szOMAcct )
{

}

OMAccount::~OMAccount()
{

}

OMAccount::OMAccount( const OMAccount & acct ) : m_AcctID( acct.m_AcctID ), m_szRetailAcct(""), m_szOMAcct("")
{
	*this = acct;
}

OMAccount & OMAccount::operator=	( const OMAccount & acct )
{
	m_AcctID		= acct.m_AcctID;
	m_szRetailAcct	= acct.m_szRetailAcct;
	m_szOMAcct		= acct.m_szOMAcct;
	return *this;
}
