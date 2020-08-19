#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "Utility.h"
#include "romvaluedefs.h"
#include "NewConnection.h"
#include "romvaluedefs.h"
#include "client_manager.h"
#include "client_manager.h"

CNewConnection::CNewConnection(const std::string& name, 
                         const std::string& sender, 
                         bool immediate, 
                         bool doSequencing, 
                         bool relogon,
                         bool doNotRecord,
                         void (*rejectFunc)(std::list<struct message*>&),
                         unsigned int tryCnt)
{
    m_doSequencing = doSequencing;
    m_immediate = immediate;
    m_loggedOn = false;
    m_blocked = false;
    m_connected = false;
    m_name = name;
    m_relogon = relogon;
    m_pAsyncServer = NULL;
    m_readyToSend = true;
    pthread_rwlock_init(&m_lock, NULL);
    m_pMsg_a = NULL;
    m_pMsgNotCompleted_a = NULL;
    m_sender = sender;
    m_rejectFunc = rejectFunc;
    m_tryCnt = tryCnt;
    m_doNotRecord = doNotRecord;
    m_sock = -1;
}

CNewConnection::~CNewConnection()
{
    if (m_pMsg_a)
    {
        DestroyMsg(m_pMsg_a);
    }
    
    if (m_pMsgNotCompleted_a)
    {
        DestroyMsg(m_pMsgNotCompleted_a);
    }
    
    struct message* pMsg = NULL;
    
    if (m_tmsgQ_a.empty() == false)
    {
        std::list<struct message*>::iterator it = m_tmsgQ_a.begin();
        while (it != m_tmsgQ_a.end())
        {
            pMsg = *it;
            DestroyMsg(pMsg);
            it++;
        }
    }
    
    if (m_tmpQ.empty() == false)
    {
        std::list<struct message*>::iterator it = m_tmpQ.begin();
        while (it != m_tmpQ.end())
        {
            pMsg = *it;
            DestroyMsg(pMsg);
            it++;
        }
    }
    
    pthread_rwlock_destroy(&m_lock);
}

bool CNewConnection::SendCmd(const std::string& msg, bool doRollingLog)
{
    char* pMsg = const_cast<char*>(msg.c_str());;
    return Send(pMsg, msg.length(), doRollingLog);
}

bool CNewConnection::Send(char* pMsg, unsigned long len, bool doRollingLog, bool lock)
{
    struct message m;
    m.name = const_cast<char*>(m_description.c_str());
    m.name_len = m_description.size();
    m.data = pMsg;
    m.len = len;
    
    if (doRollingLog)
    {
        dart_rolling_log_going(&m, m_sender.c_str(), m_sender.length());
    }

    return Send(&m, lock);
}


bool CNewConnection::Send(struct message* m, bool lock)
{  
    bool rc = false;
    
    if (lock)
    {
        pthread_rwlock_wrlock(&m_lock);
    }
    
    if (m_immediate)
    {
        if (NULL == m_pAsyncServer)
        {
            rc = SendImmediately(m);
        }
        else
        {
            rc = SendImmediately_a(m);
        }
    }
    else
    {
        if (NULL == m_pAsyncServer)
        {
            rc = SendNoneImmediately(m);
        }
        else
        {
            rc = SendNoneImmediately_a(m);
        }
    }

    if (lock)
    {
        pthread_rwlock_unlock(&m_lock);
    }
    
    return rc;
}

bool CNewConnection::SendNoneImmediately(struct message* pM)
{
    bool rc = false;
    struct message m;
    CopyMessage(m, *pM);
    
    if (m_loggedOn == false)
    {
        char err[128];
        strcpy(err, "Not loggedOn yet");
        struct message msg;
        msg.name = const_cast<char*>(m_description.c_str());
        msg.name_len = m_description.length();
        msg.data = err;
        msg.len = strlen(err);
        dart_rolling_log_going(&msg, m_sender.c_str(), m_sender.length());
        char cmd = (m.data)[0];
        
        if (cmd == ROMVALUE::COMMAND_STATUS ||
            cmd == ROMVALUE::COMMAND_ORDER ||
            cmd == ROMVALUE::COMMAND_REPLACE ||
            cmd == ROMVALUE::COMMAND_CANCEL ||
            cmd == ROMVALUE::COMMAND_CANCELS)
        {
            if (m_connected)
            {
                if (true == m_doSequencing) //resending is still going on so delay saving to outgoing file
                {
                    struct message* pM = CreateMsg(m);
                    m_tmpQ.push_back(pM);
                }
            }
            else
            {
                RecordSentMessage(&m);
                if (false == m_doSequencing)
                {
                    m_msgQ.append(m.data, m.len);
                }
            }
        }
    }
    else
    {
        int bytes_sent_t = 0;
        int len = m.len;
        struct message logmsg;
        CopyMessage(logmsg, m);
        RecordSentMessage(&logmsg);
        
        bytes_sent_t = send(&m);
       
        if (bytes_sent_t == len)
        {
            rc = true;
        }
        else
        {
            if (false == m_doSequencing)
            {
                m_msgQ.append(logmsg.data, logmsg.len);
            }
        }
    }

    return rc;
}



void CNewConnection::SendQueuedMessages()
{
    int bytes_sent_t = 0;
    unsigned long len = 0;
    char* pBegin = NULL;

    std::string tmpStr;

    if (m_msgQ.size() > 0)
    {
        m_msgQ.swap(tmpStr); //prevent std::string memory from nonstop growing 
        pBegin = const_cast<char*>(tmpStr.c_str());
        len = tmpStr.size();

        struct message msg;
        msg.name = const_cast<char*>(m_description.c_str());
        msg.name_len = m_description.size();
        msg.data = const_cast<char*>(tmpStr.c_str());
        msg.len = tmpStr.size();
        
        bytes_sent_t = send(&msg);
        
        //clean up the left over for the partially sent message but not the whole m_msgQ!!!
        if (bytes_sent_t > 0 &&
            bytes_sent_t < len)
        {
           if (tmpStr[bytes_sent_t - 1] != ROMVALUE::MESSAGE_END)
           {
               int firstEnd = tmpStr.find_first_of(ROMVALUE::MESSAGE_END, bytes_sent_t);
               if (firstEnd >= 0 && 
                   firstEnd + 1 != len )
               {
                   m_msgQ = tmpStr.substr(firstEnd + 1);
               }
           }
        }
        
        /*
        unsigned short tryCnt = 0;
        
        while (bytes_sent_t < len)
        {
            msg.data = pBegin;
            msg.len = len - bytes_sent_t;
            bytes_sent = send_message(&msg);

            if (bytes_sent >= 0)
            {
                bytes_sent_t += bytes_sent;
                pBegin += bytes_sent;
                
                tryCnt++;
                if (tryCnt > 300)
                {
                    
                }
            }
            else
            {
                break;
            }
        }

        if (bytes_sent_t < len)
        {
            m_msgQ = pBegin;
            //need to get rid of the left over assuming that 
            //the other side will disregard the partial message upon disconnection
            int index = m_msgQ.find_first_of(ROMFIELDS::ENDER);
            if (index >= 0)
            {
                index++;
                if (index < m_msgQ.size())
                {
                    m_msgQ[index] = '\0';
                }
            }
            else
            {
                m_msgQ.clear();
            }
        }*/
    }
}

void CNewConnection::SendQueuedMessages_a()
{
    unsigned long bytes_sent_t = 0;
    int bytes_sent = 0;
    unsigned long len = 0;
    char* pBegin = NULL;

    std::string tmpStr;

    if (m_msgQ.size() > 0)
    {
        m_msgQ.swap(tmpStr); //prevent std::string memory from nonstop growing 
        pBegin = const_cast<char*>(tmpStr.c_str());
        len = tmpStr.size();

        struct message msg;
        msg.name = const_cast<char*>(m_description.c_str());
        msg.name_len = m_description.size();
        msg.data = pBegin;
        msg.len = len;
        
        bytes_sent = send_a(&msg);
        
        if (bytes_sent_t > 0 &&
            bytes_sent_t < len)
        {
            //take out left over and assign it to m_pMsg_a
            
            if (msg.data[bytes_sent_t - 1] != ROMVALUE::MESSAGE_END)
            {
                int firstEnd = tmpStr.find_first_of(ROMVALUE::MESSAGE_END, bytes_sent_t);
                if (firstEnd >= 0 && 
                    firstEnd + 1 != len )
                {
                    m_msgQ = tmpStr.substr(firstEnd + 1);
                    msg.len = firstEnd - bytes_sent_t;
                    msg.data = const_cast<char*>(tmpStr.substr(bytes_sent_t, msg.len).c_str());

                    if (m_pMsg_a)
                    {
                        CNewConnection::DestroyMsg(m_pMsg_a);
                    }
                    m_pMsg_a = CreateMsg(msg);
                }
            }
        }
        
        /*
        while (bytes_sent_t < len)
        {
            msg.data = pBegin;
            msg.len = len - bytes_sent_t;
            bytes_sent = send_message(&msg);

            if (bytes_sent >= 0)
            {
                bytes_sent_t += bytes_sent;
                pBegin += bytes_sent;
            }
            else
            {
                break;
            }
        }

        if (bytes_sent_t < len)
        {
            m_msgQ = pBegin;
            //need to get rid of the left over assuming that 
            //the other side will disregard the partial message upon disconnection
            int index = m_msgQ.find_first_of(ROMFIELDS::ENDER);
            if (index >= 0)
            {
                index++;
                if (index < m_msgQ.size())
                {
                    m_msgQ[index] = '\0';
                }
            }
            else
            {
                m_msgQ.clear();
            }
        }*/
    }
}

unsigned long CNewConnection::send(struct message* pM)
{
    unsigned long bytes_sent_t = 0;
    int bytes_sent = 0;
    unsigned int tryCnt = 0;
    unsigned long len = pM->len;
    
    while (bytes_sent_t < len)
    {
        bytes_sent = send_message(pM);

        if (bytes_sent >= 0)
        {
            bytes_sent_t += bytes_sent;
            pM->data += bytes_sent;
            pM->len -= bytes_sent;
            tryCnt++;

            if (m_tryCnt == tryCnt)
            {
                if (bytes_sent_t < len)
                {
                    //send a warning email
                    std::string subj("Sending Problem!!!");
                    std::string msg("Check status of ");
                    msg.append(m_name);

                    CUtility::SendTechEMail(subj, msg);

                    //kill the connection
                    kickoff_connection(const_cast<char*>(m_description.c_str()), m_description.size());

                    //log err
                    char err[128];
                    strcpy(err, "Try Count has reached");
                    struct message m;
                    m.name = const_cast<char*>(m_description.c_str());
                    m.name_len = m_description.length();
                    m.data = err;
                    m.len = strlen(err);
                    dart_rolling_log_going(&m, m_sender.c_str(), m_sender.length());
                }
                
                break;
            }
        }
        else
        {
            //kill the connection
            kickoff_connection(const_cast<char*>(m_description.c_str()), m_description.size());
            
            char err[128];
            strcpy(err, "Send failed");
            struct message m;
            m.name = const_cast<char*>(m_description.c_str());
            m.name_len = m_description.length();
            m.data = err;
            m.len = strlen(err);
            dart_rolling_log_going(&m, m_sender.c_str(), m_sender.length());
            break;
        }
    }
    
    return bytes_sent_t;
}   

bool CNewConnection::sendMsg(struct message& m)
{
    unsigned long bytes_sent_t = 0;
    int bytes_sent = 0;
    unsigned int tryCnt = 0;
    unsigned long len = m.len;
    unsigned long maxTrys = 100000;
    bool rc = true;
    
    while (bytes_sent_t < len)
    {
        bytes_sent = send_message(&m);

        if (bytes_sent >= 0)
        {
            bytes_sent_t += bytes_sent;
            m.data += bytes_sent;
            m.len -= bytes_sent;
            tryCnt++;

            if (maxTrys == tryCnt)
            {
                if (bytes_sent_t < len)
                {
                    //send a warning email
                    std::string subj("Sending Problem!!!");
                    std::string msg("Check status of ");
                    msg.append(m.name, m.name_len);

                    CUtility::SendTechEMail(subj, msg);

                    //kill the connection
                    kick_off_connection(m.name, m.name_len);
                    rc = false;
                }
                
                break;
            }
        }
        else
        {
            //kill the connection
            kick_off_connection(m.name, m.name_len);
            rc = false;
            break;
        }
    }
    
    return rc;
}
int CNewConnection::send_a(struct message* pM)
{
    int bytes_sent_t = 0;
    int bytes_sent = 0;
    int len = pM->len;
    struct message m;
    CopyMessage(m, *pM);
    
    while (bytes_sent_t < len)
    {
        bytes_sent = send_message(&m);

        if (bytes_sent > 0)
        {
            bytes_sent_t += bytes_sent;
            m.data += bytes_sent;
            m.len -= bytes_sent;
        }
        else if (0 == bytes_sent) //recoverable condition
        {
            m_readyToSend = false;
            queue_async_send(m_pAsyncServer, m.name, m.name_len, (void*)this);
                                         
            char err[128];
            strcpy(err, "wouldblock condition has been hit\n");
            struct message msg;
            msg.name = const_cast<char*>(m_description.c_str());
            msg.name_len = m_description.length();
            msg.data = err;
            msg.len = strlen(err);
            dart_rolling_log_going(&msg, m_sender.c_str(), m_sender.length()); 
            
            break;
        }
        else
        {
            bytes_sent_t = -1;
            break;
        }
    }
    
    return bytes_sent_t;
}

bool CNewConnection::SendImmediately(struct message* pM)
{
    bool rc = false;
    struct message msg, m;
    msg.name = const_cast<char*>(m_description.c_str());
    msg.name_len = m_description.size();
    CopyMessage(m, *pM);
       
    if (m_loggedOn == false)
    {
        char err[128];
        strcpy(err, "Not LoggedOn yet");
        msg.name = const_cast<char*>(m_description.c_str());
        msg.name_len = m_description.length();
        msg.data = err;
        msg.len = strlen(err);
        dart_rolling_log_going(&msg, m_sender.c_str(), m_sender.length());
    }
    else
    {
        struct message logmsg;
        CopyMessage(logmsg, m);
        int len = m.len;
        int bytes_sent_t = send(&m);
        
        if (bytes_sent_t == len)
        {
            RecordSentMessage(&logmsg);
            rc = true;
        }
    }

    return rc;
}


void CNewConnection::SetIpPort(int sock)
{
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    socklen_t nSockAddrLen = (socklen_t)sizeof(sockAddr);

    if (!getpeername(sock, (struct sockaddr*)&sockAddr, &nSockAddrLen))
    {
        SetPORT(ntohs(sockAddr.sin_port));
        SetIP(inet_ntoa(sockAddr.sin_addr));
    }
}

void CNewConnection::CopyMessage(struct message& target, const struct message& source)
{
    target.name_len = source.name_len;
    target.name = source.name;
    target.len = source.len;
    target.data = source.data;
}

struct message*  CNewConnection::CreateMsg(struct message& msg)
{
    struct message* ret = (struct message*)calloc(1, sizeof(struct message));
    ret->name_len = msg.name_len;
    ret->name = (char*)calloc(1, ret->name_len + 1);
    strncpy(ret->name, msg.name, ret->name_len);
    ret->len = msg.len;
    ret->data = (char*)calloc(1, ret->len + 1);
    strncpy(ret->data, msg.data, ret->len);
    return ret;
}

void CNewConnection::DestroyMsg(struct message* pMsg)
{
    free(pMsg->data);
    free(pMsg->name);
    free(pMsg);
    pMsg = NULL;
}

void CNewConnection::SetLoggedOn(bool value)
{
    pthread_rwlock_wrlock(&m_lock);
    
    m_loggedOn = value;
    if (true == m_loggedOn)
    {
        while (m_tmpQ.empty() == false)
        {
            struct message* pMsg = m_tmpQ.front();
            record_sent_message(pMsg);
            CNewConnection::DestroyMsg(pMsg);
            m_tmpQ.pop_front();
        }
    
        if (false == m_doSequencing)
        {
            if (m_pAsyncServer)
            {
                if (m_tmsgQ_a.empty() == false &&
                    m_rejectFunc != NULL)
                {
                    m_rejectFunc(m_tmsgQ_a);
                    DestroyMsgQ_a();
                }
                SendQueuedMessages_a();
            }
            else
            {
                SendQueuedMessages();
            }
        }
        else
        {
            //The other side is no longer interested in the queued messages
            m_msgQ.clear();
        }
    }

    pthread_rwlock_unlock(&m_lock);
}

void CNewConnection::DestroyMsgQ_a()
{
    std::list<struct message*> tmpQ;
    m_tmsgQ_a.swap(tmpQ);
    struct message* m = NULL;
    
    while (tmpQ.empty() == false)
    {
        m = tmpQ.front();
        CNewConnection::DestroyMsg(m);
        tmpQ.pop_front();
    }
}


void CNewConnection::RecordSentMessage(struct message* m)
{
    if (m_doNotRecord)
    {
        return;
        
    }
    char cmd = (m->data)[0];
    if (cmd == ROMVALUE::COMMAND_STATUS ||
        cmd == ROMVALUE::COMMAND_ORDER ||
        cmd == ROMVALUE::COMMAND_REPLACE ||
        cmd == ROMVALUE::COMMAND_CANCEL ||
        cmd == ROMVALUE::COMMAND_CANCELS)
    {
        record_sent_message(m);
    }
}


void CNewConnection::kickoff_connection(char *name, size_t name_len)
{
    m_loggedOn = false;
    m_connected = false;
    
    if (m_pAsyncServer)
    {
        async_kick_off_connection(m_pAsyncServer, name, name_len);
    }
    else
    {
        kick_off_connection(name, name_len);
    }
}


void CNewConnection::OnSend(const char* name, int name_len, void* pass_back)
{
    CNewConnection* pCon = (CNewConnection*)pass_back;
   
    //for debug purpose
    struct message msg;
    msg.name = const_cast<char*>(pCon->Description().c_str());
    msg.name_len = pCon->Description().size();
    char err[128];
    strcpy(err, "OnSend() is called\n");
    msg.data = err;
    msg.len = strlen(err);
    dart_rolling_log_going(&msg, pCon->Sender().c_str(), pCon->Sender().length());
    
    pCon->OnSend();
}

void CNewConnection::OnSend()
{
    pthread_rwlock_wrlock(&m_lock);
    
    m_readyToSend = true;
    
    //Sending left over
    if (m_pMsg_a != NULL)
    {
        int len = send_a(m_pMsg_a);
        if (len == m_pMsg_a->len)
        {
            DestroyMsg(m_pMsg_a);
            m_pMsg_a = NULL;
          
            if (m_pMsgNotCompleted_a)
            {
                RecordSentMessage(m_pMsgNotCompleted_a);
                DestroyMsg(m_pMsgNotCompleted_a);
                m_pMsgNotCompleted_a = NULL;
            }
        }
        else if (len > 0)  //partially sent again
        {
            struct message m;
            CopyMessage(m, *m_pMsg_a);
            m.data += len;
            m.len -= len;
            struct message* pM = CreateMsg(m);
            DestroyMsg(m_pMsg_a);
            m_pMsg_a = pM;
        }
    }
    
    //Sending queued messages caused by would/block condition
    if (NULL == m_pMsg_a)
    {
        struct message* m = NULL;
        int len = 0;

        while (m_tmsgQ_a.empty() == false)
        {
            m = m_tmsgQ_a.front();
            len = send_a(m);
            if (len == m->len)
            {
                RecordSentMessage(m);
                m_tmsgQ_a.pop_front();
                CNewConnection::DestroyMsg(m);
            }
            else if (len > 0)  //partially sent
            {
                m_tmsgQ_a.pop_front();
                struct message mm;
                CopyMessage(mm, *m);
                mm.data += len;
                mm.len -= len;
                m_pMsg_a = CreateMsg(mm);
                m_pMsgNotCompleted_a = CreateMsg(*m);
                CNewConnection::DestroyMsg(m);
                break;
            }
            else
            {
                break;
            }
        }
    }
    
    
    if (NULL == m_pMsg_a &&
        false == m_immediate )
    {
        SendQueuedMessages_a();
    }
    
    pthread_rwlock_unlock(&m_lock);
}

bool CNewConnection::SendImmediately_a(struct message* pM)
{
    bool rc = true;
    struct message msg;
    msg.name = const_cast<char*>(m_description.c_str());
    msg.name_len = m_description.size();
        
    if (m_loggedOn == false)
    {
        char err[128];
        strcpy(err, "Not LoggedOn yet");
        
        msg.data = err;
        msg.len = strlen(err);
        dart_rolling_log_going(&msg, m_sender.c_str(), m_sender.length());
        rc = false;
    }
    else
    {
        if (true == m_readyToSend)
        {
            int len = send_a(pM);
            if (len == pM->len)         //the whole message was sent
            {
                RecordSentMessage(pM);
            }
            else if (len > 0)           //the message was partially sent
            {
                struct message m;
                CopyMessage(m, *pM);
                m.data += len;
                m.len -= len;
                m_pMsg_a = CreateMsg(m);//save the leftover of this partially sent message
                m_pMsgNotCompleted_a = CreateMsg(*pM); //delay outgoing log of this message
            }
            else if (len == 0) //not sent at all so save it in the temp queue
            {
                struct message* pMsg = CreateMsg(*pM);
                m_tmsgQ_a.push_back(pMsg);
            }
            else
            {
                rc = false;
            }
        }
        else
        {
            //It is debateble to reject or to queue!!!
            struct message* pMsg = CreateMsg(*pM);
            m_tmsgQ_a.push_back(pMsg);
        }
    }

    return rc;
}

bool CNewConnection::SendNoneImmediately_a(struct message* pM)
{
    bool rc = true;
    struct message msg;
    msg.name = const_cast<char*>(m_description.c_str());
    msg.name_len = m_description.size();
       
    if (m_loggedOn == false)
    {
        rc = false;
        char err[128];
        strcpy(err, "Not loggedOn yet");
        msg.data = err;
        msg.len = strlen(err);
        dart_rolling_log_going(&msg, m_sender.c_str(), m_sender.length());
        char cmd = (pM->data)[0];
                
        if (cmd == ROMVALUE::COMMAND_STATUS ||
            cmd == ROMVALUE::COMMAND_ORDER ||
            cmd == ROMVALUE::COMMAND_REPLACE ||
            cmd == ROMVALUE::COMMAND_CANCEL ||
            cmd == ROMVALUE::COMMAND_CANCELS)
        {
            if (m_connected)
            {
                if (true == m_doSequencing) //resending is still going on so delay saving to outgoing file
                {
                    struct message* pMsg = CreateMsg(*pM);
                    m_tmpQ.push_back(pMsg);
                }
            }
            else
            {
                RecordSentMessage(pM);
                if (false == m_doSequencing)
                {
                    m_msgQ.append(pM->data, pM->len);
                }
            }
        }
    }
    else
    {
        RecordSentMessage(pM);
        
        if (false == m_readyToSend)
        {
            struct message* pMsg = CreateMsg(*pM);
            m_tmsgQ_a.push_back(pMsg);
        }
        else
        {
            int len = send_a(pM);
            //if (len == pM->len)                 //the whole message was sent
            //{
            //   rc = true;
            //}
            if (len > 0 && 
                len != pM->len)                   //the message was partially sent
            {
                struct message m;
                CopyMessage(m, *pM);
                m.data += len;
                m.len -= len;
                m_pMsg_a = CreateMsg(m);        //save the leftover of this partially sent message
                m_pMsgNotCompleted_a = CreateMsg(*pM); //delay outgoing log of this message
            }
            else if (len == 0) //not sent at all so save it in the temp queue
            {
                struct message* pMsg = CreateMsg(*pM);
                m_tmsgQ_a.push_back(pMsg);
            }
            else
            {
                rc = false;
            }
        }
    }

    return rc;
}

void CNewConnection::SetConnected(bool value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_readyToSend = true;
    m_connected = value;
    
    if (false == m_connected)
    {
        m_loggedOn = false;
    }
    
    if (m_pMsg_a) //get rid of partial message
    {
        DestroyMsg(m_pMsg_a);
        m_pMsg_a = NULL;
    }

    if (m_pMsgNotCompleted_a) //get rid of not yet recorded message
    {
        DestroyMsg(m_pMsgNotCompleted_a);
        m_pMsgNotCompleted_a = NULL;
    }
    
    while (m_tmpQ.empty() == false)  
    {
        struct message* pMsg = m_tmpQ.front();
        record_sent_message(pMsg);
        DestroyMsg(pMsg);
        m_tmpQ.pop_front();
    }
 
    pthread_rwlock_unlock(&m_lock);
}

bool CNewConnection::SendWritev(tWriteV& wvc)
{
    bool rc = true;
    int vecs_sent = 0, total_vecs_sent = 0, errCode = 0;
    int bytes_sent = 0, offset = 0;
    int total_vecs = wvc.iovcnt;
    
    struct iovec* pVec = &(wvc.iov[0]);
    
    while ((bytes_sent = send_bunches(pVec, total_vecs - total_vecs_sent, m_sock, &errCode, &vecs_sent)) >= 0)
    {
        //resume the modified element with the remembered offset so that the original vector is intact 
        pVec->iov_base -= offset;
        pVec->iov_len += offset;
        
        if (total_vecs_sent < total_vecs )
        {
            //adjust the vector and send again
            total_vecs_sent += vecs_sent;
            offset = bytes_sent;
            
            for (int i = 0; i < vecs_sent; i++)
            {
                offset -= pVec->iov_len;
                pVec++;
            }
            
            //offset into the new start buffer
            
            pVec = &(wvc.iov[total_vecs_sent]);
            pVec->iov_base += offset;
            pVec->iov_len -= offset;
        }
    }
    
    if (total_vecs != total_vecs_sent)
    {
        rc = false;
    }
    return rc;
}

bool CNewConnection::SendWritev_a(tWriteV& wvc, int& total_vecs_sent, int& offset)
{
    bool rc = true;
    int vecs_sent = 0, errCode = 0;
    int bytes_sent = 0;
    int total_vecs = wvc.iovcnt;
    
    struct iovec* pVec = &(wvc.iov[0]);
    
    while ((bytes_sent = send_bunches(pVec, total_vecs - total_vecs_sent, m_sock, &errCode, &vecs_sent)) > 0)
    {
        //resume the modified element with the remembered offset so that the original vector is intact 
        pVec->iov_base -= offset;
        pVec->iov_len += offset;
        
        if (total_vecs_sent < total_vecs )
        {
            //adjust the vector and send again
            total_vecs_sent += vecs_sent;
            offset = bytes_sent;
            
            for (int i = 0; i < vecs_sent; i++)
            {
                offset -= pVec->iov_len;
                pVec++;
            }
            
            //offset into the new start buffer
            
            pVec = &(wvc.iov[total_vecs_sent]);
            pVec->iov_base += offset;
            pVec->iov_len -= offset;
        }
    }
    
    if (bytes_sent < 0)
    {
        rc = false;
    }
    else if (bytes_sent == 0) //would block condition is hit
    {
        
    }
    
    return rc;
}

void CNewConnection::GetLeftover(tWriteV& wvc, size_t curVec, int offset, std::string& leftover)
{
    int index = curVec;
    int bytesToRead = (wvc.iov)[index].iov_len - offset;
    
    leftover.append((char*)((wvc.iov)[index].iov_base) + offset, bytesToRead);
    index++;
    
    for (; index < wvc.iovcnt; index++)
    {
        bytesToRead = (wvc.iov)[index].iov_len;
        leftover.append((char*)((wvc.iov)[index].iov_base) + offset, bytesToRead);
    }
}