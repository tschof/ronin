// ExchangeHandler.h: interface for the CExchangeHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTINETHANDLER_H__E0817967_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_ExchangeHANDLER_H__E0817967_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SendCancelReject.h"
#include "ExchangeApp.h"
#include "OrderHandler.h"
#include "OptionMemTagSet.h"
#include "Exchangesession.h"
#include "OrderTypeValidator.h"
#include "DatabaseSettingsInterface.h"
#include "DatabaseSettingsFactory.h"

typedef vector<COrder*> OrderVector;

class CExchangeHandler : public COrderHandler  
{
public:

	// incoming
	virtual void ProcessMessage(COrder& Order)
	{
		BOOL bMapCreated = FALSE;

		if ( (m_pApp->Host()).LoggedOn() != TRUE)
		{
			RejectOrder("Not logged in to exchange", Order);
			return;
		}
		
		try
		{
			m_orderTypeValidator->IsSupportedTIF((COrder::TimeInForce) (Order.TIF()));
			m_orderTypeValidator->IsSupportedOrderType((COrder::OrderType) (Order.Type()));
			
			long SeqNum = ( m_pApp->Exchange()).GetNextOutgoingSeqNum();
			Order.SetOrderTag(GetNewOrdTagString(SeqNum));
			bMapCreated = m_pTags->CreateNewMap(Order.StaffordTag(), "", 0, Order.SourceId());

			if(OnProcessOrder(Order))
			{
				m_pTags->SetSequenceTag(SeqNum);
				m_pTags->SetLeavesShares(Order.Shares());
				m_pTags->SetOriginalShares(Order.Shares());
				m_pTags->SetOrderTag(Order.OrderTag());
				m_pTags->SetCancelCount(0);
				m_pTags->SetReplaceCount(0);
				m_pTags->SetOrderAction(ORDACT_NONE);
				m_pTags->SetTradeFor(Order.TradeFor());
				if (Order.ClearingAccount().IsEmpty())
				{
					m_pTags->SetClearingAccount("");
				}
				else
				{
					m_pTags->SetClearingAccount(Order.ClearingAccount());
				}
				//status update
				m_pApp->Status().SetStatus(Order);
				m_pApp->ProcessDebug("ORDER SENT", Order.Debug());
			}
			//should be removed after verifying that it isnt used for any of the exchanges (depricate)
			else
			{
				RejectOrder(m_pApp->Name() + ":EH:PM(Order):Unable to send order:" + Order.Instructions()  , Order);				
				m_pTags->RemoveTags();
			}

		}
		catch(CTIFException e)
		{
			CString reason;
			reason.Format(m_pApp->Name() + ":EH:PM(Order):Unable to send order:Unsupported TimeInForce:%d", e.TimeInForce());
			RejectOrder(reason, Order);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
		catch(COrderTypeException e)
		{
			CString reason;
			reason.Format(m_pApp->Name() + ":EH:PM(Order):Unable to send order:Unsupported Order Type:%d", e.OrderType());
			RejectOrder(reason, Order);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
		catch (CString Message)
		{
			CString reason;
			reason.Format(m_pApp->Name() + ":EH:PM(Order):Unable to send order:" + Message);
			RejectOrder(reason, Order);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() + ":EH:PM(Order):Unable to send order:Exception: ", "Unknown exception");
			RejectOrder("Unknown exception", Order);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
	}

	virtual void ProcessMessage(CCancel& Cancel)
	{
		try
		{
			if (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Cancel.StaffordTag()))
			{
				Cancel.SetExchangeTag(m_pTags->ExchangeTag());
				Cancel.SetOrderTag(m_pTags->OrderTag());
				
				long SeqNum = ( m_pApp->Exchange()).GetNextOutgoingSeqNum();
				
				Cancel.SetCancelTag(GetNewOrdTagString(SeqNum) );

				/* Can't use this in the scenario where reject never comes back for the cancel pending hence sending a duplicate orderid from yesterday might hurt
				if(!strcmp(m_pTags->CancelTag(),""))
					Cancel.SetCancelTag(GetNewOrdTagString(SeqNum) );
				else
					Cancel.SetCancelTag(m_pTags->CancelTag());
				*/

				if(OnProcessCancel(Cancel))
				{
					m_pTags->SetSequenceTag(SeqNum);

					m_pTags->SetCancelTag(Cancel.CancelTag());

					m_pTags->SetCancelCount(m_pTags->CancelCount()+1);
					m_pTags->SetOrderAction(ORDACT_CXL);
					// update Status
					m_pApp->Status().SetStatus(Cancel);
					m_pApp->ProcessDebug("CANCEL SENT", Cancel.Debug());
				}
				//should be removed after verifying that it isnt used for any of the exchanges (depricate)
				else
				{
					m_pApp->Status().SetStatus(Cancel);//send cancel pending
					RejectCancel(m_pApp->Name() + ":EH:PM(Cancel):Unable to send to exchange for cancellation:" + Cancel.Instructions(),Cancel);
				}
			}
			else
			{
				m_pApp->Status().SetStatus(Cancel);//send cancel pending
				RejectCancel(m_pApp->Name() + ":EH:PM(Cancel):Can't find Original Order to cancel, hence rejected.",Cancel);
			}
		}
		catch (CString Message)
		{
			m_pApp->Status().SetStatus(Cancel);//send cancel pending
			CString reason;
			reason.Format(m_pApp->Name() + ":EH:PM(Cancel):Unable to send to exchange to cancel:" + Message);
			RejectCancel(reason, Cancel);
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Cancel):Unable to send to exchange for cancellation:", Message);
		}	
		catch (...)
		{
			m_pApp->Status().SetStatus(Cancel);//send cancel pending
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Cancel):Exception: ", "Unknown exception occurred while canceling order.");
			RejectCancel(m_pApp->Name() +":EH:PM(Cancel):Unknown exception while canceling order, cancel rejected.", Cancel);
		}

	}

	virtual void ProcessMessage(CReplace& Replace)
	{
		try
		{
			if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Replace.StaffordTag()))
			{
				//should move this into the OnProcessReplace functions
				/*if(m_pTags->ReplaceCount() > 0)
				{
					RejectReplace("The order is already cancel/replace pending",Replace);
					return;
				}*/
				
				m_orderTypeValidator->IsSupportedTIF((COrder::TimeInForce) (Replace.TIF()));
				m_orderTypeValidator->IsSupportedOrderType((COrder::OrderType) (Replace.Type()));

				long SeqNum = ( m_pApp->Exchange()).GetNextOutgoingSeqNum();

				// set the exchange's tag an the fix order tag
				Replace.SetExchangeTag(m_pTags->ExchangeTag());
				Replace.SetOrderTag(m_pTags->OrderTag());
				Replace.SetCancelTag(GetNewOrdTagString(SeqNum) );
				//Replace.SetOriginalShares(m_pTags->OriginalShares());
				
				Replace.SetClearingAccount(m_pTags->ClearingAccount());
				

				if(OnProcessReplace(Replace) )
				{
					m_pTags->SetSequenceTag(SeqNum);
					//m_pTags->SetReplaceShares(Replace.Shares());
					m_pTags->SetReplaceTag(Replace.CancelTag());
					m_pTags->SetReplaceCount(m_pTags->ReplaceCount()+1);
					m_pTags->SetOrderAction(ORDACT_REPLACE);
					
					m_pApp->ProcessDebug("REPLACE SENT", "");
					m_pApp->Status().SetStatus(Replace);
				}
				//should be removed after verifying that it isnt used for any of the exchanges (depricate)
				else
				{
					m_pApp->Status().SetStatus(Replace);//send replace pending
					// generate reject
					RejectReplace(m_pApp->Name() + ":EH:PM(Replace):Unable to send replace:" + Replace.Instructions(),Replace);
				}
			}
			else
			{
				m_pApp->Status().SetStatus(Replace);//send replace pending
				// generate reject
				RejectReplace(m_pApp->Name() + ":EH:PM(Replace):Can't find Original Order to replace, hence rejected.",Replace);
			}

		}
		catch (CString Message)
		{
			m_pApp->Status().SetStatus(Replace);//send replace pending
			CString reason;
			reason.Format(m_pApp->Name() + ":EH:PM(Replace):Unable to send to exchange to replace:" + Message);
			RejectReplace(reason, Replace);
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Replace):Unable to send to exchange to replace:", Message);
		}	
		catch (...)
		{
			m_pApp->Status().SetStatus(Replace);//send replace pending
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Replace):Exception: ", "Unknown exception");
			RejectReplace("Unknown Exception",Replace);
		}
	}

	virtual void ProcessMessage(CRequest& Order)
	{
		;
	}

	// outgoing
	virtual void ProcessMessage(const char *Status)
	{
	}

	virtual void ProcessMessage(CExecution& Execution)
	{
		try
		{
			m_bCancel = FALSE;

			if (Execution.TransactionType() == "3")
			{
				m_pApp->ProcessDebug("Status Msg Ignored: ", Execution.Debug());
				return;
			} 

			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag())
				|| (m_bCancel = m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OrderTag()))
				|| m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Execution.OrderTag())
				|| m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OriginalTag())
				|| (m_bCancel = m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OriginalTag()))
				|| m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Execution.OriginalTag()))
			{
				//this sourceid may be set to the pcname of OM
				Execution.SetSourceId(m_pTags->Client());
				// get the stafford tag based off of the order tag
				Execution.SetStaffordTag(m_pTags->StaffordTag());

				//Check to see if fill came in even though the order was cancel pending, if so then set status as Filled. i.e. DVG
				if(Execution.ExecType() == COrder::Filled)
					Execution.SetStatus(COrder::Filled);
				else if(Execution.Status() == COrder::Filled)
					Execution.SetExecType(COrder::Filled);
				else if((Execution.ExecType() == COrder::PartiallyFilled) && (Execution.Status() != COrder::Replaced) )
					Execution.SetStatus(COrder::PartiallyFilled);
				else if((Execution.Status() == COrder::PartiallyFilled) && (Execution.ExecType() != COrder::Replaced))
					Execution.SetExecType(COrder::PartiallyFilled);
				//check for cancel/replace pending
				else if(!NeedProcessPending() && (Execution.ExecType() == COrder::PendingCancel || Execution.Status() == COrder::PendingCancel))
				{
					m_pApp->ProcessDebug("PENDING IGNORED", Execution.Debug());
					return;
				}

				m_pApp->ProcessDebug("RECV STATUS", Execution.Debug());
				
				if(OnProcessExecution(Execution))
				{

					//seems redundant unless it would create another entry in the map if this condition was removed
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
					}
					// update Status
					m_pApp->Status().SetStatus(Execution);
				}
	
			}
			else
			{
				CString Error;
				Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
				m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Execution): ", Error);
			}
		}
		catch (const char *Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Execution) exception:", Message);
		}	
		catch (CString Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Execution) exception:", Message);
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Execution) exception: ", "Unknown exception");
		}
	}

	void ProcessMessage(CCancelReject& Reject)
	{
		BOOL bCancel = FALSE;
		BOOL bReplace = FALSE;
		try
		{
			if ((bCancel = m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Reject.CancelTag()))
				|| (bReplace = m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Reject.CancelTag())) )//ISE/CBOE Sends replace rejects back in CancelRejects as well
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
				if(	OnProcessCancelReject(Reject)) //ISE / PHLX / CBOE (NO-IMPLEMENTATION)
				{
					if(bCancel)
					{
						m_pTags->SetCancelCount(m_pTags->CancelCount()-1);
						if(m_pTags->CancelCount() < 1)
						{
							m_pTags->RemoveCancelTag();
							m_pTags->UnsetOrderAction(ORDACT_CXL);
						}
						m_pApp->Status().SetStatus(Reject);
					}
					else if(bReplace) //special handling of replace reject for ISE
					{
						m_pTags->SetReplaceCount(m_pTags->ReplaceCount()-1);
						if(m_pTags->ReplaceCount() < 1)
						{
							m_pTags->RemoveReplaceTag();
							m_pTags->UnsetOrderAction(ORDACT_REPLACE);
							//the below two statements might not be needed if at all for CBOE, check and confirm
							m_pTags->SetReplacePrice(0.0); //this should only be reset when all the cancels have been reject or when the replace has been confirmed
							m_pTags->SetReplaceShares(0); //this should only be reset when all the cancels have been reject or when the replace has been confirmed
						}
						CReplaceReject reject(Reject);
						m_pApp->Status().SetStatus(reject);
					}
				}
			}
			else
			{
				//the order might have already been closed and removed from the tag map
				m_pApp->ProcessErrorEx(MODE_ERR_BASIC,m_pApp->Name() +":EH:PM(CancelReject) CxlID Not Found ", Reject.Debug());
			}
		}
		catch (const char *Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(CancelReject) exception: ", Message);
		}	
		catch (CString Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(CancelReject) exception: ", Message);
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(CancelReject) exception: ", "Unknown exception");
		}
	}

	void ProcessMessage(CReplaceReject& Reject)
	{
		try
		{
			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.CancelTag())
				|| m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Reject.CancelTag())
				|| m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag())
				|| m_pTags->GetTags(MAP_REPLACE_TAG_QUERY, Reject.OrderTag()))
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
				if(	OnProcessReplaceReject(Reject)) //nothing done yet
				{
					m_pApp->Status().SetStatus(Reject);
					m_pTags->SetReplaceCount(m_pTags->ReplaceCount()-1);
					if(m_pTags->ReplaceCount() < 1)
					{
						m_pTags->RemoveReplaceTag();
						m_pTags->UnsetOrderAction(ORDACT_REPLACE);
						m_pTags->SetReplacePrice(0.0); //this should only be reset when all the cancels have been reject or when the replace has been confirmed
						m_pTags->SetReplaceShares(0); //this should only be reset when all the cancels have been reject or when the replace has been confirmed
					}
				}
			}
			else
			{
				m_pApp->ProcessErrorEx(MODE_ERR_BASIC,m_pApp->Name() +":EH:PM(ReplaceReject) CxlID Not Found ", Reject.Debug());
			}
		}
		catch (const char *Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(ReplaceReject) exception: ", Message);
		}
		catch (CString Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(ReplaceReject) exception: ", Message);
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(ReplaceReject) exception: ", "Unknown exception");
		}
	}

	virtual void ProcessMessage(CReject& Reject)
	{
		try
		{
			m_pApp->ProcessDebug("RECV STATUS", Reject.Debug());

			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag())
				|| m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Reject.OrderTag())
				|| m_pTags->GetTags(MAP_SEQUENCE_TAG_QUERY, Reject.OrderTag()))
			{
				// set tags
				Reject.SetOrderTag(m_pTags->OrderTag());
				Reject.SetExchangeTag(m_pTags->ExchangeTag());
				Reject.SetCancelTag(m_pTags->CancelTag());
				Reject.SetStaffordTag(m_pTags->StaffordTag());
				Reject.SetSourceId(m_pTags->Client());

				if(m_pTags->IsOrderActionSet(ORDACT_REPLACE))
				{
					m_pApp->Status().SetStatus((CReplaceReject&)Reject);
				}
				else if(m_pTags->IsOrderActionSet(ORDACT_CXL))
				{
					m_pApp->Status().SetStatus((CCancelReject&)Reject);
				}
				else
				{
					// update Status
					m_pApp->Status().SetStatus(Reject);
					m_pTags->RemoveTags();
				}
			}
			else
			{
				CString l_Err = "Can't find reject ID " + Reject.OrderTag() + " on " + Reject.Debug();
				l_Err += "\n" + Reject.Text();
				m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Reject): ", l_Err);
			}
		}
		catch (const char *Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Reject) exception: ", Message);
		}	
		catch (CString Message)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Reject) exception: ", Message);
		}	
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() +":EH:PM(Reject) exception: ", "Unknown exception");
		}
	}

//spread order processing
virtual void ProcessMessage(CString& String)
{
	OrderVector orderVector;
	int			posSeperator;
	int			nLength = String.GetLength();

	 for(int index = 0; (-1 != (posSeperator = String.Find(index, SPREAD_ORDER_SEPARATOR))); )
	{
		COrder *pOrder = new COrder(CCsv(String.Mid(index,posSeperator)));
		orderVector.push_back(pOrder);
		if(nLength > posSeperator + 1)
			index = posSeperator + 1;
	}


	BOOL bMapCreated = FALSE;

	if ( (((CMessageApp*) AfxGetApp())->Host()).LoggedOn() != TRUE)
	{
		for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			RejectOrder("Not logged in to exchange", orderVector[i]);
		}
		return;
	}
	
	try
	{

		long SeqNum = ( ((CMessageApp*) AfxGetApp())->Exchange()).GetNextOutgoingSeqNum();
		
		for(int i = 0; i < nLength; i++)
			orderVector[i]->SetOrderTag(GetNewOrdTagString(SeqNum) + char(i+48));
		
		bMapCreated = m_pTags->CreateNewMap(orderVector[i]->StaffordTag(), "", 0, orderVector[i]->SourceId());

		if(OnProcessSpread(orderVector))
		{
			for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
			{
				m_pTags->SetSequenceTag(SeqNum);//what will a subsequent lookup on this value result in.
				m_pTags->SetLeavesShares(orderVector[i]->Shares());//Fix this
				m_pTags->SetOriginalShares(orderVector[i]->Shares());//Fix this
				m_pTags->SetOrderTag(orderVector[i]->OrderTag());
				m_pTags->SetCancelCount(0);
				m_pTags->SetReplaceCount(0);
				m_pTags->SetOrderAction(ORDACT_NONE);
				//status update
				m_pApp->Status().SetStatus((COrder&)orderVector[i]);
				m_pApp->ProcessDebug("ORDER SENT", orderVector[i]->Debug());
			}
		}
		//should be removed after verifying that it isnt used for any of the exchanges (depricate)
		else
		{
			for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
			{
				RejectOrder(m_pApp->Name() + ":EH:PM(Order):Unable to send order:" +  orderVector[i]->Instructions()  ,  orderVector[i]);				
				m_pTags->RemoveTags();
			}
		}

	}
	catch(CTIFException e)
	{
		CString reason;
		reason.Format(m_pApp->Name() + ":EH:PM(Order):Unable to send order:Unsupported TimeInForce:%d", e.TimeInForce());
		for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			RejectOrder(reason, orderVector[i]);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
	}
	catch(COrderTypeException e)
	{
		CString reason;
		reason.Format(m_pApp->Name() + ":EH:PM(Order):Unable to send order:Unsupported Order Type:%d", e.OrderType());
		for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			RejectOrder(reason, orderVector[i]);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
	}
	catch (CString Message)
	{
		CString reason;
		reason.Format(m_pApp->Name() + ":EH:PM(Order):Unable to send order:" + Message);
		for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			RejectOrder(reason, orderVector[i]);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
	}	
	catch (...)
	{
		m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,m_pApp->Name() + ":EH:PM(Order):Unable to send order:Exception: ", "Unknown exception");
		for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			RejectOrder("Unknown exception", orderVector[i]);
			if(bMapCreated)
				m_pTags->RemoveTags();
		}
	}
}

inline COptionMemTagSet*& GetMemTag()			{return m_pTags;}
	
protected:
	virtual BOOL OnProcessOrder(COrder& Order) { return TRUE; }
	virtual BOOL OnProcessCancel(CCancel& Cancel){ return TRUE; }
	virtual BOOL OnProcessReplace(CReplace& Replace){ return TRUE; }
	virtual BOOL OnProcessSpread(OrderVector& orderVector){ return FALSE; }

	virtual	BOOL OnProcessExecution(CExecution& Execution) { return TRUE; }
	virtual BOOL OnProcessCancelReject(CCancelReject& Reject){ return TRUE; }
	virtual	BOOL OnProcessReject(CReject& Reject){ return TRUE; }
	virtual BOOL OnProcessReplaceReject(CReplaceReject& Reject){ return TRUE; }

	virtual void InitializeMemoryMap() {};
	virtual void InitializeOrderValidator() {};

	//reject order before it hits the exchange
	virtual void RejectOrder(const char* Reason, const COrder& Order)
	{
		try
		{
			CReject	Reject(Reason, Order);			
//			m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), -1, Order.SourceId());
			m_pApp->Status().SetStatus(Reject);
			m_pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		}
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ORDER REJECT CAUSED A EXCEPTION", "");
		}
	}

	virtual void RejectCancel(const char* Reason, const CCancel& Cancel)
	{
		try
		{
			CSendCancelReject Reject(Cancel);		
			// set the cancel tag which is generated by the fix session
//			m_pTags->SetCancelTag(Cancel.CancelTag());
			// update Status
			Reject.SetText(Reason);
			m_pApp->Status().SetStatus(Reject);
			m_pApp->ProcessDebug("CANCEL REJECT", Cancel.Debug());
		}
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"CANCEL REJECT CAUSED A EXCEPTION", "");
		}
	}

	virtual void RejectReplace(const char* Reason, const CReplace& Replace)
	{
		try
		{
			CReplaceReject Reject(Reason, Replace);
			// update Status
			m_pApp->Status().SetStatus(Reject);
			m_pApp->ProcessDebug("REPLACE REJECT", Replace.Debug());
		}
		catch (...)
		{
			m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"REPLACE REJECT CAUSED A EXCEPTION", "");
		}
	}


protected:
	CMessageApp			*m_pApp;
	COptionMemTagSet	*m_pTags;
	COrderTypeValidator	*m_orderTypeValidator;
	BOOL				 m_bCancel;

	virtual bool IsModify() {return true;}
	virtual bool NeedProcessPending() {return false;}
	virtual CString& GetNewOrdTagString(long SeqNum)
	{
		// WORK ONLY FOR SINGLE THREADED!!!
		static CString OrdTag;
		OrdTag.Format("%s%d", CTime::GetCurrentTime().Format("%Y%m%d"),SeqNum);
		return OrdTag;
	}

public:
	CExchangeHandler()
	{
		m_pTags		= NULL;
		m_pApp		= (CMessageApp *)AfxGetApp();
		m_bCancel	= FALSE;		
	}

	virtual ~CExchangeHandler()
	{
	}

protected:

	virtual	void OnReplaced(CExecution& Execution)
	{
		if(strcmp(Execution.OrderTag(),"") == 0)
			m_pTags->SetOrderTag(m_pTags->ReplaceTag());
		else
			m_pTags->SetOrderTag(Execution.OrderTag());
		
		//set tags
		m_pTags->RemoveReplaceTag();
		m_pTags->SetReplaceCount(0);
		m_pTags->RemoveCancelTag();
		m_pTags->SetCancelCount(0);
		
		//set the shares right
		m_pTags->SetLeavesShares(Execution.Leaves()); //Leaves Qty being set
		//never gets updated after orginal quantity
		//m_pTags->SetOriginalShares(Execution.Shares()); //Original Order Qty being set
		m_pTags->SetOrderAction(ORDACT_NONE);
		m_pTags->SetOrderAction(ORDACT_OPEN);

		if(IsModify())
			m_pTags->SetLastCumQty(Execution.Shares() - Execution.Leaves()); //make sure that these fields are available in execution reports from all the fields
		else
			m_pTags->SetLastCumQty(0);

		Execution.SetShares(Execution.Leaves());
		//set status to replace
		Execution.SetStatus(COrder::Replaced);
	}

};

#endif // !defined(AFX_ExchangeHANDLER_H__E0817967_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
