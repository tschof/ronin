#ifndef __DART_LOG_HPP__
#define __DART_LOG_HPP__

#include <ace/Log_Msg.h>

#define INFO_PREFIX      ACE_TEXT ("%T (P:%P|T:%t) -INFO- ")
#define DEBUG_PREFIX     ACE_TEXT ("%T (P:%P|T:%t) -DEBUG- ")
#define WARNING_PREFIX   ACE_TEXT ("%T (P:%P|T:%t) -WARNING- ")
#define ERROR_PREFIX     ACE_TEXT ("%T (P:%P|T:%t) -ERROR- ")
#define CRITICAL_PREFIX  ACE_TEXT ("%T (P:%P|T:%t) -CRITICAL- ")

#define DART_INFO(...)          \
        ACE_DEBUG ((LM_INFO,    \
                    INFO_PREFIX \
                    __VA_ARGS__))

#define DART_DEBUG(...)          \
        ACE_DEBUG ((LM_DEBUG,    \
                    DEBUG_PREFIX \
                    __VA_ARGS__))

#define DART_WARNING(...)          \
        ACE_DEBUG ((LM_WARNING,    \
                    WARNING_PREFIX \
                    __VA_ARGS__))

#define DART_ERROR(...)          \
        ACE_DEBUG ((LM_ERROR,    \
                    ERROR_PREFIX \
                    __VA_ARGS__))

#define DART_CRITICAL(...)          \
        ACE_DEBUG ((LM_CRITICAL,    \
                    CRITICAL_PREFIX \
                    __VA_ARGS__))

#endif
