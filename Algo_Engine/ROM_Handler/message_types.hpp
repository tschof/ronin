#ifndef __MESSAGE_TYPES_HPP__
#define __MESSAGE_TYPES_HPP__

#include <ace/Message_Block.h>
class Session;

enum Message_Type
{
  // Request messages from disk, the contents of the
  // message block contains the name of the client.
  MB_REQUEST_MESSAGES_FROM_FILESTORE = ACE_Message_Block::MB_USER,
  MB_MESSAGE_RETRIEVED_FROM_FILESTORE,
  MB_MESSAGE_RETRIEVAL_COMPLETED,
  MB_STORE_MESSAGE,
  MB_STORE_DATA,
  MB_REQUEST_DATA,
  MB_SESSION_SOURCE_NAME,
};

const size_t MAX_SESSION_ID_LEN (16);

struct Request_Data
{
  size_t start_sequence;
  char session_id [MAX_SESSION_ID_LEN];
  Session *requestor;
};

#endif
