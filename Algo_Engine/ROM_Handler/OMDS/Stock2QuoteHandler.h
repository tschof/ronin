// Stock2QuoteHandler.h: interface for the Stock2QuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOCK2QUOTEHANDLER_H__E1E7F28E_65ED_4ED0_A3D8_1658E560FC8F__INCLUDED_)
#define AFX_STOCK2QUOTEHANDLER_H__E1E7F28E_65ED_4ED0_A3D8_1658E560FC8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuoteHandler.h"

class Stock2QuoteHandler : public QuoteHandler  
{
public:
	Stock2QuoteHandler(CConTib	* pConTib);
	virtual ~Stock2QuoteHandler();

	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL );

};

#endif // !defined(AFX_STOCK2QUOTEHANDLER_H__E1E7F28E_65ED_4ED0_A3D8_1658E560FC8F__INCLUDED_)
