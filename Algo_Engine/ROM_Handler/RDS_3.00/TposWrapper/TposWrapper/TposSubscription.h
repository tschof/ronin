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
	public ref class TposPrice
	{
	public:
		String^		m_Date;
   		String^		m_InstrumentID;
		Double		m_Price;
	};

	public ref class TposTraderAccount
	{
	public:
		String^		ValidTraderID;
   		String^		TradingGroup;
		String^		Portfolio;
		String^		ClearingFirm;
		String^		ClearingAccount;
		String^		ClearingSubAccount;
		String^		TraderAcronym;
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
		String^		m_InstrumentID;
		String^		m_ValidTraderKey;
   
		String^		m_TradeDate;
		String^		m_AsOfDate;

		String^		m_SecType;
		String^		m_Symbol;
		String^		m_Ticker;
		String^		m_UndSymbol;
		String^     m_UndSecType;
		String^		m_ExpDate;  // Format: YYYY-MM-DD
		double		m_Strike;
		String^		m_PutCall;
		String^		m_DisplayFormat;
		double		m_Multiplier;

		double		m_AvgCost;
		double		m_RealizedPnL;
		double		m_OpenMark;
		
		int			m_NotionalAmount;
		int			m_Version;
		int			m_OpenQty;
		int			m_CurQty;
 	};

	public ref class TposTrade
	{
	public:
		TposTrade() {}
		TposTrade(TradeDesc*);

		String^		m_TradeID;
		String^		m_TradeGroup;
		String^		m_Portfolio;
		String^		m_TraderAcronym;
		String^		m_ClearingAccount;
		int			m_Version;

		String^		m_SecType;
		String^		m_Ticker;
		String^		m_Symbol;
		String^		m_UndSymbol;
		String^		m_UndSecType;
		String^		m_ExpDate;  // Format: YYYY-MM-DD
		double		m_Strike;
		String^		m_PutCall;

		String^		m_TradeDate;
		String^		m_TradeTime;
		String^		m_Exchange;
		String^		m_ShortSaleFlag;
		String^		m_ContraBroker;
		String^		m_ContraFirm;
		String^		m_TradeSource;
		String^		m_ExtTradeID;
		String^		m_ExtDescription;
		String^		m_Note;
		String^		m_ActiveState;
		String^		m_ClearingStatus;
		String^		m_SettleDate;

		String^		m_LastModDate;
		String^		m_LastModTime;
		String^		m_LastModReason;
		int			m_LastModReasonID;

		int			m_Qty;
		double		m_Price;
		double		m_Commission;
	};

	public ref class TposTradeCallback
	{
	public:
		virtual void		OnTrade(TposTrade^ trade);
	};

	public ref class TposPositionCallback
	{
	public:
		virtual void		OnPosition(TposPosition^ pos);
	};

	public ref class TposPriceCallback
	{
	public:
		virtual void		OnPrice(TposPrice^ pos);
	};

	public class TposNativeCB
	{
	public:
		gcroot<TposTradeCallback^>	m_tposcb; 
		gcroot<TposPositionCallback^>	m_tposposcb;
		gcroot<TposPriceCallback^>	m_tpospricecb;
	};
}
