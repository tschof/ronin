#ifndef _ABNAMRO_HANDLER_
#define _ABNAMRO_HANDLER_

#include "Exchangehandler.h"
#include "optfixsession.h"

class CABNAMROHandler : public CExchangeHandler
{
public:
	CABNAMROHandler();

	virtual ~CABNAMROHandler()
	{
		delete m_pTags;
		delete m_orderTypeValidator;
	}

protected:
	inline BOOL OnProcessOrder(COrder& Order)
	{
		Order.SetSourceId(m_senderSubID);
		Order.SetSecurityType(COrder::Option);

		BOOL result = ((COptFixSession*)&(m_pApp->Exchange()))->ProcessEntry(Order);
		
		Order.SetSourceId(m_pTags->Client());
		return result;
	}

	inline BOOL OnProcessCancel(CCancel& Cancel)
	{
		Cancel.SetSourceId(m_senderSubID);
		Cancel.SetLeavesQty(m_pTags->LeavesShares());
		Cancel.SetShares(m_pTags->LeavesShares());//TEMP
		//Cancel.SetShares(m_pTags->OriginalShares());//TEMP
		
		BOOL result = ((COptFixSession*)&(m_pApp->Exchange()))->ProcessCancel(Cancel);
		
		Cancel.SetSourceId(m_pTags->Client());
		return result;
	}

	inline BOOL OnProcessReplace(CReplace& Replace)
	{
		CString Msg("AH:OPR:ABN doesn't support true cancel replace.");
		throw Msg;
	}

	inline BOOL OnProcessExecution(CExecution& Execution)
	{
		if(COrder::Stopped == Execution.Status())
			return FALSE;

		m_pTags->SetLastCumQty(Execution.Cumulative());
		return TRUE;
	}

	inline BOOL OnProcessCancelReject(CCancelReject& Reject)
	{
		return TRUE;
	}

	inline virtual void ProcessMessage(CReject& Reject)
	{
		int index1, index2;

		if(-1 != (index1 = Reject.Text().Find("##")) )
			if(-1 != (index2 = Reject.Text().Find("##", index1+2)) )		
			{
				Reject.SetOrderTag(Reject.Text().Mid( index1+2, index2 - (index1+2) ));
			}

		CExchangeHandler::ProcessMessage(Reject);
	}

	inline BOOL OnProcessReject(CReject& Reject)
	{
		return TRUE;
	}

private:
	void InitializeMemoryMap();
	void InitializeOrderValidator();
private:
	CString m_senderSubID;
};


#endif