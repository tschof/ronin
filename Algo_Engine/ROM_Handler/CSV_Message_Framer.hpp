#ifndef __CSV_MESSAGE_FRAMER_HPP__
#define __CSV_MESSAGE_FRAMER_HPP__

#include <ace/Message_Block.h>
#include <stdexcept>

namespace dart
{
  /**
   * @class CSV_Message_Framer
   * @brief Process a byte stream for individual CSV messages
   *
   * An object of this class is meant to be instantiated when a data stream of
   * CSV messages is instantiated and every byte of data, in order, is passed
   * to that object in an ACE_Message_Block.  Call the frame method in a loop
   * until it returns false indicating no more messages could have been pulled
   * out.  In between successive calls to frame that return true, i.e. while in
   * the loop, do not modify the message block in any way.  Once frame returns
   * false, it is then safe to dispose or modify the message block.  Any
   * residual data not included in the returned messages is stored internally
   * in the object and will be prepended to the next data put into the object.
   */
  class CSV_Message_Framer
  {
  public:
    CSV_Message_Framer (size_t initial_buffer_size = 0);

    /**
     * @arg mb A pointer to a message block containing data read from a stream
     *         of CSV messages.
     * @return False if an error occurs storing the buffer of data.
     */
    bool add (ACE_Message_Block *mb);

    /**
     * @arg mb If the return value is true, the framed message is copied into
     *         the ACE_Message_Block pointed to by mb.  If false, there is no
     *         change.
     * @return True if a message was framed from the internal buffer.
     *         
     */
    bool next_message (ACE_Message_Block *mb);

    /**
     * @return True if any residual data in internal buffers.
     */
    bool is_empty ();

    /**
     * @return A pointer to a message block containing the entire message
     * buffer.  This breaks encapsulation somewhat, but there are times when
     * access to the entire buffer is needed.
     */
    ACE_Message_Block *get_buffer();

    /**
     * @arg buffer A pointer to an ACE message block containing data taken from
     * a stream of CSV messages, the message block is modified as data is taken
     * out by the framer.
     *
     * @arg msg A pointer to a pointer that gets filled in with a dynamically
     * allocated message block containing a single CSV message or a null pointer
     * indicating no more messages could be extracted from the buffer.
     *
     * @return True indicating a single message was extracted from the buffer,
     * false indicating no more messages could be extracted from the buffer.
     */
    bool frame (ACE_Message_Block *buffer, ACE_Message_Block **msg);

  private:
    // Do not copy or assign these objects
    CSV_Message_Framer (const CSV_Message_Framer&);
    CSV_Message_Framer& operator= (const CSV_Message_Framer&);

    // State
    ACE_Message_Block m_buffer;
    ACE_Message_Block buffer_;
  };
}

#endif
