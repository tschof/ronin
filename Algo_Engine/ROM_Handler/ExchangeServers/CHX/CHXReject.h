#if !defined(CCHXREJECT_H)
#define CCHXREJECT_H

#include "Reject.h"
#include "CHXReplace.h"

class CCHXReject : public CReject
{
public:
	CCHXReject(const CString& Reason, const CCHXReplace& Replace);
};


#endif