// OmExecution.h: interface for the COmExecution class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OMEXECUTION_H__85478713_867F_401D_A371_36FD845CB85F__INCLUDED_)
#define AFX_OMEXECUTION_H__85478713_867F_401D_A371_36FD845CB85F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <iostream>

namespace FIX42
{
	class ExecutionReport;
};

class COmExecution  
{
public:
	COmExecution( const FIX42::ExecutionReport & report );
	virtual ~COmExecution();

	friend std::ostream & operator<< ( std::ostream & stream, const COmExecution & exe );
	
	std::ostream & operator<< ( std::ostream & stream );
private:
	std::string		m_szTraderAcr;
	std::string		m_szClearingID;
	std::string		m_szExeTime;
	long			m_nQty;
	float			m_fPrice;
	std::string		m_szOrderTag;
	std::string		m_szExchange;
	std::string		m_szSymbol;
	std::string		m_szSecType;
	std::string		m_szBase;
	float			m_fStrike;
	std::string		m_szCallPut;
	std::string		m_szExpire;
	int				m_nSide;
	int				m_nOpenClose;
	std::string		m_szLocalAcct;
	std::string		m_szRetailAcct;
	std::string		m_szFirm;
	std::string		m_szExecID;
};

inline std::ostream & operator<<( std::ostream & stream, const COmExecution & exe )
{
	return stream	<< exe.m_szTraderAcr	<< ","
					<< exe.m_szClearingID	<< ","
					<< exe.m_szExeTime		<< ","
					<< exe.m_nQty			<< ","
					<< exe.m_fPrice			<< ","
					<< exe.m_szOrderTag		<< ","
					<< exe.m_szExchange		<< ","
					<< exe.m_szSymbol		<< ","
					<< exe.m_szSecType		<< ","
					<< exe.m_szBase			<< ","
					<< exe.m_fStrike		<< ","
					<< exe.m_szCallPut		<< ","
					<< exe.m_szExpire		<< ","
					<< exe.m_nSide			<< ","
					<< exe.m_nOpenClose		<< ","
					<< exe.m_szLocalAcct	<< ","
					<< exe.m_szRetailAcct	<< ","
					<< exe.m_szFirm			<< ","
					<< exe.m_szExecID		<< std::endl;
}

#endif // !defined(AFX_OMEXECUTION_H__85478713_867F_401D_A371_36FD845CB85F__INCLUDED_)
