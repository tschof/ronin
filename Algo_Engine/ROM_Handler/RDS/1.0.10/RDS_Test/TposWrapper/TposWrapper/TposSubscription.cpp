#include "stdafx.h"
#include "tpos/TposPubMessage.h"
#include "TposSubscription.h"

#using "mscorlib.dll"
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace TposWrapper;

TposTrade :: TposTrade(TradeDesc* val)
{
	m_TradeID = gcnew String(val->getTradeID().c_str());
	m_TradeGroup = gcnew String(val->getTradeGroup().c_str());
	m_Portfolio  = gcnew String(val->getPortfolio().c_str() );
	m_TraderAcronym = gcnew String(val->getTrader().c_str() );
	m_ClearingAccount  = gcnew String(val->getClearingAccount().c_str());
	m_Version    = val->getVersion();
	m_SecType    = gcnew String(val->getSecType().c_str() );
	m_Symbol     = gcnew String(val->getSymbol().c_str() );
	m_UndSymbol  = gcnew String(val->getUndSymbol().c_str() );
	m_UndSecType = gcnew String(val->getUndType().c_str());
	m_ExpDate    = gcnew String(val->getExpDate().c_str() );
	m_Strike     = val->getStrike();
	m_PutCall    = gcnew String(val->getPutCall().c_str() );
	m_Qty		= val->getQty();
	m_Price		= val->getPrice();
	m_TradeDate = gcnew String(val->getTradeDate().c_str() );
	m_TradeTime = gcnew String(val->getTradeTime().c_str() );
	m_LastModDate = gcnew String(val->getLastModDate().c_str());
	m_LastModTime = gcnew String(val->getLastModTime().c_str() );
	m_ExtTradeID = gcnew String(val->getExtTradeID().c_str());
	m_LastModReason = gcnew String(val->getLastModReason().c_str());
}

//
TposPosition :: TposPosition(PositionDesc* val)
{
	m_TradeGroup = gcnew String(val->getTradeGroup().c_str());
	m_Portfolio  = gcnew String(val->getPortfolio().c_str() );
	m_ClearingAccount  = gcnew String(val->getClearingAccount().c_str());
	m_TraderAcronym  = gcnew String(val->getTrader().c_str() );
	m_AsOfDate   = gcnew String(val->getAsOfDate().c_str() );
	m_SecType    = gcnew String(val->getSecType().c_str() );
	m_Symbol     = gcnew String(val->getSymbol().c_str() );
	m_UndSymbol  = gcnew String(val->getUndSymbol().c_str() );
	m_UndSecType = gcnew String(val->getUndType().c_str());
	m_ExpDate    = gcnew String(val->getExpDate().c_str() );
	m_Strike     = val->getStrike();
	m_PutCall    = gcnew String(val->getPutCall().c_str() );
	m_OpenQty    = val->getOpenQty();
	m_CurQty	 = val->getCurrentQty();
}

//
void TposTradeCallback :: OnTrade(TposTrade^ trade)
{
	printf("Original OnTrade Callback\n");
}