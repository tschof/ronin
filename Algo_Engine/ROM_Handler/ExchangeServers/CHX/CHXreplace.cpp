#include "stdafx.h"
#include "CHXReplace.h"

#pragma warning(disable:4786)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCHXReplace::CCHXReplace(const CCsv&	Message)
{
	Set(Message);
}

void CCHXReplace::Set(const CCsv&	Message)
{
	int		Index;
	CString	Symbol;

	Symbol = Message.Get(CSV_SYMBOL);

	if ((Index = Symbol.Find('.')) > 0)
	{
		if ((Symbol.Mid(Index+1)).FindOneOf("1234567890") < 0)
			Symbol = Symbol.Left(Index);
	}

	m_Symbol = Symbol;
	m_Trader = Message.Get(CSV_TRADER);
	
	m_StaffordTag		= Message.Get(CSV_STAFFORD_TAG);
	m_Capacity			= Message.cGet(CSV_CAPACITY); 
	m_Type				= Message.nGet(CSV_TYPE); 
	m_Side				= Message.nGet(CSV_SIDE);
	m_TIF				= Message.nGet(CSV_TIF);
	m_Shares			= Message.nGet(CSV_SHARES);
	m_Price				= Message.dGet(CSV_PRICE);
	m_StopPrice			= Message.dGet(CSV_STOPPRICE);
	m_OriginalShares	= m_Shares;
	m_Owner				= Message.Get(CSV_OWNER);
	m_Instructions		= Message.Get(CSV_INSTRUCTIONS);
}