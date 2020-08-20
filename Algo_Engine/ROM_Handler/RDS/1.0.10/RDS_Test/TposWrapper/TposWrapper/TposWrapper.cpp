// This is the main DLL file.

#include "stdafx.h"
#include "tpos/TposPubMessage.h"
#include "tpos/TposClientTrade.h"
#include "TposWrapper.h"

#using "mscorlib.dll"
using namespace System;
using namespace System::Collections;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace TposWrapper;

#include <map>
std::map<std::string,tgp> * s_tradeidmap = 0;

// public delegate int CBggg(TposPubMessage * msg, IntPtr param);

SecurityDesc :: SecurityDesc(securityInfoX& val)
{
	this->securityType	= gcnew String(val.securityType.c_str());
	this->baseSymbol	= gcnew String(val.baseSymbol.c_str());
	this->fullSymbol	= gcnew String(val.fullSymbol.c_str());
	this->exchange	= gcnew String(val.exchange.c_str());
	this->strike = val.strike;
	this->expirationDate	= gcnew String(val.expirationDate.c_str());
	this->contractSize = val.contractSize;
	this->putCall	= gcnew String(val.putCall.c_str());
	this->tickSize = val.tickSize;
	this->genericMDSymbol	= gcnew String(val.genericMDSymbol.c_str());
	this->dataSourceInfo	= gcnew String(val.dataSourceInfo.c_str());
	this->longName = gcnew String(val.longName.c_str());
}

OptionDesc :: OptionDesc(optionInfo& val)
{
	//opraSymbol = gcnew String(val.opraSymbol.c_str());
	opraSymbol = gcnew String(val.genericMDSymbol.c_str());
	exchange = gcnew String(val.exchange.c_str());
	strike = val.strike;
	expirationDate = gcnew String(val.expirationDate.c_str());
	contractSize = val.contractSize;
	
	putCall = gcnew String(val.putCall.c_str());
	spcStlFlag = gcnew String(val.spcStlFlag.c_str());
	tickSize = val.tickSize;
}

FutureDesc :: FutureDesc(futureInfo& val)
{
	fullSymbol = gcnew String(val.fullSymbol.c_str());
	exchange = gcnew String(val.exchange.c_str());
	expirationDate = gcnew String(val.expirationDate.c_str());
	contractSize = val.contractSize;
	tickSize = val.tickSize;

	curCode = gcnew String(val.curCode.c_str());
	longName = gcnew String(val.longName.c_str());
}
//

TposProxy :: TposProxy(String^ user, String^ pass, String^ server, int port, String^ tposface)
: m_tpos(0)
{
	IntPtr puser = Marshal::StringToHGlobalAnsi(user);
	IntPtr ppass = Marshal::StringToHGlobalAnsi(pass);
	IntPtr pserver = Marshal::StringToHGlobalAnsi(server);
	IntPtr pface = Marshal::StringToHGlobalAnsi(tposface);
	
	m_tpos = new TposClientConnection((char*)(void*)puser, (char*)(void*)ppass, (char*)(void*)pserver, port, (char*)(void*)pface);
	
	Marshal::FreeHGlobal(puser);
	Marshal::FreeHGlobal(ppass);
	Marshal::FreeHGlobal(pserver);
	Marshal::FreeHGlobal(pface);
    s_tradeidmap = new std::map<std::string,tgp>;
}

int TposProxy :: Connect()
{
	m_tpos->connect();
	return 1;
}

int TposProxy :: deleteTrade(
			String^ tradegroup, 
			String^ portfolio,
			String^ externalId
			)
{
	IntPtr ptradegroup = Marshal::StringToHGlobalAnsi(tradegroup);
	IntPtr pportfolio = Marshal::StringToHGlobalAnsi(portfolio);
	IntPtr pexternalId = Marshal::StringToHGlobalAnsi(externalId);
 
	////////////////////////////////////////////////
   // retrieve trade to delete
   TposClientBasket basket2( (char*)(void*)ptradegroup,(char*)(void*)pportfolio);
   m_tpos->retrieveWithExTradeID( (char*)(void*)pexternalId, basket2);

   ////////////////////////////////////////////////
   // mark it deleted
   TposClientTrade trade3;
   basket2.getTrade(0, trade3);
   trade3.markDeleted();

   ////////////////////////////////////////////////
   // send it to server for deletion
   basket2.addTrade( trade3 );
   int cresult = m_tpos->deleteTradeWithExTradeID( trade3 );
   if( cresult  < 0 )
      cout <<  TposErrorString( cresult ) << endl;
	
   Marshal::FreeHGlobal(ptradegroup);
	Marshal::FreeHGlobal(pportfolio);
	Marshal::FreeHGlobal(pexternalId);
	return 0;

}

int TposProxy :: enterFutureTrade(
			String^ tradegroup,
			String^ portfolio,
			String^ cfirm, 
			String^ caccount,
			String^ trader,
			String^ sectype,
			//String^ basesymbol,
			String^ ticker,
			//String^ expdate,
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
			String^ note)
{
	TposClientTrade trade1;
	IntPtr ptradegroup = Marshal::StringToHGlobalAnsi(tradegroup);
	IntPtr pportfolio = Marshal::StringToHGlobalAnsi(portfolio);
	IntPtr pcfirm = Marshal::StringToHGlobalAnsi(cfirm);
	IntPtr pcaccount = Marshal::StringToHGlobalAnsi(caccount);
	IntPtr ptrader = Marshal::StringToHGlobalAnsi(trader);
	IntPtr psectype = Marshal::StringToHGlobalAnsi(sectype);
	//IntPtr pbasesymbol = Marshal::StringToHGlobalAnsi(basesymbol);
	IntPtr pticker = Marshal::StringToHGlobalAnsi(ticker);
	//IntPtr pexpdate = Marshal::StringToHGlobalAnsi(expdate);
	IntPtr ptradedate = Marshal::StringToHGlobalAnsi(tradedate);
	IntPtr ptradetime = Marshal::StringToHGlobalAnsi(tradetime);
	IntPtr pexchange = Marshal::StringToHGlobalAnsi(exchange);
	IntPtr pshortsale = Marshal::StringToHGlobalAnsi(shortsale);
	IntPtr pcurrency = Marshal::StringToHGlobalAnsi(currency);
	IntPtr pcontrabroker = Marshal::StringToHGlobalAnsi(contrabroker);
	IntPtr pcontrafirm = Marshal::StringToHGlobalAnsi(contrafirm);
	IntPtr psource = Marshal::StringToHGlobalAnsi(source);
	IntPtr pextID = Marshal::StringToHGlobalAnsi(extID);
	IntPtr pextdesc = Marshal::StringToHGlobalAnsi(extdesc);
	IntPtr pnote = Marshal::StringToHGlobalAnsi(note);
	IntPtr pactivestate = Marshal::StringToHGlobalAnsi(activeState);

	trade1.setString( "TradingGroup", (char*)(void*)ptradegroup );
	trade1.setString( "Portfolio", (char*)(void*)pportfolio );
	trade1.setString( "ClearingFirm", (char*)(void*)pcfirm );
   	trade1.setString( "ClearingAccount", (char*)(void*)pcaccount );
   	trade1.setString( "TraderAcronym", (char*)(void*)ptrader );
   	trade1.setString( "SecurityType", (char*)(void*)psectype );
   	//trade1.setString( "Symbol", (char*)(void*)pbasesymbol );
   	trade1.setString( "ExtendedSymbol", (char*)(void*)pticker );
   	//trade1.setString( "ExpirationDate", (char*)(void*)pexpdate );
   	trade1.setString( "TradeDate", (char*)(void*)ptradedate );
  	trade1.setString( "TradeTime", (char*)(void*)ptradetime );
  	trade1.setString( "Exchange", (char*)(void*)pexchange );
  	trade1.setString( "ShortSaleFlag", (char*)(void*)pshortsale );
  	trade1.setString( "BaseCurrency", (char*)(void*)pcurrency );
  	trade1.setString( "CrossCurrency", (char*)(void*)pcurrency );
   	trade1.setString( "ContraBroker", (char*)(void*)pcontrabroker );
   	trade1.setString( "ContraFirm", (char*)(void*)pcontrafirm );
   	trade1.setString( "TradeSource", (char*)(void*)psource );
   	trade1.setString( "ExternalTradeID", (char*)(void*)pextID );
   	trade1.setString( "ExternalTradeDesc", (char*)(void*)pextdesc );
   	trade1.setString( "Note", (char*)(void*)pnote );
   	trade1.setDouble( "Price", price );
   	trade1.setString( "ClearingStatus", (char*)(void*)pactivestate );
   	trade1.setDouble( "Commission", commission );
   	trade1.setInt( "Qty", qty );

	trade1.dump( std::cout );
	if( m_tpos )
s_debugFilter = 1;
	int result = m_tpos->enterOrUpdateTrade( trade1 );
	if( result == TPOS_ATTEMPT_TO_EDIT_VTK || result == TPOS_SECURITY_VALIDATION_CHANGE )
        deleteTrade( tradegroup,portfolio,extID );
	m_tpos->enterOrUpdateTrade( trade1 );
	
	/////////////////////////////////////////////////////////////////////////
	std::string id( (char*)(void*)pextID );
	tgp ttt;
	ttt.group = (char*)(void*)ptradegroup;
	ttt.portfolio = (char*)(void*)pportfolio;
	std::map<std::string,tgp>::iterator iter = s_tradeidmap->find( id );
	if( iter != s_tradeidmap->end() )
	{
		if( (*iter).second.group != ttt.group || (*iter).second.portfolio != ttt.portfolio )
		{
			deleteTrade( gcnew String((*iter).second.group.c_str()),gcnew String((*iter).second.portfolio.c_str()), gcnew String(id.c_str()) );
			s_tradeidmap->erase(iter);
		}
	}
	s_tradeidmap->insert( pair<std::string,tgp>( id, ttt ) );
	/////////////////////////////////////////////////////////////////

	trade1.dump( std::cout );
   
	Marshal::FreeHGlobal(ptradegroup);
	Marshal::FreeHGlobal(pportfolio);
	Marshal::FreeHGlobal(pcfirm);
	Marshal::FreeHGlobal(pcaccount);
	Marshal::FreeHGlobal(ptrader);
	Marshal::FreeHGlobal(psectype);
	//Marshal::FreeHGlobal(pbasesymbol);
	Marshal::FreeHGlobal(pticker);
	//Marshal::FreeHGlobal(pexpdate);
	Marshal::FreeHGlobal(ptradedate);
	Marshal::FreeHGlobal(ptradetime);
	Marshal::FreeHGlobal(pexchange);
	Marshal::FreeHGlobal(pshortsale);
	Marshal::FreeHGlobal(pcurrency);
	Marshal::FreeHGlobal(pcontrabroker);
	Marshal::FreeHGlobal(pcontrafirm);
	Marshal::FreeHGlobal(psource);
	Marshal::FreeHGlobal(pextID);
	Marshal::FreeHGlobal(pextdesc);
	Marshal::FreeHGlobal(pnote);
	Marshal::FreeHGlobal(pactivestate);

	return 1;
}

int TposProxy :: storeID(
			String^ tradegroup,
			String^ portfolio, 
			String^ extID )
{
	IntPtr ptradegroup = Marshal::StringToHGlobalAnsi(tradegroup);
	IntPtr pportfolio = Marshal::StringToHGlobalAnsi(portfolio);
    IntPtr pextID = Marshal::StringToHGlobalAnsi(extID);
	/////////////////////////////////////////////////////////////////////////
	std::string id( (char*)(void*)pextID );
	tgp ttt;
	ttt.group = (char*)(void*)ptradegroup;
	ttt.portfolio = (char*)(void*)pportfolio;
	std::map<std::string,tgp>::iterator iter = s_tradeidmap->find( id );
	if( iter != s_tradeidmap->end() )
	{
		if( (*iter).second.group != ttt.group || (*iter).second.portfolio != ttt.portfolio )
		{
			deleteTrade( gcnew String((*iter).second.group.c_str()),gcnew String((*iter).second.portfolio.c_str()), gcnew String(id.c_str()) );
			s_tradeidmap->erase(iter);
		}
	}
	s_tradeidmap->insert( pair<std::string,tgp>( id, ttt ) );
	/////////////////////////////////////////////////////////////////
	Marshal::FreeHGlobal(ptradegroup);
	Marshal::FreeHGlobal(pportfolio);
	Marshal::FreeHGlobal(pextID);
	return 1;
}

int TposProxy :: enterFixedIncomeTrade(
			String^ tradegroup,
			String^ portfolio,
			String^ cfirm, 
			String^ caccount,
			String^ trader,
			String^ sectype,
			String^ cexInstID,
			String^ iexInstID,
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
			String^ note)
{
	TposClientTrade trade1;
	IntPtr ptradegroup = Marshal::StringToHGlobalAnsi(tradegroup);
	IntPtr pportfolio = Marshal::StringToHGlobalAnsi(portfolio);
	IntPtr pcfirm = Marshal::StringToHGlobalAnsi(cfirm);
	IntPtr pcaccount = Marshal::StringToHGlobalAnsi(caccount);
	IntPtr ptrader = Marshal::StringToHGlobalAnsi(trader);
	IntPtr psectype = Marshal::StringToHGlobalAnsi(sectype);
	IntPtr pcextInstID = Marshal::StringToHGlobalAnsi(cexInstID);
	IntPtr piextInstID = Marshal::StringToHGlobalAnsi(iexInstID);
	IntPtr ptradedate = Marshal::StringToHGlobalAnsi(tradedate);
	IntPtr ptradetime = Marshal::StringToHGlobalAnsi(tradetime);
	IntPtr pexchange = Marshal::StringToHGlobalAnsi(exchange);
	IntPtr pshortsale = Marshal::StringToHGlobalAnsi(shortsale);
	IntPtr pcurrency = Marshal::StringToHGlobalAnsi(currency);
	IntPtr pcontrabroker = Marshal::StringToHGlobalAnsi(contrabroker);
	IntPtr pcontrafirm = Marshal::StringToHGlobalAnsi(contrafirm);
	IntPtr psource = Marshal::StringToHGlobalAnsi(source);
	IntPtr pextID = Marshal::StringToHGlobalAnsi(extID);
	IntPtr pextdesc = Marshal::StringToHGlobalAnsi(extdesc);
	IntPtr pnote = Marshal::StringToHGlobalAnsi(note);
	IntPtr pclearingStatus = Marshal::StringToHGlobalAnsi(clearingStatus);
	IntPtr pclearingRecord = Marshal::StringToHGlobalAnsi(clearingRecord);
	IntPtr psettleDate = Marshal::StringToHGlobalAnsi(settleDate);
	IntPtr pactivestate = Marshal::StringToHGlobalAnsi(activeState);

	trade1.setString( "TradingGroup", (char*)(void*)ptradegroup );
	trade1.setString( "Portfolio", (char*)(void*)pportfolio );
	trade1.setString( "ClearingFirm", (char*)(void*)pcfirm );
   	trade1.setString( "ClearingAccount", (char*)(void*)pcaccount );
   	trade1.setString( "TraderAcronym", (char*)(void*)ptrader );
   	trade1.setString( "SecurityType", (char*)(void*)psectype );
   	trade1.setString( "CUSIP", (char*)(void*)pcextInstID );
   	trade1.setString( "ISIN", (char*)(void*)piextInstID );
   	trade1.setString( "TradeDate", (char*)(void*)ptradedate );
  	trade1.setString( "TradeTime", (char*)(void*)ptradetime );
  	trade1.setString( "Exchange", (char*)(void*)pexchange );
  	trade1.setString( "ShortSaleFlag", (char*)(void*)pshortsale );
  	trade1.setString( "BaseCurrency", (char*)(void*)pcurrency );
  	trade1.setString( "CrossCurrency", (char*)(void*)pcurrency );
   	trade1.setString( "ContraBroker", (char*)(void*)pcontrabroker );
   	trade1.setString( "ContraFirm", (char*)(void*)pcontrafirm );
   	trade1.setString( "TradeSource", (char*)(void*)psource );
   	trade1.setString( "ExternalTradeID", (char*)(void*)pextID );
   	trade1.setString( "ExternalTradeDesc", (char*)(void*)pextdesc );
	trade1.setString( "ClearingStatus", (char*)(void*)pclearingStatus );
	trade1.setString( "ClearingRecord", (char*)(void*)pclearingRecord );
   	trade1.setString( "Note", (char*)(void*)pnote );
   	trade1.setDouble( "Price", price );
   	trade1.setInt( "Qty", qty );
	trade1.setString( "SettleDate", (char*)(void*)psettleDate );
   	trade1.setString( "ClearingStatus", (char*)(void*)pactivestate );
   	trade1.setDouble( "Commission", commission );

	trade1.dump( std::cout );
	if( m_tpos )
s_debugFilter = 1;
	int result = m_tpos->enterOrUpdateTrade( trade1 );
	if( result == TPOS_ATTEMPT_TO_EDIT_VTK || result == TPOS_SECURITY_VALIDATION_CHANGE )
        deleteTrade( tradegroup,portfolio,extID );
	m_tpos->enterOrUpdateTrade( trade1 );


	/////////////////////////////////////////////////////////////////////////
	std::string id( (char*)(void*)pextID );
	tgp ttt;
	ttt.group = (char*)(void*)ptradegroup;
	ttt.portfolio = (char*)(void*)pportfolio;
	std::map<std::string,tgp>::iterator iter = s_tradeidmap->find( id );
	if( iter != s_tradeidmap->end() )
	{
		if( (*iter).second.group != ttt.group || (*iter).second.portfolio != ttt.portfolio )
		{
			deleteTrade( gcnew String((*iter).second.group.c_str()),gcnew String((*iter).second.portfolio.c_str()), gcnew String(id.c_str()) );
			s_tradeidmap->erase(iter);
		}
	}
	s_tradeidmap->insert( pair<std::string,tgp>( id, ttt ) );
	/////////////////////////////////////////////////////////////////

	trade1.dump( std::cout );
   
	Marshal::FreeHGlobal(ptradegroup);
	Marshal::FreeHGlobal(pportfolio);
	Marshal::FreeHGlobal(pcfirm);
	Marshal::FreeHGlobal(pcaccount);
	Marshal::FreeHGlobal(ptrader);
	Marshal::FreeHGlobal(psectype);
	Marshal::FreeHGlobal(pcextInstID);
	Marshal::FreeHGlobal(piextInstID);
	Marshal::FreeHGlobal(ptradedate);
	Marshal::FreeHGlobal(ptradetime);
	Marshal::FreeHGlobal(pexchange);
	Marshal::FreeHGlobal(pshortsale);
	Marshal::FreeHGlobal(pcurrency);
	Marshal::FreeHGlobal(pcontrabroker);
	Marshal::FreeHGlobal(pcontrafirm);
	Marshal::FreeHGlobal(psource);
	Marshal::FreeHGlobal(pextID);
	Marshal::FreeHGlobal(pextdesc);
	Marshal::FreeHGlobal(pnote);
	Marshal::FreeHGlobal(psettleDate);
	Marshal::FreeHGlobal(pactivestate);
	return 1;
}

int TposProxy :: enterOptionTrade(
			String^ tradegroup,
			String^ portfolio,
			String^ cfirm, 
			String^ caccount,
			String^ trader,
			String^ sectype,
			String^ basesymbol,
			String^ expdate,
			String^ callput,
			double strike,
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
			String^ note)
{
	TposClientTrade trade1;
	IntPtr ptradegroup = Marshal::StringToHGlobalAnsi(tradegroup);
	IntPtr pportfolio = Marshal::StringToHGlobalAnsi(portfolio);
	IntPtr pcfirm = Marshal::StringToHGlobalAnsi(cfirm);
	IntPtr pcaccount = Marshal::StringToHGlobalAnsi(caccount);
	IntPtr ptrader = Marshal::StringToHGlobalAnsi(trader);
	IntPtr psectype = Marshal::StringToHGlobalAnsi(sectype);
	IntPtr pbasesymbol = Marshal::StringToHGlobalAnsi(basesymbol);
	IntPtr pexpdate = Marshal::StringToHGlobalAnsi(expdate);
	IntPtr pcallput = Marshal::StringToHGlobalAnsi(callput);
	IntPtr ptradedate = Marshal::StringToHGlobalAnsi(tradedate);
	IntPtr ptradetime = Marshal::StringToHGlobalAnsi(tradetime);
	IntPtr pexchange = Marshal::StringToHGlobalAnsi(exchange);
	IntPtr pshortsale = Marshal::StringToHGlobalAnsi(shortsale);
	IntPtr pcurrency = Marshal::StringToHGlobalAnsi(currency);
	IntPtr pcontrabroker = Marshal::StringToHGlobalAnsi(contrabroker);
	IntPtr pcontrafirm = Marshal::StringToHGlobalAnsi(contrafirm);
	IntPtr psource = Marshal::StringToHGlobalAnsi(source);
	IntPtr pextID = Marshal::StringToHGlobalAnsi(extID);
	IntPtr pextdesc = Marshal::StringToHGlobalAnsi(extdesc);
	IntPtr pnote = Marshal::StringToHGlobalAnsi(note);
	IntPtr pactivestate = Marshal::StringToHGlobalAnsi(activeState);

	trade1.setString( "TradingGroup", (char*)(void*)ptradegroup );
	trade1.setString( "Portfolio", (char*)(void*)pportfolio );
	trade1.setString( "ClearingFirm", (char*)(void*)pcfirm );
   	trade1.setString( "ClearingAccount", (char*)(void*)pcaccount );
   	trade1.setString( "TraderAcronym", (char*)(void*)ptrader );
   	trade1.setString( "SecurityType", (char*)(void*)psectype );
   	trade1.setString( "Symbol", (char*)(void*)pbasesymbol );
   	trade1.setString( "ExpirationDate", (char*)(void*)pexpdate );
   	trade1.setString( "PutCall", (char*)(void*)pcallput );
   	trade1.setDouble( "Strike", strike );
   	trade1.setString( "TradeDate", (char*)(void*)ptradedate );
  	trade1.setString( "TradeTime", (char*)(void*)ptradetime );
  	trade1.setString( "Exchange", (char*)(void*)pexchange );
  	trade1.setString( "ShortSaleFlag", (char*)(void*)pshortsale );
  	trade1.setString( "BaseCurrency", (char*)(void*)pcurrency );
  	trade1.setString( "CrossCurrency", (char*)(void*)pcurrency );
   	trade1.setString( "ContraBroker", (char*)(void*)pcontrabroker );
   	trade1.setString( "ContraFirm", (char*)(void*)pcontrafirm );
   	trade1.setString( "TradeSource", (char*)(void*)psource );
   	trade1.setString( "ExternalTradeID", (char*)(void*)pextID );
   	trade1.setString( "ExternalTradeDesc", (char*)(void*)pextdesc );
   	trade1.setString( "Note", (char*)(void*)pnote );
   	trade1.setDouble( "Price", price );
   	trade1.setInt( "Qty", qty );
   	trade1.setString( "ClearingStatus", (char*)(void*)pactivestate );
   	trade1.setDouble( "Commission", commission );

	trade1.dump( std::cout );
	if( m_tpos )
s_debugFilter = 1;
	int result = m_tpos->enterOrUpdateTrade( trade1 );
	if( result == TPOS_ATTEMPT_TO_EDIT_VTK || result == TPOS_SECURITY_VALIDATION_CHANGE )
        deleteTrade( tradegroup,portfolio,extID );
	m_tpos->enterOrUpdateTrade( trade1 );

	/////////////////////////////////////////////////////////////////////////
	std::string id( (char*)(void*)pextID );
	tgp ttt;
	ttt.group = (char*)(void*)ptradegroup;
	ttt.portfolio = (char*)(void*)pportfolio;
	std::map<std::string,tgp>::iterator iter = s_tradeidmap->find( id );
	if( iter != s_tradeidmap->end() )
	{
		if( (*iter).second.group != ttt.group || (*iter).second.portfolio != ttt.portfolio )
		{
			deleteTrade( gcnew String((*iter).second.group.c_str()),gcnew String((*iter).second.portfolio.c_str()), gcnew String(id.c_str()) );
			s_tradeidmap->erase(iter);
		}
	}
	s_tradeidmap->insert( pair<std::string,tgp>( id, ttt ) );
	/////////////////////////////////////////////////////////////////


	trade1.dump( std::cout );
   
	Marshal::FreeHGlobal(ptradegroup);
	Marshal::FreeHGlobal(pportfolio);
	Marshal::FreeHGlobal(pcfirm);
	Marshal::FreeHGlobal(pcaccount);
	Marshal::FreeHGlobal(ptrader);
	Marshal::FreeHGlobal(psectype);
	Marshal::FreeHGlobal(pbasesymbol);
	Marshal::FreeHGlobal(pcallput);
	Marshal::FreeHGlobal(pexpdate);
	Marshal::FreeHGlobal(ptradedate);
	Marshal::FreeHGlobal(ptradetime);
	Marshal::FreeHGlobal(pexchange);
	Marshal::FreeHGlobal(pshortsale);
	Marshal::FreeHGlobal(pcurrency);
	Marshal::FreeHGlobal(pcontrabroker);
	Marshal::FreeHGlobal(pcontrafirm);
	Marshal::FreeHGlobal(psource);
	Marshal::FreeHGlobal(pextID);
	Marshal::FreeHGlobal(pextdesc);
	Marshal::FreeHGlobal(pnote);
	Marshal::FreeHGlobal(pactivestate);

	return 1;
}

int TposProxy :: GetSecurityByTicker(String^ tickers, ArrayList^ out)
{
	if ( ! m_tpos)
		return 0;

	out->Clear();
	array<Char>^ seperators = {' ', ','};
	array<String^>^ splits = tickers->Split(seperators);
	IEnumerator^ myEnum = splits->GetEnumerator();
	while ( myEnum->MoveNext() )
	{
		String^ ticker = safe_cast<String^>(myEnum->Current);
		if ( ticker->Trim()->Equals( "" ) )
			continue;

		vector<securityInfoX> si;
		IntPtr pticker = Marshal::StringToHGlobalAnsi(ticker);
		m_tpos->getSecuritybyTicker( (char*)(void*)pticker, si );
		for (vector<securityInfoX>::iterator itr = si.begin(); itr != si.end(); itr++ )
		{
			securityInfoX& sec = *itr;
			out->Add(gcnew SecurityDesc(sec));
		}

		Marshal::FreeHGlobal(pticker);
	}
	return 1;
}


int TposProxy :: GetOptionChain(String^ underlying, ArrayList^ out)
{
	vector<optionInfo> oi;
	if ( m_tpos)
	{
		IntPtr punderlying = Marshal::StringToHGlobalAnsi(underlying);
		m_tpos->getOptionChain( (char*)(void*)punderlying, "", oi );

		for (vector<optionInfo>::iterator itr = oi.begin();	itr != oi.end(); itr++ )
		{
			optionInfo& val = *itr;
			out->Add(gcnew OptionDesc(val));
		}

		Marshal::FreeHGlobal(punderlying);
	}
	return 1;
}

int TposProxy :: GetFutureChainByBase(String^ base, ArrayList^ out)
{
	vector<futureInfo> fi;
	if ( m_tpos)
	{
		IntPtr pbase = Marshal::StringToHGlobalAnsi(base);
		m_tpos->getFutureChain( (char*)(void*)pbase, "", fi );

		for (vector<futureInfo>::iterator itr = fi.begin();	itr != fi.end(); itr++ )
		{
			futureInfo& val = *itr;
			out->Add(gcnew FutureDesc(val));
		}

		Marshal::FreeHGlobal(pbase);
	}
	return 1;
}

int TposProxy :: GetFutureChain(String^ underlying, ArrayList^ out)
{
	vector<futureInfo> fi;
	if ( m_tpos)
	{
		IntPtr punderlying = Marshal::StringToHGlobalAnsi(underlying);
		m_tpos->getFutureChainUnd( (char*)(void*)punderlying, "", fi );

		for (vector<futureInfo>::iterator itr = fi.begin(); itr != fi.end(); itr++ )
		{
			futureInfo& val = *itr;
			out->Add(gcnew FutureDesc(val));
		}

		Marshal::FreeHGlobal(punderlying);
	}
	return 1;
}


int TposProxy :: GetSSFutureChain(String^ underlying, ArrayList^ out)
{
	vector<futureInfo> fi;
	if ( m_tpos)
	{
		IntPtr punderlying = Marshal::StringToHGlobalAnsi(underlying);
		m_tpos->getSSFutureChain( (char*)(void*)punderlying, "", fi );

		for (vector<futureInfo>::iterator itr = fi.begin(); itr != fi.end(); itr++ )
		{
			futureInfo& val = *itr;
			out->Add(gcnew FutureDesc(val));
		}

		Marshal::FreeHGlobal(punderlying);
	}
	return 1;
}

int TposProxy :: GetMappedAccount( String^ account, ArrayList^ out )
{
	IntPtr paccount = Marshal::StringToHGlobalAnsi(account);
	const char * vtk = 0;;
	std::string saccount = (char*)(void*)paccount;
	Marshal::FreeHGlobal(paccount);
	vtk = m_tpos->getTposMapValue( "Account", saccount.c_str() );
	const TposAccount * acc = 0;
	if( vtk )
	{
		acc = m_tpos->getTposAccount( vtk );
		if( acc )
		{		
			//String^ a1 = acc->m_portfolioSymbol;
			out->Add(gcnew String(acc->m_tradingGroupSymbol.c_str()));
			out->Add(gcnew String(acc->m_portfolioSymbol.c_str()));
			out->Add(gcnew String(acc->m_clearingFirmSymbol.c_str()));
			out->Add(gcnew String(acc->m_clearingAccount.c_str()));
			out->Add(gcnew String(acc->m_traderAcronym.c_str()));
			return 1;
		}
	}

	return out->Count;
}

String^ TposProxy :: GetMappedSecurity( String^ sec )
{
	IntPtr psec = Marshal::StringToHGlobalAnsi(sec);
	const char * ret_sec = 0;
	std::string ssec = (char*)(void*)psec;
	Marshal::FreeHGlobal(psec);
	ret_sec = m_tpos->getTposMapValue( "Security", ssec.c_str() );
	if( ret_sec )
	{
			return gcnew String(ret_sec);
			//String^ a1 = acc->m_portfolioSymbol;
			//out->Add(gcnew String(acc->m_tradingGroupSymbol.c_str()));
			//out->Add(gcnew String(acc->m_portfolioSymbol.c_str()));
			//out->Add(gcnew String(acc->m_clearingFirmSymbol.c_str()));
			//out->Add(gcnew String(acc->m_clearingAccount.c_str()));
			//out->Add(gcnew String(acc->m_traderAcronym.c_str()));
			//return 1;
	}
	return gcnew String("");
}
int TposProxy :: GetTraderAccounts(ArrayList^ out)
{
	if ( m_tpos == 0 )
		Console::WriteLine("TPOS is not connected");

	vector<TposAccount>   accounts;
	m_tpos->getAccountList(accounts);

	Console::WriteLine("Received accounts:   " + accounts.size() );
	Hashtable^ hash = gcnew Hashtable();

	for (vector<TposAccount>::iterator itr = accounts.begin();itr != accounts.end(); itr++)
	{
		TposAccount* val = &(*itr);

		string skey = val->m_tradingGroupSymbol + "." + val->m_portfolioSymbol + "." + val->m_traderAcronym;
		String^ key = gcnew String(skey.c_str());
		if ( ! hash->Contains(key) )
		{
			TposTraderAccount^ acct = gcnew TposTraderAccount();
			acct->m_TradingGroup = gcnew String(val->m_tradingGroupSymbol.c_str());
			acct->m_Portfolio  = gcnew String(val->m_portfolioSymbol.c_str() );
			acct->m_TraderAcronym  = gcnew String(val->m_traderAcronym.c_str() );
			hash->Add(key, acct);
		}

		// acct->m_ValidTraderID = gcnew String(val->m_validTraderID.c_str() );
		// acct->m_ClearingFirm = gcnew String(val->m_clearingFirmSymbol.c_str());
		// acct->m_ClearingSubAccount  = gcnew String(val->m_clearingSubAccount.c_str() );
		// acct->m_TraderAcronym  = gcnew String(val->m_traderAcronym.c_str() );
		// out->Add(acct);
	}
	out->AddRange(hash->Values);
	return out->Count;
}
// for Different conneciton
//
int TposProxy :: GetBODPositions(String^ grp, String^ pf, ArrayList^ out)
{
	if ( m_tpos == 0 )
		return -1;

	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);

	vector<PositionDesc*> positions;
	m_tpos->getPositionsAsOf(positions, (char*)(void*)group, (char*)(void*)portfolio, 0);
	cout << "get positions: " << (char*)(void*)group << ",  " << (char*)(void*)portfolio << ", " 
			<<  ", " << positions.size() << endl;

	for (vector<PositionDesc*>::iterator itr = positions.begin(); itr != positions.end(); itr++ )
	{
		PositionDesc* val = *itr;
		if ( val == 0)
			continue;

		out->Add(gcnew TposPosition(val));
		delete val;
	}

	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);

	return out->Count;
}
int TposProxy :: GetBODPositions(String^ grp, String^ pf, String^ tdr, ArrayList^ out)
{
	if ( m_tpos == 0 )
		return -1;

	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);
	IntPtr trader = Marshal::StringToHGlobalAnsi(tdr);

	vector<PositionDesc*> positions;
	m_tpos->getPositionsTrader(positions, (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader, 0);
	cout << "get positions: " << (char*)(void*)group << ",  " << (char*)(void*)portfolio << ", " 
			<< (char*)(void*)trader <<  ", " << positions.size() << endl;

	for (vector<PositionDesc*>::iterator itr = positions.begin(); itr != positions.end(); itr++ )
	{
		PositionDesc* val = *itr;
		if ( val == 0)
			continue;

		out->Add(gcnew TposPosition(val));
		delete val;
	}

	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(trader);
	Marshal::FreeHGlobal(portfolio);

	return out->Count;
}

int TposProxy :: GetBODPositions(String^ grp, String^ pf, ArrayList^ traderAcronyms, ArrayList^ out)
{
	if ( m_tpos == 0 )
		return -1;

	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);

	for (int i = 0; i<traderAcronyms->Count; i++)
	{
		IntPtr trader = Marshal::StringToHGlobalAnsi((String^)traderAcronyms[i]);

		vector<PositionDesc*> positions;
		m_tpos->getPositionsAsOf(positions, (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader, 0);
		cout << "get positions: " << (char*)(void*)group << ",  " << (char*)(void*)portfolio << ", " 
			<< (char*)(void*)trader << ", "  << positions.size() << endl;

		for (vector<PositionDesc*>::iterator itr = positions.begin(); itr != positions.end(); itr++ )
		{
			PositionDesc* val = *itr;
			if ( val == 0)
				continue;
			
			out->Add(gcnew TposPosition(val));
			delete val;
		}
		Marshal::FreeHGlobal(trader);
	}

	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);

	return out->Count;
}

/*
int ggg(TposPubMessage * msg, IntPtr param)
{
	char * val = 0;
	int  ival = 0;
	double dval = 0;
	TposTrade^ trade = gcnew TposTrade();

	msg->getString( "TradingGroup", val );
    trade->m_TradeGroup = gcnew String(val);
	msg->getString( "Portfolio", val );
	trade->m_Portfolio = gcnew String(val);
	msg->getString( "TraderAcronym", val );
	trade->m_TraderAcronym = gcnew String(val);
    msg->getString( "ClearingAccount", val );
    trade->m_ClearingAccount = gcnew String(val);
	msg->getInt( "Version",  ival);
	trade->m_Version = ival;

	msg->getString( "TradeSource", val );
    trade->m_TradeSource = gcnew String(val);
    msg->getString( "ExternalTradeID", val );
	trade->m_ExtTradeID = gcnew String(val);
   
	msg->getString( "TradeDate", val );
	trade->m_TradeDate = gcnew String(val);
    msg->getString( "TradeTime", val );
	trade->m_TradeTime = gcnew String(val);

	msg->getString( "SecurityType", val );
	trade->m_SecType = gcnew String(val);
	msg->getString( "Symbol", val );
	trade->m_ClearingAccount = gcnew String(val);
    msg->getString( "ExpirationDate", val );
	trade->m_ExpDate = gcnew String(val);
    msg->getString( "PutCall", val );
    trade->m_PutCall = gcnew String(val);
    msg->getDouble( "Strike",  dval );
	trade->m_Strike = dval;

    msg->getDouble( "Price",  dval);
	trade->m_Price = dval;
    msg->getInt( "Qty",  ival);
	trade->m_Qty = ival;

	GCHandle gch = GCHandle::FromIntPtr(param);
	TposTradeCallback^ realcb = (TposTradeCallback^)gch.Target;
	realcb->OnTrade(trade);

	return 1;
}
*/

//int __cdecl tposCallbackFunc( TposPubMessage * msg, void* user_data )
int tposCallbackFunc( TposPubMessage * msg, void* user_data )
{
	char * val = 0;
	int  ival = 0;
	double dval = 0;
	TposTrade^ trade = gcnew TposTrade();

	msg->getString( "TradingGroup", val );
    trade->m_TradeGroup = gcnew String(val);
	msg->getString( "Portfolio", val );
	trade->m_Portfolio = gcnew String(val);
	msg->getString( "TraderAcronym", val );
	trade->m_TraderAcronym = gcnew String(val);
    msg->getString( "ClearingAccount", val );
    trade->m_ClearingAccount = gcnew String(val);
	msg->getInt( "Version",  ival);
	trade->m_Version = ival;

	Threading::Thread^ thread = Threading::Thread::CurrentThread;
	AppDomain^ dm = thread->GetDomain();

	msg->getString( "TradeSource", val );
    trade->m_TradeSource = gcnew String(val);
    msg->getString( "ExternalTradeID", val );
	trade->m_ExtTradeID = gcnew String(val);
   
	msg->getString( "TradeDate", val );
	trade->m_TradeDate = gcnew String(val);
    msg->getString( "TradeTime", val );
	trade->m_TradeTime = gcnew String(val);

	msg->getString( "SecurityType", val );
	trade->m_SecType = gcnew String(val);
	msg->getString( "Symbol", val );
	trade->m_Symbol = gcnew String(val);
	msg->getString( "UndSymbol", val );
	trade->m_UndSymbol = gcnew String(val);
    msg->getString( "ExpirationDate", val );
	trade->m_ExpDate = gcnew String(val);
    msg->getString( "PutCall", val );
    trade->m_PutCall = gcnew String(val);
    msg->getDouble( "Strike",  dval );
	trade->m_Strike = dval;
	//msg->getString("ClearingAccount", val);
	// trade->m_ClearingAccount = gcnew String(val);

    msg->getDouble( "Price",  dval);
	trade->m_Price = dval;
    msg->getInt( "Qty",  ival);
	trade->m_Qty = ival;

	try {
		// TposNativeCB* cb = static_cast<TposNativeCB*>(user_data);
		TposNativeCB* cb = (TposNativeCB*)user_data;
		// cout << cb << endl;
		// BACK  cb->m_tposcb->OnTrade(trade);

		gcroot<TposTradeCallback^>* a  = new gcroot<TposTradeCallback^>(cb->m_tposcb);
		(*a)->OnTrade(trade);
	}
	catch(Exception^ ex)
	{
		String^ exstr = ex->ToString();
		StreamWriter^ stream = gcnew StreamWriter("C:\\temp\\a.log", true);
		stream->WriteLine(exstr);
	}
	return( 1 );
}


int TposProxy :: GetTrades(String^ user, String^ grp, String^ pf, String^ tdr, ArrayList^ out)
{
	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);
	IntPtr trader = Marshal::StringToHGlobalAnsi(tdr);

	vector<TradeDesc*> trades;
	m_tpos->getTrades(trades, (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader, 0,0,0,0,0,0,0,0);
	cout << "get trades: " << (char*)(void*)group << ",  " << (char*)(void*)portfolio  << ", " << (char*)(void*)trader<< endl;
	String^ romtag = "Rx" + user + "-";

	for (vector<TradeDesc*>::iterator itr = trades.begin();	itr != trades.end(); itr++ )
	{
		TradeDesc* val = *itr;
		if ( val == 0)
			continue;

		String^ exId = gcnew String(val->getExtTradeID().c_str());

		if ( String::IsNullOrEmpty(user) || ! exId->StartsWith(romtag) )
		{
			out->Add(gcnew TposTrade(val));
		}
		delete val;
	}

	Marshal::FreeHGlobal(trader);
	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);

	return out->Count;
}

/*
   int getTrades( vector<TradeDesc *> & r_trades,
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          const char * a_beginLastModDate,
                          const char * a_beginLastModTime,
                          const char * a_endLastModDate,
                          const char * a_endLastModTime,
                          const char * a_beginTradeDate,
                          const char * a_beginTradeTime,
                          const char * a_endTradeDate,
                          const char * a_endTradeTime,
                          bool a_allVersions = false );
*/

int TposProxy :: GetTrades(String^ user, String^ grp, String^ pf, ArrayList^ traderAcronyms, ArrayList^ out)
{
	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);
	String^ romtag = "Rx" + user + "-";

	for (int i = 0; i<traderAcronyms->Count; i++)
	{
		IntPtr trader = Marshal::StringToHGlobalAnsi((String^)traderAcronyms[i]);

		vector<TradeDesc*> trades;
		m_tpos->getTrades(trades, (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader, 0,0,0,0,0,0,0,0);
		cout << "get trades: " << (char*)(void*)group << ",  " << (char*)(void*)portfolio  << ", " << (char*)(void*)trader<< endl;

		for (vector<TradeDesc*>::iterator itr = trades.begin(); itr != trades.end(); itr++ )
		{
			TradeDesc* val = *itr;
			if ( val == 0)
				continue;

			String^ exId = gcnew String(val->getExtTradeID().c_str());
			if ( String::IsNullOrEmpty(user) || ! exId->StartsWith(romtag) )
			{
				out->Add(gcnew TposTrade(val));
			}
			delete val;
		}
		Marshal::FreeHGlobal(trader);
	}

	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);

	return out->Count;
}

void TposProxy :: SubscribeTrades(String^ grp, String^ pf, String^ tdr, TposTradeCallback ^cb)
{
	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);
	IntPtr trader = Marshal::StringToHGlobalAnsi(tdr);

	//ObjRef^ oref = RemotingServices::Marshal(cb);

	Threading::Thread^ thread = Threading::Thread::CurrentThread;
	AppDomain^ dm = thread->GetDomain();
	m_ncb  =  new TposNativeCB();
	m_ncb->m_tposcb = gcroot<TposTradeCallback^>(cb);
	m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,  "23:59:59",0,  "23:59:59",0, tposCallbackFunc, (TposNativeCB*)m_ncb);

	/*
	TposNativeCB*  ncb = new TposNativeCB();
	ncb->m_tposcb = cb;
	m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,  "23:59:59",0,  "23:59:59",0, tposCallbackFunc, ncb);
	// m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,  "23:59:59",0,  "23:59:59",0, tposCallbackFunc, ncb );
	*/
	/*
	m_ncb = new TposNativeCB();
	m_ncb->m_tposcb = cb;
    m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,  "23:59:59",0,  "23:59:59",0, tposCallbackFunc, m_ncb );
	*/
	/*
	// GCHandle gch = GCHandle::Alloc(cb);
	GCHandle gch = GCHandle::Alloc(cb);
	///CBggg^ aCBggg = gcnew CBggg(ggg);
	m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,  "23:59:59",0,  "23:59:59",0, ggg, GCHandle::ToIntPtr(gch) );
	*/

	// gch.Free();
	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);
	Marshal::FreeHGlobal(trader);
}

void TposProxy :: SubscribeTrades(String^ grp, String^ pf, ArrayList^ traderAcronyms, TposTradeCallback ^cb)
{
	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);

	Threading::Thread^ thread = Threading::Thread::CurrentThread;
	AppDomain^ dm = thread->GetDomain();

	m_ncb  =  new TposNativeCB();
	m_ncb->m_tposcb = cb;
	
	// TposNativeCB*  ncb = new TposNativeCB();
	// ncb->m_tposcb = cb;
   
	for (int i = 0; i<traderAcronyms->Count; i++)
	{
		IntPtr trader = Marshal::StringToHGlobalAnsi((String^)traderAcronyms[i]);
		// m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,0,0,0,0, tposCallbackFunc, ncb );
		m_tpos->subscribeTrades( (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader,0,0,0,0,0, tposCallbackFunc, m_ncb );

		Marshal::FreeHGlobal(trader);
	}
 
	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);
}

void TposProxy :: UnsubscribeTrades(String^ grp, String^ pf, String^ tdr)
{
	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);
	IntPtr trader = Marshal::StringToHGlobalAnsi(tdr);

	m_tpos->unSubscribeTrade((char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader);
		
	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);
	Marshal::FreeHGlobal(trader);
}

String^ TposProxy :: RtrvTrades(String^ user, String^ grp, String^ pf, String^ tdr, String^ bDate, String^ bTime, ArrayList^ out)
{
	String^ ret = gcnew String(bTime);
	IntPtr group = Marshal::StringToHGlobalAnsi(grp);
	IntPtr portfolio = Marshal::StringToHGlobalAnsi(pf);
	IntPtr trader = Marshal::StringToHGlobalAnsi(tdr);
	String^ romtag = "Rx" + user + "-";

	IntPtr bDatePtr, bTimePtr;
	const char* pDate = 0;
	const char* pTime = 0;
	if ( ! String::IsNullOrEmpty(bDate) )
	{
		bDatePtr = Marshal::StringToHGlobalAnsi(bDate);
		pDate = (const char*) (void*) bDatePtr;
	}
	if ( ! String::IsNullOrEmpty(bTime))
	{
		bTimePtr = Marshal::StringToHGlobalAnsi(bTime);
		pTime = (const char*) (void*) bTimePtr;
	}

	vector<TradeDesc*> trades;
	m_tpos->getTrades(trades, (char*)(void*)group, (char*)(void*)portfolio, (char*)(void*)trader, pDate, pTime, 0,0,   0,0,0,0);
	cout << "get trades: " << (char*)(void*)group << ",  " << (char*)(void*)portfolio  << ", " << (char*)(void*)trader<< endl;

	for (vector<TradeDesc*>::iterator itr = trades.begin();	itr != trades.end(); itr++ )
	{
		TradeDesc* val = *itr;
		if ( val == 0 )
			continue;
	
		TposTrade^ trade = gcnew TposTrade(val);
		if ( trade->m_LastModTime->CompareTo(ret) > 0 )
			ret = trade->m_LastModTime;

		if ( String::IsNullOrEmpty(user) || ! trade->m_ExtTradeID->StartsWith(romtag) )
		{
			out->Add(trade);
		}
		// out->Add(trade);

		delete val;
	}

	Marshal::FreeHGlobal(trader);
	Marshal::FreeHGlobal(group);
	Marshal::FreeHGlobal(portfolio);
	if ( ! String::IsNullOrEmpty(bDate) )
		Marshal::FreeHGlobal(bDatePtr);
	if ( ! String::IsNullOrEmpty(bTime) )
		Marshal::FreeHGlobal(bTimePtr);

	return ret;
}
