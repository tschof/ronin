#ifndef _DESTREQUEST_H__
#define _DESTREQUEST_H__

#include <list>
#include "Csv.h"

class CDestRequest
{
public:

  enum REQUEST_TYPE
  {
    RECONNECT_CHECK,
    SESSIONEND_CHECK,
    LOGGEDON,
    BLOCK,
    UNBLOCK,
    CONNECTED,
    DISCONNECTED,
    DISCONNECT,
    RELOADDESTS,
    QUERYDEST,
    QUERYDESTS,
    RELOADIPPORT
  };

  CDestRequest(unsigned short reqType, std::string id, unsigned short now = 0);
	~CDestRequest(void);

  unsigned short RequestType() { return m_reqType; }
  const std::string& ID() { return m_id; }
  unsigned int now() { return m_now; }

private:
	unsigned short m_reqType;
  std::string m_id;
  std::string m_requester;
  unsigned short m_now;
};

typedef std::list<CDestRequest*> tDestRequestList;
typedef tDestRequestList::iterator tDestRequestListIT;

#endif //_DESTREQUEST_H__
