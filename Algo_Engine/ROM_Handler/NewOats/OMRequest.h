// OMRequest.h: interface for the COMRequest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OMREQUEST_H__8F258132_E8A8_45A0_BFF4_D40116C75900__INCLUDED_)
#define AFX_OMREQUEST_H__8F258132_E8A8_45A0_BFF4_D40116C75900__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataStore.h"
#include "QueueRequest.h"
#include "Lock.h"

class COMRequest : public CQueueRequest, public DataStoreObj<COMRequest> 
{
public:
	COMRequest();
	virtual ~COMRequest();

	virtual bool Process();

	void Bust( const CString & szMsg )
	{
		ZeroMemory( m_szMsg, sizeof(m_szMsg) );
		strcpy( m_szMsg, (LPCSTR)szMsg );
	}

private:
	char m_szMsg[1024];
};

#endif // !defined(AFX_OMREQUEST_H__8F258132_E8A8_45A0_BFF4_D40116C75900__INCLUDED_)
