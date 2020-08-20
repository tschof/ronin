#include "stdafx.h"
#include "OatsServer.h"
#include <iostream>
#include "Logger.h"
#include "RomSessionCallBack.h"
#include "RomValueDefs.h"
#include "RomFieldDefs.h"
#include "Config.h"
#include "DBConnection.h"
#include "psapi.h"

#include <time.h>

COatsServer::COatsServer(void)
{
	m_pAdminThread = NULL;
	m_pDBThread = NULL;
	m_adminCount = 0;	
	m_romCount = 0;
	//m_rvdOatsMsgCount = 0;
}

COatsServer::~COatsServer(void)
{
}

bool COatsServer::InitServer()
{
	bool rc = true;

	m_pAdminThread = new CAdminThread();
	m_wsaEvents[ADMIN_EVT_INDEX] = m_pAdminThread->AdminRquestEvt();
	SetEndTimeCheckTimer();
	m_pAdminThread->Start();

	m_pDBThread = new CDBThread();
	m_pDBThread->Start();

	for (int i = MIN_ROM_DISCONNECT_EVT_INDEX; i <= MAX_ROM_DISCONNECT_EVT_INDEX; i++)
	{
		m_EvtIndexList.push_back(i);
		m_wsaEvents[i] = WSACreateEvent();
	}

	m_pServerSock = new ServerSocket((CConfig::getInstance()).RomServerIP(), (CConfig::getInstance()).RomServerPort());

	BOOL bound = m_pServerSock->Bind();
	if(bound) 
	{
		m_wsaEvents[SERVER_EVT_INDEX] = m_pServerSock->CreateAcceptEvent();
	} 
	else 
	{
		std::string errMsg = "Bind() failed in COatsServer::InitServer()";
		std::cerr << errMsg.c_str() << std::endl;
		(Logger::getInstance()).LogError("Bind() failed in COatsServer::InitServer()");
		rc = false;
	}

	return rc;
}

void COatsServer::Run()
{
	m_bLive = true;

	if (InitServer() == false)
	{
		Shutdown();
		return;
	}
	
	while(m_bLive) 
	{
		DWORD waitResult = WSAWaitForMultipleEvents(MAX_ROM_DISCONNECT_EVT_INDEX + 1, m_wsaEvents, FALSE, 10000, TRUE);

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
				case 0:
					{
						SOCKET sock = m_pServerSock->Accept();
						int evtIndex = -1;
						noReset = true;

						while (sock != INVALID_SOCKET)
						{
							CRomSession* pC = new CRomSession(sock);
							evtIndex = GetDisconnectEvtIndex();

							if (evtIndex == -1)
							{
								std::cerr << "No enough Disconnect event available" << std::endl;
								Logger::getInstance().LogError("No enough Disconnect event available");
								delete pC;
							}
							else
							{
								pC->SetWSAEvent(m_wsaEvents[evtIndex], FD_CLOSE);
								CRomSessionCallBack<COatsServer, CRomSession>* pCallBack = new CRomSessionCallBack<COatsServer,CRomSession>(this,pC);
								pC->SetListener(pCallBack);
								m_RomDisconnectEvtIndexToSessionMap.insert(make_pair<int, CRomSession*>(evtIndex, pC));
								sock = m_pServerSock->Accept();
							}
						}
						break;
					}
                case ADMIN_EVT_INDEX:
                    {
						noReset = true;
                        ProcessAdminMessages();
                        break;
                    }
				case CHECK_ENDTIME_INDEX:
					{
						DeleteDeadRomSessions();

						//In case there are still some messages not sent to the MSQ yet
						m_pDBThread->CheckDumpRequests();

						if (SessionEnd())
						{
							m_bLive = false;
						}
						
						//Send warning email if too many messages are queued up
						//CheckQueueRatio();
						CheckQueueSize();

						break;
					}
				default:
					{
						if (result <= MAX_ROM_DISCONNECT_EVT_INDEX &&
							result >= MIN_ROM_DISCONNECT_EVT_INDEX)
						{
							// ROM disconnect
							CRomSession* pC = RemoveRomSessionFromMaps(result);
							std::cerr<< "ROM" << pC->Login() << " disconnected"<<std::endl;
							WSACloseEvent(m_wsaEvents[result]);
							FreeDisconnectEvtIndex(result);
							m_wsaEvents[result] = WSACreateEvent();
							//CRomSession* pC = RemoveRomSessionFromMaps(result);
							if (pC)
							{
								if (pC->LoggedOn() == true)
								{
									m_romCount--;
									pC->SetLoggedOn(false);
									std::stringstream stm;
									stm << "ROM"
										<< pC->Login()
										<< " Disconnected from Oats";
									(CConfig::getInstance()).SendEmail(stm.str(), stm.str());
								}

								UpdateRomInstances();
								CloseRomSession(pC);
							}
						}

						break;
					}
				}
				
				if (false == noReset &&
					result <= MAX_ROM_DISCONNECT_EVT_INDEX &&
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

void COatsServer::Shutdown()
{
	//(CConfig::getInstance()).UpdateRunDate();
	m_pDBThread->Stop(3000);
	m_pAdminThread->Stop(3000);
}

CRomSession* COatsServer::RemoveRomSessionFromMaps(int index)
{
	CRomSession* pC = NULL;
	tRomSessionMapIT it = m_RomDisconnectEvtIndexToSessionMap.find(index);

	if (it != m_RomDisconnectEvtIndexToSessionMap.end())
	{
		pC = it->second;
		m_RomDisconnectEvtIndexToSessionMap.erase(it);

		if (pC->Login() != -1)
		{
			it = m_RomLoginToSessionMap.find(pC->Login());
			if (it != m_RomLoginToSessionMap.end())
			{
				m_RomLoginToSessionMap.erase(it);
			}
		}
	}

	return pC;
}

int COatsServer::ProcessMessage(std::string& l_message, CRomSession* pC)
{
	int rc = 0;
	std::string msgs;

 	int index = l_message.find_last_of(MESSAGE_END);

	if (index != std::string::npos)
	{
		rc = index + 1;
		msgs = l_message.substr(0, rc);

		if (ROMVALUE::COMMAND_LOGON == l_message[0])
		{
			CRomCsv csv(l_message);
			
			int id = csv.nGet(RFIX::CSV_TRADER);
			unsigned long lastIncomingSeq = GetLastIncmingSeq(id);
			pC->SetLogin(id, lastIncomingSeq);
			//Need to handle duplicated login later
			m_RomLoginToSessionMap.insert(make_pair<int, CRomSession*>(id, pC));
			m_romCount++;
			pC->SetLoggedOn(true);
			
			CRomCsv logon;
			logon.SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_LOGON);

			if (pC->LastIncomingSeq())
			{
				logon.SetAt(RFIX::CSV_LASTRECEIVED_SEQ, pC->LastIncomingSeq());
			}
			
			pC->Send(logon.Message());
		
			UpdateRomInstances();

			std::stringstream stm;
			stm << "ROM"
				<< pC->Login()
				<< " Logged on:"
				<< logon.Message();

			Logger::getInstance().LogMessage(stm.str());
			std::cerr << stm.str().c_str() << std::endl;
		}
		else
		{
			if (msgs.empty() == false)
			{
				m_pAdminThread->AddMessages(msgs);
				if ((CConfig::getInstance()).dbEnabled() == true)
				{
					unsigned int numMsgs = GetOatsMsgCnt(msgs);
					pC->UpdateLastIncomingSeq(numMsgs);
					m_pDBThread->AddMessages(msgs);
				}
			}
		}
	}

	return rc;
}
/*
	std::list<CRomCsv*> oatsAdminMsgs, adminResponses;
	std::list<CQueueRequest*> oatsDBMsgs;

	while(index != std::string::npos) 
	{           
		int len = index - currOff;
		std::string l_sub = l_message.substr(currOff, len);
		CRomCsv* pCsv = new CRomCsv(l_sub);

		if (pCsv->cGet(RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_RESPONSETOADMIN)
		{
			std::stringstream stm;
			stm << "Received admin response from ROM"
				<< pC->Login()
				<< ":"
				<< pCsv->Message();
			Logger::getInstance().LogMessage(stm.str());

			adminResponses.push_back(pCsv);
		}
		else if (pCsv->cGet(RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_LOGON)
		{
			int id = pCsv->nGet(RFIX::CSV_TRADER);
			pC->SetLogin(id);
			m_RomLoginToSessionMap.insert(make_pair<int, CRomSession*>(id, pC));
			m_romCount++;
			pC->SetLoggedOn(true);

			pC->Send(pCsv->Message());

			UpdateRomInstances();

			std::stringstream stm;
			stm << "ROM"
				<< pC->Login()
				<< " Logged on";
			Logger::getInstance().LogMessage(stm.str());

			delete pCsv;
		}
		else if (pCsv->cGet(RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_LOGOUT)
		{
			pC->SetLoggedOn(false);
			m_romCount--;
			UpdateRomInstances();

			std::stringstream stm;
			stm << "ROM"
				<< pC->Login()
				<< " Logged off";
			Logger::getInstance().LogMessage(stm.str());

			delete pCsv;
		}
		else
		{
			m_rvdOatsMsgCount++;
			oatsAdminMsgs.push_back(pCsv);

			if ((CConfig::getInstance()).dbEnabled() == true)
			{
				CQueueRequest* pMsg = new COrderMsg(pCsv);
				oatsDBMsgs.push_back(pMsg);
			}
		}

		currOff = index + 1;
		index = l_message.find(MESSAGE_END, currOff);
	}

	if (oatsAdminMsgs.empty() == false)
	{
		m_pAdminThread->AddOatsMessages(oatsAdminMsgs);
	}

	if (oatsDBMsgs.empty() == false)
	{
		m_pDBThread->AddMessages(oatsDBMsgs);
	}

	if (adminResponses.empty() == false)
	{
		m_pAdminThread->AddAdminResponseMessages(adminResponses);
	}

	return currOff;
}*/


void COatsServer::ProcessAdminMessages()
{
	std::string l_messages;
	std::list<CRomCsv*> msgs;
	CRomCsv* pCsv = NULL;

	m_pAdminThread->RetrieveAdminRequestMessages(msgs);
	while (msgs.empty() == false)
	{
		pCsv = msgs.front();
		ProcessAdminMessage(pCsv);
		msgs.pop_front();
		delete pCsv;
	}
}

void COatsServer::ProcessAdminMessage(CRomCsv* pCsv)
{
	char cmd = pCsv->cGet(RFIX::CSV_COMMAND);

	if (cmd == ROMVALUE::COMMAND_ADMIN)
	{
		if (pCsv->nGet(RFIX::CSV_COMMAND_FREAK) == ROMVALUE::QUERY_OATS)
		{
			ProcessQuery(pCsv);
		}
		else if (pCsv->nGet(RFIX::CSV_COMMAND_FREAK) == ROMVALUE::OATS_REVOVER)
		{
			ProcessRecover(pCsv);
		}
		else
		{
			if (pCsv->IsFieldEmpty(RFIX::CSV_SERVER_FREAK) == false) // unicast
			{
				int romID = pCsv->nGet(RFIX::CSV_SERVER_FREAK);
				SendToRom(pCsv->Message(), romID);
			}
			else // broadcaset
			{
				SendToRom(pCsv->Message());
			}
		}
	}
	else if (cmd == ROMVALUE::COMMAND_LOGON)
	{
		UpdateRomInstances(pCsv->nGet(RFIX::CSV_CLIENT_FREAK));
		m_adminCount++;
	}
	else if (cmd == ROMVALUE::COMMAND_LOGOUT)
	{
		m_adminCount--;
	}
	else
	{
		if (cmd == ROMVALUE::COMMAND_CANCEL ||
			cmd == ROMVALUE::COMMAND_CANCELS ||
			cmd == ROMVALUE::COMMAND_STATUS ||
			cmd == ROMVALUE::COMMAND_ZAP)
		{
			std::string romTag = pCsv->GetAt(RFIX::CSV_STAFFORD_TAG);
			if (romTag.empty() == false)
			{
				int romID = GetRomIDFromRomTag(romTag);
				SendToRom(pCsv->Message(), romID);
			}
			else
			{
				SendToRom(pCsv->Message());
			}
		}
		else if (cmd == ROMVALUE::COMMAND_RELOADDESTCONFIG)
		{
			int romID = pCsv->nGet(RFIX::CSV_SERVER_FREAK);
			SendToRom(pCsv->Message(), romID);
		}
		else if (cmd == ROMVALUE::COMMAND_TEXT)
		{
			//Broad cast text to all clients
			SendToRom(pCsv->Message());
		}
	}
}

void COatsServer::UpdateRomInstances(int adminID)
{
	CRomCsv* pCsv = new CRomCsv;
	pCsv->SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
	pCsv->SetAt(RFIX::CSV_COMMAND_FREAK, ROMVALUE::ROM_INSTANCES_UPDATE);

	if (adminID != -1)
	{
		pCsv->SetAt(RFIX::CSV_CLIENT_FREAK, adminID);
	}

	std::stringstream stm;

	tRomSessionMapIT it = m_RomLoginToSessionMap.begin();

	while (it != m_RomLoginToSessionMap.end())
	{
		CRomSession* pC = it->second;
		if (pC->LoggedOn())
		{
			int id = it->first;
			it++;

			if (it == m_RomLoginToSessionMap.end())
			{
				stm << id;
			}
			else
			{
				stm << id
					<< '|';
			}
		}
	}

	pCsv->SetAt(RFIX::CSV_ROM_INSTANCES, stm.str());
	m_pAdminThread->AddAdminResponseMessage(pCsv);
}

void COatsServer::SendToRom(std::string& msg, int romID)
{
	tRomSessionMapIT it;
	
	std::string header = "From Admin to ROM<ID=";
	if (romID != 0)
	{
		it = m_RomLoginToSessionMap.find(romID);
		if (it != m_RomLoginToSessionMap.end())
		{
			CRomSession* pC = it->second;
			pC->Send(msg);

			std::stringstream stm;
			stm << "From Admin to ROM<ID="
				<< romID
				<< ">:"
				<< msg.c_str();

			Logger::getInstance().LogMessage( stm.str() );
		}
	}
	else
	{
		it = m_RomLoginToSessionMap.begin();
		while (it != m_RomLoginToSessionMap.end())
		{
			CRomSession* pC = it->second;
			pC->Send(msg);

			std::stringstream stm;
			stm << "From Admin to ROM<ID="
				<< pC->Login()
				<< ">:"
				<< msg.c_str();

			Logger::getInstance().LogMessage( stm.str() );
			it++;
		}
	}
}

int COatsServer::GetRomIDFromRomTag(const std::string& romTag)
{
	int index1 = romTag.find_last_of('-');
	int index2 = romTag.find('-');
	short romInstanceOffset = (CConfig::getInstance()).romInstanceOffset();
	int len = index1 - index2 - romInstanceOffset;
	std::string sub = romTag.substr(index2 + 1, len);
	return atoi(sub.c_str());
}

int COatsServer::GetDisconnectEvtIndex()
{
	int rc = -1;
	if (m_EvtIndexList.empty() != true)
	{
		rc = m_EvtIndexList.front();
		m_EvtIndexList.pop_front();
	}
	return rc;
}

void COatsServer::FreeDisconnectEvtIndex(int index)
{
	m_EvtIndexList.push_back(index);
}

void COatsServer::ProcessQuery(CRomCsv* pCsv)
{
	CRomCsv* l_pCsv = new CRomCsv(pCsv->Message());
	l_pCsv->SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
	l_pCsv->SetAt(RFIX::CSV_COMMAND_FREAK, ROMVALUE::QUERY_OATS);
	l_pCsv->SetAt(RFIX::CSV_CLIENT_FREAK, pCsv->nGet(RFIX::CSV_CLIENT_FREAK));

	std::stringstream stm;
	stm << m_romCount
		<< '|'
		<< m_adminCount
		<< '|'
		<< m_pDBThread->GetReceivedCnt()
		<< '|'
		<< m_pDBThread->GetInsertedCnt()
		<< '|'
		<< m_pDBThread->GetProcessedCnt();

	l_pCsv->SetAt(RFIX::CSV_TEXT, stm.str());
	m_pAdminThread->AddAdminResponseMessage(l_pCsv);
}

void COatsServer::ProcessRecover(CRomCsv* pCsv)
{
	std::string fileName = pCsv->Get(RFIX::CSV_TEXT);

	FILE* fp = fopen(fileName.c_str(), "r");
	if (fp != NULL)
	{
		fclose(fp);
		m_pDBThread->AddMessages(pCsv->Message());
		m_pDBThread->StartRecovering();
		
		while (m_pDBThread->FinishedRecovering() == false)
		{
			Sleep(100);
		}

		pCsv->SetAt(RFIX::CSV_TEXT, "DB Recover Finished");
	}
	else
	{
		pCsv->SetAt(RFIX::CSV_TEXT, "DB Recover Failed");
	}

	//notify recovering is finished.
	pCsv->SetAt(RFIX::CSV_COMMAND, ROMVALUE::COMMAND_RESPONSETOADMIN);
	Logger::getInstance().LogMessage(pCsv->Message());
	m_pAdminThread->AddMessages(pCsv->Message());
}

void COatsServer::CloseRomSession(CRomSession* pC)
{
	pC->Close();
	m_DeadRomSessionList.push_back(pC);
}


void COatsServer::DeleteDeadRomSessions()
{
	CRomSession* pC = NULL;

	while (m_DeadRomSessionList.empty() != true)
	{
		pC = m_DeadRomSessionList.front();
		delete pC;
		m_DeadRomSessionList.pop_front();
	}
}

void COatsServer::SetEndTimeCheckTimer()
{
	m_wsaEvents[CHECK_ENDTIME_INDEX] = CreateWaitableTimer(NULL, FALSE, NULL);
	_int64 ns = -20000 * 10000;
	LARGE_INTEGER l;
	l.QuadPart = ns;
	SetWaitableTimer(m_wsaEvents[CHECK_ENDTIME_INDEX], &l, 20000, NULL, NULL, FALSE);
}

bool COatsServer::SessionEnd()
{
	bool rc = false;

	time_t t = time(NULL);
	tm * pDT = localtime( &t );

    DWORD now = pDT->tm_hour * 100 + pDT->tm_min; 

	if (now >= (CConfig::getInstance()).EndTime())
	{
		//Make sure that no messages are in the queue
		long nQueued = m_pDBThread->GetQueuedCnt();
		if (0 == nQueued)
		{
			rc = true;
		}
		else
		{
			static int cnt = 0;

			if (cnt < 5)
			{
				std::string subject = "Oats Warning";
				std::stringstream stm;
				stm << "Oats End Time has been reached but there are still "
					<< nQueued 
					<< " messages queued";
				(CConfig::getInstance()).SendEmail(subject, stm.str());
				cnt++;
			}
		}
	}

	return rc;
}


void COatsServer::CheckQueueRatio()
{
	static unsigned short step = 1;
	long nReceived = m_pDBThread->GetReceivedCnt();
	long nProcessed = m_pDBThread->GetProcessedCnt();

	if (0 == nReceived || 0 == nProcessed)
	{
		return;
	}

	unsigned short curRatio = (nReceived - nProcessed) / nProcessed;
	if (curRatio / (CConfig::getInstance()).WarningQueueRatio() >= step)
	{
		std::string subject = "Oats Warning";
		std::stringstream stm;
		stm << "The ratio of received messages and processed messages is greater than "
			<< step
			<< ". Please contact DBA!!";
		step++;
		(CConfig::getInstance()).SendEmail(subject, stm.str());
	}
}

void COatsServer::CheckQueueSize()
{
	static unsigned short step = 1;
	//long curQueueSize = m_pDBThread->GetReceivedCnt() - (CDBConnection::getInstance()).GetProcessedCnt();
	long curQueueSize = m_pDBThread->GetQueuedCnt();
	unsigned short ratio = curQueueSize / (CConfig::getInstance()).WarningQueueSize();

	if (ratio >= step)
	{
		step = ratio + 1;

		std::string subject = "Oats Warning";
		std::stringstream stm;
		stm << curQueueSize
			<< " Messages have been queued. DB insertion may be slow. "
			<< "Please contact DBA!!";

		(CConfig::getInstance()).SendEmail(subject, stm.str());
	}
}

unsigned int COatsServer::GetOatsMsgCnt(std::string& msgs)
{
	unsigned int rc = 0;
	unsigned int offset = 0;

	unsigned int index = msgs.find(MESSAGE_END, offset);

	while (index != std::string::npos)
	{
		if (msgs[offset] == 'S' ||
			msgs[offset] == 'E')
		{
			rc++;
		}
		offset = index + 1;
		index = msgs.find(MESSAGE_END, offset);
	}

	return rc;
}


unsigned long COatsServer::GetLastIncmingSeq(int romInstance)
{
	std::string secStr[5] = {"Equity", "Option", "Future", "Currency", "Complex"};
	unsigned long rc = 0, seq = 0;
	std::string filename = "";

	for (int i = 0; i < 5; i++)
	{
		std::stringstream stm;
		stm << "Current ROM"
			<< romInstance
			<< " "
			<< secStr[i]
			<< " CSV File";

		(CConfig::getInstance()).GetConfig("Settings", stm.str(), filename);
		seq = GetLastIncomingSeqFromFile(filename);
		if (seq > rc)
		{
			rc = seq;
		}
	}

	return rc;
}

unsigned long COatsServer::GetLastIncomingSeqFromFile(const std::string& filename)
{
	unsigned long rc = 0;

	char	CurrentDate[128];
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	sprintf(CurrentDate, "%04u-%02u-%02u", systime.wYear, systime.wMonth, systime.wDay);
	std::string datestr = CurrentDate;

	if (filename.find(datestr) != std::string::npos)
	{
		FILE* fp = NULL;

		char buffer[1000];
		char* pTmp = NULL;

		memset(buffer, 0, sizeof(buffer));

		fp = fopen(filename.c_str(), "r");
		if (fp != NULL)
		{
			fseek(fp, 0, SEEK_END);
			int ps = ftell(fp);
			int offset = ps - sizeof(buffer), sizeToRead = sizeof(buffer);

			if (ps < sizeof(buffer)) // in case the data are smaller than sizeof(buffer)
			{
				offset = 0;
				sizeToRead = ps;
			}

			fseek(fp, offset, SEEK_SET);
			fread(buffer, sizeToRead, 1, fp);
			pTmp = strrchr(buffer, '\n');
			
			if (pTmp != NULL)
			{
				*pTmp = '\0';
				pTmp = strrchr(buffer, '\n');

				if (pTmp != NULL)
				{
					std::string sMsg = pTmp + 1;
					CRomCsv csv(sMsg);
					rc = csv.nGet(RFIX::CSV_OUTGOING_SEQ);
				}
			}

			fclose(fp);
		}
	}

	return rc;
}
