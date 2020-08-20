#ifndef __HTTP_SERVER_SINGLETON_HPP__
#define __HTTP_SERVER_SINGLETON_HPP__

#include <ace/Singleton.h>
#include "Http_Server.hpp"

typedef ACE_Singleton <Http_Server, ACE_SYNCH_MUTEX> HTTP_SERVER;

#endif
