#include "stdafx.h"
#include "AdminThread.h"
#include "OatsServer.h"
#include "RomValueDefs.h"
#include "RomFieldDefs.h"
#include "RomCsv.h"
#include "OrderMsg.h"
#include "DBConnection.h"
#include "Config.h"
#include "AdminSessionCallBack.h"
#include "SecTypeFilter.h"
#include "AdminConnectionStateListener.h"

CAdminThread::CAdminThread() : m_SeqMap((CConfig::getInstance()).SeqMapFile(), (CConfig::getInstance()).SeqMapObj())
{
	m_AdminRquestEvt = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_wsaEvents[KILL_EVENT_INDEX] = Kill();
	m_wsaEvents[OATS_MESSAGE_EVENT_INDEX] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_wsaEvents[ADMINRESPONSE_MESSAGE_EVENT_INDEX] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	SetCheckTimer(CHECKDEADSESSION_EVENT_INDEX, 10000);
}

CAdminThread::~CAdminThread(void)
{
	CloseCsvFiles();
}

void CAdminThread::CloseCsvFiles()
{
	tLogMapIT it = m_LogMap.begin();
	while (it != m_LogMap.end())
	{
		delete it->second;
		it++;
	}

	m_LogMap.clear();
}


void CAdminThread::FreeAdminID(int id)
{
	m_DisconnectEventIndexList.push_back(id);
}

int CAdminThread::CreateAdminID()
{
	int rc = -1;
	if (m_DisconnectEventIndexList.empty() == false)
	{
		rc = m_DisconnectEventIndexList.front();
		m_DisconnectEventIndexList.pop_front();
	}
	return rc;
}


void CAdminThread::RetrieveAdminRequestMessages(std::list<CRomCsv*>& msgs)
{
	m_AdminRequestBuffer.RetriveBuffer(msgs);
}

void CAdminThread::AddAdminResponseMessages(std::list<CRomCsv*>& msgs)
{
	m_AdminResponseBuffer.AddBuffer(msgs);
	::SetEvent(m_wsaEvents[ADMINRESPONSE_MESSAGE_EVENT_INDEX]);
}

void CAdminThread::AddAdminResponseMessage(CRomCsv* pCsv)
{
	m_AdminResponseBuffer.AddBuffer(pCsv);
	::SetEvent(m_wsaEvents[ADMINRESPONSE_MESSAGE_EVENT_INDEX]);
}

void CAdminThread::ProcessMessage(std::string& l_message, CAdminSession* pC)
{
	std::stringstream stm;
	stm << "Received From Admin<ID="
		<< pC->ID()
		<< ">:"
		<< l_message;

	Logger::getInstance().LogMessage(stm.str());
	std::string recoverFile = "";
	int recoverOption = 0;

	CRomCsv* pCsv = new CRomCsv(l_message);

	if (pCsv->cGet(RFIX::CSV_COMMAND) != ROMVALUE::COMMAND_STATUS &&
		pCsv->cGet(RFIX::CSV_COMMAND) != ROMVALUE::COMMAND_CANCELS)
	{
		pCsv->SetAt(RFIX::CSV_CLIENT_FREAK, pC->ID());
	}

	CRomCsv l_Csv(pCsv->Message());

	if (pCsv->cGet(RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_LOGON)
	{
		int mode = pCsv->nGet(RFIX::CSV_INSTRUCTIONS);
		pC->SetMode(mode);
		std::string exlOwners = pCsv->Get(RFIX::CSV_EXCLUDED_OWNERS);
		std::string includedDests = pCsv->Get(RFIX::CSV_INCLUDED_DESTS);
		pC->SetIncludedDests(includedDests);
		pC->SetExecludedOwners(exlOwners);
		pC->SetLoggedOn(true);
	}
	else if (pCsv->cGet(RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_LOGOUT)
	{
		pC->SetLoggedOn(false);
	}
	else if (pCsv->cGet(RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_ADMIN &&
			 pCsv->nGet(RFIX::CSV_COMMAND_FREAK) == ROMVALUE::OATS_REVOVER)
	{
		recoverFile = pCsv->Get(RFIX::CSV_TEXT);
		recoverOption = pCsv->nGet(RFIX::CSV_RECOVER_OPTION);
	}

	if (recoverOption != 1 &&
		recoverOption != 2) //not csv file recover only
	{
		m_AdminRequestBuffer.AddBuffer(pCsv);
		::SetEvent(m_AdminRquestEvt);
	}

	if (2 == recoverOption ||
		4 == recoverOption)
	{
		l_Csv.SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

		if (ProcessCsvFullRecover(recoverFile))
		{
			l_Csv.SetAt(RFIX::CSV_TEXT, "CSV Files Recover Finished");
		}
		else
		{
			l_Csv.SetAt(RFIX::CSV_TEXT, "CSV Files Recover Failed");
		}

		std::string* pMsg = new std::string(l_Csv.Message());
		pC->Send(pMsg);
	}
	else if (1 == recoverOption ||
			 3 == recoverOption)
	{
		l_Csv.SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);

		if (ProcessCsvPartialRecover(recoverFile))
		{
			l_Csv.SetAt(RFIX::CSV_TEXT, "CSV Files Recover Finished");
		}
		else
		{
			l_Csv.SetAt(RFIX::CSV_TEXT, "CSV Files Recover Failed");
		}

		std::string* pMsg = new std::string(l_Csv.Message());
		pC->Send(pMsg);
	}
}

void CAdminThread::GetDateFromRomTag(const std::string& romTag, std::string& date)
{
	int index = romTag.find_last_of('-');
	date = std::string("20") + romTag.substr(index - 6, 6);
}

void CAdminThread::GetRomLoginFromRomTag(const std::string& romTag, std::string& romLogin)
{
	int index1 = romTag.find_last_of('-');
	int index2 = romTag.find('-');
	short romInstanceOffset = (CConfig::getInstance()).romInstanceOffset();
	int loginLen = index1 - romInstanceOffset - index2;
	romLogin = std::string("ROM") + romTag.substr(index2 + 1, loginLen);
}

bool CAdminThread::ProcessCsvFullRecover(const std::string& recoverFile)
{
	std::string sDate = "";
	std::string romLogin = "";
	std::string l_msg = "";
	char secType;
	bool rc = false;

	FILE* fp = fopen(recoverFile.c_str(), "r");
	if (fp != NULL)
	{
		char buffer[1024];

		while (fgets(buffer, 1024, fp) != NULL)
		{
			l_msg = buffer;

			if (l_msg.size() == 1 ||
				l_msg.size() == 0) //in case recover file has empty lines
			{
				continue;
			}

			if (buffer[0] == ROMVALUE::COMMAND_LOGON)
			{
				continue;
			}
			else if (buffer[0] != ROMVALUE::COMMAND_ORDER &&
					 buffer[0] != ROMVALUE::COMMAND_STATUS)
			{
				l_msg = l_msg.substr(20);
			}

			CRomCsv csv(l_msg);
			static bool first = true;
			if (first)
			{
				PreProcessCsvFullRecover(csv, romLogin, sDate);
				first = false;
			}
			secType = GetZLogSecType(csv);
			LogRecoverCsv(csv.Message(), secType, romLogin, sDate);
		}

		rc = true;
	}

	PostProcessCsvFullRecover();
	return rc;
}

bool CAdminThread::ProcessCsvPartialRecover(const std::string& recoverFile)
{
	bool rc = false;
	FILE* fp = fopen(recoverFile.c_str(), "r");
	if (fp != NULL)
	{
		char buffer[1024];
		std::string l_msg = "";
		std::string romLogin = "";
		char secType;

		while (fgets(buffer, 1024, fp) != NULL)
		{
			l_msg = buffer;

			if (l_msg.size() == 1 ||
				l_msg.size() == 0) //in case recover file has empty lines
			{
				continue;
			}

			if (buffer[0] == ROMVALUE::COMMAND_LOGON)
			{
				continue;
			}
			else if (buffer[0] != ROMVALUE::COMMAND_ORDER &&
					 buffer[0] != ROMVALUE::COMMAND_STATUS)
			{
				l_msg = l_msg.substr(20);
			}
						
			CRomCsv csv(l_msg);
			secType = GetZLogSecType(csv);
			GetRomLoginFromRomTag(csv.GetAt(RFIX::CSV_STAFFORD_TAG), romLogin);
			LogCsv(csv.Message(), secType, romLogin);
		}

		rc = true;
	}
	return rc;
}


void CAdminThread::PostProcessCsvFullRecover()
{
	CloseCsvFiles();
}

void CAdminThread::PreProcessCsvFullRecover(const CRomCsv& csv, std::string& romLogin, std::string& date)
{
	CloseCsvFiles();
	//rename the target directory
	GetDateFromRomTag(csv.GetAt(RFIX::CSV_STAFFORD_TAG), date);
	GetRomLoginFromRomTag(csv.GetAt(RFIX::CSV_STAFFORD_TAG), romLogin);
		
	char secStrings[5][10] = {"Equity", "Option", "Future", "Currency", "Complex"};
	
	for (int i = 0; i < 5; i++)
	{
		std::stringstream stm;
		stm << (CConfig::getInstance()).Drive() 
			<< "OATS"
			<< "\\"
			<< secStrings[i]
			<< "\\"
			<< date;
		
		std::string oldName = stm.str();
		std::string newName = stm.str() + std::string("_BAD");
		rename(oldName.c_str(), newName.c_str());
	}
}


void CAdminThread::InitServer()
{
	ZLog::CreateCurrentDayDirectories();

	//CDBConnection::getInstance().Open( (CConfig::getInstance()).DBServer(), (CConfig::getInstance()).DBLogin(), (CConfig::getInstance()).DBPassword(), (CConfig::getInstance()).Database() );

	m_pServerSock = new ServerSocket((CConfig::getInstance()).AdminServerIP(), (CConfig::getInstance()).AdminServerPort());

	BOOL bound = m_pServerSock->Bind();
	if(bound) 
	{
		m_wsaEvents[SERVER_LISTENING_EVENT_INDEX] = m_pServerSock->CreateAcceptEvent();
	} 
	else 
	{
		m_wsaEvents[SERVER_LISTENING_EVENT_INDEX] = WSACreateEvent();
	}

	for (int i = MIN_ADMINDISCONNECT_EVENT_INDEX; i <= MAX_ADMINDISCONNECT_EVENT_INDEX; i++)
	{
		m_wsaEvents[i] = WSACreateEvent();
		m_DisconnectEventIndexList.push_back(i);
	}
}

void CAdminThread::Run()
{
	InitServer();
	bool bLive = true;

	while(bLive) 
	{
		DWORD waitResult = WSAWaitForMultipleEvents(MAX_ADMINDISCONNECT_EVENT_INDEX + 1, m_wsaEvents, FALSE, 10000, TRUE);

		switch(waitResult) 
		{
		case WAIT_IO_COMPLETION:
			{
				break;
			}
		case WSA_WAIT_TIMEOUT:
			{
				break;
			}
		default:
			{
				DWORD result = waitResult - WSA_WAIT_EVENT_0;
				bool noReset = false;

				switch(result) {
				case SERVER_LISTENING_EVENT_INDEX:
					{
						SOCKET sock = m_pServerSock->Accept();
						int evtIndex = -1;
						noReset = true;
						while (sock != INVALID_SOCKET)
						{
							CAdminSession* pC = new CAdminSession(sock);

							CAdminConnectionStateListener<CAdminSession>* pStateListener = new CAdminConnectionStateListener<CAdminSession>(pC, &CAdminSession::OnSend);
							pC->SetSocketStateListener(pStateListener);


							evtIndex = CreateAdminID();
							pC->SetWSAEvent(m_wsaEvents[evtIndex], FD_CLOSE);
							CAdminSessionCallBack<CAdminThread, CAdminSession>* pCallBack = new CAdminSessionCallBack<CAdminThread, CAdminSession>(this, pC);
							pC->SetListener(pCallBack);
							pC->SetID(evtIndex);
							m_AdminSessionMap.insert(std::make_pair<int, CAdminSession*>(evtIndex, pC));

							sock = m_pServerSock->Accept();
							
							std::string IP;
							pC->GetPeerIP(IP);

							std::stringstream stm;
							stm << "OrderMaster Connected from "
								<< IP
								<< "\n";
							(Logger::getInstance()).LogMessage(stm.str());
							std::cerr<< stm.str().c_str() << std::endl;
						}
						break;
					}
                case OATS_MESSAGE_EVENT_INDEX:
                    {
						noReset = true;
                       	ProcessMessages();
                        break;
                    }
				case CHECKDEADSESSION_EVENT_INDEX:
					{
						noReset = true;
						DeleteDeadAdminSessions(); //Where to put this???
						break;
					}
				case ADMINRESPONSE_MESSAGE_EVENT_INDEX:
					{
						noReset = true;
						ProcessAdminResponses();
						break;
					}
				case KILL_EVENT_INDEX:
					{
						noReset = true;
						bLive = false;
						break;
					}
				default:
					{
						if (result <= MAX_ADMINDISCONNECT_EVENT_INDEX &&
							result >= MIN_ADMINDISCONNECT_EVENT_INDEX)
						{
							// OrderMaster disconnect
							WSACloseEvent(m_wsaEvents[result]);
							FreeAdminID(result);
							m_wsaEvents[result] = WSACreateEvent();
							tAdminSessionMapIT it = m_AdminSessionMap.find(result);
							if (it != m_AdminSessionMap.end())
							{
								CAdminSession* pC = (*it).second;

								if (pC->LoggedOn() == true)
								{
									CRomCsv* pCsv = new CRomCsv();
									pCsv->SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_LOGOUT);
									m_AdminRequestBuffer.AddBuffer(pCsv);
									WSASetEvent(m_AdminRquestEvt);
								}
								
								std::string IP;
								pC->GetPeerIP(IP);
								std::stringstream stm;
								stm << "OrderMaster Disconnected from "
									<< IP
									<< "\n";

								std::cerr<< stm.str().c_str() <<std::endl;
								(Logger::getInstance()).LogMessage(stm.str());
								m_AdminSessionMap.erase(it);
								CloseAdminSession(pC);
							}
						}
				
						break;
					}
				}
				
				if (false == noReset &&
					result <= MAX_ADMINDISCONNECT_EVENT_INDEX &&
					result >= 0)
				{
					WSAResetEvent(m_wsaEvents[result]);
				}

				break;
			}
		}
	}

    Shutdown();
}


void CAdminThread::ProcessAdminResponses()
{
	std::list<CRomCsv*> msgs;
	
	m_AdminResponseBuffer.RetriveBuffer(msgs);
	CRomCsv* pCsv = NULL;

	while (msgs.empty() == false)
	{
		pCsv = msgs.front();
		msgs.pop_front();
		ProcessAdminResponse(*pCsv);
		delete pCsv;
	}
}

void CAdminThread::ProcessAdminResponse(CRomCsv& csv)
{
	if (csv.IsFieldEmpty(RFIX::CSV_CLIENT_FREAK) != true)
	{
		int adminID = csv.nGet(RFIX::CSV_CLIENT_FREAK);
		tAdminSessionMapIT it = m_AdminSessionMap.find(adminID);
		if (it != m_AdminSessionMap.end())
		{
			CAdminSession* pC = it->second;
			std::stringstream stm;
			stm << "Sending admin response to Admin<ID="
				<< pC->ID()
				<< ">:"
				<< csv.Message();
			(Logger::getInstance()).LogMessage(stm.str());
			std::string* pMsg = new std::string(csv.Message());
			pC->Send(pMsg);
		}
	}
	else //broadcast
	{
		tAdminSessionMapIT it = m_AdminSessionMap.begin();
		while (it != m_AdminSessionMap.end())
		{
			CAdminSession* pC = it->second;
			std::stringstream stm;
			stm << "Sending admin response to Admin<ID="
				<< pC->ID()
				<< ">:"
				<< csv.Message();
			(Logger::getInstance()).LogMessage(stm.str());
			
			std::string* pMsg = new std::string(csv.Message());
			pC->Send(pMsg);

			it++;
		}
	}
}
void CAdminThread::Shutdown()
{
	//clean up admin sessions
	tAdminSessionMapIT it = m_AdminSessionMap.begin();
	while (it != m_AdminSessionMap.end())
	{
		delete (*it).second;
		it++;
	}
	m_AdminSessionMap.clear();
}

void CAdminThread::CloseAdminSession(CAdminSession* pC)
{
	pC->Close();
	m_DeadAdminSessionList.push_back(pC);
}

void CAdminThread::DeleteDeadAdminSessions()
{
	CAdminSession* pC = NULL;
	while (m_DeadAdminSessionList.empty() != true)
	{
		pC = m_DeadAdminSessionList.front();
		delete pC;
		m_DeadAdminSessionList.pop_front();
	}
}

void CAdminThread::AddMessages(const std::string& msgs)
{
	m_sMsgBuffer.AddBuffer(msgs);
	::SetEvent(m_wsaEvents[OATS_MESSAGE_EVENT_INDEX]);
}

void CAdminThread::ProcessMessages()
{
	std::string msgs;

	m_sMsgBuffer.RetriveBuffer(msgs);

	int index = msgs.find(MESSAGE_END);
	int currOff = 0, len = 0;
	std::string msg;
	CRomCsv* pCsv = NULL;
	
	tMsgMap msgMap;
	tMsgMapIT it;
	std::string* pMsgs = NULL;
	char secType;

	std::string romLogin;

	while (index != std::string::npos)
	{
		len = index - currOff;
		msg = msgs.substr(currOff, len);
		currOff = index + 1;

		if (ROMVALUE::COMMAND_ORDER == msg[0] ||
			ROMVALUE::COMMAND_STATUS == msg[0])
		{
			CRomCsv csv(msg);
			csv.SetAt(RFIX::CSV_OATSMSG_SEQ, m_SeqMap.NextSequence());
			ProcessOatsMessage(csv);

			char key[20];
			secType = GetZLogSecType(csv);
			GetRomLoginFromRomTag(csv.GetAt(RFIX::CSV_STAFFORD_TAG), romLogin);
			sprintf(key, "%s%c", romLogin.c_str(), secType);
			it = msgMap.find(key);
			if (it == msgMap.end())
			{
				pMsgs = new std::string;
				msgMap.insert(make_pair<std::string, std::string*>(key, pMsgs));
			}
			else
			{
				pMsgs = it->second;
			}
			pMsgs->append(csv.Message());
		}
		else if (ROMVALUE::COMMAND_RESPONSETOADMIN == msg[0])
		{
			pCsv = new CRomCsv(msg);
			ProcessAdminResponse(*pCsv);
			delete pCsv;
		}

		index = msgs.find(MESSAGE_END, currOff);
	}

	int size = 0;
	std::string sKey;
	it = msgMap.begin();
	while (it != msgMap.end())
	{
		sKey = it->first;
		size = sKey.length();
		secType = sKey[size - 1];
		romLogin = sKey.substr(0, size - 1);
		pMsgs = it->second;
		LogCsv(*pMsgs, secType, romLogin);
		it++;
		delete pMsgs;
	}
}

void CAdminThread::ProcessOatsMessage(CRomCsv& csv)
{
	tAdminSessionMapIT it = m_AdminSessionMap.begin();
	CAdminSession* pC = NULL;

	//csv.SetAt(RFIX::CSV_OATSMSG_SEQ, m_SeqMap.NextSequence());
	while (it != m_AdminSessionMap.end())
	{
		pC = it->second;
		if (pC->OwnerIncluded(csv.GetAt(RFIX::CSV_OWNER)) == true && 
			pC->DestIncluded(csv.nGet(RFIX::CSV_ORDERDEST)) == true &&
			(CSecTypeFilter::IsComplexOrder(csv) == true ||
			 CSecTypeFilter::Block(csv.cGet(RFIX::CSV_SECURITY_TYPE), pC->Mode()) == false))
		{
			std::string* pMsg = new std::string(csv.Message());
			pC->Send(pMsg);
		}
		it++;
	}
}



void CAdminThread::ProcessAdminResponses(std::list<CRomCsv*>& csvList)
{
	CRomCsv* pCsv = NULL;

	while (csvList.empty() == false)
	{
		pCsv = csvList.front();
		csvList.pop_front();

		if (pCsv->IsFieldEmpty(RFIX::CSV_CLIENT_FREAK) != true)
		{
			int adminID = pCsv->nGet(RFIX::CSV_CLIENT_FREAK);
			tAdminSessionMapIT it = m_AdminSessionMap.find(adminID);
			if (it != m_AdminSessionMap.end())
			{
				CAdminSession* pC = it->second;
				std::stringstream stm;
				stm << "Sending admin response to Admin<ID="
					<< pC->ID()
					<< ">:"
					<< pCsv->Message();
				(Logger::getInstance()).LogMessage(stm.str());
				std::string* pMsg = new std::string(pCsv->Message());
				pC->Peek(pMsg);
			}
		}
		else //broadcast
		{
			tAdminSessionMapIT it = m_AdminSessionMap.begin();
			while (it != m_AdminSessionMap.end())
			{
				CAdminSession* pC = it->second;
				std::stringstream stm;
				stm << "Sending admin response to Admin<ID="
					<< pC->ID()
					<< ">:"
					<< pCsv->Message();
				(Logger::getInstance()).LogMessage(stm.str());

				std::string* pMsg = new std::string(pCsv->Message());
				pC->Peek(pMsg);

				it++;
			}
		}

		delete pCsv;
	}
}

void CAdminThread::LogCsv(const std::string& msgs, char secType, std::string romLogin)
{
	char key[20];
	sprintf(key, "%s%c", romLogin.c_str(), secType);

	tLogMapIT it = m_LogMap.find(key);
	ZLog* pLog = NULL;

	if (it == m_LogMap.end())
	{
		pLog = new ZLog(secType, romLogin);
		m_LogMap.insert(make_pair<std::string, ZLog*>(key, pLog));
	}
	else
	{
		pLog = it->second;
	}

	pLog->Log(msgs);
}

void CAdminThread::LogRecoverCsv(const std::string& msgs, char secType, std::string romLogin, std::string date)
{
	static bool firstOne = true;

	if (firstOne)
	{
		CloseCsvFiles();
		firstOne = false;
	}

	char key[20];
	sprintf(key, "%s%c", romLogin.c_str(), secType);

	tLogMapIT it = m_LogMap.find(key);
	ZLog* pLog = NULL;

	if (it == m_LogMap.end())
	{
		pLog = new ZLog(secType, romLogin, date);
		m_LogMap.insert(make_pair<std::string, ZLog*>(key, pLog));
	}
	else
	{
		pLog = it->second;
	}

	pLog->Log(msgs);
}


char CAdminThread::GetZLogSecType(CRomCsv& csv)
{
	char rc = ZLog::EQUITY;

	if (CSecTypeFilter::IsComplexOrder(csv) == true)
	{
		return ZLog::COMPLEX;
	}
	
	char value = csv.cGet(RFIX::CSV_SECURITY_TYPE);

	if (COrderMsg::IsFutureType(value))
	{
		rc = ZLog::FUTURE;
	}
	else if (COrderMsg::IsOptionType(value))
	{
		rc = ZLog::OPTION;
	}
	else if (COrderMsg::IsCurrencyType(value))
	{
		rc = ZLog::CURRENCY;
	}
	else if (COrderMsg::IsComplexType(value))
	{
		rc = ZLog::COMPLEX;
	}

	return rc;
}

void CAdminThread::SetCheckTimer(DWORD index, long period)
{
	m_wsaEvents[index] = CreateWaitableTimer(NULL, FALSE, NULL);
	_int64 ns = -period * 10000;
	LARGE_INTEGER l;
	l.QuadPart = ns;
	SetWaitableTimer(m_wsaEvents[index], &l, period, NULL, NULL, FALSE);
}


