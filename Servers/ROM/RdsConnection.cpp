#include "RdsConnection.h"
#include "Config.h"

CRdsConnection::CRdsConnection() :
CConnection("RDS", CConfig::getInstance().InstanceName(), true, false, false)
{
    m_connecting = false;
    SetDescription("RDS");
    SetArgs();
    int rdsPort = CConfig::getInstance().rdsPort();
    const std::string& rdsIP = CConfig::getInstance().rdsIP();
    SetIP(rdsIP);
    SetPORT(rdsPort);
}

CRdsConnection::~CRdsConnection()
{
    if (m_args.name)
    {
        free (m_args.name);
    }
    
    if (m_args.password)
    {
        free (m_args.password);
    }
    
    if (m_args.reset_time)
    {
        free (m_args.reset_time);
    }
}

void CRdsConnection::SetArgs()
{
    memset(&m_args, 0, sizeof(struct init_args));
    m_args.name_len = 3;
    m_args.name = (char*)calloc(1, 4);
    strcpy(m_args.name, "RDS");
    
    const std::string& name = CConfig::getInstance().InstanceName();
   
    m_args.p_len = name.size();
    m_args.password = (char*)calloc(1, m_args.p_len + 1);
    strcpy(m_args.password, name.c_str());
    m_args.un_len = m_args.p_len;
    m_args.username = (char*)calloc(1, m_args.un_len + 1);
    strcpy(m_args.username, name.c_str());
    m_args.cancel_only = 0;
    m_args.log_outgoing = 1;

    m_args.reset_time = (struct seq_reset_time*)calloc(1, sizeof(struct seq_reset_time));
    (m_args.reset_time)->new_incoming = 0;
    (m_args.reset_time)->new_outgoing = 0;
    (m_args.reset_time)->last_logon = 0;
    (m_args.reset_time)->create_new = true;
}