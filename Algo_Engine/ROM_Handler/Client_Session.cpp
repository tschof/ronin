#include "Connection.hpp"
#include "Client_Session.hpp"
#include <bullseye/bullseye_common.hpp>

#if 0
namespace dart {
  Connector::Connector (Client_Session *session)
      : session_ (session)
  {}

  Connection *Connector::make_handler ()
  {
    Connection *connection = new Connection (session_);
    return connection;
  }

  Client_Session::Client_Session ()
    : connect_pending_ (false)
    , connector_ (this)
  {}

  int Client_Session::open (void *) {
    if (this->connector_.open () == -1)
      ACE_ERROR_RETURN ((LM_ERROR, DART_LOG ("%p\n"), __FUNCTION__), -1);
    return (this->connect () ? 0 : -1);
  }

  bool Client_Session::connect () {
    bool result (false);
    if (this->connection_ == 0 && connect_pending_ == false) {
      ACE_INET_Addr connect_addr (static_cast <u_short> (8008), "ddart-ltc-rom1");
      if (this->connector_.connect (connect_addr) == -1)
        ACE_ERROR_RETURN ((LM_ERROR, DART_LOG ("%p\n"), __FUNCTION__), false);
      connect_pending_ = true;
      result = true;
    }
    return result;
  }

  void Client_Session::on_new_connection (Connection *new_connection) {
    if (this->connection_)
      ACE_DEBUG ((LM_DEBUG, DART_LOG ("Logic error!  New client connection "
        "established when one already exists!\n")));
    else {
      this->connection (new_connection);
      // As a client we will send a Logon immediately upon connecting
      this->logon ();
    }
  }

  void Client_Session::on_data_message (ACE_Message_Block*) {

  }
}
#endif
