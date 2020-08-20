#ifndef __CLIENTHANDLER_H__
#define __CLIENTHANDLER_H__

#include "BaseHandler.h"
#include "RomCsv.h"

class CClientHandler : public CBaseHandler
{
public:
	CClientHandler();
	~CClientHandler();

	virtual void open(ACE_HANDLE h, ACE_Message_Block& mb);
	//virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result);
	virtual void ProcessMessage(const char* pMsg, int size);
	virtual void RemoveHandler();

	bool PendingPwdCheck()				{ return m_pendingPwdCheck; }
	void SetPendingPwdCheck(bool value) { m_pendingPwdCheck = value; }
	//bool Send(CRomCsv& msg);

private:
	bool m_pendingPwdCheck;
};

#endif //__CLIENTHANDLER_H__
