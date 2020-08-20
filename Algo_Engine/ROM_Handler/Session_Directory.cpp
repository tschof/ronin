#include "Session_Directory.hpp"
#include "Session.hpp"
#include "Session_Control_Strategy.hpp"
#include <Configurator/Configuration.h>

Session *Session_Directory::lookup_server_session(const std::string &key)
{
    const std::string config_username
      (CONFIGURATION::instance()->get("Bullseye", "ServerUsername"));
    const std::string config_password
      (CONFIGURATION::instance()->get("Bullseye", "ServerPassword"));

  Session *psession = 0;
  Session_Dictionary::const_iterator iter = this->dict_.find (key);
  if (iter == this->dict_.end ())
  {
    // Does not already exist, create anew.
    psession = new Session(new Server_Session_Control_Strategy);
    Logon_Credentials credentials = {config_username,config_password};
    psession->logon_credentials(credentials);
    psession->on_connect();
    this->dict_ [key] = psession;
  }
  else
    psession = iter->second;
  return psession;
}
