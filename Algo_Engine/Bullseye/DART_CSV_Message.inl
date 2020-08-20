template <unsigned int Field_Count>
Non_Modifying_Read_Only_CSV_Indexer<Field_Count>::Non_Modifying_Read_Only_CSV_Indexer(const char *buffer, size_t len)
  : buf_(buffer)
  , len_(len)
  , error_(false)
  , nextidx_(0)
  , lastidx_(0xffffffff)
{
  if (buffer == 0 || len == 0)
    error_ = true;
  memset(indexes_,-1,sizeof(indexes_));
  memset(tmpbuf_,0,sizeof(tmpbuf_));
  this->indexes_[0].offset=0;
}

template <unsigned int Field_Count>
bool Non_Modifying_Read_Only_CSV_Indexer<Field_Count>::error() const
{
  return error_;
}

template <unsigned int Field_Count>
const char *Non_Modifying_Read_Only_CSV_Indexer<Field_Count>::getstring(size_t reqidx)
{
  const char *pdata = 0;

  // All assumptions within this if statement assume that we
  // have space in our index for the ith element requested.
  if (reqidx<Num_Fields && reqidx<this->lastidx_ && this->len_)
  {
    // nextidx_ tracks the last known indexed field
    if (reqidx < this->nextidx_)
    {
      // Already indexed
      if (this->indexes_[reqidx].len == 0)
        pdata = 0;
      else
      {
        memset(tmpbuf_,0,sizeof(tmpbuf_));
        strncpy(this->tmpbuf_,
          this->buf_+indexes_[reqidx].offset,
          indexes_[reqidx].len);
        tmpbuf_[sizeof(tmpbuf_)-1]='\0';
        pdata = this->tmpbuf_;
      }
    }
    else
    {
      // Not yet indexed
      const char *end = this->buf_+this->len_;
      const char *sof = indexes_[nextidx_].offset + buf_;
      for (const char *p=sof  ;  (p != end)  ;  ++p, sof = indexes_[nextidx_].offset + buf_)
      {
        if (*p==',' || *p=='\n' || *p=='\0')
        {
          this->indexes_[this->nextidx_].len = static_cast<Index_Type>(p-sof);
          ++this->nextidx_;
          this->indexes_[this->nextidx_].offset
            = this->indexes_[this->nextidx_-1].offset
            + this->indexes_[this->nextidx_-1].len
            + 1; // Account for the comma

          if (*p=='\n' || *p=='\0')
            this->lastidx_ = this->nextidx_;

          if (reqidx<this->nextidx_)
            break;
        }
      }

      pdata = this->getstring(reqidx);
    }
  }
  return pdata;
}
