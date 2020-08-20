// FutureHandler.cpp: implementation of the FutureHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "FutureHandler.h"
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

FutureHandler::FutureHandler(CConTib * pConTib) : QuoteHandler(pConTib)
{

}

FutureHandler::~FutureHandler()
{

}

CDTibMessage * FutureHandler::crack( const std::string & subject, const RvDatum & data, void * pClosure )
{
	CDATib * pDATib = m_pConTib->getDATib();
	return pDATib->factoryFutureQuote( subject, data );
}