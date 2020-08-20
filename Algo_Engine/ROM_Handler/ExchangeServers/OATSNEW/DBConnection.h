// DBConnection.h: interface for the CDBConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCONNECTION_H__8F5AA5EC_A9E5_4202_80FD_07053B36DCBD__INCLUDED_)
#define AFX_DBCONNECTION_H__8F5AA5EC_A9E5_4202_80FD_07053B36DCBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <exception>
#include <string>
#include <vector>
#include "Logger.h"
#include "Lock.h"

class COrderMsg;
class DboConnection;

class CDBConnection  
{
	CDBConnection();
public:
	virtual ~CDBConnection();

	static CDBConnection & getInstance()
	{
		static CDBConnection conn;
		return conn;
	}

	bool		Open( TCHAR *tszDSNName, TCHAR *tszUserName, TCHAR *tszPassword, TCHAR * tszDatabase );
	bool		Close();

	bool		InsertOrder( COrderMsg * pOrder, int nConnection = 0 );

	void		SetLogLevel(int nLevel) { m_nLevel = nLevel; }
	int			GetLogLevel() { return m_nLevel; }
	std::string	BuildInsertString( COrderMsg * pOrder );
	static long	GetCount() { return m_nCount; }
private:
	void	GenerateOrderNum( COrderMsg * pOrder );
	void	GenerateOrderNum1(COrderMsg* pOrder);

	//DboConnection *m_pConn;
	int								m_nThreads;
	std::vector<DboConnection *>	m_Conn;
	static long m_nCount;
	bool		m_bIsOpen;
	CString		m_szUser;
	CString		m_szPass;
	CString		m_szDSN;
	int			m_nLevel;
	Lock		m_Lock;
	bool		m_bUsingOmInstance;
};

#endif // !defined(AFX_DBCONNECTION_H__8F5AA5EC_A9E5_4202_80FD_07053B36DCBD__INCLUDED_)
