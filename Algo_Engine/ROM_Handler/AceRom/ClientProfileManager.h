#ifndef __CLIENTPROFILEMANAGER_H__
#define __CLIENTPROFILEMANAGER_H__

typedef struct
{
	long OrderChange;
	long ShareChange;
	double DollarChange;
} tBalanceUpdate;

class CClientProfileManager : public ACE_Task<ACE_MT_SYNCH>
{
public:
	CClientProfileManager();
	~CClientProfileManager();

	static CClientProfileManager & CClientProfileManager()
	{
		static CClientProfileManager theClientProfileManager;
		return theClientProfileManager;
	}

	bool AcceptOrder(CRomCsv* pCsv, std::string& errStr); //synch blocking call
	bool BalanceUpdate(tBalanceUpdate* pUpdate);//asynch

private:

};

#end //__CLIENTPROFILEMANAGER_H__