#ifndef __RW_FIX_MSG_H__
#define __RW_FIX_MSG_H__

#include "rw_msg.h"
#include "message_layout.h"

/* Make a rw_msg (conforms to the rw_msg interface) that represents a
   FIX message.  The size_t pointer/size defines an array of FIX
   message tag orderings.  The order defined by this array will be
   kept in creating the message.  The interface does not require the
   fields to be defined in this way, they can be get/set in random
   order.  The ordering requirement only applies when the fields are
   concatenated during serialization. */
rw_msg_t *make_rw_fix_msg(message_layout_t *);

#endif
