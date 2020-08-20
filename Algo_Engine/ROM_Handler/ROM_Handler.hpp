#ifndef __ROM_HANDLER_HPP__
#define __ROM_HANDLER_HPP__

#include <ace/Asynch_IO.h>
#include <ace/Message_Block.h>
#include <ace/OS.h>
#include <ace/Task.h>
#include <ace/Reactor_Notification_Strategy.h>

#include "ROM_Session.hpp"
#include "ROM_Connection.hpp"
#include "CSV_Message_Framer.hpp"

namespace dart
{

class ROM_Handler
  : public ACE_Service_Handler
  , public ACE_Task <ACE_MT_SYNCH>
  , public dart::ROM_Connection
{
public:

  ROM_Handler (ROM_Session *psession = 0, const std::string &name = "CLIENTROM");
  ~ROM_Handler ();

  // ACE_Service_Handler overrides
  virtual void open (ACE_HANDLE h, ACE_Message_Block&);
  virtual void handle_read_stream (const ACE_Asynch_Read_Stream::Result &result);
  virtual void handle_write_stream (const ACE_Asynch_Write_Stream::Result &result);
  virtual void handle_time_out (const ACE_Time_Value &tv,
                                const void *act = 0);

  // ACE_Task override
  virtual int handle_output (ACE_HANDLE);


  // ROM_Connection overrides
  virtual void disconnect ();
  virtual void send (ACE_Message_Block*);
  virtual bool send (CRomCsv*);
  virtual std::string identify ();

protected:

  virtual void die ();

private:

  void handle_rom_message (ACE_Message_Block&);

  ACE_Asynch_Read_Stream reader_;
  ACE_Asynch_Write_Stream writer_;
  ROM_Session *session_;
  std::string name_;
  CSV_Message_Framer framer_;
  ACE_Reactor_Notification_Strategy notifier_;
};

}

#endif

