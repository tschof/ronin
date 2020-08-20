#ifndef __DESTHANDLER_H__
#define __DESTHANDLER_H__

#include "BaseHandler.h"

class CDestHandler : public CBaseHandler
{
public:
	CDestHandler();
	~CDestHandler();

	virtual void open(ACE_HANDLE h, ACE_Message_Block& mb);
	//virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result& result);
	virtual void RemoveHandler();
	virtual void ProcessMessage(const char* pMsg, int size);

private:

};

#endif //__DESTHANDLER_H__
