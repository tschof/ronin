#pragma once

// ACE
#include <ace/Singleton.h>
#include <ace/Mutex.h>

// Std C++
#include <map>
#include <string>

/**
 * @class Configuration
 * @brief Global configuration database.
 * 
 * Meant to be an infrequently used singleton interface to getting configuration
 * data, will serve to ease porting to other OSes.  Values should be read through
 * this Singleton object and cached locally.
 */
class Configuration
{
public:
  Configuration();
  ~Configuration();
  void filename(const char*);
  std::string get(const char*, const char*);
  int get_int(const char*, const char*, int);

private:
  const char *filename_;
  typedef std::map <std::string, std::string> MapT;
  MapT strvals_;
  ACE_Thread_Mutex mutex_;
};

typedef ACE_Singleton <Configuration, ACE_SYNCH_MUTEX> CONFIGURATION;
