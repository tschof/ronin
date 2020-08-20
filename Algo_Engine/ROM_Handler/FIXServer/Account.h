// Account.h: interface for the Account class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNT_H__9FD176D6_6299_47C8_B142_60ABBFDD7F5D__INCLUDED_)
#define AFX_ACCOUNT_H__9FD176D6_6299_47C8_B142_60ABBFDD7F5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AccountID.h"

class OMAccount  
{
public:
	OMAccount( const AccountID & id, const std::string & szRetail, const std::string & szOMAcct );
	virtual ~OMAccount();

	OMAccount( const OMAccount & acct );
	OMAccount & operator=	( const OMAccount & acct );

	AccountID	m_AcctID;	
	std::string m_szRetailAcct;
	std::string m_szOMAcct;
};

#endif // !defined(AFX_ACCOUNT_H__9FD176D6_6299_47C8_B142_60ABBFDD7F5D__INCLUDED_)
