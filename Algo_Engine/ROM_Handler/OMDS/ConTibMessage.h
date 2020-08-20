// ConTibMessage.h: interface for the CConTibMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTIBMESSAGE_H__BEC84628_57C3_47C8_B3FB_9E2DC732783E__INCLUDED_)
#define AFX_CONTIBMESSAGE_H__BEC84628_57C3_47C8_B3FB_9E2DC732783E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <rvcpp.h>
#include <rvevm.h>
#include "tibmsg.h"

#include "OMDSCommon.h"

class CConTibMessage
{
public:
	CConTibMessage();
	virtual ~CConTibMessage();

	const RvDatum*		m_pRvDatum;
	std::string				m_subject;
};

typedef std::list<CConTibMessage *>	ConTibMsgList;

#endif // !defined(AFX_CONTIBMESSAGE_H__BEC84628_57C3_47C8_B3FB_9E2DC732783E__INCLUDED_)
