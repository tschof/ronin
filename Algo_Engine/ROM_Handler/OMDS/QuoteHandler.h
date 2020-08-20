// QuoteHandler.h: interface for the QuoteHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUOTEHANDLER_H__50287BCA_7885_4428_B12F_EFE7ED1EAB6A__INCLUDED_)
#define AFX_QUOTEHANDLER_H__50287BCA_7885_4428_B12F_EFE7ED1EAB6A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <rvcpp.h>
#include <string>

class CDTibMessage;
class CConTib;
class QuoteHandler : public RvDataCallback  
{
public:
	QuoteHandler( CConTib * pConTib );
	virtual ~QuoteHandler();
	
	void onData(const char * pSubject, RvSender * pReplySender, const RvDatum &data, RvListener * pInvoker);
	virtual CDTibMessage * crack( const std::string & subject, const RvDatum & data, void * pClosure = NULL ) = 0;
protected:
	CConTib	*	m_pConTib;
};

#endif // !defined(AFX_QUOTEHANDLER_H__50287BCA_7885_4428_B12F_EFE7ED1EAB6A__INCLUDED_)
