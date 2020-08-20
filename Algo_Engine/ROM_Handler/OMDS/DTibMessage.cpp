// DTibMessage.cpp: implementation of the CDTibMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DTibMessage.h"
#include "Main.h"
#include "MessageProcessor.h"
#include "DataManager.h"
#include "ConTib.h"
#include "DClientRequest.h"
#include <sstream>
#include <iomanip>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ====================================================================
// virtual Data Tib Message
// ====================================================================
std::string CDateConverter::f2s( float f, int nPrecision )
{
	std::string	reply = " ";
	try
	{
		std::stringstream stm;
		stm <<  std::setfill('0') << std::setprecision(nPrecision) << std::fixed << f;
		reply = stm.str();
	}
	catch(...)
	{
		throw exception("Unkown Error"); // added throw
	};
	return(reply);
};

std::string CDateConverter::l2s(long l)
{
	if( l < 0 )
		return " ";
	if( l > 1,000,000 )
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDateConverter::l2s() data corrution" );
		return "";
	}
	char buff[50];
	memset(buff,0,50);
	sprintf(buff,"%d",l);
	return(buff);
};

CDTibMessage::CDTibMessage() : m_nRetry(0)
{
	m_type = 0 ;
	m_subject = "" ;
	m_pRequestManager = CMain::getInstance()->getMessageProcessor()->getRequestManager();
	m_pDataManager = CMain::getInstance()->getDataManager();
	m_base		= "";
	m_ric		= "";
	m_symbol	= "";
	m_secType	= "";	
	m_nRetry	= 0;
}

CDTibMessage::~CDTibMessage()
{

}

CDTibMessage::CDTibMessage( const CDTibMessage & msg )
{
	(*this) = msg;
}

CDTibMessage & CDTibMessage::operator=( const CDTibMessage & msg )
{
	m_type				= msg.m_type;
	m_pRequestManager	= msg.m_pRequestManager;
	m_pDataManager		= msg.m_pDataManager;
	m_nRetry			= 0;

	m_subject			= msg.m_subject;
	m_subject.begin();
	m_base				= msg.m_base;
	m_base.begin();
	m_ric				= msg.m_ric;
	m_ric.begin();
	m_symbol			= msg.m_symbol;
	m_symbol.begin();
	m_secType			= msg.m_secType;	
	m_secType.begin();

	return *this;
}

void CDTibMessage::setSubject( const std::string & subject ) 
{ 
	if( subject.empty() )
		return;
	m_subject = subject;
	m_subject.begin();
}

void CDTibMessage::setBase( const std::string & szBase )
{
	if( szBase.empty() )
		return;
	m_base = szBase;
	m_base.begin();
	// Translation
//	if( m_base.find('^') != -1 )
//		m_base.replace( m_base.find('^'), 1, "/" ); 
	if( m_base.find('/') != -1 )
		m_base.replace( m_base.find('/'), 1, "^" ); 

}

void CDTibMessage::setSymbol( const std::string & szSymbol )
{
	if( szSymbol.empty() )
		return;
	m_symbol = szSymbol;
	m_symbol.begin();
}

void CDTibMessage::setRic( const std::string & szRic )
{
	if( szRic.empty() )
		return;
	m_ric = szRic;
	m_ric.begin();
}

void CDTibMessage::setSecType( const std::string & szSecType )
{
	if( szSecType.empty() )
		return;
	m_secType = szSecType;
	m_secType.begin();
}

void CDTibMessage::Process()
{
	// don't try/catch here, exception handled by caller
	std::string szRequest = GetClientRequest();
	if( szRequest.empty() )
		return;
	try
	{
		updateCache();
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTibMessage::Process() updateCache() exception"+szRequest);
	}
	// double dispatch - 1
	m_pRequestManager->sendToClient( szRequest, this );
}


// ====================================================================
// CDTMOptionQuoteBest
// ====================================================================
CDTMOptionQuoteBest::CDTMOptionQuoteBest()
{
	m_type = DATATIBMESSAGE_OPTIONQUOTEBEST;
	m_last = m_ask = m_bid = -1.0;
	m_volume = -1;
	m_askSize = m_bidSize = m_lastSize = -1;
	m_symbol = "";
	m_askEx = "";
	m_bidEx = "";
	m_lastEx = "";
	m_changedFields = 0;
	m_delta = -1.0;		//	-1.0 means on delta
	m_change = -1.0; // missing before

}

CDTMOptionQuoteBest::~CDTMOptionQuoteBest()
{

}

CDTMOptionQuoteBest::CDTMOptionQuoteBest( const CDTMOptionQuoteBest & msg )
{
	( *this ) = msg;
}

CDTMOptionQuoteBest & CDTMOptionQuoteBest::operator=( const CDTMOptionQuoteBest & msg )
{
	CDTibMessage::operator =( msg );
	m_bidSize			= msg.m_bidSize;
	m_askSize			= msg.m_askSize;
	m_lastSize			= msg.m_lastSize;
	m_changedFields		= msg.m_changedFields;
	m_volume			= msg.m_volume;
	m_last				= msg.m_last;
	m_bid				= msg.m_bid;
	m_ask				= msg.m_ask;
	m_delta				= msg.m_delta;
	m_change			= msg.m_change;

	m_askEx				= msg.m_askEx;
	m_askEx.begin();
	m_bidEx				= msg.m_bidEx;
	m_bidEx.begin();
	m_lastEx			= msg.m_lastEx;	
	m_lastEx.begin();

	return (*this);
}

void CDTMOptionQuoteBest::setAskEx( const std::string & szAskEx )
{
	if( szAskEx.empty() )
		return;
	m_askEx = szAskEx;
	m_askEx.begin();
}

void CDTMOptionQuoteBest::setBidEx( const std::string & szBidEx )
{
	if( szBidEx.empty() )
		return;
	m_bidEx = szBidEx;
	m_bidEx.begin();
}

void CDTMOptionQuoteBest::setLastEx	( const std::string & szLastEx )
{
	if( szLastEx.empty() )
		return;
	m_lastEx = szLastEx;
	m_lastEx.begin();
}

// ====================================================================
// CDTMStockQuote
// ====================================================================

CDTMStockQuote::CDTMStockQuote()
{
	m_type = DATATIBMESSAGE_STOCKQUOTE;
	m_change = m_close = m_high = m_last = m_low = m_open = -1.0;
	m_volume = -1;
	m_szPrcTick = "";
	m_szDisplay = "";
}

CDTMStockQuote::~CDTMStockQuote()
{

}

CDTMStockQuote::CDTMStockQuote( const CDTMStockQuote & msg )
{
	( *this ) = msg;
}

CDTMStockQuote & CDTMStockQuote::operator=( const CDTMStockQuote & msg )
{
	CDTMOptionQuote::operator =( msg );
	m_open		= msg.m_open;
	m_high		= msg.m_high;
	m_low		= msg.m_low;
	m_close		= msg.m_close;
	m_szPrcTick = msg.m_szPrcTick;
	m_szPrcTick.begin();
	m_szDisplay = msg.m_szDisplay;
	m_szDisplay.begin();
	return (*this);
}

void CDTMStockQuote::updateCache()
{
	m_pDataManager->updateStockQuote( this );
}	

std::string CDTMStockQuote::CreateResponse( const std::string & szUnder )
{
	return CreateResponse();
}


std::string CDTMStockQuote::CreateResponse()
{
	std::string		exchange = "";
	std::string		reply = "";
	if( m_base.empty() || m_ric.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: StockQuote.");
		return "";
	}
	try
	{
		BOOL	bUpdate = false;
		// This is for the level 1 quote
		reply  = "<QS|" + m_base + "|" ;
		exchange = getExchange();
		if( exchange != "O" )
		{
			if( m_ric.find(".") != -1 )
				exchange = m_ric.substr(m_ric.find(".")+1,-1);
		}

		if (exchange.length() > 0)
		{
			reply += exchange ; 
		}
		else
		{
			reply += " ";
		}
		reply += "|";

		if (m_last > 0.0)
		{
			reply += CDateConverter::f2s(m_last);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_lastSize > 0)
		{
			reply += CDateConverter::l2s(m_lastSize);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if ( m_bid > 0.0 )
		{
			reply += CDateConverter::f2s(m_bid);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_bidSize > 0)
		{
			reply += CDateConverter::l2s(m_bidSize);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if ( m_ask > 0.0 )
		{
			reply += CDateConverter::f2s(m_ask);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_askSize > 0)
		{
			reply += CDateConverter::l2s(m_askSize);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if( m_changedFields & FIELD_QUOTE_OPEN )
		{
			reply += CDateConverter::f2s(m_open);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if( m_changedFields & FIELD_QUOTE_HIGH )
		{
			reply += CDateConverter::f2s(m_high);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_LOW)
		{
			reply += CDateConverter::f2s(m_low);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_CLOSE)
		{
			reply += CDateConverter::f2s(m_close);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_VOLUME)
		{
			reply += CDateConverter::l2s(m_volume);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_NET)
		{
			reply += CDateConverter::f2s(m_change);
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}

		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_PRCTICK)
		{
			reply += m_szPrcTick;
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}

		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_DISPLAY)
		{
			reply += m_szDisplay;
			bUpdate = TRUE;
		}
		else
		{
			reply += " ";
		}

		reply += ">" ;
		if (!bUpdate)
			reply = "";
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTMStockQuote createResponse exception "+m_base);
		reply = "";
	}

	return(reply);
}

// ====================================================================
// CDTMStockQuote
// ====================================================================
CDTMStock2Quote::CDTMStock2Quote()
{
	m_type = DATATIBMESSAGE_STOCK2QUOTE;
	m_bid = m_ask = -1.0f;
	m_bidSize = m_askSize = -1;
	m_changedFields = 0;
	m_mmname = "";
}

CDTMStock2Quote::~CDTMStock2Quote()
{

}

CDTMStock2Quote::CDTMStock2Quote( const CDTMStock2Quote & msg )
{
	( *this ) = msg;
}

CDTMStock2Quote & CDTMStock2Quote::operator=( const CDTMStock2Quote & msg )
{
	CDTibMessage::operator =( msg );
	m_changedFields = msg.m_changedFields;
	m_bid			= msg.m_bid;
	m_bidSize		= msg.m_bidSize;
	m_ask			= msg.m_ask;
	m_askSize		= msg.m_askSize;
	m_mmname		= msg.m_mmname;
	m_mmname.begin();
	return (*this);
}
void CDTMStock2Quote::setMM( const std::string & szMMName )
{
	if( szMMName.empty() )
		return;
	m_mmname = szMMName;
	m_mmname.begin();
}

void CDTMStock2Quote::updateCache()
{
	m_pDataManager->updateStock2Quote( this );
}	

std::string CDTMStock2Quote::CreateResponse( const std::string & szUnder )
{
	return CreateResponse();
}

std::string CDTMStock2Quote::CreateResponse()
{
	std::string		reply = "";
	if( m_mmname.empty() || m_base.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: mmname Stock2Quote.");
		return "";
	}
	try
	{
		BOOL	bUpdate = FALSE;
		
		reply = "<QS2|" + m_base + "|"; 
		if( !m_mmname.empty() )
			reply += m_mmname;
		else
			reply += " ";
		reply += "|";

		if ( m_bid >= 0.0 && m_bidSize >= 0)
		{
			reply += CDateConverter::f2s(m_bid);
			reply += "|" ;
			reply += CDateConverter::l2s(m_bidSize);
			bUpdate = TRUE;
		}
		else
		{
			reply += " | ";
		}
		reply += "|" ;

		if (m_ask >= 0.0 && m_askSize >= 0)
		{
			reply += CDateConverter::f2s(m_ask);
			reply += "|" ;
			reply += CDateConverter::l2s(m_askSize);
			bUpdate = TRUE;
		}
		else
		{
			reply += " | ";
		}
		reply += ">" ;

		if( bUpdate == FALSE )
			return "";

	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTMStock2Quote createResponse exception "+m_base);
		reply = "";
	}
	return reply;
}

void CDTOptionSeries::Process()
{
	// don't try/catch here
	std::string szRequest = GetClientRequest();
	if( szRequest.empty() )
		return;
	m_pRequestManager->sendToClient( szRequest, this );
	m_pRequestManager->clearClients( GetClientRequest() );
}

CDTOptionSeries::CDTOptionSeries( const std::string & szUnder )
{
	m_type = DATATIBMESSAGE_OPTIONSERIES;
	m_base = szUnder;
	m_base.begin();
	m_subject = "CMINI.REC."+m_base+".CHI";
}

CDTOptionSeries::CDTOptionSeries( const CDTOptionSeries & msg )
{
	*this = msg;
}

CDTOptionSeries & CDTOptionSeries::operator=( const CDTOptionSeries & msg )
{
	CDTibMessage::operator =( msg );
	m_OptionSeries = msg.m_OptionSeries;
	return *this;
}


void CDTOptionSeries::updateCache()
{
	m_pDataManager->updateOptionSeries( this );
}	

std::string CDTOptionSeries::CreateResponse()
{
	std::string	response = "";
	std::stringstream st;
	if( m_OptionSeries.m_ric.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: OptionSeries.");
		return "";
	}
	try
	{
		int								i = 0;
		char							buff[50];
		double							strike = 0.0;
		double							last = 0.0;
		std::string						exch = "";
		std::string						optionClass = "";
		std::string						expiration = "";
		std::string						exchanges = m_OptionSeries.m_optionAditionalExchanges;

		tFloatStrikeMap::iterator		fsmIt;
		tStringExpirationMap::iterator	semIt;
		CStrike							st;
		CExpiration						exp;

		response = "<OS|" + m_OptionSeries.m_ric + "|" + m_OptionSeries.m_optionExchange + "|" + m_OptionSeries.m_secType;
		bool bDelta = false; // m_pDataManager->hasDelta(m_OptionSeries.m_ric);
		if(bDelta)
		{
			response += "|1";
		}
		else
		{
			response += "|0";
		}

		last = m_pDataManager->getStockLast(m_OptionSeries.m_ric);
		if(last > 0.0)
		{
			response += "|";
			response += CDateConverter::f2s(last);
		}
		else
		{
			response += "| ";
		}

		std::string exAcr = m_pDataManager->getExchangeAcr(m_OptionSeries.m_optionExchange);
		response += "|<EX|" + m_OptionSeries.m_optionExchange + "|" + exAcr + ">";

		if( !exchanges.empty() )
		{
			i = exchanges.find('.') + 1;
			exchanges = exchanges.substr(i,-1);
			while (exchanges.size() > 0)
			{
				// add exchange to response
				i = exchanges.find('.');
				exch = exchanges.substr(0,i);
				if(i++>0)
				{
					exchanges = exchanges.substr(i,-1);
				}
				else
				{
					exchanges = "";
				}

				response += "|<EX" ;

				if(exch.length()>0)
				{
					response += "|" + exch;
				}
				else
				{
					response += "| ";
				}

				exch = m_pDataManager->getExchangeAcr(exch);
				if(exch.length()>0)
				{
					response += "|" + exch;
				}
				else
				{
					response += "| ";
				}

				response += ">";
			}
		}

		CDateConverter	dateConverter;
		for(semIt = m_OptionSeries.m_expirationMap.begin();
			semIt != m_OptionSeries.m_expirationMap.end();
			semIt++	)
		{
			expiration = dateConverter.convertDate(semIt->first);
			exp = semIt->second;

			if((expiration.length())>0 )
			{
				response += "|<XP|" + expiration;

				for(fsmIt = exp.m_strikeMap.begin();
					fsmIt != exp.m_strikeMap.end();
					fsmIt++	)
				{
					strike = atof(fsmIt->first.c_str());
					st = fsmIt->second;

					if( strike > 0.0 )
					{
						sprintf(buff,"|<ST|%.3f",strike);
						response += buff;

						try
						{
							optionClass = st.m_callRic.substr(0,st.m_callRic.length() - 2);
//							optionClass = st.m_callRic.substr(0,3);
						}
						catch(...)
						{
							optionClass = st.m_callRic;
						}

//						if (optionClass.length() > 0)
						if (m_OptionSeries.m_ric.compare(optionClass))
						{
							response += "|" + optionClass;
						}
						else
						{
							response += "| " ;
						}

						if(st.m_putRic.length()>0)
						{
							response += "|" + st.m_putRic ;
						}
						else
						{
							response += "| " ;
						}

						if(st.m_callRic.length()>0)
						{
							response += "|" + st.m_callRic ;
						}
						else
						{
							response += "| " ;
						}

						response += "|";
						if (st.m_putDelta >= 0)
						{
							response += CDateConverter::f2s(st.m_putDelta);
						}
						else
						{
							response += " ";
						}

						response += "|" ;
						if (st.m_callDelta >= 0)
						{
							response += CDateConverter::f2s(st.m_callDelta);
						}
						else
						{
							response += " ";
						}
						response += ">";
					}
				}

				response += ">";
			}
		}
		
		
		response += ">";
		CMain::getInstance()->m_log.write(LOG_DEBUG, "responseOptionSeries " + response);
	}
	catch (...)
	{
		// option series must have been deleted
		CMain::getInstance()->m_log.write(LOG_ERROR, "responseOptionSeries exception "+m_OptionSeries.m_ric);
		response = "";
	}
	return (response);
}

std::string CDTOptionSeries::CreateResponse( const std::string & szUnder )
{
	try
	{
		CConTib * pTib = CMain::getInstance()->getConnections()->getConTib();
		if( szUnder.find(".TO") != -1 )
			m_OptionSeries = pTib->getOptionSeries( m_base.substr(0,m_base.find(".TO")), "TO", 3000 );
		else
			m_OptionSeries = pTib->getOptionSeries( m_base );
		updateCache();
		return CreateResponse();
	}
	catch(...)
	{
		Sleep(100);
		throw new CMaskNotFoundException();
	}
}

// ====================================================================
// CDTMOptionQuote
// ====================================================================
CDTMOptionQuote::CDTMOptionQuote()
{
	m_type = DATATIBMESSAGE_OPTIONQUOTE;
	m_last = m_ask = m_bid = -1.0;
	m_volume = m_askSize = m_bidSize = m_lastSize = -1;
	m_symbol = "";
	m_changedFields = 0;
	m_exchangeStatus = "";
	m_delta = -1.0;		//	-1.0 means no delta
	m_change = -1.0;	// missing earlier
	m_exchange = "";	// missing earlier
}

CDTMOptionQuote::~CDTMOptionQuote()
{

}

CDTMOptionQuote::CDTMOptionQuote( const CDTMOptionQuote & msg )
{
	*this = msg;
}

CDTMOptionQuote & CDTMOptionQuote::operator=( const CDTMOptionQuote & msg )
{
	CDTibMessage::operator =( msg );
	m_changedFields	= msg.m_changedFields;
	m_volume		= msg.m_volume;
	m_last			= msg.m_last;
	m_bid			= msg.m_bid;
	m_ask			= msg.m_ask;
	m_delta			= msg.m_delta;
	m_change		= msg.m_change;
	m_bidSize		= msg.m_bidSize;
	m_askSize		= msg.m_askSize;
	m_lastSize		= msg.m_lastSize;

	m_exchangeStatus	= msg.m_exchangeStatus;
	m_exchangeStatus.begin();
	m_exchange			= msg.m_exchange;	
	m_exchange.begin();
	
	return *this;
}

void CDTMOptionQuote::setExStatus( const std::string & szStatus )
{
	if( szStatus.empty() )
		return;
	m_exchangeStatus = szStatus;
	m_exchangeStatus.begin();
}

void CDTMOptionQuote::setExchange( const std::string & szExchg )
{
	if( szExchg.empty() )
		return;
	m_exchange = szExchg;
	m_exchange.begin();
}


void CDTMOptionQuote::updateCache()
{
	m_pDataManager->updateOptionQuote( this );
}

std::string CDTMOptionQuote::CreateResponse(const std::string & szUnder)
{
	std::string		reply = "";
	if( m_symbol.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: OptionQuote.");
		return "";
	}
	try
	{
		reply = "<QO|" + szUnder + "|" ;

		reply += m_symbol;
		reply += "|";

		if(m_exchange.length() > 0)
		{
			reply += m_exchange; // m_pDataManager->getExchangeAcr(exchange);
		}
		else
		{
			reply += " ";
		}
		reply += "|";

		if (m_changedFields & FIELD_QUOTE_LAST)
		{
			reply += CDateConverter::f2s(m_last);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_LASTSIZE)
		{
			reply += CDateConverter::l2s(m_lastSize);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_BID)
		{
			reply += CDateConverter::f2s(m_bid);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_BIDSIZE)
		{
			reply += CDateConverter::l2s(m_bidSize);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_ASK)
		{
			reply += CDateConverter::f2s(m_ask);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_ASKSIZE)
		{
			reply += CDateConverter::l2s(m_askSize);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if(m_changedFields & FIELD_QUOTE_VOLUME)
		{
			reply += CDateConverter::l2s(m_volume);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_delta >= 0.0)
		{
			reply += CDateConverter::f2s(m_delta);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_exchangeStatus.length() > 0)
		{
			reply += m_exchangeStatus;
		}
		else
		{
			reply += " ";
		}

		reply += ">";

	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTMOptionQuote createResponse exception "+m_symbol);
		reply = "";
	}
	return reply;
}

void CDTMOptionQuoteBest::updateCache()
{
	m_pDataManager->updateOptionQuoteBest( this );
}

std::string CDTMOptionQuoteBest::CreateResponse(const std::string & szUnder)
{
	if( m_ric.empty() || szUnder.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: OptionQuoteBest.");
		return "";
	}
	try
	{
		std::string			reply = "";
		if( szUnder.empty() )
		{
			return "";
		}
		reply  = "<QOB|" + szUnder + "|" + m_ric + "|" ;

		if (m_lastEx.length() > 0)
			reply += m_lastEx;
		else
			reply += " ";
		reply += "|";

		if (m_last >= 0.0)
			reply += CDateConverter::f2s(m_last);
		else
			reply += " ";
		reply += "|" ;
		
		if (m_lastSize >= 0)
			reply += CDateConverter::l2s(m_lastSize);
		else
			reply += " ";
		reply += "|";

		if (m_bidEx.length() > 0)
			reply += m_bidEx;
		else
			reply += " ";
		reply += "|";

		if (m_bid >= 0.0)
			reply += CDateConverter::f2s(m_bid);
		else
			reply += " ";
		reply += "|" ;

		if (m_bidSize >= 0)
			reply += CDateConverter::l2s(m_bidSize);
		else
			reply += " ";
		reply += "|" ;

		if (m_askEx.length() > 0)
			reply += m_askEx;
		else
			reply += " ";
		reply += "|" ;

		if (m_ask >= 0.0)
			reply += CDateConverter::f2s(m_ask);
		else
			reply += " ";
		reply += "|" ;

		if (m_askSize >= 0)
			reply += CDateConverter::l2s(m_askSize);
		else
			reply += " ";
		reply += "|" ;

		if (m_volume >= 0)
			reply += CDateConverter::l2s(m_volume);
		else
			reply += " ";
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_NET)
		{
			reply += CDateConverter::f2s(m_change);
		}
		else
		{
			reply += " ";
		}

		reply += ">";

		return reply;
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTMOptionQuoteBest createResponse exception "+m_symbol);
		return "";
	}
}

CDTMGreeks::CDTMGreeks( const CDTMGreeks & msg )
{
	*this = msg;
}

CDTMGreeks & CDTMGreeks::operator=( const CDTMGreeks & msg )
{
	CDTibMessage::operator =( msg );
	m_expiration	= msg.m_expiration;
	m_expiration.begin();
	m_greekMap		= msg.m_greekMap;
	return *this;
}

void CDTMGreeks::setExpiration( const std::string & exp )
{
	m_expiration = exp;
	m_expiration.begin();
}

std::string	CDTMGreeks::CreateResponse(const std::string & szUnder)
{
	std::string		reply = "";
	if( m_base.empty() || m_expiration.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg:CDTMGreeks.");
		return "";
	}
	try
	{
		tFloatStrikeMap::iterator it;
		for( it = m_greekMap.begin(); it != m_greekMap.end(); it++ )
		{
			reply += "<GD|" + m_base + "|"; 
			reply += m_expiration;
			reply += "|";
			reply += (*it).first; // strike
			reply += "|";
			if( (*it).second.m_callDelta > 0 )
				reply += CDateConverter::f2s((*it).second.m_callDelta);
			else
				reply += "0.00";
			reply += "|";
			if( (*it).second.m_putDelta > 0 )
				reply += CDateConverter::f2s((*it).second.m_putDelta);
			else
				reply += "0.00";
			reply += ">\n" ;
		}
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTMStock2Quote createResponse exception "+m_base);
		reply = "";
	}
	return reply;
}

void CDTMGreeks::updateCache()
{
	m_pDataManager->updateGreeks( this );
}


///////////////////////////////////
void CDTFutureSeries::Process()
{
	// don't try/catch here
	std::string szRequest = GetClientRequest();
	if( szRequest.empty() )
		return;
	m_pRequestManager->sendToClient( szRequest, this );
	m_pRequestManager->clearClients( GetClientRequest() );
}

CDTFutureSeries::CDTFutureSeries( const std::string & szUnder )
{
	m_type = DATATIBMESSAGE_FUTURESERIES;
	m_base = szUnder;
	m_base.begin();
	m_subject = "CMINI.REC."+m_base+".CHI";
}

CDTFutureSeries::CDTFutureSeries( const CDTFutureSeries & msg )
{
	*this = msg;
}

CDTFutureSeries & CDTFutureSeries::operator=( const CDTFutureSeries & msg )
{
	CDTibMessage::operator =( msg );
	m_FutureSeries = msg.m_FutureSeries;
	return *this;
}


void CDTFutureSeries::updateCache()
{
	m_pDataManager->updateFutureSeries( this );
}	

std::string CDTFutureSeries::CreateResponse()
{
	std::string	reply = "";
	std::stringstream st;
	if( m_base.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: OptionSeries.");
		return "";
	}
	if( m_FutureSeries.size() == 0 )
		return reply;
	try
	{
		reply	= "<FS|";
		reply	+= m_base;
		reply	+= "|";

		CFutureSeries::iterator it;
		for( it = m_FutureSeries.begin(); it != m_FutureSeries.end(); it++ )
		{
			reply	+= "<";
			
			std::string szKey = (*it).first;
			std::string tic			= (*it).second.getTic();
			reply	+= tic;
			reply	+= "|";
			std::string base		= (*it).second.getBase();
			std::string expire		= (*it).second.getExpire();
			
			reply	+= expire;
			reply	+= "|";

			std::string symbol		= (*it).second.getSymbol();

			reply	+= symbol;
			reply	+= "|";

			std::string feed		= (*it).second.getFeed();

			std::string exchange	= (*it).second.getExchange();
			reply	+= exchange;
			reply	+= "|";

			float ticSize			= (*it).second.getTickSize();
			reply	+= CDateConverter::f2s(ticSize);
			reply	+= "|";

			float mult				= (*it).second.getMultipler();
			reply	+= CDateConverter::f2s(mult);
			reply	+= ">";
		}
		reply	+= ">";
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTFutureSeries createResponse exception "+m_base);
		reply = "";
	}
	return (reply);
}

std::string CDTFutureSeries::CreateResponse( const std::string & szUnder )
{
	try
	{
		CConTib * pTib = CMain::getInstance()->getConnections()->getConTib();
		m_FutureSeries = pTib->getFutureSeries( m_base );
		updateCache();
		return CreateResponse();
	}
	catch(...)
	{
		Sleep(100);
		throw new CMaskNotFoundException();
	}
}

CDTMFutureQuote::CDTMFutureQuote()
{
	m_type = DATATIBMESSAGE_FUTUREQUOTE;
	m_open			= -1.0f;
	m_high			= -1.0f;
	m_low			= -1.0f;
	m_close			= -1.0f;
	m_last = m_ask = m_bid = -1.0;
	m_volume = m_askSize = m_bidSize = m_lastSize = -1;
	m_symbol = "";
	m_changedFields = 0;
	m_change = -1.0;	// missing earlier
}

void CDTMFutureQuote::updateCache()
{
	m_pDataManager->updateFutureQuote( this );
}

std::string	CDTMFutureQuote::CreateResponse	(const std::string & szUnder)
{
	std::string		reply = "";
	if( m_symbol.empty() )
	{
		CMain::getInstance()->m_log.write(LOG_DEBUG, "CreateResponse Bad TibMsg: FutureQuote.");
		return "";
	}
	try
	{
		reply = "<QF|" + szUnder + "|" ;

		reply += m_symbol;
		reply += "|";

		if (m_changedFields & FIELD_QUOTE_LAST)
		{
			reply += CDateConverter::f2s(m_last,4);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_LASTSIZE)
		{
			reply += CDateConverter::l2s(m_lastSize);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_BID)
		{
			reply += CDateConverter::f2s(m_bid,4);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_BIDSIZE)
		{
			reply += CDateConverter::l2s(m_bidSize);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_ASK)
		{
			reply += CDateConverter::f2s(m_ask,4);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_ASKSIZE)
		{
			reply += CDateConverter::l2s(m_askSize);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if(m_changedFields & FIELD_QUOTE_VOLUME)
		{
			reply += CDateConverter::l2s(m_volume);
		}
		else
		{
			reply += " ";
		}
		reply += "|" ;

		if (m_changedFields & FIELD_QUOTE_NET)
		{
			reply += CDateConverter::f2s(m_change,4);
		}
		else
		{
			reply += " ";
		}

		reply += ">";

	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "CDTMFutureQuote createResponse exception "+m_symbol);
		reply = "";
	}
	return reply;
}

CDTMFutureQuote::CDTMFutureQuote( const CDTMFutureQuote & msg )
{
	*this = msg;
}

CDTMFutureQuote & CDTMFutureQuote::operator=( const CDTMFutureQuote & msg )
{
	CDTibMessage::operator =( msg );
	m_changedFields	= msg.m_changedFields;
	m_volume		= msg.m_volume;
	m_last			= msg.m_last;
	m_bid			= msg.m_bid;
	m_ask			= msg.m_ask;
	m_change		= msg.m_change;
	m_bidSize		= msg.m_bidSize;
	m_askSize		= msg.m_askSize;
	m_lastSize		= msg.m_lastSize;
	m_open			= msg.m_open;
	m_high			= msg.m_high;
	m_low			= msg.m_low;
	m_close			= msg.m_close;
	return *this;
}

CDTCMask::CDTCMask( const std::string & szUnder )
{
	m_base = szUnder;
	m_base.begin();
	m_subject = "CMINI.REC."+m_base+".CHI";
}

void CDTCMask::Process()
{
	try
	{
		if( !m_pDataManager->hasOptionSeries( m_base ) )
			return;
		CMain::getInstance()->m_log.write(LOG_WARNING, "cmask updated for "+m_base);
		CConTib * pTib = CMain::getInstance()->getConnections()->getConTib();
		CDTOptionSeries series( m_base );
		series.setSeries( pTib->getOptionSeries( m_base ) );
		m_pDataManager->updateOptionSeries( &series );
	}
	catch(...)
	{
		CMain::getInstance()->m_log.write(LOG_ERROR, "cmask updated exception for "+m_base);
		return;
	}
}


