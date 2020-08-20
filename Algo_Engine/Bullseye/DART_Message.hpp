#ifndef __DART_MESSAGE_HPP__
#define __DART_MESSAGE_HPP__

namespace dart
{
  /**
   * @class Read_Only_Fixed_Size_Message
   *
   * @brief Class wrapper that does not allow modification of the data and
   * assumes a fixed upper bound on the number of fields.
   *
   * This class employs several optimizations: 1) the buffer is not copied
   * and indexed in-place; 2) the buffer is read-only so no buffering need
   * be done for data added; 3) the message fields are not unbounded, there
   * is a definite upper limit to the number of fields, this allows us to
   * have a fixed-size footprint that doesn't require allocating memory;
   * objects of this class are ideal for allocating on the stack or within
   * a fixed length buffer of some sort.
   */
  template <unsigned int Field_Count=80>
  class Read_Only_Fixed_Size_Message
  {
  public:
    static const unsigned int MAX_FIELDS=Field_Count;
    Read_Only_Fixed_Size_Message (char*, size_t);
    void dump (std::ostream&);
    const char *get (size_t);
  private:
    int indexes_ [Field_Count];
    char *buffer;
  };

  typedef Read_Only_Fixed_Size_Message<> Read_Only_Msg;
}

#include "DART_Message.inl"

#endif
