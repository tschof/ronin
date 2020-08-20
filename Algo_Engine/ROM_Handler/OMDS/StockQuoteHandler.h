// StockQuoteHandler.h: interface for the StockQuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOCKQUOTEHANDLER_H__4EF1819B_FCD1_4BD2_B95C_949705E907A0__INCLUDED_)
#define AFX_STOCKQUOTEHANDLER_H__4EF1819B_FCD1_4BD2_B95C_949705E907A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuoteHandler.h"

class StockQuoteHandler : public QuoteHandler  
{
public:
	StockQuoteHandler(CConTib	* pConTib);
	virtual ~StockQuoteHandler();

	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL );

};

#endif // !defined(AFX_STOCKQUOTEHANDLER_H__4EF1819B_FCD1_4BD2_B95C_949705E907A0__INCLUDED_)
