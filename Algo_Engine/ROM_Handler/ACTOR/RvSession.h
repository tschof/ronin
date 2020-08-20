#include <afxmt.h>
#include "rvcpp.h"
#include "rvevmwin.h"
#include "TibMsg.h"
#include "MessageApp.h"

class CRvCallBackHandler;

class CRvSession
{

public:
	RvSession			*m_pSession;
	
private:
	
	// rv session
		rv_Session		m_RvEmbeddedSession;

	// rv listeners
		CObList			m_Handlers;

	// subjects
		CString			m_OrderSubject;
		CString			m_CancelSubject;
		CString			m_StatusRequestSubject;
		CString			m_StatusSubject;

	// server info
		char			m_Server[128];
		char			m_Port[128];
	
	// rv sender
		RvSender			*m_pSender;

public:
	int				ProcessStatusReport(const char *Status, const CString& StaffordTag = "");
	int				ProcessHeartBeat(const char *Status);

	COrderHandler	*Handler();		

public:
	CRvSession();
	virtual ~CRvSession();
};

class CRvHandler : public CObject
{
public:
	RvListener		*m_pListener;
	RvDataCallback	*m_pHandler;

public:
	CRvHandler()
		{
		m_pListener = NULL;
		m_pHandler = NULL;
		}
	~CRvHandler()
		{
		if (m_pListener)
			delete m_pListener;
		if (m_pHandler)
			delete m_pHandler;
		}
};
		
class CRvCallBackHandler : public RvDataCallback
{
private:
	CRvSession	*m_pSession;

public:
	virtual void onData(const char* subject,
			RvSender*         replySender,
			const RvDatum&    data,
			RvListener*       invoker) = 0;

public:
	COrderHandler	*Handler()				{ return m_pSession->Handler(); }
	
public:
	CRvCallBackHandler(CRvSession *pSession) : RvDataCallback() { m_pSession = pSession; }
	~CRvCallBackHandler() {};
};

class CRvOrderCallBack : public CRvCallBackHandler
{
public:
	void onData(const char* subject,
			RvSender*         replySender,
			const RvDatum&    data,
			RvListener*       invoker);

public:
	CRvOrderCallBack(CRvSession *pSession) : CRvCallBackHandler(pSession) { }
	~CRvOrderCallBack() { }
};

/*class CRvCancelCallBack : public CRvCallBackHandler
{
public:
	void onData(const char* subject,
			RvSender*         replySender,
			const RvDatum&    data,
			RvListener*       invoker);

public:
	CRvCancelCallBack(CRvSession *pSession) : CRvCallBackHandler(pSession) { }
	~CRvCancelCallBack() { }
};

class CRvStatusCallBack : public CRvCallBackHandler
{
public:
	void onData(const char* subject,
			RvSender*         replySender,
			const RvDatum&    data,
			RvListener*       invoker);

public:
	CRvStatusCallBack(CRvSession *pSession) : CRvCallBackHandler(pSession) { }
	~CRvStatusCallBack() { }
};
*/