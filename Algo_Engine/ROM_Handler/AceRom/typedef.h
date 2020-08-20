#ifndef _TYPEDEF_H__
#define _TYPEDEF_H__

#include "ace/Message_Block.h"
#include "ROMMessage.h"
#include <string>
#include <list>

typedef std::list<ACE_Message_Block*> tMsgBlkList;
typedef tMsgBlkList::iterator tMsgBlkListIt;
typedef std::list<ACE_Message_Block*> tMsgBlkList;
typedef tMsgBlkList::iterator tMsgBlkListIt;
typedef std::list<CRomMessage*> tRomMsgList;
typedef tRomMsgList::iterator tRomMsgListIt;


#endif //_TYPEDEF_H__