// ConClientListener.h: interface for the CConClientListener class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_CONCLIENTLISTENER_H__DCACDF13_8993_48B7_B4AD_A593827A73F4__INCLUDED_)
#define AFX_CONCLIENTLISTENER_H__DCACDF13_8993_48B7_B4AD_A593827A73F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"

class CConClient;

class CConClientListener  :	public CAsyncSocket
{
public:
	void pendingAccept();

	CConClientListener(CConClient * pConClient);
	virtual ~CConClientListener();

	virtual void OnAccept( int nErrorCode );

private:
	CConClient *	m_pCallback;
};

#endif // !defined(AFX_CONCLIENTLISTENER_H__DCACDF13_8993_48B7_B4AD_A593827A73F4__INCLUDED_)
