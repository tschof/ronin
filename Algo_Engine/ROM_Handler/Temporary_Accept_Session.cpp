#include "Temporary_Accept_Session.hpp"
#include "ROM_Handler/RomCsv.h"
#include "Session_Registrar.hpp"
#include "Connection.hpp"

#if 0
namespace dart {
  /// There's nothing to do for new connections, we're waiting for a logon
  /// message so that we can then find the correct Session for the user.
  void Temporary_Accept_Session::on_new_connection (Connection*) {}

  int Temporary_Accept_Session::on_logon (ACE_Message_Block *mb) {
    std::string csv_str (mb->rd_ptr (), mb->length ());
    CRomCsv csv (csv_str);
    const std::string user (csv.GetAt (RFIX::CSV_TRADER));
    Session *user_session
      = SESSION_REGISTRAR::instance ()->find_server_session (user);
    if (user_session) {
      if (user_session->connection ()) {
        this->reject_logon ("Already logged on");
      }
      else {
        user_session->connection (this->connection ());
        this->connection ()->session (user_session);
        user_session->put (mb);
      }
    }
    else {
      this->reject_logon ("Internal error");
    }
    return 0;
  }

  void Temporary_Accept_Session::on_data_message (ACE_Message_Block*) {}
}
#endif
