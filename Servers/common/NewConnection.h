#ifndef _CONNECTION_H__
#define _CONNECTION_H__

#include "pthread.h"
#include <string>
#include <list>
#include "client_manager.h"
#include "NewCsv.h"

class CNewConnection
{
public:

#ifdef _WIN32
    typedef int async_server_con_set;
#endif

    static void OnSend(const char* name, int name_len, void* pass_back);
    static bool sendMsg(struct message& m);
    
    CNewConnection(const std::string& name, 
                const std::string& sender, 
                bool immediate, 
                bool doSequencingm, 
                bool relogon,
                bool doNotRecord = false,
                void (*rejectFunc)(std::list<struct message*>&) = NULL,
                unsigned int tryCnt = 100000);
    ~CNewConnection();

    //It is assumed that msg is wraped with '\n'!!!
    virtual bool SendCmd(const std::string& msg, bool doRollingLog = true);

    inline bool LoggedOn()
    {
        pthread_rwlock_rdlock(&m_lock);
        bool rc = m_loggedOn;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline bool Connected()
    {
        pthread_rwlock_rdlock(&m_lock);
        bool rc = m_connected;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline const std::string& IP()
    {
        pthread_rwlock_rdlock(&m_lock);
        const std::string& rc = m_ip;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline unsigned short Port()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned short rc = m_port;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline bool Blocked()
    {
        pthread_rwlock_rdlock(&m_lock);
        bool rc = m_blocked;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline bool relogon()
    {
        return m_relogon;
    }

    inline void SetRelogon(bool value)
    {
        m_relogon = value;
    }

    void SetLoggedOn(bool value);

    inline void SetIP(const std::string& value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_ip = value;
        pthread_rwlock_unlock(&m_lock);
    }

    inline void SetPORT(unsigned short value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_port = value;
        pthread_rwlock_unlock(&m_lock);
    }

    inline void SetBlocked(bool value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_blocked = value;
        pthread_rwlock_unlock(&m_lock);
    }

    inline void SetDoSequencing(bool value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_doSequencing = value;
        pthread_rwlock_unlock(&m_lock);
    }

    inline void SetAsyncServer(async_server_con_set* pAsyncServer)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_pAsyncServer = pAsyncServer;
        pthread_rwlock_unlock(&m_lock);
    }
    
    inline void SetTryCnt(unsigned int value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_tryCnt = value;
        pthread_rwlock_unlock(&m_lock);
    }
    
    inline const std::string& Name()const
    {
        return m_name;
    }
    
    inline const std::string& Description()
    {
        return m_description;
    }
    inline void SetDescription(const std::string& value)
    {
        m_description = value;
    }
   
    void SetIpPort(int sock);
    bool Send(char* msg, unsigned long len, bool doRollingLog = true, bool lock = true); //null terminated c string is assumed!!!
    void kickoff_connection(char *name, size_t name_len);
    const std::string& Sender() { return m_sender; }
    void SetConnected(bool value);
    void OnSend();
    bool SendWritev(tWriteV& wvc);
    bool SendWritev_a(tWriteV& wvc, int& vecs_sent, int& offset);
    void SetSocket(int value) { m_sock = value; }
    
private:
    bool Send(struct message* m, bool lock = true);
    void CopyMessage(struct message& target, const struct message& source);
    struct message* CreateMsg(struct message& msg);
    static void DestroyMsg(struct message* pMsg);
    void SendQueuedMessages();  //for nonimmediate sending only!!
    void RecordSentMessage(struct message* m);
    bool SendImmediately(struct message* pM);
    bool SendNoneImmediately(struct message* pM);
    bool SendImmediately_a(struct message* pM);
    bool SendNoneImmediately_a(struct message* pM);
    void DestroyMsgQ_a();
     
    void SendQueuedMessages_a();
    //bool Send_a(struct message* pM);
    int send_a(struct message* pM);
    unsigned long send(struct message* pM);
    
    //void GetLeftover(tIovec* msg, size_t bytesSent, std::string& leftover);
    void GetLeftover(tWriteV& wvc, size_t curVec, int offset, std::string& leftover);
    
    std::string m_name;      //can be client login or destination name
    std::string m_msgQ;      //holding msgs for client of no sequencing
    std::string m_description;
    std::string m_sender;
    std::string m_ip;
    unsigned short m_port;
    unsigned int m_tryCnt;
    int  m_sock;
    bool m_blocked;
    bool m_connected;
    bool m_doSequencing;
    bool m_immediate;
    bool m_doNotRecord;
    bool m_loggedOn;
    bool m_relogon;
    bool m_readyToSend;
    std::list<struct message*> m_tmpQ;      //holding msgs whose resend log has to be delayed
    std::list<struct message*> m_tmsgQ_a;    //holding msgs in would block condition
    struct message* m_pMsg_a;               //holding partial message
    struct message* m_pMsgNotCompleted_a;   //holding the not yet recorded message 
                                            
    async_server_con_set* m_pAsyncServer;
    void (*m_rejectFunc)(std::list<struct message*>&);
    pthread_rwlock_t m_lock;
};

#endif //_CONNECTION_H__
