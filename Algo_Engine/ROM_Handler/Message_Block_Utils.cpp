#include "Message_Block_Utils.hpp"
#include <ace/Message_Block.h>
#include <ace/Task.h>
#include <string>
#include "ROM_Handler/romcsv.h"

namespace dart {
  ACE_Message_Block *serialize_pointer (ACE_Task_Base *p) {
    ACE_Message_Block *pmsg = new ACE_Message_Block (sizeof (ACE_Task_Base*));
    if (pmsg->copy ((const char*) &p, sizeof (void*)) == -1) {
      delete pmsg;
      pmsg = 0;
    }
    return pmsg;
  }

  void *deserialize_pointer (ACE_Message_Block *pmsg) {
    void *p = 0;
    if (pmsg && (pmsg->length () == sizeof (void*))) {
      void **pp = (void**) pmsg->rd_ptr ();
      if (pp)
        p = *pp;
    }
    return p;
  }

  bool send_message (ACE_Task_Base &task,
                     CRomCsv &message,
                     const ACE_Message_Block::ACE_Message_Type &type) {
    bool result (false);
    const std::string &message_str (message.Message ());
    ACE_Message_Block *mb = new ACE_Message_Block (message_str.size ());
    mb->msg_type (type);
    if (mb->copy (message_str.c_str (), message_str.size ()) == 0 &&
        task.put (mb) != -1)
      result = true;
    return result;
  }

  bool send_control_message (ACE_Task_Base &task,
                             const ACE_Message_Block::ACE_Message_Type type) {
    bool result (false);
    ACE_Message_Block *mb = new ACE_Message_Block (0, type);
    if (task.put (mb) != -1)
      result = true;
    return result;
  }
}
