// QuoteHandler.cpp: implementation of the QuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "omdsserver.h"
#include "QuoteHandler.h"
#include "ConTib.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QuoteHandler::QuoteHandler(CConTib	* pConTib) : m_pConTib( pConTib )
{

}

QuoteHandler::~QuoteHandler()
{

}

void QuoteHandler::onData(const char * pSubject, RvSender * pReplySender, const RvDatum &data, RvListener * pInvoker)
{
	std::string subject = pInvoker->subject();
	CDTibMessage * pDTM = crack(subject,data, pInvoker->closureArg);
	if (pDTM != NULL)
	{
		m_pConTib->addMessage( pDTM );
	}
}

