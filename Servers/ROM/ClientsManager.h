#ifndef __CLIENTSMANAGER_H__
#define __CLIENTSMANAGER_H__

//#include "RomMap.h"
#include "RomMapN.h"
#include "client_manager.h"
#include "Csv.h"
#include "ClientConnection.h"
#include "romvaluedefs.h"
#include "romfielddefs.h"
#include "ROMTypedef.h"
#include "dart_connection_object.h"
#include "ClientProfileManagerNew.h"
#include "ThreadSet.h"
#include "romtimer.h"
#include <set>

class CClientsManager
{
public:

    typedef struct PWDCHG
    {
        std::string login;
        std::string newPassword;
        std::string logonMsg;
    } tPWDCHG;

    ~CClientsManager();

    static CClientsManager & getInstance()
    {
        static CClientsManager theManager;
        return theManager;
    }

    static void MessageCallback(struct message* m, void* ob);
    static void LogonCallback(con_obj* co, int is_loggedon,char* log, int len);
    static void DummyLowActivityCallback(con_obj* obj) {}
    static char* ResetPad(unsigned long seq, char* version, size_t* len)
    {
        return NULL;
    }

    static void* ValidateNewPassword(void* pData);
    //static bool ProcessHeartBeat(time_t now, void* obj);
    static void ProcessHeartBeat(std::set<void*>& objs);

    bool Send(const std::string& login, CCsv& msg, std::string& err);
    bool Send(const std::string& login, char* pMsg, unsigned long len, std::string& err);
    void BroadCast(const std::string& msg);
    void SetBlocked(const std::string& login, bool value);
    void UpdateConnection(const std::string& login, int sock, int updateCode, const std::string& logonMsg);
    //void UpdateValidLogins(ROMTYPEDEF::tLoginList& logins);
    //void BuildValidLogins(ROMTYPEDEF::tLoginList& logins, queue_t loginQ);
    void BuildValidLogins(std::set<CTraderData*>& data, queue_t loginQ);
    void GetLoggedOnLogins(std::string& logins);
    void Disconnect(const std::string& login);
    bool Find(const std::string& login, bool& loggedOn);
    bool Find(const std::string& login);
    void GetAllConnections(std::list<CClientConnection*>& conList);
    bool GetConnection(const std::string& login, CClientConnection*& con);
    //bool AddClient(const std::string& login, const std::string& password, int* msgrateLimit);
    bool AddClient(CTraderData& data);
    bool StartService();
    //void AddNewClient(const std::string& name, const std::string& pwd, int* msgrateLimit);
    void AddNewClient(CTraderData& data);
    void UpdateClientPassword(const std::string& name, const std::string& pwd);
    const async_server_con_set* asyncServer()
    {
        return m_pAsyncServer;
    }

    bool SetMsgRateExempt(const std::string& login, bool exempt);
    bool UpdateMsgRateLimits(const std::string& trader, int* limits);

    void ProcessHeartBeat(time_t now);
    void UpdateHB(const CTraderData& data);
    void StartMDS();
    
private:
    CClientsManager();
    struct init_args* CreateClientIniArg(const ROMTYPEDEF::tLoginStruct& login, bool newcon);
    void DestroyClientIniArg(struct init_args* pArg);
    CClientConnection* FindConnection(const std::string& login);
    
    void ProcessInvalidPasswordUpdate(const std::string& logonMsg);
    void ProcessConfirmPasswordUpdate(const std::string& logonMsg);

    CRomMap<CClientConnection*> m_Connections;
    //CThreadSet<std::string> m_Relogins;

    async_server_con_set* m_pAsyncServer; //not used for sync server
    struct async_parse_args* m_AsyncArgs; //for the particular listener
    std::set<CClientConnection*> m_hbClients;
    pthread_spinlock_t m_hb_lock;
    CRomTimer* m_hbTimer;
};

#endif //__CLIENTSMANAGER_H__
