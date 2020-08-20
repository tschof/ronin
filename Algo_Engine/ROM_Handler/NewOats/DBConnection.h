#ifndef _DBCONNECTION_H__
#define _DBCONNECTION_H__


//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "Windows.h"
#include <exception>
#include <string>
#include <vector>
#include "Logger.h"
#include "Count.h"

class COrderMsg;
class DboConnection;

class CDBConnection  
{
public:
	CDBConnection();
	virtual ~CDBConnection();

	//static CDBConnection & getInstance()
	//{
	//	static CDBConnection conn;
	//	return conn;
	//}

	//bool		Open( const char* DSNName, const char* UserName, const char* Password, const char* Database );
	//bool		Close();

	//bool		InsertOrder( COrderMsg * pOrder, int nConnection = 0 );
	bool		InsertOrder( COrderMsg * pOrder);
	//bool		InsertMessage(const CRomCsv& csv);
	bool		InsertMessage( COrderMsg * pOrder);
	void		SetLogLevel(int nLevel) { m_nLevel = nLevel; }
	int			GetLogLevel() { return m_nLevel; }
	std::string	BuildInsertString( COrderMsg * pOrder );
	long GetInsertedCnt()	{ return m_InsertedCnt.Value(); }
	long GetProcessedCnt()	{ return m_ProcessedCnt.Value(); }
private:
	void	GenerateOrderNum( COrderMsg * pOrder );
	void	GenerateOrderNumNew(COrderMsg* pOrder);
	
	//int								m_nThreads;
	//std::vector<DboConnection *>	m_Conn;
	DboConnection*					m_pConn;
	//static long						m_nCount;
	bool							m_bIsOpen;
	//std::string						m_szUser;
	//std::string						m_szPass;
	//std::string						m_szDSN;
	int								m_nLevel;
	CCount							m_InsertedCnt;
	CCount							m_ProcessedCnt;
	int								m_numEmail;
};

#endif // _DBCONNECTION_H__
