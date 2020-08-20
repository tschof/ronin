#ifndef __TEMPORARY_ACCEPT_SESSION_HPP__
#define __TEMPORARY_ACCEPT_SESSION_HPP__

#include "Session.hpp"

namespace dart {
  /**
   * @class Temporary_Accept_Session
   * @brief All connections must have a Session attached to process messages,
   * as the name implies this is a temporary session that exists just long
   * enough to receive a logon message, validate the logon, and find the
   * correct session to service the connection.
   */
#if 0
  class Temporary_Accept_Session : public Session {
  public:
    virtual void on_new_connection (Connection*);
    virtual int on_logon (ACE_Message_Block*);
    virtual void on_data_message (ACE_Message_Block*);
  };
#endif
}

#endif
