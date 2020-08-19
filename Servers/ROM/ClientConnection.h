#ifndef __CLIENTCONNECTION_H__
#define __CLIENTCONNECTION_H__

#include "Connection.h"
#include "RomCount.h"
#include "ThreadValue.h"
#include "romvaluedefs.h"
#include <sys/time.h>

class CRomOrderNew;

class CMsgLimit
{
public:
    CMsgLimit();
    ~CMsgLimit();

    bool IncrementCntHit();
    int rateLimit();
    void Set(int value);
    const std::string& CurViolationTime() { return m_curViolationTime; }
    const std::string& LastResetTime() { return m_lastResetTime; }
    
private:
    //time_t m_time;
    struct timeval m_tval;
    int m_cnt;
    int m_rateLimit;
    std::string m_lastResetTime;
    std::string m_curViolationTime;
    pthread_spinlock_t m_lock;
};

class CClientConnection : public CConnection
{
public:

    static char s_SECSTR[ROMVALUE::SECNUM][20];

    CClientConnection(const std::string& login,
                      const std::string& password,
                      int* msgrateLimit);

    ~CClientConnection();

    //struct init_args* args() { return &m_args; }
    int CancelOnDisconnect()
    {
        return m_CancelOnDisconnect;
    }
    void SetCancelOnDisconnect(int value)
    {
        m_CancelOnDisconnect = value;
    }

    void SetPassword(const std::string& value)
    {
        m_password = value;
    }

    const std::string& password() { return m_password; }

    bool IncrementIncomingCntHit(unsigned short secIndex);
    //bool hitLimit(unsigned short secIndex);
    bool DoMsgRateControl(unsigned short secIndex)
    {
        return (m_exempted || m_msgLimit[secIndex].rateLimit() == -1)? false : true;
    }

    int msgrateLimit(unsigned short secIndex)
    {
        return m_msgLimit[secIndex].rateLimit();
    }

    void SetMsgRateLimitExempt(bool value)
    {
        m_exempted = value;
    }

    bool msgrateExempted()
    {
        return m_exempted;
    }

    bool IncrementMsgCntCheck(unsigned short secIndex, std::string& err);

    void UpdateMsgRateLimits(int* limit);
    
private:

    void GetMsgRateViolationError(std::string& err, unsigned short secIndex, int rate);

    int m_CancelOnDisconnect;
    std::string m_password;

    CMsgLimit m_msgLimit[ROMVALUE::SECNUM];
    bool m_exempted;
};

#endif //__CLIENTCONNECTION_H__
