// RequestFactory.h: interface for the CRequestFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTFACTORY_H__ED504044_4F6E_4878_B1EB_E0F036C7BED8__INCLUDED_)
#define AFX_REQUESTFACTORY_H__ED504044_4F6E_4878_B1EB_E0F036C7BED8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueueRequest;
class CCsv;

class CRequestFactory  
{
	CRequestFactory();
public:
	virtual ~CRequestFactory();
	
	static CRequestFactory & getInstance()
	{
		static CRequestFactory factory;
		return factory;
	}

	CQueueRequest * createRequest( CCsv * pCsv );
	CQueueRequest * createRequest(const CString& msg, bool bAdmin = true );
};

#endif // !defined(AFX_REQUESTFACTORY_H__ED504044_4F6E_4878_B1EB_E0F036C7BED8__INCLUDED_)
