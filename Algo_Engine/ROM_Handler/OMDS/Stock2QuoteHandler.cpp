// Stock2QuoteHandler.cpp: implementation of the Stock2QuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "Stock2QuoteHandler.h"
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

Stock2QuoteHandler::Stock2QuoteHandler(CConTib	* pConTib) : QuoteHandler(pConTib)
{

}

Stock2QuoteHandler::~Stock2QuoteHandler()
{

}

CDTibMessage * Stock2QuoteHandler::crack( const std::string & subject, const RvDatum & data, void * pClosure )
{
	CDATib * pDATib = m_pConTib->getDATib();
	return pDATib->factoryStock2Quote( subject, data );
}