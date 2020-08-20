#ifndef __DART_CSV_MESSAGE_HPP__
#define __DART_CSV_MESSAGE_HPP__

template <unsigned int Field_Count=80>
class Non_Modifying_Read_Only_CSV_Indexer
{
public:
  static const size_t Num_Fields = Field_Count;
  Non_Modifying_Read_Only_CSV_Indexer(const char *buffer, size_t len);
  bool error() const;
  const char *getstring(size_t reqidx);

private:
  const char *buf_;
  size_t      len_;
  bool        error_;

  // The next field to be indexed, the offset of the nextidx_ index element of
  // indexes_ contains the offset into the buffer buf_ at the next point at
  // which to start indexing.
  size_t      nextidx_;
  size_t      lastidx_;

  typedef short Index_Type;
  struct Field_Info
  {
    Index_Type offset;
    Index_Type len;
  };

  Field_Info indexes_[Field_Count+1];

  char tmpbuf_[64];
};

typedef Non_Modifying_Read_Only_CSV_Indexer<> Csv_Msg;

#include "DART_CSV_Message.inl"

#endif
