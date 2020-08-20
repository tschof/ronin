#include "BaseHandler.h"
#include "romfielddefs.h"
#include "romvaluedefs.h"

CBaseHandler::CBaseHandler()
{
	m_loggedOn = false;
}

CBaseHandler::~CBaseHandler()
{
	if (this->handle() != ACE_INVALID_HANDLE)
	{
		ACE_OS::closesocket(this->handle());
	}
}

void CBaseHandler::addresses(const ACE_INET_Addr& remote_address, const ACE_INET_Addr& local_address)
{
	m_remoteAddr.set(remote_address);
}

void CBaseHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result& result)
{
	DecrementWriteCnt();
	result.message_block().release();
}

void CBaseHandler::handle_read_stream(const ACE_Asynch_Read_Stream::Result& result)
{
	DecrementReadCnt();
	ACE_Message_Block& mb = result.message_block();

	if (!result.success() || result.bytes_transferred() == 0)
	{
		mb.release();
		if (PendingReadCnt() == 0 &&
			PendingWriteCnt() == 0)
		{
			RemoveHandler();
			delete this;
		}
	}
	else
	{
		ProcessMessages(mb);
		mb.reset();
	}
}

//bool CBaseHandler::Send(const std::string& msg)
//{
//}

bool CBaseHandler::Send(CRomCsv& msg)
{
	ACE_Message_Block* l_mb;
	int size = msg.Message().size();
	ACE_NEW_NORETURN(l_mb, ACE_Message_Block(size));
	l_mb->copy(msg.Message().c_str(), size);
	bool rc = Write(*l_mb);
	//do log message
	return rc;
}

bool CBaseHandler::Write(ACE_Message_Block& mb)
{
	bool rc = false;
	if (this->m_writer.write(mb, mb.space()) == 0)
	{
		rc = true;
		IncrementWriteCnt();
	}
	return rc;
}

bool CBaseHandler::Read(ACE_Message_Block& mb)
{
	bool rc = false;
	if (this->m_reader.read(mb, mb.space()) == 0)
	{
		rc = true;
		IncrementReadCnt();
	}
	return rc;
}

bool CBaseHandler::InitRead()
{
	bool rc = true;

	for (int i = 0; i < g_MsgBlkNum; i++)
	{
		ACE_NEW_NORETURN(m_pBuffers[i], ACE_Message_Block(1024));
		if (Read(*m_pBuffers[i]) == false)
		{
			m_pBuffers[i]->release();
			rc = false;
			break;
		}
	}

	return rc;
}

void CBaseHandler::ProcessMessages(ACE_Message_Block& in_mb)
{
	char* ptrCur = in_mb.rd_ptr();
	int len = 0;
	int indexCur = 0;

	if (m_leftover.length() > 0)
	{
		m_leftover.append(ptrCur, in_mb.length());
		ptrCur = const_cast<char*>(m_leftover.c_str());
	}

	char* ptr = strchr(ptrCur, ROMVALUE::MESSAGE_END);
	while (ptr != NULL)
	{
		len = ptr - ptrCur;
		ProcessMessage(ptrCur, len);
		ptrCur = ptr + 1;
		indexCur += len + 1;
		ptr = strchr(ptrCur, ROMVALUE::MESSAGE_END);
	}

	if (indexCur + 1 == m_leftover.length())
	{
		m_leftover.clear();
	}
	else
	{
		m_leftover = m_leftover.substr(indexCur);
	}
}

