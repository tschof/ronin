// StockQuoteHandler.cpp: implementation of the StockQuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "StockQuoteHandler.h"
#include "ConTib.h"
#include "DATib.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StockQuoteHandler::StockQuoteHandler(CConTib * pConTib) : QuoteHandler(pConTib)
{

}

StockQuoteHandler::~StockQuoteHandler()
{

}

CDTibMessage * StockQuoteHandler::crack( const std::string & subject, const RvDatum & data, void * pClosure )
{
	CDATib * pDATib = m_pConTib->getDATib();
	return pDATib->factoryStockQuote( subject, data, pClosure );
}