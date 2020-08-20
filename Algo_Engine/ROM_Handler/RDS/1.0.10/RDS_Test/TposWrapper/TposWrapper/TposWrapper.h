// TposWrapper.h
#pragma once
#include "stdafx.h"
#include <vector>
#include "tpos/TposClientInfo.h"
#include "tpos/TposClientConnection.h"
#include "TposSubscription.h"

using namespace System;
using namespace System::Collections;
using namespace System::Data;

//  elvin.lib oncrpc.lib pthreadVC.lib Ws2_32.lib
// $(OutDir)\$(ProjectName).dll
// mscoree.lib msvcmrtd.lib tpos_api_static_debug.lib elvindbg.lib oncrpcdbg.lib pthreadVCdbg.lib Ws2_32.lib

namespace TposWrapper
{
	public ref class SecurityDesc
	{
	public:
		SecurityDesc() {}
		SecurityDesc(securityInfoX&);

		String^		securityType;
		String^		baseSymbol;
		String^		fullSymbol;
   		String^		exchange;
		double		strike;
		String^		expirationDate;
		double		contractSize;
		String^		putCall;
		double		tickSize;
		String^		genericMDSymbol;
		String^		dataSourceInfo;
		String^		longName;
	};

	public ref class OptionDesc
	{
	public:
		OptionDesc() {}
		OptionDesc(optionInfo&);

		String^		opraSymbol;
		String^		exchange;
		double		strike;
		String^		expirationDate;
		String^		putCall;
		double		tickSize;
		double		contractSize;
		String^		spcStlFlag;
	};

	public ref class FutureDesc
	{
	public:
		FutureDesc() {}
		FutureDesc(futureInfo&);

		String^		fullSymbol;
		String^		exchange;
		String^		expirationDate;
		double		contractSize;
		double		tickSize;
		String^		curCode;
		String^		longName;
	};

	struct tgp
	{
		std::string group;
		std::string portfolio;
	};
	public ref class TposProxy
	{
	public:
		TposClientConnection*  m_tpos;
		TposNativeCB*		m_ncb;


	public:
		TposProxy(String^ user, String^ pass, String^ server, int port, String^ tposface); 
		virtual ~TposProxy() 
		{
			if (!m_tpos)
				delete m_tpos;
		};

		int Connect();

		int deleteTrade(
			String^ tradegroup, 
			String^ portfolio,
			String^ externalId
			);

        int storeID(
			String^ tradegroup,
			String^ portfolio, 
			String^ extID );

		int enterFutureTrade(
			String^ tradegroup, 
			String^ portfolio,
			String^ cfirm, 
			String^ caccount,
			String^ trader,
			String^ sectype,
			String^ ticker,
			String^ tradedate,
			String^ tradetime,
			int qty,
			double price,
			String^ exchange,
			String^ shortsale,
			String^ currency,
			String^ contrabroker,
			String^ contrafirm,
			String^ source,
			String^ extID,
			String^ extdesc,
			String^ activeState,
			double commission,
			String^ note);

		int enterOptionTrade(
			String^ tradegroup, 
			String^ portfolio,
			String^ cfirm, 
			String^ caccount,
			String^ trader,
			String^ sectype,
			String^ basesymbol,
			String^ expdate,
			String^ callput,
			double  strike,
			String^ tradedate,
			String^ tradetime,
			int qty,
			double price,
			String^ exchange,
			String^ shortsale,
			String^ currency,
			String^ contrabroker,
			String^ contrafirm,
			String^ source,
			String^ extID,
			String^ extdesc,
			String^ activeState,
			double commission,
			String^ note);

		int enterFixedIncomeTrade(
			String^ tradegroup, 
			String^ portfolio,
			String^ cfirm, 
			String^ caccount,
			String^ trader,
			String^ sectype,
			String^ cextInstID,
			String^ iextInstID,
			String^ tradedate,
			String^ tradetime,
			int qty,
			double price,
			String^ exchange,
			String^ shortsale,
			String^ currency,
			String^ contrabroker,
			String^ contrafirm,
			String^ source,
			String^ extID,
			String^ extdesc,
			String^ clearingStatus,
			String^ clearingRecord,
			String^ settleDate,
			String^ activeState,
			double commission,
			String^ note);

		int GetSecurityByTicker(String^ tickers, ArrayList^ out);
		int GetOptionChain(String^ underlying, ArrayList^ out);
 		int GetFutureChainByBase(String^ base, ArrayList^ out);
		int GetFutureChain(String^ underlying, ArrayList^ out);
		int GetSSFutureChain(String^ underlying, ArrayList^ out);

		int GetMappedAccount( String^ account, ArrayList^ out);
		String^ GetMappedSecurity( String^ security );
		int   GetTraderAccounts(ArrayList^ out);
		int   GetBODPositions(String^ grp, String^ pf, ArrayList^ out);
		int   GetBODPositions(String^ grp, String^ pf, String^ trader, ArrayList^ out);
		int   GetTrades(String^ user, String^ grp, String^ pf, String^ trader, ArrayList^ out);
		void  SubscribeTrades(String^ grp, String^ pf, String^ trader,  TposTradeCallback^ cb);
		void  UnsubscribeTrades(String^ grp, String^ pf, String^ trader);
		String^   RtrvTrades(String^ user, String^ grp, String^ pf, String^ trader, String^ bDate, String^ bTime, ArrayList^ out);

		int   GetBODPositions(String^ grp, String^ pf, ArrayList^ traderAcronyms, ArrayList^ out);
		int   GetTrades(String^ user, String^ grp, String^ pf, ArrayList^ traderAcronyms, ArrayList^ out);
		void  SubscribeTrades(String^ grp, String^ pf, ArrayList^ traderAcronyms,  TposTradeCallback^ cb);
		// int   GetBODPositions(String^ grp, String^ pf, String^ acct, String^ dt, ArrayList^ out);
		// int   GetTrades(String^ grp, String^ pf, String^ acct, String^ dt, ArrayList^ out);
		// void  SubscribeTrades(String^ grp, String^ pf, String^ acct, TposTradeCallback^ cb);


};
}