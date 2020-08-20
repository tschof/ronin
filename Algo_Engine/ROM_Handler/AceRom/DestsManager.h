#ifndef __DESTSMANAGER_H__
#define __DESTSMANAGER_H__

#include "BaseManager.h"
#include "Desthandler.h"

//class CDestsManager : public CBaseManager
class CDestsManager : public CBaseManager<CDestHandler>
{
public:

	static CDestsManager & getInstance()
	{
		static CDestsManager destsManager;
		return destsManager;
	}

	CDestsManager();
	~CDestsManager();

	virtual int svc();

	void OnDestConnect(CDestHandler* pHandler);
	void OnDestLogonAck(const std::string& login, CDestHandler* pHandler);

private:

	

};

#endif //__DESTSMANAGER_H__
