#include "ClientHandler.h"
#include "ClientsManager.h"
#include "OrderManager.h"

CClientHandler::CClientHandler()
{
	
}

CClientHandler::~CClientHandler()
{
}


void CClientHandler::open(ACE_HANDLE h, ACE_Message_Block& mb)
{
	this->handle(h);
	
	if (this->m_reader.open(*this) != 0 ||
		this->m_writer.open(*this) != 0)
	{
		ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) %p\n"), ACE_TEXT("CClientHandler.open")));
		delete this;
		return;
	}

	//notify Oats thread there is a client connection
	//get IP/Port and call AddHandler using IP/Port as the key

	//issure a read
	InitRead();
}

void CClientHandler::RemoveHandler()
{
	CClientsManager::getInstance().RemoveHandler(ID());
	//notify oats thread
}

/*
bool CClientHandler::Send(CRomCsv& msg)
{
	ACE_Message_Block* l_mb;
	int size = msg.Message().size();
	ACE_NEW_NORETURN(l_mb, ACE_Message_Block(size));
	l_mb->copy(msg.Message().data());
	bool rc = Write(*l_mb);
	//do log message
	return rc;
}
*/
void CClientHandler::ProcessMessage(const char* pMsg, int size)
{
	CRomCsv* pCsv = new CRomCsv(pMsg, size);
	if (LoggedOn())
	{
		COrderManager::getInstance().AddClientRequest(pCsv);
	}
	else
	{
		pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_LOGONREJECT);
		pCsv->SetAt(ROMFIELDS::CSV_TEXT, "Can not process message until client loggedOn");
		Send(*pCsv);
		delete pCsv;
		//reject the message
	}
}
