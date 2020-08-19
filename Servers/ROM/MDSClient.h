#ifndef _MDSCLIENT_H__
#define _MDSCLIENT_H__

#include "Utility.h"
#include "dart_connection_object.h"
#include "ThreadValue.h"
#include "common_structs.h"
#include "RomMapN.h"
#include "romtimer.h"
#include "RomOrderNew.h"

typedef struct
{
    long bid;
    long offer;
    bool exempted;
} tBestOfferBid;

static const char CMD_LOGON = 'L';
static const char CMD_DATA = 'S';
static const char CMD_HB = 'H';
static const char CMD_ONOPEN_DATA = 'O';

class CMDSClient
{
public:

    static CMDSClient & getInstance()
    {
        static CMDSClient theObj;
        return theObj;
    }

    //static char sBuffer[128];
    //static char* p;
    //static size_t bytes_missing;

    static void DummyLowActivityCallback(con_obj* obj) {}
    static unsigned long parse_message(const char *message,
                                            unsigned long len,
                                            int *cut_con, struct parse_con *pc);
    static unsigned long Dummybuild_offsets(   char *data,
                                        unsigned long size,
                                        unsigned long byte_offset,
                                        unsigned long *seq_num);
    static void ConnectCallback(con_obj* co, int is_connected,char* log,
                                int len);
    static void MessageCallback(struct message* m, void* ob);
    static void DummyOnSend(const char* name, int name_len, void* pass_back) {}
    static int send_logon(struct async_parse_args *pc);
    static int send_logon(struct parse_args *pc);
    static int send_logon(parse_con* con);
    //static bool reconnect_hb(time_t now, void* obj);
    static void reconnect_hb(std::set<void*>& objs);
    //bool ValidateOptionPrice(bool buy, const double& price, const double& nbbo, std::string& err);
    static void StartConnect(std::set<void*>& objs);
    static void EndConnect(std::set<void*>& objs);
    static bool NeedValidatePrice(const CRomOrderNew& order, bool bReplace);

    ~CMDSClient();

    void init();
    //bool ValidatePrice(const std::string& symbol, bool buy, long price, const char* owner, bool field39, std::string& err);
    bool ValidatePrice(const CRomOrderNew& orderl, bool bReplace, long price, bool field39, std::string& err);
    //bool ValidateOptionPrice(const std::string& trader, bool buy, const double& price, const std::string& nbbo, std::string& err);
    bool ValidateOptionPrice(const CRomOrderNew& order, bool bReplace, const std::string& nbbo, std::string& err);
    
    bool Connected();
    void ProcessData(const std::string& symbol, const tBestOfferBid& ob, bool onopen);
    void ProcessDisconnect();
    //void ProcessConnect(con_obj* con);
    //void ProcessHB();
    void SetCon(con_obj* con, bool connectedBefore);
    void SendHB();
    bool SendCmd(char type);
    void Reconnect_HB();
    void SetUnResponsive(bool value);
    void SetExempted(const std::string& symbol, bool value);
    void SetConnecting(bool value);
    bool Connecting();
      
    bool ShouldLive() { return m_shouldlive; }
    void SetShouldLive(bool value) { m_shouldlive = value; }
    
    void SetTraderExempt(const std::string& trader, bool bExempted);
    bool OptionExempted(const std::string& trader);

    double GetEquityMidPrice(const std::string& symbol);
    
private:

    CMDSClient();
    void Connect();
    bool send_to_mds(char *data, unsigned long len);
    void drop_connection();
    void DumpData();
    void Load();
    bool EmailFlag();
    void SetEmailFlag(bool value);
    
    CRomMap<tBestOfferBid> m_bestOfferBid;
    CRomMap<tBestOfferBid> m_bestOfferBid_OnOpen;
    std::set<std::string> m_OptionExemptedTraders;
    CThreadValue<bool> m_unresponsive;
    struct init_args m_args;
    con_obj* m_con;
    bool m_equityExempted;
    bool m_optionExempted;
    bool m_connecting;
    bool m_shouldlive;
    bool m_emailFlag;
    
    CRomTimer* m_pConnectTimer;
    CRomTimer* m_pDisconnectTimer;
    pthread_spinlock_t m_Lock;
    pthread_spinlock_t m_exemptLock;
    pthread_spinlock_t m_emailLock;
    CRomTimer* m_hbTimer;
};






#endif //_MDSCLIENT_H__
