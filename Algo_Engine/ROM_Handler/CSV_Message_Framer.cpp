#include "CSV_Message_Framer.hpp"
#include <ace/Log_Msg.h>
#include <cassert>

namespace dart
{
  CSV_Message_Framer::CSV_Message_Framer (size_t initial_buffer_size)
    : m_buffer (initial_buffer_size)
  {
  }

  bool CSV_Message_Framer::add (ACE_Message_Block *mb)
  {
    bool result (false);
    assert(this->buffer_.rd_ptr() <= this->buffer_.wr_ptr());
    if (mb)
    {
      if (this->buffer_.crunch () == 0)
      {
        if (this->buffer_.copy (mb->rd_ptr (), mb->length ()) == 0)
          result = true;
        else if (errno == ENOSPC)
        {
          // Not enough room, try and resize to accommodate the new buffer
          const size_t growby = mb->length () - this->buffer_.space ();
          if (this->buffer_.size (growby + this->buffer_.capacity ()) == 0 &&
              this->buffer_.copy (mb->rd_ptr (), mb->length ()) == 0)
            result = true;
        }
      }
    }
    assert(this->buffer_.rd_ptr() <= this->buffer_.wr_ptr());
    return result;
  }

  bool CSV_Message_Framer::next_message (ACE_Message_Block *mb)
  {
    bool result (false);
    assert(this->buffer_.rd_ptr() <= this->buffer_.wr_ptr());
    if (this->buffer_.length ())
    {
      char *p = this->buffer_.rd_ptr();
      char *const end = p + this->buffer_.length();
      for ( ; p < end; ++p)
      {
        assert(this->buffer_.rd_ptr() <= this->buffer_.wr_ptr());
        if (*p == '\n')
        {
          const size_t msgsize = p - this->buffer_.rd_ptr () + 1;
          if (msgsize > 1)
          {
            if (mb->copy (this->buffer_.rd_ptr (), msgsize) == 0)
              result = true;
            else if (errno == ENOSPC)
            {
              // Not enough room, try and resize to accommodate the new buffer
              const size_t growby = msgsize - mb->space ();
              if (mb->size (growby + mb->capacity ()) == 0 &&
                  mb->copy (this->buffer_.rd_ptr (), msgsize) == 0)
              result = true;
            }
          }
          this->buffer_.rd_ptr (msgsize);
          break;
        }
      }
    }

    return result;
  }

  bool
  CSV_Message_Framer::frame (ACE_Message_Block *buffer,
                             ACE_Message_Block **result)
  {
    if (buffer == 0 || result == 0)
      throw std::invalid_argument ("null pointer");

    // For the most part, CSV messages will be smaller than the MTU on the
    // network (messages being ~200-300 bytes) and senders will most commonly
    // have the Nagle algorithm disabled on their TCP/IP stack such that these
    // messages are put onto their TCP/IP stack for sending in one complete
    // buffer and are then sent immediately by the stack in one packet.  To meet
    // this general case we verify here that we have no residual buffered
    // messages so we can simply verify the message block contains a single
    // message and then queue that message for processing.
    //
    // To be completely correct in our handling of these messages, however, we
    // must also account for message fragmentation across packets and reads from
    // our host TCP/IP stack and for the presence of multiple messages within a
    // buffer read from our local TCP/IP stack.  If message fragmentation occurs
    // we must buffer the message fragments until receiving a complete message.
    // If message aggregation occurs in the buffer we read from our TCP/IP stack
    // we must correctly frame the messages and any fragmented messages in the
    // buffer read and queue each of these messages separately for processing.

    if (this->m_buffer.length () == 0 && buffer->length () > 0) {
      // We have no message fragments from previous reads stored away, so we may
      // assume that the first byte of this buffer just read from the socket is
      // the first byte of a CSV message received from our peer.

      // Next, determine if an entire message exists
      ACE_Message_Block *msg = 0;
      for (char *p = buffer->rd_ptr (); p != buffer->end (); ++p) {
        if (*p == '\n') {
          if ((p - buffer->rd_ptr ()) == 0) {
            buffer->rd_ptr (1);
          }
          else {
            msg = new ACE_Message_Block (p - buffer->rd_ptr ());
            if (msg->copy (buffer->rd_ptr (), p - buffer->rd_ptr ()) == -1) {
              throw std::runtime_error ("copy failed");
            }
            buffer->rd_ptr (p - buffer->rd_ptr () + 1);
            *result = msg;
            return true;
          }
        }
      }
    }

    return false;
  }

  ACE_Message_Block *CSV_Message_Framer::get_buffer()
  {
    return &this->buffer_;
  }
}
