#include "Configuration.h"
#include <windows.h>

Configuration::Configuration()
: filename_(0)
{
}

Configuration::~Configuration()
{
}

void Configuration::filename(const char *filename)
{
  this->filename_ = filename;
}

int Configuration::get_int(const char *section, const char *key, int default)
{
  ACE_Guard <ACE_Thread_Mutex> guard(this->mutex_);
  return GetPrivateProfileInt(section, key, default, this->filename_);
}

std::string Configuration::get(const char *section, const char *key)
{
  ACE_Guard <ACE_Thread_Mutex> guard(this->mutex_);

  std::string result;
  const std::string mapkey(std::string(section) + std::string("\\") + std::string(key));
  MapT::iterator setting_iterator(this->strvals_.find(mapkey));
  if (setting_iterator != this->strvals_.end())
    result = setting_iterator->second; // It's been cached
  else
  {
    // Get from INI file, cache it and return the result
    char buffer[4]; // Set an artificially small buffer for testing.
    memset(buffer, 0, sizeof(buffer));
    DWORD bytesread = GetPrivateProfileStringA(
      section,
      key,
      "",
      buffer,
      sizeof(buffer),
      this->filename_);

    if (bytesread < (sizeof(buffer)-1))
      result = buffer;
    else
    {
      size_t pbuflen(sizeof(buffer));
      char *pbuf = 0;
      do
      {
        pbuflen *= 2;
        delete[]pbuf;
        pbuf = new char[pbuflen];
      }
      while (GetPrivateProfileStringA(
        section,
        key,
        "",
        pbuf,
        pbuflen,
        this->filename_) == (pbuflen-1));
      result = pbuf;
      delete[]pbuf;

      this->strvals_[mapkey] = result;
    }
  }
  return result;
}
