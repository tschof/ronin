#include <ace/Message_Block.h>
#include "Connection.hpp"
#include "Session_Registrar.hpp"
#include "Client_Session.hpp"
#include "Server_Session.hpp"
#include "ROM_Handler/ROMCSV.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include <algorithm>

namespace dart {
#if 0
  Session_Registrar::Session_Registrar ()
    : client_session_ (0)
  {}

  Session *Session_Registrar::find_server_session (const std::string &user) {
    using namespace std;
    Session *session = 0;
    if (!user.empty ()) {
      string upper_case_user_name;
      transform (user.begin (),
                 user.end (),
                 back_inserter (upper_case_user_name),
                 ::toupper);
      const Registrations::const_iterator user_iterator
        (this->registrations_.find (upper_case_user_name));
      if (user_iterator == this->registrations_.end ()) {
        session = new Server_Session ();
        this->registrations_ [upper_case_user_name] = session;
      }
      else
        session = user_iterator->second;
    }
    return session;
  }

  Session *Session_Registrar::find_server_session (ACE_Message_Block *mb) {
    using namespace std;
    Session *session = 0;
    if (mb && mb->length ()) {
      string message_str (mb->rd_ptr (), mb->length ());
      CRomCsv message (message_str);
      if (message.cGet (RFIX::CSV_COMMAND) == ROMVALUE::COMMAND_LOGON) {
        string user (message.GetAt (RFIX::CSV_TRADER));
        if (!user.empty ())
          session = find_server_session (user);
      }
    }
    return session;
  }

  Session *Session_Registrar::retrieve_client_session () {
    if (client_session_ == 0)
      client_session_ = new Client_Session;
    return client_session_;
  }
#endif
}