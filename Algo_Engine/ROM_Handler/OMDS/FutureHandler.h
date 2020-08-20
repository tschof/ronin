// FutureHandler.h: interface for the FutureHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUTUREHANDLER_H__5F56EBC2_4F3B_436F_88D3_9A7ABBF72941__INCLUDED_)
#define AFX_FUTUREHANDLER_H__5F56EBC2_4F3B_436F_88D3_9A7ABBF72941__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuoteHandler.h"

class FutureHandler  : public QuoteHandler   
{
public:
	FutureHandler(CConTib	* pConTib);
	virtual ~FutureHandler();
	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL );

};

#endif // !defined(AFX_FUTUREHANDLER_H__5F56EBC2_4F3B_436F_88D3_9A7ABBF72941__INCLUDED_)
