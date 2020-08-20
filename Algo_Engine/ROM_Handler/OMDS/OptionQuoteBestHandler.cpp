// OptionQuoteBestHandler.cpp: implementation of the OptionQuoteBestHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "OptionQuoteBestHandler.h"
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

OptionQuoteBestHandler::OptionQuoteBestHandler(CConTib	* pConTib) : QuoteHandler(pConTib)
{

}

OptionQuoteBestHandler::~OptionQuoteBestHandler()
{

}

CDTibMessage * OptionQuoteBestHandler::crack( const std::string & subject, const RvDatum & data, void * pClosure )
{
	CDATib * pDATib = m_pConTib->getDATib();
	return pDATib->factoryOptionQuoteBest( subject, data );
}