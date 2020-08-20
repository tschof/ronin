#ifndef __BASEHANDLER_H__
#define __BASEHANDLER_H__

#include "ace/Asynch_IO.h"
#include "ace/Proactor.h"
#include "ace/Message_Block.h"
#include "ace/SOCK_Stream.h"
#include "typedef.h"

#include <list>
#include "BaseManager.h"
#include "RomValueDefs.h"
#include "RomCsv.h"

static const int g_MsgBlkNum = 10;

class CBaseHandler : public ACE_Service_Handler
{
public:
	//typedef std::list<ACE_Message_Block*> tMsgBlkList;
	//typedef tMsgBlkList::iterator tMsgBlkListIt;

	CBaseHandler();
	~CBaseHandler();

	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result& result);
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result);
	virtual void addresses(const ACE_INET_Addr& remote_address, const ACE_INET_Addr& local_address);

	virtual void ProcessMessage(const char* pMsg, int size) = 0;
	virtual void RemoveHandler() = 0;

	bool InitRead();
	void ProcessMessages(ACE_Message_Block& in_mb);
	bool Send(CRomCsv& msg);
	bool LoggedOn() { return m_loggedOn; }
	bool Blocked()	{ return m_blocked; }
	std::string ID()	{ return m_ID; }

	void SetLoggedOn(bool value) { m_loggedOn = value; }
	void SetBlocked(bool value) { m_blocked = value;}
	void SetID(const std::string& value) { m_ID = value; }

	bool Write(ACE_Message_Block& mb);
	bool Read(ACE_Message_Block& mb);
	
	void IncrementWriteCnt()			{ m_pendingWriteCnt++; }
	void DecrementWriteCnt()			{ m_pendingWriteCnt--; }
	void IncrementReadCnt()				{ m_pendingReadCnt++; }
	void DecrementReadCnt()				{ m_pendingReadCnt--; }

	unsigned int PendingReadCnt()		{ return m_pendingWriteCnt; }
	unsigned int PendingWriteCnt()		{ return m_pendingWriteCnt; }

	//void BuildMessages(ACE_Message_Block& in_mb, int type, tMsgBlkList& out_mbs);
	//void BuildMessages(ACE_Message_Block& in_mb, tMsgBlkList& out_mbs);
	//void BuildMessages(ACE_Message_Block& in_mb, tRomMsgList& msgList);

	const ACE_INET_Addr& Address()			{ return m_remoteAddr; }

protected:
	ACE_Asynch_Read_Stream m_reader;
	ACE_Asynch_Write_Stream m_writer;

private:
	
	ACE_INET_Addr m_remoteAddr;

	bool m_loggedOn;
	bool m_blocked;
	//char m_leftover[1024];
	std::string m_leftover;
	std::string m_ID;
	ACE_Message_Block* m_pBuffers[g_MsgBlkNum];
	unsigned int m_pendingReadCnt;
	unsigned int m_pendingWriteCnt;
};

#endif //__BASEHANDLER_H__
