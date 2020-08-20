#include <cassert>
namespace dart
{
template <unsigned int Field_Count>
Read_Only_Fixed_Size_Message<Field_Count>::Read_Only_Fixed_Size_Message
(char *p, size_t nbytes)
  : buffer (p)
{
  memset (indexes_, -1, sizeof (indexes_));
  char *begin = p;
  char *end = p + nbytes;
  char *field_begin = 0;
  for (size_t idx (0); p != end; ++p)
  {
    if (*p == ',' || *p == '\n')
    {
      if (field_begin)
      {
        if (idx > Field_Count)
          break;
        indexes_ [idx] = field_begin - begin;
        field_begin = 0;
      }
      ++idx;
      *p = '\0';
      if (*p == '\n')
        break;
      else
        continue;
    }
    else if (field_begin == 0)
    {
      field_begin = p;
    }
  }
}

template <unsigned int Field_Count>
void Read_Only_Fixed_Size_Message<Field_Count>::dump (std::ostream &stream)
{
  for (size_t i (0); i < Field_Count; ++i)
  {
    if (indexes_ [i] != -1)
    {
      const size_t index (indexes_ [i]);
      const char *p = buffer + index;
      stream << i << '=' << p << ',';
    }
  }
  stream << '\n';
}

template <unsigned int Field_Count>
const char *Read_Only_Fixed_Size_Message<Field_Count>::get (size_t idx)
{
  size_t offset (0);
  if (idx > Field_Count || (offset = indexes_ [idx]) == -1)
    return 0;
  return buffer + offset;
}
}
