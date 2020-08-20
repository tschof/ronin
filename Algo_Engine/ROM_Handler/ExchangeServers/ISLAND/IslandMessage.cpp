// IslandMessage.cpp: implementation of the CIslandMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IslandMessage.h"
#include "Order.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// static members

CMapStringToString	CIslandHeader::m_Reasons;

// static init
void CIslandMessage::Init();
// static translator
CTranslator	CIslandMessage::m_Translator;
// translation of exchange codes to stafford codes
const int	CIslandMessage::ToStaffordSide(char ExchangeCode);
// translation of stafford codes to exchange codes
 const char CIslandMessage::ToExchangeSide(int StaffordCode);
// translation of exchange codes to stafford codes
const int	CIslandMessage::ToStaffordSide(char ExchangeCode)	{ return(m_Translator.Lookup(m_Translator.m_ToStaffordSide,	ExchangeCode)); }
// translation of stafford codes to exchange codes
const char CIslandMessage::ToExchangeSide(int StaffordCode)		{ return(m_Translator.Lookup(m_Translator.m_ToExchangeSide,	StaffordCode)); }
// fetch functions
CString CIslandMessage::Get(const CString& Message, int Offset, int Length)
{
	CString workString;

	workString	= Message.Mid(Offset, Length);
	workString.TrimRight();
	workString.TrimLeft();

	return workString;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIslandMessage::CIslandMessage()
{

}

CIslandMessage::~CIslandMessage()
{

}

void CIslandMessage::Init()
{

	m_Translator.m_ToExchangeSide[COrder::FYI]					= CIslandMessage::Buy;
	m_Translator.m_ToExchangeSide[COrder::Buy]					= CIslandMessage::Buy;
	m_Translator.m_ToExchangeSide[COrder::Sell]					= CIslandMessage::Sell;
	m_Translator.m_ToExchangeSide[COrder::BuyMinus]				= CIslandMessage::Buy;
	m_Translator.m_ToExchangeSide[COrder::SellPlus]				= CIslandMessage::Sell;
	m_Translator.m_ToExchangeSide[COrder::Short]				= CIslandMessage::Short;
	m_Translator.m_ToExchangeSide[COrder::Exempt]				= CIslandMessage::Exempt;
	m_Translator.m_ToExchangeSide[COrder::Undisclosed]			= CIslandMessage::Buy;
	m_Translator.m_ToExchangeSide[COrder::Cross]				= CIslandMessage::Buy;

	m_Translator.m_ToStaffordSide[CIslandMessage::Buy]			= COrder::Buy;
	m_Translator.m_ToStaffordSide[CIslandMessage::Sell]			= COrder::Sell;
	m_Translator.m_ToStaffordSide[CIslandMessage::Short]		= COrder::Short;
	m_Translator.m_ToStaffordSide[CIslandMessage::Exempt]		= COrder::Exempt;
}

const CString& CIslandMessage::Build()
{
	m_Fields.BuildMessage();
	return (m_Fields.GetMessage());
}

void CIslandHeader::Init()
{
	m_Reasons["O"]	= "Logout Request Message Received";
	m_Reasons["E"]	= "Failed To Respond To Heartbeats";
	m_Reasons["J"]	= "Login Reject";
	m_Reasons["N"]	= "Goodnight";
	m_Reasons["M"]	= "Manaul Supervisory Logout";
}

CIslandHeader::CIslandHeader(const CString&	Message)
{
	m_Type = Message[0];

	if (m_Type == 'W')
	{
		m_Version	= Message.Mid(1, 5)[0];
		m_Text		= Message.Mid(6, 60)[0];
	}
	else if (m_Type == 'G')
	{
		if (Message.GetLength() > 1)
			m_Reasons.Lookup(CString(Message[1]), m_Text);
		else
			m_Text = "Logout";
	}
	else if (m_Type == 'S')
	{
		m_Sequence	= atoi(Message.Mid(1, 10));
		m_Stamp		= atoi(Message.Mid(11, 5));
	}
	else if (m_Type == 'J')	// reject
	{
	}
}

CIslandMessage::CIslandMessage(const CIslandHeader& Header, const CString& Message)
{
}