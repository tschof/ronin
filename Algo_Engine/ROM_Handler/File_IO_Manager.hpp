#ifndef __FILE_IO_HPP__
#define __FILE_IO_HPP__

// ACE
#include <ace/Task.h>
#include <ace/Singleton.h>

// Std C++
#include <map>

// App
#include "Message_Types.hpp"


class File_IO_Manager
  : public ACE_Task <ACE_MT_SYNCH>
{
public:
  File_IO_Manager ();
  virtual int svc ();
private:
  typedef std::map <std::string, FILE*> Open_Files;
  Open_Files open_files_;
};

typedef ACE_Singleton <File_IO_Manager, ACE-SYNCH_MUTEX> FILE_STORE_WRITER;

File_IO_Manager::File_IO_Manager ()
{
  // Spawn a single thread for processing read/write requests
  this->activate ();
}

int File_IO_Manager::svc ()
{
  ACE_Message_Block *mb = 0;
  while (this->getq (mb) != -1)
  {
    switch (mb->msg_type ())
    {
    case MB_STOP:
      return 0;

    case MB_STORE_DATA:
      // Normal case, write to file.  First data block contains name of session
      // as a null-terminated string, second contains actual message.
      ACE_ASSERT (mb->length ());
      if (mb->length ())
      {
        FILE *f = this->open_files_ [mb->rd_ptr ()];
        if (!f)
        {
          const std::string filename (std::string (mb->rd_ptr ()) + ".csv");
          f = fopen (filename.c_str (), "w+");
          ACE_ASSERT (f);
          if (f)
            this->open_files_ [filename] = f;
          else
            goto NEXT_MESSAGE;
        }

        if (mb->cont () && mb->cont ()->msg_type () == MB_DATA)
        {
          if (fwrite (mb->cont ()->rd_ptr (),
                      sizeof (char),
                      mb->cont ()->length (),
                      f) < mb->cont ()->length ())
          {
          }
        }
      }
      break;

    case MB_REQUEST_DATA:
      if (mb->length () >= sizeof (Request_Data))
      {
        // Find the file descriptor for this session id.  If
        // open, do a flush on it in preparation for the read.
        Request_Data *preq_data = (Request_Data) mb->rd_ptr ();
        FILE *f = this->open_files_ [preq_data->session_id];
        if (f && fflush (f) < 0)
        {
          // Complain
          ACE_ASSERT (false);
          goto NEXT_MESSAGE;
        }

        // Open this file for reading
        const std::string filename (std::string (preq_data->session_id) + ".csv");
        FILE *read_desc = fopen (filename.c_str (), "r");
        if (read_desc)
        {
          // while (Read the file in chunks)
          // {
          //   Frame the messages from the chunks
          //   Filter messages not needed
          //   Send to the requestor
          // }
        }
      }
      else
      {
        // Complain
        ACE_ASSERT (false);
        goto NEXT_MESSAGE;
      }
    }

NEXT_MESSAGE:
    mb->release ();
  }

  return 0;
}

#endif
