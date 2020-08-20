// Order.cpp: implementation of the COrder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Message.h"
#include "Order.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char				COrder::m_Destinations[4][12];
char				COrder::m_StatusStrings[20][36];
char				COrder::m_Sides[12][2];

const char		*COrder::StatusString(int Status);
const char		*COrder::SideString(int Side);
const char		*COrder::Destination(int Destin);
const CString	COrder::GetHHMMSS();

COrder::COrder()
{
	m_Slot = 0;
}

void COrder::Init()
{
	
	strcpy(m_Destinations[COrder::TNTO],				"TNTO");
	strcpy(m_Destinations[COrder::INSTINET],			"INCA");
	strcpy(m_Destinations[COrder::NYSE],				"NYSE");
	strcpy(m_Destinations[COrder::ACT],					"ACT" );

	strcpy(m_StatusStrings[COrder::New],				"New");
	strcpy(m_StatusStrings[COrder::PartiallyFilled],"Part/Open");
	strcpy(m_StatusStrings[COrder::Filled],			"Fill");
	strcpy(m_StatusStrings[COrder::DoneForDay],		"Done");
	strcpy(m_StatusStrings[COrder::Canceled],			"Out");
	strcpy(m_StatusStrings[COrder::Replaced],			"Rplcd");
	strcpy(m_StatusStrings[COrder::PendingCancel],	"Pndng");
	strcpy(m_StatusStrings[COrder::Stopped],			"Stop");
	strcpy(m_StatusStrings[COrder::Rejected],			"Rej");
	strcpy(m_StatusStrings[COrder::Suspended],		"Suspnd");
	strcpy(m_StatusStrings[COrder::PendingNew],		"New?");
	strcpy(m_StatusStrings[COrder::Calculated],		"Calc");
	strcpy(m_StatusStrings[COrder::Expired],			"Exprd");
	strcpy(m_StatusStrings[COrder::Open],				"Open");
	strcpy(m_StatusStrings[COrder::CancelRejected],  "X Rej");

	strcpy(m_Sides[COrder::FYI],			"U");
	strcpy(m_Sides[COrder::Buy],			"B");
	strcpy(m_Sides[COrder::Sell],			"S");
	strcpy(m_Sides[COrder::Short],		"Z");
	strcpy(m_Sides[COrder::Exempt],		"Z");
	strcpy(m_Sides[COrder::BuyMinus],	"B");
	strcpy(m_Sides[COrder::SellPlus],	"S");
	strcpy(m_Sides[COrder::Undisclosed],"U");
	strcpy(m_Sides[COrder::Cross],		"X");
	
}

COrder::COrder(CCsv& Message)
{
	Set(Message);
}

void COrder::Set(const CCsv& Message)
{
	int		Index;
	CString	Symbol;

	m_ClearingAccount	= Message.Get(CSV_CLEARINGACCOUNT);

	strcpy(m_Instructions,		Message.Get(CSV_INSTRUCTIONS).GetBuffer(35));

	Symbol = Message.Get(CSV_SYMBOL);
	strcpy(m_FullSymbol,		Symbol);

	if ((Index = Symbol.Find('.')) > 0)
		Symbol = Symbol.Left(Index);

	strcpy(m_Symbol,			Symbol);
	strcpy(m_Trader,			Message.Get(CSV_TRADER));
	
	m_TradeFor			= Message.Get(CSV_TRADE_FOR);
	m_ClearingId		= Message.Get(CSV_CLEARINGID);
	m_Tag					= Message.nGet(CSV_TAG);
	m_StaffordTag		= Message.Get(CSV_STAFFORD_TAG);
	m_Exchange			= Message.nGet(CSV_EXCHANGE); 
	m_Capacity			= Message.cGet(CSV_CAPACITY); 
	m_Type				= Message.nGet(CSV_TYPE); 
	m_PriceCheck		= Message.cGet(CSV_PRICECHECK); 
	m_Side				= Message.nGet(CSV_SIDE);
	m_TIF					= Message.nGet(CSV_TIF);
	m_Floor				= Message.nGet(CSV_FLOOR);
	m_Shares				= Message.nGet(CSV_SHARES);
	m_Price				= Message.dGet(CSV_PRICE);
	m_StopPrice			= Message.dGet(CSV_STOPPRICE);
	m_OriginalShares	= m_Shares;
	m_MinQty				= Message.nGet(CSV_MINQTY);
	m_Discretionary	= Message.nGet(CSV_DISCRETIONARY);
	m_Solicited			= Message.nGet(CSV_SOLICITED);
	m_Status				= Message.nGet(CSV_STATUS);
	m_Firm				= Message.Get(CSV_FIRM);
	m_LocalAccount		= Message.Get(CSV_LOCALACCOUNT);
	
	strcpy(m_sPrice,	CPrice::ToString(m_sPrice, m_Price));

	sprintf(m_Debug, "Order To %s %d Of %s At %s By %s On %s",
		SideString(),
		m_Shares,
		m_Symbol,
		m_sPrice,
		m_Trader,
		Destination());
}

COrder::COrder(COrder *pOrder)
{
	m_ClearingAccount			= pOrder->m_ClearingAccount;
	m_Firm						= pOrder->m_Firm;
	m_LocalAccount				= pOrder->m_LocalAccount;
	m_ClearingId				= pOrder->m_ClearingId;
	m_OrderTag					= pOrder->m_OrderTag;
	m_CancelTag					= pOrder->m_CancelTag;
	m_ExchangeTag				= pOrder->m_ExchangeTag;
	m_StaffordTag				= pOrder->m_StaffordTag;

	strcpy(m_Instructions,	pOrder->m_Instructions);
	strcpy(m_Symbol,			pOrder->m_Symbol);
	strcpy(m_FullSymbol,		pOrder->m_FullSymbol);
	strcpy(m_Trader,			pOrder->m_Trader);
	strcpy(m_sPrice,			pOrder->m_sPrice);
	strcpy(m_Owner,			pOrder->m_Owner);
	strcpy(m_SourceId,		pOrder->m_SourceId);

	m_Tag							= pOrder->m_Tag;
	m_Exchange					= pOrder->m_Exchange;
	m_Capacity					= pOrder->m_Capacity;
	m_Type						= pOrder->m_Type;
	m_PriceCheck				= pOrder->m_PriceCheck;
	m_Side						= pOrder->m_Side;
	m_TIF							= pOrder->m_TIF;
	m_Floor						= pOrder->m_Floor;
	m_Shares						= pOrder->m_Shares;
	m_Price						= pOrder->m_Price;
	m_StopPrice					= pOrder->m_StopPrice;
	m_OriginalShares			= pOrder->m_OriginalShares;
	m_Status						= pOrder->m_Status;
	m_MinQty						= pOrder->m_MinQty;
	m_Discretionary			= pOrder->m_Discretionary;
	m_Solicited					= pOrder->m_Solicited;
	m_Slot						= pOrder->m_Slot;

}

COrder::~COrder()
{

}
