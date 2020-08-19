#include "SessionWrap.h"


CSessionWrap::CSessionWrap(const FIX::SessionID& sessionID) :
    m_FixSessionID(sessionID.getBeginString(),	sessionID.getSenderCompID(), sessionID.getTargetCompID())
{
}


CSessionWrap::~CSessionWrap()
{
    FIX::Message* msgPtr = NULL;

    while (m_MsgQ.empty() == false)
    {
        msgPtr = m_MsgQ.front();
        m_MsgQ.pop_front();
        delete msgPtr;
    }
}

void CSessionWrap::Send(FIX::Message* pMsg)
{
    m_MsgQ.push_back(pMsg);
    FIX::Message* msgPtr = NULL;

    while (m_MsgQ.empty() == false)
    {
        msgPtr = m_MsgQ.front();
        m_MsgQ.pop_front();

        try
        {
            FIX::Session::sendToTarget( *msgPtr, m_FixSessionID );
        }
        catch (FIX::SessionNotFound&)
        {
            m_MsgQ.push_front(msgPtr);
            break;
        }

        delete msgPtr;
    }
}

