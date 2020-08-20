#include "Server_Session.hpp"

namespace dart {
  Server_Session::Server_Session () {}

  void Server_Session::on_new_connection (Connection *new_connection) {
    // TODO: Send a logon reject 
    if (this->connection ())
      this->disconnect ();
    this->connection (new_connection);
  }

  void Server_Session::on_data_message (ACE_Message_Block*) {
  }
}
