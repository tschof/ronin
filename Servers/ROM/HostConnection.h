#ifndef __HOSTCONNECTION_H__
#define __HOSTCONNECTION_H__

#include "Connection.h"
#include "client_manager.h"

class CHostConnection : public CConnection
{
public:
    CHostConnection(const std::string& id,
                    const std::string& name,
                    bool immediate,
                    bool doQueueing,
                    void (*RejectCallBack)(std::list<struct message*>&));

    ~CHostConnection();

    inline unsigned short CloseTime()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned short rc = m_closetime;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline unsigned short OpenTime()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned short rc = m_opentime;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline unsigned short EndWDay()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned short rc = m_end_wday;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline unsigned short DeathTime()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned short rc = m_deathtime;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }


    void SetCloseTime(unsigned short value);
    void SetOpenTime(unsigned short value);
    void SetDeathTime(unsigned short value);
    inline bool Connecting()
    {
        pthread_rwlock_rdlock(&m_lock);
        bool rc = m_connecting;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline bool ExchangeLoggedOn()
    {
        pthread_rwlock_rdlock(&m_lock);
        bool rc = m_exchangeLoggedOn;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline void SetExchangeLoggedOn(bool value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_exchangeLoggedOn = value;
        pthread_rwlock_unlock(&m_lock);
    }

    inline void SetConnecting(bool value)
    {
        pthread_rwlock_wrlock(&m_lock);

        if (value == true)
        {
            m_connectingCnt++;
        }
        m_connecting = value;

        pthread_rwlock_unlock(&m_lock);
    }

    //inline void SetBlockFlip(bool value)
    //{
    //    pthread_rwlock_wrlock(&m_lock);
    //    m_blockFlip = value;
    //    pthread_rwlock_unlock(&m_lock);
    //}

    inline void SetLastLogonTime(time_t value)
    {
        pthread_rwlock_wrlock(&m_lock);
        (m_args.reset_time)->last_logon = value;
        pthread_rwlock_unlock(&m_lock);
    }

    //inline bool BlockFlip()
    //{
    //    pthread_rwlock_rdlock(&m_lock);
    //    bool rc = m_blockFlip;
    //    pthread_rwlock_unlock(&m_lock);
    //    return rc;
    //}

    inline unsigned int connectingCnt()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned int rc = m_connectingCnt;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }



    inline void SetServerResetDay(unsigned short wDay)
    {
        pthread_rwlock_wrlock(&m_lock);
        (m_args.reset_time)->reset_day = wDay;
        pthread_rwlock_unlock(&m_lock);
    }

    inline void SetEndWDay(unsigned short wDay)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_end_wday = wDay;
        pthread_rwlock_unlock(&m_lock);
    }

    inline unsigned short ResetDay()
    {
        pthread_rwlock_rdlock(&m_lock);
        unsigned short rc = (m_args.reset_time)->reset_day;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline bool SessionShouldHaveEnded()
    {
        pthread_rwlock_rdlock(&m_lock);
        bool rc = m_sessionShouldHaveEnded;
        pthread_rwlock_unlock(&m_lock);
        return rc;
    }

    inline void SetSessionShouldHaveEnded(bool value)
    {
        pthread_rwlock_wrlock(&m_lock);
        m_sessionShouldHaveEnded = value;
        pthread_rwlock_unlock(&m_lock);
    }

    struct init_args* args();

    //bool newCon()
    //{
    //    return m_newCon;
    //}
    void SetConnected(bool value);
    bool ShouldDestUp(time_t now);
    bool ShouldDestUp();
    void SetRelogon(bool value);
    
private:
    void CreateArg();
    void DeleteArg();

    bool m_connecting;
    //bool m_blockFlip;
    bool m_exchangeLoggedOn;
    bool m_sessionShouldHaveEnded;   //set by the session ending timer
    unsigned int m_connectingCnt;
    unsigned short m_opentime;
    unsigned short m_closetime;
    unsigned short m_end_wday;
    unsigned short m_deathtime;

    struct init_args m_args;
    pthread_rwlock_t m_lock;
};

#endif //__HOSTCONNECTION_H__
