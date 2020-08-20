#ifndef __ALGO_MANAGER_HPP__
#define __ALGO_MANAGER_HPP__

#include <ace/Task.h>
#include <ace/OS.h>
#include "Session_Directory.hpp"

class Algo_Manager : public ACE_Task <ACE_MT_SYNCH>
{
public:
  virtual int put (ACE_Message_Block*, ACE_Time_Value * = 0);
  virtual int svc ();
};

int Algo_Manager::put (ACE_Message_Block *mb, ACE_Time_Value*)
{
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%s\n"), __FUNCTION__));
  mb->release ();
  return 0;
}

int Algo_Manager::svc ()
{
  for (size_t i (0); i < 10; ++i)
  {
    ACE_OS::sleep (1);
    ACE_Message_Block *mb = 0;
    time_t now (time (0));
    const char *ctime_str = ctime (&now);
    ACE_NEW_RETURN (mb, ACE_Message_Block (ACE_OS::strlen (ctime_str) + 1), -1);
    if (mb->copy (ctime_str) != -1)
    {
      Session *psession = SESSION_DIRECTORY::instance ()->lookup ("DAVIDC");
      if (psession && mb->length () > 0)
        psession->send (mb);
      else
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("No session for DAVIDC!\n")));
    }
    else
      ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Copy failed!\n")));
  }
  return 0;
}

typedef ACE_Singleton <Algo_Manager, ACE_SYNCH_MUTEX> ALGO_MANAGER;

#endif
