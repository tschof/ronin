#include "IncaFixSession.h"
#include "FixAppMsgs.h"
#include "Cancel.h"
#include "Replace.h"

long CIncaFixSession::ProcessEntry(COrder& a_Order, const char *TargetSubId)
{
	long l_SeqNum = GetNextOutgoingSeqNum();
	const MapIntToChar& l_SideMap = Translator().m_ToExchangeSide;
	MapIntToChar::const_iterator l_Iter = l_SideMap.find(a_Order.Side());
	if(l_Iter == l_SideMap.end())
		return 0;
	char l_Side = l_Iter->second;
	const MapIntToChar& l_TypeMap = Translator().m_ToExchangeType;
	if((l_Iter = l_TypeMap.find(a_Order.Type())) == l_TypeMap.end())
		return 0;
	CFixOrder l_Order(
		l_SeqNum, 
		GetCurrentTimeStr(), 
		a_Order.OrderTag(), 
		FIX_AutoPublic,
		a_Order.Symbol(), 
		(EFixSide) l_Side, 
		(EFixOrdType) l_Iter->second
	);

	l_Order.TargetSubID(TargetSubId);
	l_Order.SenderSubID(a_Order.SourceId());
	//l_Order.Price(a_Order.dPrice());
	l_Order.OrderQty(a_Order.Shares());
	l_Order.Account(a_Order.ClearingId());

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
	else if (a_Order.Type() != COrder::Market)
	{
		l_Order.Price(a_Order.dPrice());
	}
	
	if (a_Order.Floor() != 0)
	{
		l_Order.MaxFloor(a_Order.Floor());
	}


	const MapIntToChar& l_TifMap = Translator().m_ToExchangeTIF;
	if((l_Iter = l_TifMap.find(a_Order.TIF())) == l_TifMap.end())
		return 0;
	l_Order.TimeInForce((EFixTimeInForce) l_Iter->second);
	if(a_Order.Capacity() == (char)COrder::Agency || a_Order.Capacity() == (char)COrder::Principal)
		l_Order.Rule80A(a_Order.Capacity());
	else
		l_Order.Rule80A((char)COrder::Agency);
	//if(a_Order.ExDestination() != "")
	//	l_Order.ExDestination(a_Order.ExDestination());

	if (strcmp(TargetSubId, "_ARC") != 0)
	{
		if(a_Order.Exchange() == COrder::INSTINET) 
		{
			TString l_ExecInstStr;
			l_ExecInstStr += FIX_CDI;
			l_Order.ExecInst(l_ExecInstStr.c_str());
		}
	}

	if(SendMsg(l_Order))
		return l_SeqNum;
	else
		return 0;
}

BOOL CIncaFixSession::ProcessCancel(CCancel& a_Cancel, const char *TargetSubId)
{
	const MapIntToChar& l_SideMap = Translator().m_ToExchangeSide;
	MapIntToChar::const_iterator l_Iter = l_SideMap.find(a_Cancel.Side());
	if(l_Iter == l_SideMap.end())
		return 0;
	char l_Side = l_Iter->second;

	CFixCancel l_Cancel(
		GetNextOutgoingSeqNum(),
		GetCurrentTimeStr(),
		a_Cancel.OrderTag(),
		a_Cancel.CancelTag(),
		a_Cancel.Symbol(),
		(EFixSide)l_Side
	);

	//if (strcmp(TargetSubId().c_str(), "") != 0)
	//{
	//	l_Cancel.TargetSubID(m_TargetSubId.c_str());
	//}
	if (strcmp(a_Cancel.OrderTag(), "") != 0 && 
		strcmp(a_Cancel.CancelTag(), "") != 0 &&
		strcmp(a_Cancel.Symbol(), "") != 0 &&
		(EFixSide)l_Side != -1)
	{
		l_Cancel.TargetSubID(TargetSubId);
		l_Cancel.SenderSubID(a_Cancel.SourceId());
		//l_Cancel.OrderID(a_Cancel.ExchangeTag());
		l_Cancel.OrderQty(a_Cancel.Shares());

		//l_Cancel.TransactTime((LPCTSTR)GetCurrentLocalTimeStr());
	}
	else
	{
		// Bulk cancel
	}

	if(CFixMsg::GetVersionNum() < 4.1)
		l_Cancel.CxlType(FIX_FullCxl);
	return SendMsg(l_Cancel);
}
/*
CString *CIncaFixSession::INCADestination(CString &Destination)
{
	CString ds;

	if (Destination == "DOT")
	{
		ds = "_DOT";
	}
	else
	{
		return NULL;
	}
}
*/

BOOL CIncaFixSession::ProcessReplace(CReplace& a_Replace, const char *TargetSubId)
{
	const MapIntToChar& l_SideMap = Translator().m_ToExchangeSide;
	const MapIntToChar& l_TypeMap = Translator().m_ToExchangeSide;

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

	if (a_Replace.Type() != COrder::Market)
	{
		l_Replace.Price(a_Replace.Price());
	}

	if (strcmp(TargetSubId, "_ARC") != 0)
	{
		TString l_ExecInstStr;
		l_ExecInstStr += FIX_CDI;
		l_Replace.ExecInst(l_ExecInstStr.c_str());
	}

	if(a_Replace.Capacity() == (char)COrder::Agency || a_Replace.Capacity() == (char)COrder::Principal)
		l_Replace.Rule80A(a_Replace.Capacity());
	else
		l_Replace.Rule80A((char)COrder::Agency);

	l_Replace.TargetSubID(TargetSubId);
	l_Replace.SenderSubID(a_Replace.SourceId());

	return SendMsg(l_Replace);
}
