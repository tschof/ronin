#ifndef __ROM_CONNECTION_HPP__
#define __ROM_CONNECTION_HPP__

class ACE_Message_Block;
class CRomCsv;

#include <string>

namespace dart
{
  /**
   * @class ROM_Connection
   *
   * @brief This is an implementation interface class using the Bridge pattern
   * that represents an idealized connection between any two ROM connections
   * in the system.
   *
   * Since the actual I/O mechanism to be used from the ACE framework has not
   * been decided (Proactor vs. Reactor, different threading models, etc.),
   * this class will serve as a Bridge to the various implementations that will
   * be used by the ROM_Session objects.  In reality, this will probably be
   * general enough an interface to be used for other kinds of connections such
   * as FIX connections as well.
   */
  class ROM_Connection
  {
  public:
    virtual void disconnect () = 0;
    virtual void send (ACE_Message_Block*) = 0;
    virtual bool send (CRomCsv*) = 0;
    virtual std::string identify () = 0;
  };
}

#endif