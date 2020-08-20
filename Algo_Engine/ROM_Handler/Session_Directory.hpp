#ifndef __SESSION_DIRECTORY_HPP__
#define __SESSION_DIRECTORY_HPP__

#include <ace/Singleton.h>
#include <map>
#include <string>

class Session;

class Session_Directory
{
public:
  // Ctor should load all sessions at startup
  Session *lookup_server_session(const std::string&);

private:
  typedef std::map <std::string, Session*> Session_Dictionary;
  Session_Dictionary dict_;
};

typedef ACE_Singleton <Session_Directory, ACE_SYNCH_MUTEX> SESSION_DIRECTORY;

#endif
