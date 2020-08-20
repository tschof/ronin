// BloombergFixSession1.cpp: implementation of the CBloombergFixSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Bloomberg.h"
#include "BloombergFixSession.h"
#include "FixAppMsgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBloombergFixSession::CBloombergFixSession()
{

}

CBloombergFixSession::~CBloombergFixSession()
{

}

long CBloombergFixSession::ProcessEntry(COrder& a_Order)
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
		(EFixSide) ToExchangeSide[a_Order.Side()], 
		(EFixOrdType) ToExchangeType[a_Order.Type()]
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
	l_Order.Rule80A(a_Order.Capacity());

	if (a_Order.Floor() != 0)
	{
		l_Order.MaxFloor(a_Order.Floor());
	}

	if(a_Order.ExDestination() != "")
		l_Order.ExDestination(a_Order.ExDestination());

	CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
	l_FieldPtr = l_Order.CreateNewField((EFixFieldID)9881);
	l_FieldPtr->Value("R");
	l_Order.InsertInBody(l_FieldPtr);

	if(SendMsg(l_Order))
		return l_SeqNum;
	else
		return 0;
}

BOOL CBloombergFixSession::ProcessReplace(CReplace& a_Replace)
{
	long l_SeqNum = OutgoingMsgs().NextMsgSeqNum();
	MapIntToChar ToExchangeSide = Translator().m_ToExchangeSide;
	MapIntToChar ToExchangeType = Translator().m_ToExchangeType;

	CFixReplace l_Replace(
		l_SeqNum,
		GetCurrentTimeStr(),
		a_Replace.OrderTag(),
		a_Replace.CancelTag(),
		FIX_AutoPublic,
		a_Replace.Symbol(),
		(EFixSide) ToExchangeSide[a_Replace.Side()],
		(EFixOrdType) ToExchangeType[a_Replace.Type()]
	);
	l_Replace.TargetSubID(TargetSubId().c_str());
	l_Replace.SenderSubID(a_Replace.SourceId());
	l_Replace.OrderQty(a_Replace.Shares());
	l_Replace.Price(a_Replace.Price());

	if (a_Replace.Floor() != 0)
	{
		l_Replace.MaxFloor(a_Replace.Floor());
	}

	CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9881);
	l_FieldPtr->Value("R");
	l_Replace.InsertInBody(l_FieldPtr);

	return SendMsg(l_Replace);
}


