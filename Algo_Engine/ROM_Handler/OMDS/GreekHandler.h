// GreekHandler.h: interface for the GreekHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GREEKHANDLER_H__FA50BA25_A83C_4EA2_A502_1AB916F5913E__INCLUDED_)
#define AFX_GREEKHANDLER_H__FA50BA25_A83C_4EA2_A502_1AB916F5913E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuoteHandler.h"

class GreekHandler : public QuoteHandler  
{
public:
	GreekHandler(CConTib	* pConTib);
	virtual ~GreekHandler();
	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL );

};

#endif // !defined(AFX_GREEKHANDLER_H__FA50BA25_A83C_4EA2_A502_1AB916F5913E__INCLUDED_)
