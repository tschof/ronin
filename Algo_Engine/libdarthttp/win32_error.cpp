#include "win32_error.hpp"

void win32_perror(DWORD lasterror)
{
  char errmsg[128];
  memset(errmsg,0,sizeof(errmsg));
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 lasterror,
                 LANG_SYSTEM_DEFAULT,
                 errmsg,
                 sizeof(errmsg),
                 0);
  if (errmsg[0] == '\0')
    strcpy_s(errmsg,sizeof(errmsg),"Unknown error");
  fprintf(stderr,"%s\n",errmsg);
}

std::string win32_strerror(DWORD lasterror)
{
  char errmsg[128];
  memset(errmsg,0,sizeof(errmsg));
  FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 lasterror,
                 LANG_SYSTEM_DEFAULT,
                 errmsg,
                 sizeof(errmsg),
                 0);
  if (errmsg[0] == '\0')
    strcpy_s(errmsg,sizeof(errmsg),"Unknown error");
  return errmsg;
}
