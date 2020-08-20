#ifndef __SEQUENCED_MESSAGE_TRACKER_HPP__
#define __SEQUENCED_MESSAGE_TRACKER_HPP__

#include <cstdio>

namespace dart
{
  /**
   * @class Sequenced_Message_Tracker
   *
   * @brief Generic class to persist and recover sequenced messages.
   *
   * The messages that are tracked can be text or binary, the only restriction
   * is that there can be only one message of a given sequence number (the
   * sequence number is a unique key) and that the sequence numbers added
   * increase monotonically.
   */

  class Sequenced_Message_Tracker
  {
  public:
    Sequenced_Message_Tracker (const char*);
    ~Sequenced_Message_Tracker ();

  private:
    const char *file_name_;
    FILE *file_;
    int next_seq_no_;

    void find_next_seq_no ();
  };
}

#endif