// ThreadClientProcessor.h: interface for the CThreadClientProcessor class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_THREADCLIENTPROCESSOR_H__5B5E4AFA_DCA0_40AC_9275_7DFD68795BA3__INCLUDED_)
#define AFX_THREADCLIENTPROCESSOR_H__5B5E4AFA_DCA0_40AC_9275_7DFD68795BA3__INCLUDED_

#include "OMDSCommon.h"
#include "MessageProcessor.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThreadClientProcessor : public CRunable
{
public:
	void run();
	CThreadClientProcessor(CMessageProcessor * pMP);
	virtual ~CThreadClientProcessor();

private:
	CMessageProcessor * m_pMessageProcessor;
	CLog				m_log;
};

#endif // !defined(AFX_THREADCLIENTPROCESSOR_H__5B5E4AFA_DCA0_40AC_9275_7DFD68795BA3__INCLUDED_)
