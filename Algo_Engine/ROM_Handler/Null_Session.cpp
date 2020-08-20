#include "Connection.hpp"
#include "Null_Session.hpp"
#include "Session_Directory.hpp"
#include <Bullseye/DART_CSV_Message.hpp>
#include "COMMON\ROMFIXENGINE\FixEngine\romfielddefs.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

int Null_Session::put (ACE_Message_Block *mb, ACE_Time_Value*)
{
  int result(-1);
  if (this->framer_.add(mb))
  {
    mb->reset ();
    if (this->framer_.next_message(mb))
    {
      // The dart::Read_Only_Msg modifies the data buffer, store
      // this so later we can pass to the actual server session.
      ACE_Message_Block *mbcopy = mb->clone();

      // The first message *must* be a Logon message.
      Csv_Msg msg(mb->rd_ptr(),mb->length());
      const char *command = msg.getstring(RFIX::CSV_COMMAND);

      if (command && command[0]==ROMVALUE::COMMAND_LOGON && command[1]=='\0')
      {
        const char *user = msg.getstring(RFIX::CSV_TRADER);
        if (user && user[0]!='\0')
        {
          Session *session = SESSION_DIRECTORY::instance()->lookup_server_session(user);
          this->io_handler_->session(session);
          session->io_handler(this->io_handler_);

          ACE_Message_Block *entire_buffer = this->framer_.get_buffer();
          if (mbcopy->copy(entire_buffer->rd_ptr(), entire_buffer->length()) == 0)
            result = session->put(mbcopy);

          mb->release();
        }
      }
    }
    else
      result = 0; // Incomplete message, okay for now
  }

  if (result == -1)
    this->shutdown_io_handler();

  return result;
}