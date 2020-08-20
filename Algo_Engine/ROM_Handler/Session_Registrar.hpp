#ifndef __SESSION_REGISTRAR_HPP__
#define __SESSION_REGISTRAR_HPP__

#include <ace/Singleton.h>
#include <map>
#include <string>

class ACE_Message_Block;

namespace dart
{
  /**
   * The Session object is responsible for all state for this particular
   * user.  All messages sent to and from the client are passed through this
   * object.  This Session object services Logins, Logouts, and Resends for
   * this particular client.  There can only be one Session object per client
   * in the system.
   */
  class Session;

  class Session_Registrar
  {
  public:
    Session_Registrar ();
    Session *find_server_session (ACE_Message_Block*);
    Session *find_server_session (const std::string&);
    Session *retrieve_client_session ();

  private:
    typedef std::map <std::string, Session*> Registrations;
    Registrations registrations_;
    Session *client_session_;
  };

  typedef ACE_Singleton <Session_Registrar, ACE_SYNCH_MUTEX> SESSION_REGISTRAR;
}

#endif
