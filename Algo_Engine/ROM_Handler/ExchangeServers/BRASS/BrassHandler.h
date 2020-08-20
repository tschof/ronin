#include "TagMapSet.h"
#include "MemTagSet.h"
#include "BrassMemTagSet.h"
#include "BrassReplace.h"
#include "BRASSInterface.h"
#include "ExchangeHandlerBase.h"
#include "BrassException.h"

class CCommonDatabase;
class CFixSession;

class CBrassHandler : public CExchangeHandlerBase
{

public:

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	CBrassHandler(CConnection& host, std::string& datFile);
	virtual ~CBrassHandler();

	//////////////////////////////////////////////////////////////////////
	// Overidden PreProcessing Methods
	//////////////////////////////////////////////////////////////////////
	virtual void OnReceive(CCancelReject& cancelReject);
	virtual void OnReceive(CReject& replace);

	//////////////////////////////////////////////////////////////////////
	// Overidden Order Processing Methods
	//////////////////////////////////////////////////////////////////////
	virtual void Process(CCancel& cancel) throw(CRejectCancel);
	virtual void Process(CCancelReject& cancelReject);
	virtual void Process(CExecution& execution);
	virtual void Process(COrder& order);
	virtual void Process(CReject& reject);
	virtual void Process(CReplace& replace);

	//////////////////////////////////////////////////////////////////////
	// Overidden Exception Processing Methods
	//////////////////////////////////////////////////////////////////////
	virtual void Reject(CCancel& cancel);
	virtual void Reject(COrder& order, const char* reason);
	virtual void Reject(CReplace& replace, const char* reason);
	virtual void OrderNotFound(CExecution& execution);

	//////////////////////////////////////////////////////////////////////
	// Overidden Memory Map Methods
	//////////////////////////////////////////////////////////////////////
	bool LoadOrder(CCancelReject& cancelReject);
	bool LoadOrder(CExecution& execution);

private:

	//////////////////////////////////////////////////////////////////////
	// Exchange Session Methods
	//////////////////////////////////////////////////////////////////////
	void ExchangeProcess(CCancel& cancel) throw(CBrassSessionCancelException);
	void ExchangeProcess(COrder& order, long& sequenceNumber) throw(CBrassSessionOrderException);
	void ExchangeProcess(CReplace& replace) throw(CBrassSessionReplaceException);

	void InitializeMemoryMap(void);

	//////////////////////////////////////////////////////////////////////
	// Execution Processing Methods
	//////////////////////////////////////////////////////////////////////
	void ProcessReplacedOrder(CExecution& execution);

	// Member variables
	CBrassMemTagSet		*m_pTags;
	CBRASSInterface		*m_pBRASSInterface;
	CString				m_DefaultAccount;
	CString				m_TDPXAccount;
};