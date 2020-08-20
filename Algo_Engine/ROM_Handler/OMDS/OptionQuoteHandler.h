// OptionQuoteHandler.h: interface for the OptionQuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONQUOTEHANDLER_H__F67F94F6_ED91_4D8C_B25C_52EA343B2AFC__INCLUDED_)
#define AFX_OPTIONQUOTEHANDLER_H__F67F94F6_ED91_4D8C_B25C_52EA343B2AFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuoteHandler.h"

class OptionQuoteHandler : public QuoteHandler  
{
public:
	OptionQuoteHandler(CConTib	* pConTib);
	virtual ~OptionQuoteHandler();

	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL );

};

#endif // !defined(AFX_OPTIONQUOTEHANDLER_H__F67F94F6_ED91_4D8C_B25C_52EA343B2AFC__INCLUDED_)
