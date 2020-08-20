#include "stdafx.h"
#include "Replace.h"
#include "BrassFixSession.h"
#include "FixAppMsgs.h"
#include "Order.h"
#include "Cancel.h"
#include "Execution.h"
#include "MessageApp.h"

long CBrassFixSession::ProcessEntry(COrder& a_Order)
{
	long l_SeqNum = OutgoingMsgs().NextMsgSeqNum();
	MapIntToChar ToExchangeSide = Translator().m_ToExchangeSide;
	MapIntToChar ToExchangeType = Translator().m_ToExchangeType;
	MapIntToChar ToExchangeTIF = Translator().m_ToExchangeTIF;

	CFixOrder l_Order(
		l_SeqNum, 
		GetCurrentTimeStr(), 
		a_Order.OrderTag(), 
		FIX_AutoPublic,
		a_Order.Symbol(), 
		(EFixSide)ToExchangeSide[a_Order.Side()], 
		(EFixOrdType)ToExchangeType[a_Order.Type()]
	);

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

	l_Order.TargetSubID(TargetSubId().c_str());
	l_Order.SenderSubID(a_Order.SourceId());
	l_Order.OrderQty(a_Order.Shares());
	l_Order.Account(a_Order.ClearingId());
	l_Order.TimeInForce((EFixTimeInForce) ToExchangeTIF[a_Order.TIF()]);

	if (a_Order.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Order.ExecInst());
		l_Order.ExecInst(ExecInst);
	}

//	if ((a_Order.Symbol()).GetLength() < 4) // listed symbol
//	{
//		l_Order.ExecBroker("PROC");
//	}

//	if (a_Order.Trader() == "CMSTDCAN")
//	{
//		l_Order.ExecBroker("GLIS");
//	}

	#ifdef _TEST
		l_Order.Rule80A('J');
	#else
		l_Order.Rule80A(a_Order.Capacity());
	#endif
	
	if (a_Order.TIF() == COrder::GoodTillDate)
	{
		int year, month, day;

		year	= atoi(a_Order.ExpTime().Mid(0, 4));
		month	= atoi(a_Order.ExpTime().Mid(4,2));
		day		= atoi(a_Order.ExpTime().Mid(6,2));

		CTime t(year, month, day, 0 , 0, 0);

		CString ExpTime;
		ExpTime.Format("%s-20:00:00", t.Format("%Y%m%d"));
		l_Order.ExpireTime((LPCTSTR)ExpTime);
	}

	if (a_Order.Floor() != 0)
	{
		l_Order.MaxFloor(a_Order.Floor());
	}

	if(a_Order.ExDestination() != "")
		l_Order.ExDestination(a_Order.ExDestination());

	if (a_Order.Instructions() == "ADOT" ||
		a_Order.Instructions() == "SGES")
	{
		l_Order.DeliverToCompID(a_Order.Instructions());
	}

	if(SendMsg(l_Order))
		return l_SeqNum;
	else
		return 0;
}

BOOL CBrassFixSession::ProcessCancel(CCancel& a_Cancel)
{
	MapIntToChar ToExchangeType = Translator().m_ToExchangeType;

	CFixCancel l_Cancel(
		OutgoingMsgs().NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Cancel.OrderTag(),
		a_Cancel.CancelTag(),
		a_Cancel.Symbol(),
		(EFixSide) ToExchangeType[a_Cancel.Side()]
	);
	l_Cancel.TargetSubID(TargetSubId().c_str());
	l_Cancel.SenderSubID(a_Cancel.SourceId());
	l_Cancel.OrderID(a_Cancel.ExchangeTag());
	l_Cancel.OrderQty(a_Cancel.Shares());
	if(CFixMsg::GetVersionNum() < 4.1)
		l_Cancel.CxlType(FIX_FullCxl);
/*
	if (a_Cancel.TIF() == COrder::GoodTillDate || a_Cancel.TIF() == COrder::GTC)
	{
		CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
		l_FieldPtr = l_Cancel.CreateNewField((EFixFieldID)9000);
		l_FieldPtr->Value(a_Cancel.Price());
		l_Cancel.InsertInBody(l_FieldPtr);
		
		l_FieldPtr = l_Cancel.CreateNewField((EFixFieldID)9001);
		l_FieldPtr->Value(a_Cancel.CumulativeQty() + a_Cancel.LeavesQty());
		l_Cancel.InsertInBody(l_FieldPtr);
		
		l_FieldPtr = l_Cancel.CreateNewField((EFixFieldID)9002);

		CString originalDate = a_Cancel.OriginalOrderTime();
		int index = -1;

		if (-1 != (index = originalDate.Find("-")))
		{
			CString year = originalDate.Mid(0,4);
			CString month = originalDate.Mid(4,2);
			CString day	= originalDate.Mid(6,2);

			originalDate.Format("%s%s%s", year, day, month);
		}

		l_FieldPtr->Value(originalDate);
		l_Cancel.InsertInBody(l_FieldPtr);
	}
*/
	if (a_Cancel.Instructions() == "ADOT" ||
		a_Cancel.Instructions() == "SGES")
	{
		l_Cancel.DeliverToCompID(a_Cancel.Instructions());
	}

	return SendMsg(l_Cancel);
}


BOOL CBrassFixSession::ProcessReplace(CReplace& a_Replace)
{
	const MapIntToChar& l_SideMap = Translator().m_ToExchangeSide;
	const MapIntToChar& l_TypeMap = Translator().m_ToExchangeSide;
	const MapIntToChar& l_TifMap  = Translator().m_ToExchangeTIF;

	MapIntToChar::const_iterator l_SideIter = l_SideMap.find(a_Replace.Side());
	MapIntToChar::const_iterator l_TypeIter = l_TypeMap.find(a_Replace.Type());
	
	CFixReplace l_Replace(
		OutgoingMsgs().NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Replace.OrderTag(),
		a_Replace.CancelTag(),
		FIX_AutoPublic,
		a_Replace.Symbol(),
		(EFixSide) l_SideIter->second,
		(EFixOrdType) l_TypeIter->second
	);
	l_Replace.OrderQty(a_Replace.Shares());
	l_Replace.Price(a_Replace.Price());

	CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9000);
	l_FieldPtr->Value(a_Replace.OriginalPrice());
	l_Replace.InsertInBody(l_FieldPtr);

	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9001);
	//l_FieldPtr->Value(a_Replace.OriginalShares());
	l_FieldPtr->Value(a_Replace.LeaveShares());
	l_Replace.InsertInBody(l_FieldPtr);
	
//	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9002);
//	l_FieldPtr->Value(CTime::GetCurrentTime().Format("%Y%d%m"));
//	l_Replace.InsertInBody(l_FieldPtr);

	if (a_Replace.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Replace.ExecInst());
		l_Replace.ExecInst(ExecInst);
	}

	if (a_Replace.TIF() != COrder::LASTTimeInForce)
	{
		MapIntToChar::const_iterator l_Tif		= l_TifMap.find(a_Replace.TIF());
		l_Replace.TimeInForce((EFixTimeInForce)l_Tif->second);
	}

	if (a_Replace.StopPrice() != 0)
	{
		l_Replace.StopPx(a_Replace.StopPrice());
	}

	if (a_Replace.TIF() == COrder::GoodTillDate || a_Replace.TIF() == COrder::GTC)
	{
		l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9002);

		CString originalDate = a_Replace.OriginalOrderTime();
		//int index = -1;

		//if (-1 != (index = originalDate.Find("-")))
		//{
			CString year = originalDate.Mid(0,4);
			CString month = originalDate.Mid(4,2);
			CString day	= originalDate.Mid(6,2);

			originalDate.Format("%s%s%s", year, day, month);
		//}

		l_FieldPtr->Value(originalDate);
		l_Replace.InsertInBody(l_FieldPtr);
	}

	if (a_Replace.Instructions() == "ADOT" ||
		a_Replace.Instructions() == "SGES")
	{
		l_Replace.DeliverToCompID(a_Replace.Instructions());
	}
	
	return SendMsg(l_Replace);
}


