#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include "ClientConnection.h"
#include "Utility.h"
#include "ClientsManager.h"
#include "Config.h"

char CClientConnection::s_SECSTR[ROMVALUE::SECNUM][20] = {"EQUITY", "OPTION", "FUTURE", "OPTIONONFUTURE"};

CClientConnection::CClientConnection(const std::string& login,
                                     const std::string& password,
                                     int* msgrateLimit)
: CConnection(login, CConfig::getInstance().InstanceName(), false, false)
{
    for (int i = 0; i < ROMVALUE::SECNUM; i++)
    {
        m_msgLimit[i].Set(msgrateLimit[i]);
    }
    SetTryCnt(CConfig::getInstance().SendTryCnt());
    m_password = password;
    SetDescription(login);
    m_CancelOnDisconnect = 0;
    m_exempted = false;
}

CClientConnection::~CClientConnection(void)
{
}

bool CClientConnection::IncrementIncomingCntHit(unsigned short secIndex)
{
    return m_msgLimit[secIndex].IncrementCntHit();
}

//bool CClientConnection::hitLimit(unsigned short secIndex)
//{
//    return m_msgLimit[secIndex].hit();
//}

bool CClientConnection::IncrementMsgCntCheck(unsigned short secIndex, std::string& err)
{
    bool rc = true;

    if (DoMsgRateControl(secIndex))
    {
        if (IncrementIncomingCntHit(secIndex))
        {
            GetMsgRateViolationError(err, secIndex, msgrateLimit(secIndex));
            rc = false;
        }
    }

    return rc;
}

void CClientConnection::GetMsgRateViolationError(std::string& err, unsigned short secIndex, int rate)
{
    char buf[256];
    sprintf(buf, "Dart:Client %s's MAX %s Message rate %d per second "
            "has been exceeded", 
            Name().c_str(), s_SECSTR[secIndex], 
            rate);
    err = buf;
}

void CClientConnection::UpdateMsgRateLimits(int* limit)
{
    for (int i = 0; i < ROMVALUE::SECNUM; i++)
    {
        m_msgLimit[i].Set(limit[i]);
    }
}

CMsgLimit::CMsgLimit()
{
    pthread_spin_init(&m_lock, PTHREAD_PROCESS_PRIVATE);
    m_rateLimit = 0;
    m_cnt = 0;
    //m_time = 0;
    m_tval.tv_sec = 0;
    m_tval.tv_usec = 0;
}

CMsgLimit::~CMsgLimit()
{
    pthread_spin_destroy(&m_lock);
}

bool CMsgLimit::IncrementCntHit()
{
    bool rc = false;

    pthread_spin_lock(&m_lock);

    if (0 == m_tval.tv_sec)
    {
        gettimeofday(&m_tval, NULL);
        CUtility::GetGmtTimeFromTimeval(m_lastResetTime, m_tval);
    }

    m_cnt++;

    if (m_cnt > m_rateLimit)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        if (tv.tv_sec == m_tval.tv_sec ||
            (tv.tv_sec == m_tval.tv_sec + 1 &&
             tv.tv_usec <= m_tval.tv_usec))
        {
            CUtility::GetGmtTimeFromTimeval(m_curViolationTime, tv);
            rc = true;
        }
        else
        {
            m_cnt = 0;
            m_tval = tv;
            CUtility::GetGmtTimeFromTimeval(m_lastResetTime, m_tval);
        }
    }

    pthread_spin_unlock(&m_lock);

    return rc;
}

int CMsgLimit::rateLimit()
{
    pthread_spin_lock(&m_lock);
    int rc = m_rateLimit;
    pthread_spin_unlock(&m_lock);

    return rc;
}

void CMsgLimit::Set(int value)
{
    pthread_spin_lock(&m_lock);
    m_rateLimit = value;
    pthread_spin_unlock(&m_lock);
}