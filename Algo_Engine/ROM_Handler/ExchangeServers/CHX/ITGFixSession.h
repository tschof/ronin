#ifndef ITGFIXSESSION
#define ITGFIXSESSION

#include "FixSession.h"
#include "ITGReplace.h"

class CITGFixSession : public CFixSession
{
	// Constructors & destructors
	public:
		CITGFixSession() {}
		~CITGFixSession() {}

	// Services
	public:
		BOOL ProcessReplace(CITGReplace& a_Replace);
};

#endif
