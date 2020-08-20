// OptionQuoteBestHandler.h: interface for the OptionQuoteBestHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONQUOTEBESTHANDLER_H__2015EC17_C11E_47F8_B529_68EA00DE9447__INCLUDED_)
#define AFX_OPTIONQUOTEBESTHANDLER_H__2015EC17_C11E_47F8_B529_68EA00DE9447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuoteHandler.h"

class OptionQuoteBestHandler : public QuoteHandler  
{
public:
	OptionQuoteBestHandler(CConTib	* pConTib);
	virtual ~OptionQuoteBestHandler();

	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL );

};

#endif // !defined(AFX_OPTIONQUOTEBESTHANDLER_H__2015EC17_C11E_47F8_B529_68EA00DE9447__INCLUDED_)
