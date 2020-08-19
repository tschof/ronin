#include "HostConnection.h"
#include "Config.h"
#include "Utility.h"

#include "ClientsManager.h"

CHostConnection::CHostConnection(const std::string& id,
                                 const std::string& name,
                                 bool immediate,
                                 bool doQueueing,
                                 void (*RejectCallBack)(std::list<struct message*>&))
: CConnection(id, CConfig::getInstance().InstanceName(), immediate, doQueueing, false, RejectCallBack)
{
    m_sessionShouldHaveEnded = false;
    m_closetime = 0;
    m_opentime = 0;
    m_end_wday = 0;
    //m_blockFlip = false;
    m_connecting = false;
    m_connectingCnt = 0;
    m_exchangeLoggedOn = true; //before all the exchange interfaces are updated to always
                               //send logon update.  After that the default value should be set
                               //false.
    SetTryCnt(CConfig::getInstance().SendTryCnt());
    SetDescription(name);
    CreateArg();
    SetAsyncServer(const_cast<async_server_con_set*>(CClientsManager::getInstance().asyncServer()));
    pthread_rwlock_init(&m_lock, NULL);
}

CHostConnection::~CHostConnection(void)
{
    DeleteArg();
    pthread_rwlock_destroy(&m_lock);
}

void CHostConnection::SetCloseTime(unsigned short value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_closetime = value;
    pthread_rwlock_unlock(&m_lock);
}

void CHostConnection::SetOpenTime(unsigned short value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_opentime = value;
    unsigned short lhour;
    CUtility::ConvertHourFromCentralToLocal(m_opentime / 100, lhour);
    (m_args.reset_time)->reset_hour = lhour;
    (m_args.reset_time)->reset_min = m_opentime % 100;
    pthread_rwlock_unlock(&m_lock);
}

void CHostConnection::SetDeathTime(unsigned short value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_deathtime = value;
    pthread_rwlock_unlock(&m_lock);
}

void CHostConnection::SetRelogon(bool value)
{
    (m_args.reset_time)->create_new = !value;
}

void CHostConnection::CreateArg()
{
    memset(&m_args, 0, sizeof(struct init_args));
    m_args.name_len = Description().size();
    m_args.name = (char*)calloc(1, m_args.name_len + 1);
    strcpy(m_args.name, Description().c_str());

    m_args.p_len = CConfig::getInstance().Login().size();
    m_args.password = (char*)calloc(1, m_args.p_len + 1);
    strcpy(m_args.password, CConfig::getInstance().Login().c_str());
    m_args.un_len = m_args.p_len;
    m_args.username = (char*)calloc(1, m_args.un_len + 1);
    strcpy(m_args.username, CConfig::getInstance().Login().c_str());

    if (strcmp(m_args.name, "OATS") != 0)
    {
        m_args.cancel_only = 1;
    }

    m_args.log_outgoing = 1;

    m_args.reset_time = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
    (m_args.reset_time)->new_incoming = 0;
    (m_args.reset_time)->new_outgoing = 0;
    (m_args.reset_time)->last_logon = 0;
    (m_args.reset_time)->create_new = true;
    //(m_args.reset_time)->reset_hour = m_opentime / 100;
    //(m_args.reset_time)->reset_min = m_opentime % 100;

    /*(m_args.reset_time)->create_new = !relogon();
    if (!relogon())
    {
        (m_args.reset_time)->reset_hour = CUtility::GetLocalTimeHH();
        (m_args.reset_time)->reset_min = CUtility::GetTimeMM() - 1;
    }
    else
    {
        (m_args.reset_time)->reset_hour = 23;
        (m_args.reset_time)->reset_min = 59;
    }*/
}

bool CHostConnection::ShouldDestUp()
{
    bool rc = true;

    //if blocking flag has been overridden to false ROM should keep connecting
    if (SessionShouldHaveEnded() && Blocked())
    {
        rc = false;
    }

    return rc;
}

bool CHostConnection::ShouldDestUp(time_t now)
{
    bool rc = false;

    pthread_rwlock_rdlock(&m_lock);

    //if ((m_args.reset_time)->reset_day == 7)
    if (EndWDay() == 7)
    {
        struct tm tmNow, tmCentralTime;
        localtime_r(&now, &tmNow);
        CUtility::AdjustToCentralTime(tmNow, tmCentralTime);
        int intNow = tmCentralTime.tm_hour * 100 + tmCentralTime.tm_min;

        if (m_closetime > m_opentime)
        {
            if (intNow >= m_opentime && intNow < m_deathtime)
            {
                rc = true;
            }
        }
        else //cross calender day session
        {
            if (intNow >= m_opentime || intNow < m_deathtime)
            {
                rc = true;
            }
        }
    }
    else
    {
        rc = true;
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CHostConnection::DeleteArg()
{
    free(m_args.name);
    free(m_args.password);
    free(m_args.username);
    free(m_args.reset_time);
}

void CHostConnection::SetConnected(bool value)
{
    CConnection::SetConnected(value);
}

struct init_args* CHostConnection::args()
{
    (m_args.reset_time)->create_new = !CUtility::IsRelogonToWeeklyServer((m_args.reset_time)->reset_day,
                                                                         m_opentime,
                                                                         (m_args.reset_time)->last_logon);
    return &m_args;
}