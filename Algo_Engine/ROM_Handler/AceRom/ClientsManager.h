#ifndef __CLIENTSMANAGER_H__
#define __CLIENTSMANAGER_H__

#include "BaseManager.h"
#include "Clienthandler.h"

class CClientsManager : public CBaseManager<CClientHandler>
//class CClientsManager : public CBaseManager
{
public:

	static CClientsManager & getInstance()
	{
		static CClientsManager clientManager;
		return clientManager;
	}

	CClientsManager();
	~CClientsManager();

	virtual int svc();

	bool AddLogin(const CRomMessage& login, CClientHandler* pHandler);
	bool Send(const std::string& receiver, CRomCsv& msg, std::string& err);

private:

};

#endif //__CLIENTSMANAGER_H__
