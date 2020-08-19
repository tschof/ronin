#ifndef _SESSIONWRAP_H__
#define _SESSIONWRAP_H__

#include <list>
#include "Message.h"
#include "Session.h"

class CSessionWrap
{
public:
    CSessionWrap(const FIX::SessionID& sessionID);
    ~CSessionWrap();

    void Send(FIX::Message* pMsg);

private:
    FIX::SessionID m_FixSessionID;
    std::list<FIX::Message*> m_MsgQ;
};

#endif //_SESSIONWRAP_H__