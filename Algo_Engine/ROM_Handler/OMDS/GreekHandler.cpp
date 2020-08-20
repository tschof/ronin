// GreekHandler.cpp: implementation of the GreekHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "GreekHandler.h"
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

GreekHandler::GreekHandler(CConTib * pConTib) : QuoteHandler(pConTib)
{

}

GreekHandler::~GreekHandler()
{

}

CDTibMessage * GreekHandler::crack( const std::string & subject, const RvDatum & data, void * pClosure )
{
	CDATib * pDATib = m_pConTib->getDATib();
	return pDATib->factoryGreeks( subject, data );
}