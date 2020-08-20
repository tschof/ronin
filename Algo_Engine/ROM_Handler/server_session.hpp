#ifndef __SERVER_SESSION_HPP__
#define __SERVER_SESSION_HPP__

#if 0
#include "Session.hpp"

namespace dart {
  class Server_Session
    : public Session {
  public:
    Server_Session ();
  protected:
    virtual void on_new_connection (Connection*);
    virtual void on_data_message (ACE_Message_Block*);
  };
}
#endif

#endif
