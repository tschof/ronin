#ifndef __ROM_CSV_MESSAGE_HPP__
#define __ROM_CSV_MESSAGE_HPP__

namespace dart
{
  /**
   * @class ROM_CSV_Read_Only_Message
   *
   * @brief An intrusive class that indexes a CSV message, providing fast
   * access to elements.
   *
   * Some design considerations are that the templated size of the index
   * allows the size of this class to be known at compile time so no dynamic
   * memory allocation is explicitly needed.  This is also inflexible in that
   * the indexing strategy cannot be changed and also causes problems in
   * passing the message around as the types are different based on the size.
   */
  template <int Field_Count>
  class ROM_CSV_Read_Only_Message
  {
  public:
    enum { field_count = Field_Count };

    ROM_CSV_Read_Only_Message (char *p, size_t nbytes)
      : buffer_ (p)
    {
      memset (indexes_, -1, sizeof (indexes_));
      if (p) {
        char *begin = p;
        char *end = p + nbytes;
        char *field_begin = 0;
        for (size_t idx (0); p != end; ++p) {
          if (*p == ',') {
            if (field_begin) {
              indexes_ [idx] = field_begin - begin;
              field_begin = 0;
            }
            ++idx;
            *p = '\0';
            continue;
          }
          else if (field_begin == 0) {
            field_begin = p;
          }
        }
      }
    }

    void dump_indexed_fields (std::ostream &stream)
    {
      for (size_t i (0); i < Field_Count; ++i) {
        if (indexes_ [i] != -1) {
          const size_t index (indexes_ [i]);
          const char *p = buffer_ + index;
          stream << i << '=' << p << ',';
        }
      }
      stream << '\n';
    }

    const char *get (size_t idx)
    {
      size_t offset (0);
      if (idx >= Field_Count || (offset = indexes_ [idx]) == -1)
        return 0;
      return buffer_ + offset;
    }

    bool get_int (size_t idx, int &i)
    {
      bool result (false);
      const char *p = this->get (idx);
      if (p) {
        std::istringstream converter (p);
        if (converter >> i)
          result = true;
      }
      return result;
    }

    bool get_double (size_t idx, double &d)
    {
      bool result (false);
      const char *p = this->get (idx);
      if (p) {
        std::istringstream converter (p);
        if (converter >> d)
          result = true;
      }
      return result;
    }

  private:
    int indexes_ [Field_Count];
    char *buffer_;
  };
}

#endif
