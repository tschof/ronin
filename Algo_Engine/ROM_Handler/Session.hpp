#ifndef __SESSION_HPP__
#define __SESSION_HPP__

#include <ace/Task.h>
#include <ace/Atomic_Op.h>
#include <list>
#include <string>

#include "CSV_Message_Framer.hpp"
#include "Session_Control_Strategy.hpp"

namespace dart {class Connection;}

class CRomCsv;

/**
 * @class IO_Event_Handler_Callback
 * @brief The interface presented to the IO_Event_Handlers (aka Connections).
 *
 * This is the bare minimum interface needed by the IO_Event_Handlers to pass
 * data along in the system.  Having this interface decouples the actual
 * Connection objects and the inner-workings of the application, including the
 * Session layer.
 */
class IO_Event_Handler_Callback
{
public:
  virtual void on_connect()=0;
  virtual void on_disconnect()=0;
  // on_data?
};

struct Logon_Credentials
{
  std::string username;
  std::string password;
};

/**
 * @class Session
 * @brief Concrete class representing the messaging session between two
 * logical entities and the operations that can be performed upon that session.
 *
 * Every object in the application uses a Session object to talk to an outside
 * party.  The Session object, in turn, manages whether there is an actual
 * connection to that peer, whether the logon/logoff and sequencing are correct
 * and other details.  The control of how a Session responds to connections,
 * disconnections, and various messages is configured by the
 * Session_Control_Strategy object that the Session is configured with.  The
 * Session_Control_Strategy determines things like whether a logon should be
 * sent immediately upon connecting (for a client connection perhaps) or to wait
 * for a logon (for a server connection perhaps).
 */
class Session
  : public ACE_Task <ACE_MT_SYNCH>
  , IO_Event_Handler_Callback
{
public:
  friend class Server_Session_Control_Strategy;
  Session (Session_Control_Strategy* = 0);

  // Session interface
  void io_handler(dart::Connection *io_handler);
  void logon_credentials(const Logon_Credentials&);
  bool shutdown_io_handler();
  Session_States current_state() const;

  virtual int  put (ACE_Message_Block*, ACE_Time_Value * = 0);
  virtual void on_message_retrieval (ACE_Message_Block*);
  virtual bool logon (ACE_Message_Block * = 0);
  virtual bool end_of_rebroadcast();
  virtual bool validate_logon_message(ACE_Message_Block*,Logon_Credentials&);
  virtual void pass_message_to_application(ACE_Message_Block*);
  virtual void increment_inbound_sequence_number();
  virtual void increment_outbound_sequence_number();
  virtual void store_message(ACE_Message_Block*);

  // Kind of confusing, but send() is called by just anyone wanting to send,
  // but on_send is called by the session control object.
  // TODO: Rename or something to make this less confusing.  Maybe even separate
  // the interface used by the session controller (on_send()) from that used
  // by the rest of the application.
  virtual bool send (ACE_Message_Block*);
  virtual bool send (CRomCsv&);
  virtual bool on_send(ACE_Message_Block*);

  // IO_Event_Handler_Callback interface implementation
  virtual void on_connect ();
  virtual void on_disconnect();

protected:
  virtual void begin_filestore_message_request (size_t);

  dart::Connection *io_handler_;
  std::list <ACE_Message_Block*> msgs_;
  std::string id_;
  bool received_logon_;
  bool sent_logon_;

  Logon_Credentials logon_credentials_;
  Logon_Credentials peer_credentials_;

  typedef ACE_Atomic_Op <ACE_SYNCH_MUTEX, size_t> Atomic_Sequence_Number;
  Atomic_Sequence_Number seq_num_in_;
  Atomic_Sequence_Number seq_num_out_;

  Session_Control_Strategy *control_strategy_;

  dart::CSV_Message_Framer msgframer_;
};

#endif
