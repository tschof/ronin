#ifndef __MESSAGE_BLOCK_UTILS_HPP__
#define __MESSAGE_BLOCK_UTILS_HPP__

class ACE_Task_Base;
#include <ace/Message_Block.h>
class CRomCsv;

namespace dart {
  ACE_Message_Block *serialize_pointer (ACE_Task_Base*);
  void *deserialize_pointer (ACE_Message_Block*);
  bool send_message (ACE_Task_Base&,
                     CRomCsv&,
                     const ACE_Message_Block::ACE_Message_Type& = ACE_Message_Block::MB_DATA);
  bool send_control_message (ACE_Task_Base&, const ACE_Message_Block::ACE_Message_Type);
}

#endif
