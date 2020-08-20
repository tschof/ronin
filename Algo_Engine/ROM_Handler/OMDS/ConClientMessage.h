// ConClientMessage.h: interface for the CConClientMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONCLIENTMESSAGE_H__B94FA956_FF18_4597_87CA_FBA5973A01D0__INCLUDED_)
#define AFX_CONCLIENTMESSAGE_H__B94FA956_FF18_4597_87CA_FBA5973A01D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"

class CConClientMessage
{
public:
	CConClientMessage();
	virtual ~CConClientMessage();

	CConClientMessage( const CConClientMessage & msg );
	CConClientMessage & operator=( const CConClientMessage & msg );

	int			m_clientId;
	std::string	message;
};

#endif // !defined(AFX_CONCLIENTMESSAGE_H__B94FA956_FF18_4597_87CA_FBA5973A01D0__INCLUDED_)
