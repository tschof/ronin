#if !defined(CBRASSREJECT_H)
#define CBRASSREJECT_H

#include "Reject.h"
#include "BrassReplace.h"

class CBrassReject : public CReject
{
public:
	CBrassReject(const CString& Reason, const CBrassReplace& Replace);
};


#endif