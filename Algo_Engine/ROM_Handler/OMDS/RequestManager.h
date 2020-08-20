// RequestManager.h: interface for the RequestManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTMANAGER_H__47D649DA_CF25_4740_8BC8_F6F1B9276B9A__INCLUDED_)
#define AFX_REQUESTMANAGER_H__47D649DA_CF25_4740_8BC8_F6F1B9276B9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <set>
#include <list>
#include "Lock.h"

class CDClientRequest;
class CConClientMessage;
class CConnections;
class CDTibMessage;
class CDataManager;
class RequestManager  
{
public:
	RequestManager(CConnections *pConnections, CDataManager *pDataManager);
	virtual ~RequestManager();

	bool addRequest						( const CConClientMessage & conClientMessage );
	void sendToClient					( const std::string & szRequestKey, CDTibMessage * pDTibMsg );
	void clearClients					( const std::string & szRequestKey );
	std::list<CDClientRequest *> GetRequestsForClient	( long nClientId );

private:
	void LoadIndexs						();
	void updateStockQuoteRequest		( const CConClientMessage & conClientMessage );
	void updateStock2QuoteRequest		( const CConClientMessage & conClientMessage );
	void updateOptionSeriesRequest		( const CConClientMessage & conClientMessage );
	void updateOptionQuoteRequest		( const CConClientMessage & conClientMessage );
	void updateOptionQuoteBestRequest	( const CConClientMessage & conClientMessage );
	void updateGreekDeltaRequest		( const CConClientMessage & conClientMessage );
	void updateFutureSeriesRequest		( const CConClientMessage & conClientMessage );
	void updateFutureQuoteRequest		( const CConClientMessage & conClientMessage );
	void deleteAllRequest				( const CConClientMessage & conClientMessage );
	void CheckEntitlement				( const CConClientMessage & conClientMessage );
	bool CheckStockEntitlement			( const std::string & szUser, const std::string & szMessage );
	bool CheckStock2Entitlement			( const std::string & szUser, const std::string & szMessage );
	bool CheckOptionEntitlement			( const std::string & szUser, const std::string & szMessage );
	bool CheckOptionBestEntitlement		( const std::string & szUser, const std::string & szMessage );
	bool CheckFutureEntitlement			( const std::string & szUser, const std::string & szMessage );


	std::string parseSymbol				( const std::string & szMessage );
	void	CreateOptionQuoteClientRequest( const CConClientMessage & conClientMessage, const std::string & szUnder );
	
	bool IsIndex( const std::string & szSymbol );
	
	typedef std::map<std::string,CDClientRequest*>	tRequestMap;
	tRequestMap			m_RequestMap;
	Lock				m_RequestLock;
	CConnections		* m_pConnections;
	CDataManager		* m_pDataManager;
	std::set<std::string>	m_Indexs;
};

#endif // !defined(AFX_REQUESTMANAGER_H__47D649DA_CF25_4740_8BC8_F6F1B9276B9A__INCLUDED_)
