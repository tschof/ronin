// AccountID.h: interface for the AccountID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCOUNTID_H__EE083954_FCDD_43C7_A4D6_17278CADCC36__INCLUDED_)
#define AFX_ACCOUNTID_H__EE083954_FCDD_43C7_A4D6_17278CADCC36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class AccountID  
{
public:
	AccountID( const std::string & szKey, const std::string & szType );
	AccountID(const std::string & szTraderAcr, const std::string & szLocalAcr, const std::string & szClearingAcID, const std::string & szFirm, const std::string & szType );
	virtual ~AccountID();

	AccountID( const AccountID & id );
	AccountID & operator=	( const AccountID & id );

	bool operator<	( const AccountID & id ) const;
	bool operator==	( const AccountID & id ) const;

	operator std::string() { return m_szKey; }
	
	static void	TrimRight( std::string & aString );

private:

	std::string		m_szTraderAcr;
	std::string		m_szLocalAcAcr;
	std::string		m_szClearingAcID;
	std::string		m_szFirmAcr;
	std::string		m_szType;
	std::string		m_szKey;
};

#endif // !defined(AFX_ACCOUNTID_H__EE083954_FCDD_43C7_A4D6_17278CADCC36__INCLUDED_)
