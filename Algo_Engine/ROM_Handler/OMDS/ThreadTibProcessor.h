// ThreadTibProcessor.h: interface for the CThreadTibProcessor class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_THREADTIBPROCESSOR_H__82A362C5_FFCD_4B07_8839_6D723E9DB722__INCLUDED_)
#define AFX_THREADTIBPROCESSOR_H__82A362C5_FFCD_4B07_8839_6D723E9DB722__INCLUDED_

#include "OMDSCommon.h"
#include "MessageProcessor.h"
#include "Log.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThreadTibProcessor : public CRunable
{
public:
	void run();
	CThreadTibProcessor(CMessageProcessor * pMP);
	virtual ~CThreadTibProcessor();

private:
	CMessageProcessor * m_pMessageProcessor;
	CLog				m_log;
};

#endif // !defined(AFX_THREADTIBPROCESSOR_H__82A362C5_FFCD_4B07_8839_6D723E9DB722__INCLUDED_)
