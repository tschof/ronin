#include "Sequenced_Message_Tracker.hpp"

#include <cstdio>
#include <stdexcept>


namespace dart
{
  struct Msg_Header
  {
    unsigned int sequence_number;
    unsigned int message_length;
  };

  Sequenced_Message_Tracker::Sequenced_Message_Tracker(const char *file_name)
    : file_name_ (file_name),
      next_seq_no_ (-1)
  {
    if (file_name_ == 0)
      throw std::runtime_error ("null pointer");

    file_ = fopen (file_name_, "a+b");
    if (file_ == 0)
      throw std::runtime_error ("cannot open file");
  }

  Sequenced_Message_Tracker::~Sequenced_Message_Tracker ()
  {
    if (file_) {
      fclose (file_);
      file_ = 0;
    }
  }

  void
  Sequenced_Message_Tracker::find_next_seq_no ()
  {
    Msg_Header header;
    memset (&header, 0, sizeof (header));
    (void) fread (&header, sizeof (header), 1, file_);
  }
}