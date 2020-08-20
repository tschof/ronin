#ifndef __CLIENT_SESSION_HPP__
#define __CLIENT_SESSION_HPP__

#if 0
#include "Session.hpp"
#include <ace/Asynch_Connector.h>

namespace dart {
  class Client_Session;

  class Connector
    : public ACE_Asynch_Connector <Connection> {
  public:
    Connector (Client_Session*);
  protected:
    virtual Connection *make_handler ();
    Client_Session *session_;
  };

  class Client_Session
    : public Session {
  public:
    Client_Session ();
    virtual int open (void* = 0);
    virtual bool connect ();
  protected:
    virtual void on_new_connection (Connection*);
    virtual void on_data_message (ACE_Message_Block*);

    Connector connector_;
    bool connect_pending_;
  };
}
#endif
#endif
