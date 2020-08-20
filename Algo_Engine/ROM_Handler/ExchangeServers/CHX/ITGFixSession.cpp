#include "stdafx.h"
#include "Replace.h"
#include "ITGFixSession.h"
#include "FixAppMsgs.h"

BOOL CITGFixSession::ProcessReplace(CITGReplace& a_Replace)
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
	l_Replace.Price(a_Replace.Price());

	/*CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9000);
	l_FieldPtr->Value(a_Replace.OriginalPrice());
	l_Replace.InsertInBody(l_FieldPtr);

	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9001);
	l_FieldPtr->Value(a_Replace.OriginalShares());
	l_Replace.InsertInBody(l_FieldPtr);
	
	l_FieldPtr = l_Replace.CreateNewField((EFixFieldID)9002);
	l_FieldPtr->Value(CTime::GetCurrentTime().Format("%Y%d%m"));
	l_Replace.InsertInBody(l_FieldPtr);*/

	return SendMsg(l_Replace);
	
	return TRUE;
}
