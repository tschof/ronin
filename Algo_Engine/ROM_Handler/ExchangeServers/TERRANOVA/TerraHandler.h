#include <exception>
#include "TagMapSet.h"
#include "MemTagSet.h"
#include "TerraStatus.h"
#include "BaseConnection.h"
#include "TerraNovaSession.h"
#include "IErrorHandler.h"
#include "ExchangeHandlerBase.h"

class CFixSession;
class CTerraMemTagSet;

class CArcaHandler : public CExchangeHandlerBase
{
public:

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	CArcaHandler(CArcaStatus& statusHandler, CConnection& host, CTerraNovaSession& exchangeSession, IErrorHandler& errorHandler, std::string& datFile);

	virtual ~CArcaHandler();

    void LoadTradeFor(CString& tradeFor, const char* staffordTag);

protected:

	//////////////////////////////////////////////////////////////////////
	// Overidden Order Processing Methods
	//////////////////////////////////////////////////////////////////////
	void Process(COrder& order);
	void Process(CCancel& cancel);
	void Process(CCancelReject& cancelReject);
	void Process(CExecution& execution);
	void Process(CReplace& replace);
	void Process(CReject& reject);

	//////////////////////////////////////////////////////////////////////
	// Overidden PreProcessing Methods
	//////////////////////////////////////////////////////////////////////
	void OnReceive(CCancelReject& cancelReject);
	void OnReceive(CExecution& execution);
	void OnReceive(CReject& reject);

	//////////////////////////////////////////////////////////////////////
	// Overidden Exception Processing Methods
	//////////////////////////////////////////////////////////////////////
	void Reject(CCancel& cancel, const char* reason);
	void Reject(COrder& order, const char* reason);
	void Reject(CReplace& replace, const char* reason);
	void OrderNotFound(const char* orderTag, const char* staffordTag, const char* debug);

	//////////////////////////////////////////////////////////////////////
	// Overidden Memory Map Methods
	//////////////////////////////////////////////////////////////////////
	bool LoadOrder(CExecution& execution);
	bool LoadOrder(CCancelReject& cancelReject);

     

private:

	//////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////
	void Initialize(std::string& datFile);
	void InitializeMemoryMap();

	//////////////////////////////////////////////////////////////////////
	// Order Processing Methods
	//////////////////////////////////////////////////////////////////////
	void ProcessReplacedOrder(CExecution& execution);
	void ProcessOrderExecution(CExecution& execution);

	void Exception(const char* message);

	//////////////////////////////////////////////////////////////////////
	// Exchange Session Methods
	//////////////////////////////////////////////////////////////////////
	void ExchangeProcess(CCancel& cancel);
	void ExchangeProcess(COrder& order, long& sequenceNumber);
	void ExchangeProcess(CReplace& replace);

	void CreateCancelTag(CString& orderTag, int sequenceNumber);

	CString					m_DefaultAccount;
	CArcaStatus&			m_statusHandler;
	CTerraNovaSession&		m_exchangeSession;
	IErrorHandler&			m_errorHandler;

	CTerraMemTagSet*		m_pTags;
};