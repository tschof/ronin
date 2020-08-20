#include "TagMapSet.h"
#include "MemTagSet.h"
#include "CHXMemTagSet.h"
#include "CHXReplace.h"
#include "OrderTypeValidator.h"

class CCommonDatabase;
class CFixSession;

class CCHXHandler : public COrderHandler
{
public:
	void ProcessMessage(COrder& Order);
	void ProcessMessage(CReplace& Replace);
	//void ProcessMessage(CCHXReplace& Replace);
	void ProcessMessage(CCancel& Order);
	
	void ProcessMessage(CExecution& pReport);
	void ProcessMessage(CCancelReject& pReject);
	void ProcessMessage(CReject& FixReject);
	void ProcessMessage(const char *Status);
		
private:
	//CTagMapSet *m_pTags;
	CCHXMemTagSet *m_pTags;
	CString	m_DefaultAccount;

	COrderTypeValidator*	m_orderTypeValidator;

public:
	CCHXHandler();
	virtual ~CCHXHandler();


};