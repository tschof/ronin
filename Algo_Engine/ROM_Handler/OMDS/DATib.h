// DATib.h: interface for the CDATib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATIB_H__E369FFD2_1A59_4DFB_8770_181E021882F9__INCLUDED_)
#define AFX_DATIB_H__E369FFD2_1A59_4DFB_8770_181E021882F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include <rvcpp.h>
#include <TibMsg.h>

class CDataManager;
class CDTibMessage;

// ====================================================================
// Data Adapter
// ====================================================================
class CDATib
{
public:
	CDATib(CDataManager * pDM);
	virtual ~CDATib();

	static int		GetTibMode();
	static bool		NextTibMsg( std::string & szSubject, TibMsg & tibMsg );
	CDTibMessage *	factoryGreeks( const std::string & subject, const RvDatum &data, void * pClosure = NULL );
	CDTibMessage *	factoryStockQuote( const std::string & subject, const RvDatum &data, void * pClosure = NULL);
	CDTibMessage *	factoryStock2Quote( const std::string & subject, const RvDatum &data, void * pClosure = NULL);
	CDTibMessage *	factoryOptionQuote( const std::string & subject, const RvDatum &data, void * pClosure = NULL);
	CDTibMessage *	factoryOptionQuoteBest( const std::string & subject, const RvDatum &data, void * pClosure = NULL);
	CDTibMessage *	factoryFutureQuote( const std::string & subject, const RvDatum &data, void * pClosure = NULL);
	CLog			m_log;
protected:
	static bool LogTibMsg( const std::string & szSubject, TibMsg & tibMsg );
	static int		m_nMode;
	static FILE *	m_File;
	CDataManager * m_pDataManager;
};


#endif // !defined(AFX_DATIB_H__E369FFD2_1A59_4DFB_8770_181E021882F9__INCLUDED_)
