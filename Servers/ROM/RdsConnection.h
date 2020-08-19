#ifndef _RDSCONNECTION_H__
#define _RDSCONNECTION_H__

#include "dart_connection_object.h"
#include "Connection.h"

class CRdsConnection : public CConnection
{
public:
    CRdsConnection();
    ~CRdsConnection();
    
    bool Connecting() { return m_connecting; }
    void SetConnecting(bool value) { m_connecting = value; }
    struct init_args* args() { return &m_args; }
    
private:
    void SetArgs();
   
    struct init_args m_args;
    bool m_connecting;
};


#endif //_POSITIONTHREAD_H__