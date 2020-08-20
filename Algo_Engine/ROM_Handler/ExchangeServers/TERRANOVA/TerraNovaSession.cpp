/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * TerraNovaSession.cpp: implementation of the CTerraNovaSession class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/05/05    joel        And ExecInst field to support AON and PNP orders
 *
 *******************************************************************************/

#include "TerraNova.h"
#include "TerraNovaSession.h"
#include "FixAppMsgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


long CTerraNovaSession::ProcessEntry(COrder& a_Order)
{
	MapIntToChar ToExchangeSide = Translator().m_ToExchangeSide;
	MapIntToChar ToExchangeType = Translator().m_ToExchangeType;
	MapIntToChar ToExchangeTIF = Translator().m_ToExchangeTIF;

	long l_SeqNum = OutgoingMsgs().NextMsgSeqNum();
	CFixOrder l_Order(
		l_SeqNum, 
		GetCurrentTimeStr(), 
		a_Order.OrderTag(), 
		FIX_AutoPublic,
		a_Order.Symbol(), 
		(EFixSide) ToExchangeSide[a_Order.Side()], 
		(EFixOrdType) ToExchangeType[a_Order.Type()]
	);
    l_Order.OnBehalfOfCompID(a_Order.TradeFor());
    l_Order.SenderSubID(a_Order.TradeFor());
    
	if (a_Order.Type() == COrder::Stop)
	{
		l_Order.StopPx(a_Order.StopPrice());
		l_Order.Price(0);
	}
	else if (a_Order.Type() == COrder::StopLimit)
	{
		l_Order.StopPx(a_Order.StopPrice());
		l_Order.Price(a_Order.dPrice());
	}
	else
	{
		l_Order.Price(a_Order.dPrice());
	}

	if (a_Order.Floor() != 0)
	{
		l_Order.MaxFloor(a_Order.Floor());
	}

	// joel
	// Add support for the ExecInst field in order to handle
	// AON (Tag 18=G) and Participate Don't Initiate (Tag 18=6) orders
	// joel
	if(a_Order.ExecInst() != COrder::LastExecInst)  
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Order.ExecInst());
		l_Order.ExecInst(ExecInst);
	}													

	l_Order.TargetSubID(TargetSubId().c_str());

	// { WXU 
	//- l_Order.SenderSubID(a_Order.SourceId());
	/*TString senderSubId = SenderSubId();
	if ( senderSubId.length() > 0 )
		l_Order.SenderSubID((const char*)senderSubId.c_str());
	else
		l_Order.SenderSubID(a_Order.SourceId());
        */
	// }

	l_Order.OrderQty(a_Order.Shares());
	l_Order.Account(a_Order.ClearingAccount());
	l_Order.TimeInForce((EFixTimeInForce) ToExchangeTIF[a_Order.TIF()]);
	l_Order.Rule80A(a_Order.Capacity());

	if (a_Order.TIF() == COrder::GoodTillDate)
	{
		int year, month, day;

		year	= atoi(a_Order.ExpTime().Mid(0, 4));
		month	= atoi(a_Order.ExpTime().Mid(4,2));
		day		= atoi(a_Order.ExpTime().Mid(6,2));

		CTime t(year, month, day, 0 , 0, 0);

		CString ExpTime;
		ExpTime.Format("%s-23:00:00", t.Format("%Y%m%d"));
		l_Order.ExpireTime((LPCTSTR)ExpTime);
	}

	if (a_Order.ExDestination() == "A" ||
		a_Order.ExDestination() == "N")
	{
		if (a_Order.Instructions() != "DOT")
		{
			l_Order.ExecBroker("ARCA");
		}
		else
		{
			if (a_Order.ExDestination() == "A")
			{
				l_Order.ExecBroker("ASE");
			}
			else if (a_Order.ExDestination() == "N")
			{
				l_Order.ExecBroker("NYS");
			}
		}
	}

	if (false == AddTradingSessions(l_Order, l_Order.OrdType()))
	{
		return 0;
	}

	if(SendMsg(l_Order))
	{
		return l_SeqNum;
	}
	else
	{
		return 0;
	}
}


BOOL CTerraNovaSession::ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec)
{
	if (a_Exec.OrdStatus() == FIX_PartiallyFilled ||
		a_Exec.OrdStatus() == FIX_Filled)
	{
		if (strcmp((a_Exec.LastMkt()).c_str(), "P") == 0)
		{
			Exec.SetText("P");
		}
	}

	const CFixField* l_pField;
	l_pField = a_Exec.GetField((EFixFieldID)9730);
	if (l_pField != NULL)
	{
		Exec.SetBillingCode((l_pField->ValueAsString()).c_str());
	}

	return TRUE;
}

BOOL CTerraNovaSession::ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace)
{
	MapIntToChar ToExchangeTIF = Translator().m_ToExchangeTIF;

	a_FixReplace.Rule80A(a_Replace.Capacity());
    CString tradeFor;
    (((CArcaApp *)AfxGetApp())->GetHandler())->LoadTradeFor(tradeFor, a_Replace.StaffordTag());
    a_FixReplace.OnBehalfOfCompID(tradeFor);//a_Replace.TradeFor());
    a_FixReplace.SenderSubID(tradeFor);
    if(a_Replace.ClearingAccount().GetLength() >0) {
        a_FixReplace.Account(a_Replace.ClearingAccount());
    }
	a_FixReplace.TimeInForce((EFixTimeInForce) ToExchangeTIF[a_Replace.TIF()]);

	if (a_Replace.TIF() == COrder::GoodTillDate)
	{
		int year, month, day;

		year	= atoi(a_Replace.ExpTime().Mid(0, 4));
		month	= atoi(a_Replace.ExpTime().Mid(4,2));
		day		= atoi(a_Replace.ExpTime().Mid(6,2));

		CTime t(year, month, day, 0 , 0, 0);

		CString ExpTime;
		ExpTime.Format("%s-23:00:00", t.Format("%Y%m%d"));
		a_FixReplace.ExpireTime((LPCTSTR)ExpTime);
	}

	if (a_Replace.ExDestination() == "A" ||
		a_Replace.ExDestination() == "N")
	{
		if (a_Replace.Instructions() != "DOT")
		{
			a_FixReplace.ExecBroker("ARCA");
		}
		else
		{
			if (a_Replace.ExDestination() == "A")
			{
				a_FixReplace.ExecBroker("ASE");
			}
			else if (a_Replace.ExDestination() == "N")
			{
				a_FixReplace.ExecBroker("NYS");
			}
		}
	}

	if (false == AddTradingSessions(a_FixReplace, a_FixReplace.OrdType()))
	{
		return FALSE;
	}

	return TRUE;
}

bool CTerraNovaSession::AddTradingSessions(CFixMsg& fixMessage, int OrderType)
{
	int noTradingSession = 0;

	// Set the trading session id.
	CFixField* pTradingSessionId = NULL;

	if (OrderType != FIX_Market)
	{
		pTradingSessionId = fixMessage.GuaranteedGetCustomFieldMulti(FIX_TradingSessionId);
		if (pTradingSessionId)
		{
			noTradingSession++;
			pTradingSessionId->Value("P1");
		}

		pTradingSessionId = fixMessage.GuaranteedGetCustomFieldMulti(FIX_TradingSessionId);

		if (pTradingSessionId)
		{
			noTradingSession++;
			pTradingSessionId->Value("P3");
		}
	}

	pTradingSessionId = fixMessage.GuaranteedGetCustomFieldMulti(FIX_TradingSessionId);

	if (pTradingSessionId)
	{
		noTradingSession++;
		pTradingSessionId->Value("P2");
	}



//	if (3 != noTradingSession)
//	{
//		return false;
//	}

	// Set the trading session id.
	CFixField* pNoTradingSession = fixMessage.GuaranteedGetCustomField(FIX_NoTradingSession);

	if (!pNoTradingSession)
	{
		return false;
	}

	pNoTradingSession->Value(noTradingSession);

	return true;
}

BOOL CTerraNovaSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{

        CString tradeFor;
        (((CArcaApp *)AfxGetApp())->GetHandler())->LoadTradeFor(tradeFor, a_Cxl.StaffordTag());
        a_FixCxl.OnBehalfOfCompID(tradeFor);
        a_FixCxl.SenderSubID(tradeFor);
        if(a_Cxl.ClearingAccount().GetLength() >0) {
          a_FixCxl.Account(a_Cxl.ClearingAccount());
        }
        return TRUE;
    
}