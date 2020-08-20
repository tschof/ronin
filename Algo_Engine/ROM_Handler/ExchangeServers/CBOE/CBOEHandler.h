// CBOEHandler.h: interface for the CCBOEHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CBOEHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
#define AFX_CBOEHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Exchangehandler.h"
#include "optfixsession.h"

class CCBOEHandler : public CExchangeHandler  
{
public:
	CCBOEHandler();

	virtual ~CCBOEHandler()
	{
		delete m_pTags;
		delete m_orderTypeValidator;
	}

protected:
	inline BOOL OnProcessOrder(COrder& Order)
	{
		Order.SetSecurityType(COrder::Option);
		return ((COptFixSession*)&(m_pApp->Exchange()))->ProcessEntry(Order);
	}

	inline BOOL OnProcessCancel(CCancel& Cancel)
	{
		CString strInstructions = Cancel.Instructions();
		strInstructions.MakeUpper();
		
		if(-1 != strInstructions.Find("P:",0))//Linkage Order
		{
			CString Msg;
			Msg.Format("CH:OPC:Linkage orders cant be canceled.");
			throw Msg;
		}

		if( m_pTags->CancelCount() > 0 && !Cancel.OwnerCancel() )
		{
			/*CString Msg;
			Msg.Format("CH:OPC:The order is already cancel pending.");
			throw Msg;*/
			Cancel.SetInstructions("CH:OPC:The order is already cancel pending.");
			return FALSE;
		}

		Cancel.SetShares(m_pTags->OriginalShares());//is this field relevant at all

		Cancel.SetSourceId("C:"+ Cancel.OrderTag());

		return ((COptFixSession*)&(m_pApp->Exchange()))->ProcessCancel(Cancel);
	}

	inline BOOL OnProcessReplace(CReplace& Replace)
	{	
		CString strInstructions = Replace.Instructions();
		strInstructions.MakeUpper();
		
		if(-1 != strInstructions.Find("P:",0))//Linkage Order
		{
			CString Msg;
			Msg.Format("CH:OPR:Linkage orders cant be replaced.");
			throw Msg;
		}
			
		if(m_pTags->ReplaceCount() > 0)
		{
			CString Msg;
			Msg.Format("CH:OPR:The order is already cancel/replace pending.");
			throw Msg;
		}

		//SPECIAL CASE: Have to cache for replace at this point since "replaced" report doesnt have it from the CBOE
		if(Replace.Type() == COrder::Stop || Replace.Type() == COrder::StopLimit)
			m_pTags->SetReplaceStopPrice(Replace.StopPrice());

		Replace.SetSourceId("R:"+ Replace.OrderTag());
		Replace.SetSecurityType(COrder::Option);
		Replace.SetShares(m_pTags->LastCumQty() + Replace.Shares());
			
		return ((COptFixSession*)&(m_pApp->Exchange()))->ProcessReplace(Replace);
	}

	inline virtual void ProcessMessage(CExecution& Execution)
	{
		try
		{
			if (Execution.TransactionType() == "3")
			{
				m_pApp->ProcessDebug("Status Msg Ignored: ", Execution.Debug());
				return;
			} 

			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag())
				|| m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OrderTag())
				|| m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Execution.OrderTag())
				|| m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OriginalTag())
				|| m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OriginalTag())
				|| m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Execution.OriginalTag()))
			{
				//this sourceid may be set to the pcname of OM
				Execution.SetSourceId(m_pTags->Client());
				// get the stafford tag based off of the order tag
				Execution.SetStaffordTag(m_pTags->StaffordTag());

				//check for cancel/replace pending
				if(!NeedProcessPending() && (Execution.ExecType() == COrder::PendingCancel || Execution.Status() == COrder::PendingCancel))
				{
					m_pApp->ProcessDebug("PENDING IGNORED", Execution.Debug());
					return;
				}

				m_pApp->ProcessDebug("RECV STATUS", Execution.Debug());
				
				if(OnProcessExecution(Execution))
				{

					m_pTags->SetExchangeTag(Execution.ExchangeTag());

					//whats the case when there is a message with a partial fill / replace
					//can we put this in the switch case
					if(Execution.ExecType() == COrder::Replaced || Execution.Status() == COrder::Replaced)
						OnReplaced(Execution);
					else if(Execution.ExecType() == COrder::Open )
						m_pTags->SetOrderAction(ORDACT_OPEN);

					switch(Execution.Status())
					{
						case COrder::Canceled:
						case COrder::Filled:
						case COrder::DoneForDay:
						case COrder::Rejected:
							m_pTags->RemoveTags();
							break;
						case COrder::PartiallyFilled:
							m_pTags->SetLeavesShares(Execution.Leaves());
							m_pTags->SetLastCumQty(Execution.Cumulative());
					}
					// update Status
					m_pApp->Status().SetStatus(Execution);
				}
	
			}
			else
			{
				//ignore "Cancelled" msg with cxl qty of zero
				if(Execution.ExecType() == COrder::Canceled && Execution.Status() == COrder::Canceled )
					if(	0 == Execution.Shares() )
						return;

				CString Error;
				Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
				// ignore "PendingCancel" msg
				if(Execution.ExecType() == COrder::PendingCancel || Execution.Status() == COrder::PendingCancel )
				{
					m_pApp->ProcessError(m_pApp->Name() +":Exchange Handler Process Late CancelPending: ", Error);
					return;
				}
				m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler Process Execution: ", Error);
			}
		}
		catch (const char *Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler ProcessMessage (Report) exception: ", Message);
		}	
		catch (CString Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler ProcessMessage (Report) exception: ", Message);
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":Exchange Handler ProcessMessage (Report) exception: ", "Unknown exception");
		}
	}

	inline virtual BOOL OnProcessExecution(CExecution& Execution)
	{
		//Ignore these status
		if(Execution.Status() == COrder::Stopped)
			return FALSE;

		if(Execution.ExecType() == COrder::ReplacePending )
		{
			m_pTags->SetOrderAction(ORDACT_REPLACE); //ignore replace acknowledgement
			return FALSE;
		}
		else if((m_pTags->ReplaceCount() > 0 || m_pTags->IsOrderActionSet(ORDACT_REPLACE)) && Execution.ExecType() == COrder::Open)
		{
			m_pTags->SetOrderAction(ORDACT_REPLACE_ACK); //set state of replace being acknowledged and save replace price and shares
//			Execution.SetExecType(COrder::Replaced);
			m_pTags->SetReplaceShares(Execution.Leaves()); //the new open qty of the replace
			m_pTags->SetReplacePrice(Execution.Price());
			m_pTags->SetTimeInForce(Execution.TimeInForce());
			//m_pTags->SetReplaceStopPrice(Execution.StopPrice());//CBOE doesnt send stop price in replaced message so cache it when u send the initial replace out
			m_pTags->SetReplaceOrderType(Execution.OrderType());
			if(!Execution.Instructions().IsEmpty())
				m_pTags->SetReplaceExecInst(Execution.Instructions().GetAt(0));

			return FALSE;
		}
		else if(Execution.ExecType() == COrder::Canceled ) //Replace processing
		{
			if(0 == Execution.Shares())
			{
				m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":CBOEHandler:OnProcessExecution:", "Received a cancelled message with zero quantity cancelled");
				return FALSE; //ignore cancel since it has a value of zero that it cancelled, this case shouldn't normally occur
			}
			else if(m_pTags->IsOrderActionSet(ORDACT_REPLACE_ACK) && m_pTags->IsOrderActionSet(ORDACT_REPLACE))
//				return FALSE; //ignore cancel since it represents a replaced message which was already taken care of with Open message
				Execution.SetExecType(COrder::Replaced);
		}
		else if(Execution.ExecType() == COrder::Filled)
		{
			if( Execution.Cumulative() != Execution.Shares() )//this is to cater to the isse where CBOE sends an incomplete fill i.e. Cum < Original Shares  yet reports it as fill, so we convert it into a partial
				Execution.SetExecType(COrder::PartiallyFilled);
		}

		//IMP::ExecType always tells what the current action of the order is.... thats why we set the Status the same since OrdStatus is used subsequently everywhere
		Execution.SetStatus(Execution.ExecType());

		return TRUE;
	}

	virtual	void OnReplaced(CExecution& Execution)
	{
		m_pTags->SetOrderTag(m_pTags->ReplaceTag());
		Execution.SetOrderTag(m_pTags->ReplaceTag());

		//set the shares right
		m_pTags->SetLeavesShares(m_pTags->ReplaceShares());
		m_pTags->SetOriginalShares(m_pTags->ReplaceShares());
		
		Execution.SetShares(m_pTags->ReplaceShares());
		Execution.SetPrice(m_pTags->ReplacePrice());

		Execution.SetTimeInForce(m_pTags->TimeInForce());

		Execution.SetOrderType(m_pTags->ReplaceOrderType());

		if(!Execution.Instructions().IsEmpty())
			Execution.SetInstructions(m_pTags->ReplaceExecInst());

		if(m_pTags->ReplaceStopPrice() > 0.0)
			Execution.SetStopPrice(m_pTags->ReplaceStopPrice());

		//set status to open
		Execution.SetStatus(COrder::Replaced);

		//set tags
		m_pTags->RemoveReplaceTag();
		m_pTags->SetReplaceCount(0);
		m_pTags->RemoveCancelTag();
		m_pTags->SetCancelCount(0);
		m_pTags->SetOrderAction(ORDACT_NONE);
		m_pTags->SetOrderAction(ORDACT_OPEN);
		m_pTags->SetReplaceShares(0);
		m_pTags->SetReplacePrice(0.0);
		m_pTags->SetReplaceStopPrice(0.0);
		m_pTags->SetReplaceExecInst(-1);
		m_pTags->SetTimeInForce(-1);
		m_pTags->SetReplaceOrderType(-1);

		m_pTags->SetLastCumQty(0); //since CBOE does not carry any prevcious CUM_QTY history forward after a replace
	}


	inline BOOL OnProcessCancelReject(CCancelReject& Reject)
	{
		return TRUE;
	}
	inline BOOL OnProcessReject(CReject& Reject)
	{
		return TRUE;
	}

	void UpdateBranchId(int sd)
	{
		if (sd == 0 )
			return;

		int firstChar = sd / 676;				// 26*26
		int secondChar = (sd-firstChar*676)/26;
		int thirdChar = sd - firstChar*676 - secondChar*26;

		char newChar;
		int len = m_branchID.GetLength();
		if ( len >=3 )
		{
			newChar = m_branchID[len-3] + firstChar; 
			m_branchID.SetAt(len-3, newChar);
		}
		if ( len >= 2)
		{
			newChar = m_branchID[len-2] + secondChar; 
			m_branchID.SetAt(len-2, newChar);
		}
		if ( len >= 1)
		{
			newChar = m_branchID[len-1] + thirdChar; 
			m_branchID.SetAt(len-1, newChar);
		}
	}

	inline CString& GetNewOrdTagString(long SeqNum)
	{
		// WORK ONLY FOR SINGLE THREADED!!!
		static int firstRun = 1;
		int sequence = SeqNum % 10000;
		if ( firstRun )
		{
			firstRun = 0;
			UpdateBranchId(SeqNum/10000);
		}
		else if (SeqNum != 0 && sequence == 0 )
			UpdateBranchId(1);
		 
		static CString OrdTag;
		OrdTag.Format("%s%04d-%s",m_branchID, sequence, CTime::GetCurrentTime().Format("%Y%m%d"));
		return OrdTag;
	}

private:
	void InitializeMemoryMap();
	void InitializeOrderValidator();
private:
	CString m_branchID;
};

#endif // !defined(AFX_CBOEHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
