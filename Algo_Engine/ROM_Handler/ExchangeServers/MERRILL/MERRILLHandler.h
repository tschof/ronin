/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILLHandler.h: interface for the CMERRILLHandler class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#if !defined(AFX_MERRILLHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
#define AFX_MERRILLHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Exchangehandler.h"

#include "fixmsg42\FixAppMsgs.h"
#include "fixmsg42\fixsession.h"

class CMERRILLHandler : public CExchangeHandler
{
public:
	CMERRILLHandler();
	virtual ~CMERRILLHandler()
	{
		delete m_pTags;
		delete m_orderTypeValidator;
	}

protected:

	inline BOOL OnProcessOrder(COrder& Order)
	{
		Order.SetSecurityType(COrder::Equity);
		return ((CFixSession*)&(m_pApp->Exchange()))->ProcessEntry(Order);
	}

	inline BOOL OnProcessCancel(CCancel& Cancel)
	{
		Cancel.SetSecurityType(COrder::Equity);
		return ((CFixSession*)&(m_pApp->Exchange()))->ProcessCancel(Cancel);
	}

	inline BOOL OnProcessReplace(CReplace& Replace)
	{
		
		Replace.SetSecurityType(COrder::Equity);
		Replace.SetSourceId("");

		if(m_pTags->ReplaceCount() > 0)
		{
			CString Msg;
			Msg.Format("AH:OPR:The order is already cancel/replace pending.");
			throw Msg;
		}

		Replace.SetShares(m_pTags->LastCumQty() + Replace.Shares());

		return ((CFixSession*)&(m_pApp->Exchange()))->ProcessReplace(Replace);
	}

	inline virtual BOOL OnProcessExecution(CExecution& Execution)
	{
		//Ignore this status
		if(Execution.Status() == COrder::Stopped)
			return FALSE;

		if(Execution.ExecType() == COrder::ReplacePending )
		{
			m_pTags->SetOrderAction(ORDACT_REPLACE); //ignore replace acknowledgement
			return FALSE;
		}

		if(Execution.Status() == COrder::PartiallyFilled)
			m_pTags->SetLastCumQty(Execution.Cumulative());

		//if(Execution.Status() == COrder::Replaced)
		//	m_pTags->SetOriginalShares(Execution.Shares());

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

		Execution.SetLeaves(m_pTags->LeavesShares());

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

	}

	inline BOOL OnProcessCancelReject(CCancelReject& Reject)
	{
		return TRUE;
	}
	inline BOOL OnProcessReject(CReject& Reject)
	{
		return TRUE;
	}

	inline virtual CString& GetNewOrdTagString(long SeqNum)
	{
		// WORK ONLY FOR SINGLE THREADED!!!
		static CString OrdTag;
		OrdTag.Format("%sROM%0.5d", CTime::GetCurrentTime().Format("%Y%m%d"),SeqNum);
		return OrdTag;
	}

private:
	void InitializeMemoryMap();
	void InitializeOrderValidator();
};

#endif // !defined(AFX_MERRILLHANDLER_H__0BAB0042_9751_4E87_9B36_226FC8D4FCC7__INCLUDED_)
