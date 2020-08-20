#ifndef __BULLSEYE_COMMON_HPP__
#define __BULLSEYE_COMMON_HPP__

// ACE
#include <ace/Singleton.h>
#include <ace/Mutex.h>

// Bullseye
#include "Configurator/Configuration.h"
#include "DART_CSV_Message.hpp"

// Connections
#include <ROM_Handler/Message_Types.hpp>

// Logging macros
#include "bullseye/DART_Log.hpp"
// Provide standard header to log messages
#define DART_LOG(msg) "%T (P:%P|T:%t) "msg

namespace bullseye {
  typedef ACE_Thread_Mutex  Mutex;
  typedef ACE_Guard <Mutex> Guard;
}

#endif
