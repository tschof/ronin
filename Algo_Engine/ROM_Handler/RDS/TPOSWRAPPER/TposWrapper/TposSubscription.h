// TposSubscription.h
#pragma once
#include "stdafx.h"
#include <vcclr.h>
#include <vector>
#include "tpos/TposDefs.h"
#include "tpos/TposClientInfo.h"
#include "tpos/TposClientConnection.h"

using namespace System;
using namespace System::Collections;
using namespace System::Data;

namespace TposWrapper
{
	public ref class TposTraderAccount
	{
	public:
		String^		m_ValidTraderID;
   		String^		m_TradingGroup;
		String^		m_Portfolio;
		String^		m_ClearingFirm;
		String^		m_ClearingAccount;
		String^		m_ClearingSubAccount;
		String^		m_TraderAcronym;
	};

	public ref class TposPosition
	{
	public:
		TposPosition() {}
		TposPosition(PositionDesc*);

		String^		m_TradeGroup;
		String^		m_Portfolio;
		String^		m_ClearingAccount;
		String^		m_TraderAcronym;
   
		String^		m_TradeDate;
		String^		m_AsOfDate;

		String^		m_SecType;
		String^		m_Symbol;
		String^		m_UndSymbol;
		String^     m_UndSecType;
		String^		m_ExpDate;  // Format: YYYY-MM-DD
		double		m_Strike;
		String^		m_PutCall;

		int			m_OpenQty;
		int			m_CurQty;
 	};

	public ref class TposTrade
	{
	public:
		TposTrade() {}
		TposTrade(TradeDesc*);

		String^     m_TradeID;
		String^		m_TradeGroup;
		String^		m_Portfolio;
		String^		m_TraderAcronym;
		String^		m_ClearingAccount;
		int			m_Version;

		String^		m_TradeDate;
		String^		m_TradeTime;
		String^     m_LastModDate;
		String^     m_LastModTime;

		String^		m_SecType;
		String^		m_Symbol;
		String^		m_UndSymbol;
		String^     m_UndSecType;
		String^		m_ExpDate;  // Format: YYYY-MM-DD
		double		m_Strike;
		String^		m_PutCall;
		String^		m_TradeSource;

		int			m_Qty;
		double		m_Price;
		String^		m_ExtTradeID;
		/*
		String^		m_ContraBroker;
		String^		m_ContraFirm;
		*/
	};

	public ref class TposTradeCallback
	{
	public:
		virtual void		OnTrade(TposTrade^ trade);
	};

	public class TposNativeCB
	{
	public:
		gcroot<TposTradeCallback^>	m_tposcb; 
	};
}