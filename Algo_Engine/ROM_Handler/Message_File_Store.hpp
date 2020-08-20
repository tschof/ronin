#ifndef __MESSAGE_FILE_STORE_HPP__
#define __MESSAGE_FILE_STORE_HPP__

#include <ace/Task.h>
#include <ace/Singleton.h>
#include "Message_Types.hpp"
#include "CSV_Message_Framer.hpp"
#include "Session.hpp"

/// Handle getting a string from a message block in a safe manner,
/// i.e. checking for null termination and, if not null-terminated,
/// using the size of the buffer to create a string.
std::string str_from_message (ACE_Message_Block &mb)
{
  const char *piter = mb.rd_ptr ();
  for ( ; piter != mb.rd_ptr () + mb.length (); ++piter)
    if (*piter == '\0')
      break;
  return std::string (mb.rd_ptr (), piter);
}

class Message_File_Store : public ACE_Task <ACE_MT_SYNCH>
{
public:
  Message_File_Store ();
  virtual int svc ();

protected:
  int retrieve_msgs (Session&, const std::string&);
  typedef std::map <std::string,FILE*> File_Handle_Map;
  File_Handle_Map file_handle_map_;
};

typedef ACE_Singleton <Message_File_Store, ACE_SYNCH_MUTEX> MESSAGE_FILE_STORE;

Message_File_Store::Message_File_Store ()
{
  this->activate ();
}

int Message_File_Store::svc ()
{
  ACE_Message_Block *mb = 0;
  while (this->getq (mb) != -1)
  {
    if (mb)
    {
      switch (mb->msg_type())
      {
      case ACE_Message_Block::MB_STOP:
        return 0;
      case MB_REQUEST_MESSAGES_FROM_FILESTORE:
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("Messages requested from file store\n")));
        // First bytes are a serialized pointer to
        // the Task to receive the messages.
        // Always use sizeof (void*) to accommodate 32 and 64 bit code
        if (mb->length () >= sizeof (void*))
        {
          Session *p = *((Session**) mb->rd_ptr ());
          mb->rd_ptr (sizeof (Session*));
          const std::string username (str_from_message (*mb));
          if (!username.empty () && this->retrieve_msgs (*p, username) == -1)
            ACE_ERROR_RETURN ((LM_ERROR,
                               ACE_TEXT ("message retrieval failed\n")),
                              -1);
        }
      }
    }
  }
  return 0;
}

int Message_File_Store::retrieve_msgs (Session& task, const std::string &filename)
{
  int result = -1;
  FILE *f = fopen (filename.c_str (), "rb");
  if (f)
  {
    size_t bytes_read (0);
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (4096), -1);
    dart::CSV_Message_Framer msg_framer;
    while (!feof (f) && !ferror (f))
    {
      bytes_read = fread (mb->wr_ptr (), sizeof (char), mb->space (), f);
      mb->wr_ptr (bytes_read);
      if (ferror (f))
        ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                           ACE_TEXT ("file read")), -1);
      if (feof (f))
        break;
      ACE_Message_Block *singlemsg = 0;
      while (msg_framer.frame (mb, &singlemsg))
      {
        if (singlemsg)
        {
          singlemsg->msg_type (MB_REQUEST_MESSAGES_FROM_FILESTORE);
          if (task.put (singlemsg) == -1)
            ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                               ACE_TEXT ("putting msg onto queue")), -1);
        }
      }
    }
  }
  else
  {
    const int last_error (errno);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Session %s: Could not open message "
                          "file with error '%s'\n"),
                filename.c_str (),
                strerror (last_error)));
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (0, MB_MESSAGE_RETRIEVAL_COMPLETED),
                    -1);
    task.on_message_retrieval (mb);
    result = 0;
  }
  return result;
}

#endif
