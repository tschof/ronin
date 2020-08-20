#ifndef _CLIENTMSG_H__
#define _CLIENTMSG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QueueRequest.h"


class CClientMsg : public CQueueRequest
{
public:

	CClientMsg();
	CClientMsg(const CString& msg, bool bAdmin = true);
	virtual ~CClientMsg(){}

	virtual bool Process();

private:
	CString m_Msg;
	bool m_bAdmin;
};


#endif // _CLIENTMSG_H__