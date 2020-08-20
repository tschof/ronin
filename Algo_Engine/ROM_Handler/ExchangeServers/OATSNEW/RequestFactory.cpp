// RequestFactory.cpp: implementation of the CRequestFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "oats.h"
#include "RequestFactory.h"
#include "Csv.h"
#include "OrderMsg.h"
#include "StatusMsg.h"
#include "Logger.h"
#include "ClientMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRequestFactory::CRequestFactory()
{

}

CRequestFactory::~CRequestFactory()
{

}

CQueueRequest * CRequestFactory::createRequest( CCsv * pCsv )
{
	CQueueRequest * pMsg = NULL;
	CString szCommand = pCsv->Get( CSV_COMMAND );
	if( !szCommand.Compare("E") )
		pMsg = new COrderMsg( pCsv );
	else if( !szCommand.Compare("S") )
		pMsg = new COrderMsg( pCsv ); // pMsg = new CStatusMsg( pCsv );
	return pMsg;
}


CQueueRequest * CRequestFactory::createRequest(const CString& msg, bool bAdmin )
{
	CQueueRequest * pMsg = new CClientMsg(msg, bAdmin);
	return pMsg;
}
