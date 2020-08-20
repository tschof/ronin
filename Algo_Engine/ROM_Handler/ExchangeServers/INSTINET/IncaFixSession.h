#ifndef INCAFIXSESSION
#define INCAFIXSESSION

#include "FixSession.h"
#include "Order.h"

class CIncaFixSession : public CFixSession
{
	// Constructors & destructors
	public:
		CIncaFixSession() {}
		~CIncaFixSession() {}
//		CString *INCADestination(CString &destination);
	// Services
	public:
		long ProcessEntry(COrder& a_Order, const char *TargetSubId);
		BOOL ProcessCancel(CCancel& a_Cancel, const char *TargetSubId);
		BOOL ProcessReplace(CReplace& a_Replace, const char *TargetSubId);
		//void GetTargetSubId(const char *StaffordTag, const char *TargetSubId);
};

#endif
