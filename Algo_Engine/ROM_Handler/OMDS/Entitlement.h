// Entitlement.h: interface for the CEntitlement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENTITLEMENT_H__CC5E551B_BB61_4415_A977_2B73D41EC4B3__INCLUDED_)
#define AFX_ENTITLEMENT_H__CC5E551B_BB61_4415_A977_2B73D41EC4B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include "Lock.h"

class CEntitlement  
{
public:
	CEntitlement();
	virtual ~CEntitlement();

	void Init( const std::string szServer, const std::string & szDBUser, const std::string & szDBPass )
	{ 
		m_szServer = szServer; 
		m_szDBUser = szDBUser;
		m_szDBPass = szDBPass;
	}
	bool IsValidUser( const std::string & szUser, const std::string & szToken );
	bool IsEntitled( const std::string & szUser, const std::string & szEx, const std::string & szSymbol );

private:
	bool Load( const std::string & szUser, const std::string & szToken );
	typedef std::map<std::string,bool> tExMap;
	typedef std::map<std::string, tExMap> tUserMap;

	std::string	m_szServer;
	std::string m_szDBUser;
	std::string m_szDBPass;

	tUserMap	m_UserMap;
	Lock		m_Lock;

};

#endif // !defined(AFX_ENTITLEMENT_H__CC5E551B_BB61_4415_A977_2B73D41EC4B3__INCLUDED_)
