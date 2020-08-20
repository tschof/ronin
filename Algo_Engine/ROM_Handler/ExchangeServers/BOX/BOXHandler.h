// BOXHandler.h: interface for the CBOXHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOXHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
#define AFX_BOXHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Exchangehandler.h"
#include "optfixsession.h"

class CBOXHandler : public CExchangeHandler
{
public:
	CBOXHandler();
	virtual ~CBOXHandler()
	{
		delete m_pTags;
		delete m_orderTypeValidator;
	}

protected:

	void ProcessMessage(CCancelReject& Reject)
	{
		try
		{
			if (m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Reject.CancelTag())
				|| m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.CancelTag()) 
				|| m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Reject.OrderTag())
				|| m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag()))
			{
				if(Reject.OrderTag().IsEmpty())
					Reject.SetOrderTag(m_pTags->OrderTag());
				if(Reject.ExchangeTag().IsEmpty())
					Reject.SetExchangeTag(m_pTags->ExchangeTag());
				if(Reject.CancelTag().IsEmpty())
					Reject.SetCancelTag(m_pTags->ReplaceTag());
				if(Reject.StaffordTag().IsEmpty())
					Reject.SetStaffordTag(m_pTags->StaffordTag());
				//if(Reject.SourceId().IsEmpty())
					Reject.SetSourceId(m_pTags->Client());

				m_pApp->ProcessDebug("RECV STATUS", Reject.Debug());

				// update Status
				if(	OnProcessCancelReject(Reject))
				{
					m_pTags->SetCancelCount(m_pTags->CancelCount()-1);
					if(m_pTags->CancelCount() < 1)
					{
						m_pTags->RemoveCancelTag();
						m_pTags->UnsetOrderAction(ORDACT_CXL);
					}
					m_pApp->Status().SetStatus(Reject);
				}
			}
			else
			{
				//the order might have already been closed and removed from the tag map
				m_pApp->ProcessErrorEx(MODE_ERR_BASIC,m_pApp->Name() +":ExchangeHandler:CancelReject CxlID Not Found ", Reject.Debug());
			}
		}
		catch (const char *Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler ProcessMessage (Cancel/Reject) exception: ", Message);
		}	
		catch (CString Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler ProcessMessage (Cancel/Reject) exception: ", Message);
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler ProcessMessage (Cancel/Reject) exception: ", "Unknown exception");
		}
	}

	inline BOOL OnProcessOrder(COrder& Order)
	{
		Order.SetSecurityType(COrder::Option);
		return ((COptFixSession*)&(m_pApp->Exchange()))->ProcessEntry(Order);
	}

	inline BOOL OnProcessCancel(CCancel& Cancel)
	{
		Cancel.SetSecurityType(COrder::Option);
		return ((COptFixSession*)&(m_pApp->Exchange()))->ProcessCancel(Cancel);
	}

	inline BOOL OnProcessReplace(CReplace& Replace)
	{
		Replace.SetSecurityType(COrder::Option);
		Replace.SetSourceId("");
	
		if(m_pTags->ReplaceCount() > 0)
		{
			CString Msg;
			Msg.Format("BH:OPR:The order is already cancel/replace pending.");
			throw Msg;
		}

		return ((COptFixSession*)&(m_pApp->Exchange()))->ProcessReplace(Replace);
	}

	inline virtual BOOL OnProcessExecution(CExecution& Execution)
	{
		//Ignore this status
		if(Execution.Status() == COrder::Stopped)
			return FALSE;
		
		else if( Execution.ExecType() == COrder::Canceled && (m_bCancel == FALSE) && (m_pTags->ReplaceCount() > 0 || m_pTags->IsOrderActionSet(ORDACT_REPLACE)))
		{
			m_pTags->SetOrderAction(ORDACT_REPLACE_ACK); //set state of replace being acknowledged and savereplace price and shares
			return FALSE;
		}
		else if(Execution.ExecType() == COrder::Open)//open can come for other OrdStatus like COrder::Exposed, COrder::DirectedThruIML and COrder::Directed
		{
			if(m_pTags->IsOrderActionSet(ORDACT_REPLACE_ACK) && m_pTags->IsOrderActionSet(ORDACT_REPLACE)) //a open message after the cancel represents a replaced scenario
				Execution.SetExecType(COrder::Replaced);
			else if(m_pTags->IsOrderActionSet(ORDACT_OPEN))//since we have already received an open from the exchange ignore subsequent opens
				return FALSE;
		}

		//IMP::ExecType always tells what the current action of the order is.... thats why we set the Status the same since OrdStatus is used subsequently everywhere
		Execution.SetStatus(Execution.ExecType());

		return TRUE;
	}

	inline BOOL OnProcessCancelReject(CCancelReject& Reject)
	{
		return TRUE;
	}
	inline BOOL OnProcessReject(CReject& Reject)
	{
		return TRUE;
	}

	inline CString& GetNewOrdTagString(long SeqNum)
	{
		// WORK ONLY FOR SINGLE THREADED!!!
		static CString OrdTag;
		OrdTag.Format("%s%06d-%s",m_branchID,SeqNum, CTime::GetCurrentTime().Format("%Y%m%d"));
		return OrdTag;
	}

private:
	void InitializeMemoryMap();
	void InitializeOrderValidator();
private:
	CString m_branchID;
};

#endif // !defined(AFX_BOXHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
