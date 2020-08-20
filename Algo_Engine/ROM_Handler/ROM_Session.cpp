// ACE
#include <ace/Message_Block.h>
#include <ace/Log_Msg.h>

// ROM_Handler
#include "ROM_Session.hpp"
#include "ROM_Connection.hpp"

// Bullseye components
#include <Decision_Engine/Algo_Manager.hpp> // Did not want this dependency
#include <Bullseye/Bullseye_Common.hpp>

// CRT
#include <cassert>

// ROM Message Includes from RomFixEngine
#include "ROM_Handler/ROMCsv.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romfielddefs.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

namespace dart
{

ROM_Session::ROM_Session (const std::string &login, const std::string &password)
: state_ (login),
  conn_ (0),
  login_ (login),
  password_ (password)
{
}


void
ROM_Session::set_connection (ROM_Connection *connection)
{
  this->conn_ = connection;
}

void
ROM_Session::handle_message (ACE_Message_Block *mb)
{
  if (mb->length () == 0) {
    return;
  }

  switch (state_.current_state ()) {
  case Connection_State::DISCONNECTED:
    handle_message_in_disconnected_state (mb);
    break;
  case Connection_State::CONNECTED:
    handle_message_in_connected_state (mb);
    break;
  default:
    assert (false);
    break;
  }
}

void
ROM_Session::get_uname_pword (std::string &uname,
                              std::string &pword)
{
  uname = this->login_;
  pword = this->password_;
}

void
ROM_Session::create_logon_message (CRomCsv &csvmsg)
{
  csvmsg.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_LOGON);

  std::string uname, pword;
  this->get_uname_pword (uname, pword);
  csvmsg.SetAt (RFIX::CSV_TRADER, uname);
  csvmsg.SetAt (RFIX::CSV_PASSWORD, pword);

  // TODO: Cancel orders?  Configurable?  Does it make sense for servers?
  csvmsg.SetAt (RFIX::CSV_CANCEL_BOOL, 0);
}

void
ROM_Session::send_csv (CRomCsv &csv)
{
  this->conn_->send (&csv);
}

void
ROM_Session::on_connect ()
{
  CRomCsv logonmsg;
  this->create_logon_message (logonmsg);
  this->send_csv (logonmsg);
  CRomCsv qmsg;
  std::string uname, pword;
  this->get_uname_pword (uname, pword);
  qmsg.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_QUERY);
  qmsg.SetAt (RFIX::CSV_OWNER, uname);
  this->send_csv (qmsg);
}

void
ROM_Session::handle_message_in_connected_state (ACE_Message_Block *msg)
{
  // Send everything but heartbeat messages to the algo manager
  if (msg && msg->length () && (msg->rd_ptr () [0] != 'H')) {

#ifdef EXTENSIVE_LOGGING
    ACE_DEBUG ((LM_TRACE,
                DART_LOG ("%s %s Message: {|%.*C|}\n"),
                this->login_.c_str (),
                __FUNCTION__,
                msg->length (),
                msg->rd_ptr ()));
#endif

    dart::ALGO_MANAGER::instance ()->put (msg);
  }
}

void
ROM_Session::handle_message_in_disconnected_state (ACE_Message_Block *msg)
{
  if (msg) {

#ifdef EXTENSIVE_LOGGING
    ACE_DEBUG ((LM_TRACE,
                DART_LOG ("%s %s Message: {|%.*C|}\n"),
                this->login_.c_str (),
                __FUNCTION__,
                msg->length (),
                msg->rd_ptr ()));
#endif

    std::string csv_str (msg->rd_ptr (), msg->length ());
    CRomCsv csvmsg (csv_str);

    const std::string msgtype = csvmsg.GetAt (RFIX::CSV_COMMAND);
    if (msgtype.length () != 1 || msgtype [0] != ROMVALUE::COMMAND_LOGON) {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT (DART_LOG ("Expecting logon message while in "
                  "non-logged-on state non-logon message received... "
                  "disconnecting"))));
      this->conn_->disconnect ();
      return;
    }

    const std::string login_id = csvmsg.GetAt (RFIX::CSV_TRADER);
    if (login_id.empty ()) {
      this->conn_->disconnect ();
      return;
    }

    this->login_ = login_id;

    this->state_.set_connected ();
  }
}

bool
ROM_Session::is_connected () const
{
  return (state_.current_state () == Connection_State::CONNECTED);
}

}
