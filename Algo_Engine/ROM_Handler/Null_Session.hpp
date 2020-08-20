#ifndef __NULL_SESSION_HPP__
#define __NULL_SESSION_HPP__

/**
 * @class Null_Session
 * @brief A Null_Session acts as a placeholder session when before the identity
 * of the peer is known.
 *
 * Session objects are pass-through objects that frame messages, track sequence
 * numbers, satisfy message resends on sequence gaps, and persist sent messages
 * for disconnected clients.  There is exactly one session object for every
 * client/server pair.
 *
 * For accepted connections, there exists a time from when the connection has
 * been created till the receipt of a login message that no unique session is
 * known.   The Null_Session takes the place of the real session.  This keeps
 * the logic in the Connections simple, they simply forward on received
 * messages and send queued messages like normal.  The Null_Session waits for a
 * login message.   Any messages received before  a  login message are rejected
 * and the connection is closed as  a  breach of protocol has occurred.  Once a
 * login message has been received  the  identity of the peer  is known and its
 * credentials can be verified.  Once verified, the Null_Session finds the
 * Session object specifically defined  for  that session (there can be at most
 * one).  If one cannot be found it is created.  The Connection is then tied to
 * the Session and the Session is tied to the Connection.  The login message is
 * passed to the Session object for further inspection (for possible resends
 * for instance).
 */

#include "Session.hpp"
#include "CSV_Message_Framer.hpp"

class Null_Session : public Session
{
public:
  virtual int put (ACE_Message_Block*, ACE_Time_Value * = 0);
private:
  dart::CSV_Message_Framer framer_;
};

#endif
