// OptionQuoteHandler.cpp: implementation of the OptionQuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "OptionQuoteHandler.h"
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

OptionQuoteHandler::OptionQuoteHandler(CConTib	* pConTib) : QuoteHandler(pConTib)
{

}

OptionQuoteHandler::~OptionQuoteHandler()
{

}

CDTibMessage * OptionQuoteHandler::crack( const std::string & subject, const RvDatum & data, void * pClosure )
{
	CDATib * pDATib = m_pConTib->getDATib();
	return pDATib->factoryOptionQuote( subject, data );
}

