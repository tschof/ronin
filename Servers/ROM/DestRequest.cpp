#include "DestRequest.h"
#include "Utility.h"

CDestRequest::CDestRequest(unsigned short reqType, std::string id, unsigned short now) : 
m_reqType(reqType),
m_id(id),
m_now(now)
{
}

CDestRequest::~CDestRequest()
{
}
