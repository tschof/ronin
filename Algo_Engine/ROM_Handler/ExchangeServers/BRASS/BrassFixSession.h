#ifndef BrassFIXSESSION
#define BrassFIXSESSION

#include "FixSession.h"
#include "BrassReplace.h"

class CBrassFixSession : public CFixSession
{
	// Constructors & destructors
	public:
		CBrassFixSession() {}
		~CBrassFixSession() {}

	// Services
	public:
		BOOL ProcessReplace(CReplace& a_Replace);
		long ProcessEntry(COrder& a_Order);
		BOOL ProcessCancel(CCancel& a_Cancel);
};

#endif
