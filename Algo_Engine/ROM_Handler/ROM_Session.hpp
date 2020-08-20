#ifndef __ROM_SESSION_HPP__
#define __ROM_SESSION_HPP__

#include <Bullseye/Bullseye_Common.hpp>
#include <string>

class ACE_Message_Block;
#include "ROM_Handler/romcsv.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

namespace dart
{
  class ROM_Connection;

  class ROM_Session
  {
  public:
    ROM_Session (const std::string& /*login*/, const std::string& /*password*/);
    virtual void handle_message (ACE_Message_Block*);
    virtual void on_connect ();
    bool is_connected () const;
    void send_csv (CRomCsv&);
    void get_uname_pword (std::string& /*uname*/,
                          std::string& /*pword*/);
    void set_connection (ROM_Connection*);
    virtual bool is_upstream_connection () = 0;

    /**
     * @class Connection_State
     * @brief This is a really simple class for really simple state keeping,
     * the only real reason it's here is to trap changes to state and do some
     * automatic logging when that happens.
     */
    class Connection_State
    {
    public:
      enum State
      {
        DISCONNECTED = 0,
        CONNECTED
      };

      Connection_State (const std::string &name)
        : state_ (DISCONNECTED),
          name_ (name)
      {
      }

      State current_state () const
      {
        return state_;
      }

      void set_connected ()
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (DART_LOG ("%s Connection_State moving to CONNECTED\n")),
          name_.c_str ()));
        state_ = CONNECTED;
      }

      void set_disconnected ()
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (DART_LOG ("%s Connection_State moving to DISCONNECTED\n")),
          name_.c_str ()));
        state_ = DISCONNECTED;
      }

    private:
      State state_;
      std::string name_;
    };


  protected:

    virtual void handle_message_in_disconnected_state (ACE_Message_Block*);
    virtual void handle_message_in_connected_state (ACE_Message_Block*);

    void create_logon_message (CRomCsv&);

    Connection_State state_;
    ROM_Connection *conn_;
    std::string login_;
    std::string password_;

    // Not implemented:
    ROM_Session& operator= (const ROM_Session&);
    ROM_Session (const ROM_Session&);
  };

  class Client_ROM_Session
    : public ROM_Session
  {
  public:
    Client_ROM_Session (const std::string &login, const std::string &password)
      : ROM_Session (login, password)
      , has_received_query_command (false)
    {
    }

    virtual bool is_upstream_connection ()
    {
      return false;
    }

    virtual void on_connect ()
    {
      CRomCsv logonmsg;
      this->create_logon_message (logonmsg);
      this->send_csv (logonmsg);
    }

    virtual void handle_message_in_connected_state (ACE_Message_Block *mb)
    {
      if (has_received_query_command) {
        ROM_Session::handle_message_in_connected_state (mb);
      }
      else {
        if (mb->length () && (mb->rd_ptr () [0] == 'Q')) {
          this->has_received_query_command = true;
        }
        else {
          ACE_DEBUG ((LM_DEBUG,
            DART_LOG ("Discarding message: {|%.*C|}\n"),
            mb->length () ? mb->length()-1 : 0,
            mb->rd_ptr ()));
        }
      }
    }

  protected:
    bool has_received_query_command;
  };

  class Server_ROM_Session
    : public ROM_Session
  {
  public:
    Server_ROM_Session (const std::string &login, const std::string &password)
      : ROM_Session (login, password)
    {
    }

    virtual bool is_upstream_connection ()
    {
      return false;
    }

    virtual void on_connect ()
    {
      // Wait for somebody to connect
    }

    virtual void handle_message_in_disconnected_state (ACE_Message_Block *msg)
    {
      ROM_Session::handle_message_in_disconnected_state (msg);

      std::string csv_str (msg->rd_ptr (), msg->length ());
      CRomCsv csvmsg (csv_str);

      CRomCsv logon_msg;
      // Echo back the logon message
      logon_msg.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_LOGON);
      logon_msg.SetAt (RFIX::CSV_TRADER, csvmsg.GetAt (RFIX::CSV_TRADER));
      logon_msg.SetAt (RFIX::CSV_PASSWORD, csvmsg.GetAt (RFIX::CSV_PASSWORD));
      this->send_csv (logon_msg);

      CRomCsv qmsg;
      std::string uname, pword;
      this->get_uname_pword (uname, pword);
      qmsg.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_QUERY);
      qmsg.SetAt (RFIX::CSV_OWNER, uname);
      this->send_csv (qmsg);
    }
  };
}

#endif
