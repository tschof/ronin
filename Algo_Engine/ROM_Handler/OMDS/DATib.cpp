// DATib.cpp: implementation of the CDATib class.
//
//////////////////////////////////////////////////////////////////////
//
//	Data Adapter Tib converts tib messages to
//	Tib Data message objects
//
#include "stdafx.h"
#include "DATib.h"
#include "DataManager.h"
#include "CMask.h"
using namespace WinRVTool;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int	CDATib::m_nMode = 0;
FILE *	CDATib::m_File = NULL;

CDATib::CDATib(CDataManager * pDM)
{
	m_pDataManager = pDM;
	CConfiguration config;
	m_nMode = config.getDWORD("tibLog");
	std::string szPath = config.getValue("logDirectory")+"\\tibmsg.log";
	m_File = fopen( szPath.c_str(), "a+" );
}

CDATib::~CDATib()
{
	if( m_File )
		fclose( m_File );
	m_File = NULL;
	m_nMode = 0;
}


int CDATib::GetTibMode()
{
	return m_nMode;
}

bool CDATib::LogTibMsg( const std::string & szSubject, TibMsg &tibMsg )
{
	if( !m_File || m_nMode != 1 )
		return false;
	TibErr err = tibMsg.Append("SUBJECT", (char *)szSubject.c_str() );
	tibMsg.PrintTib( m_File );
	return true;
}

bool CDATib::NextTibMsg( std::string & szSubject, TibMsg &tibMsg )
{
	if( !m_File || m_nMode != 2 )
		return false;
	if( tibMsg.Read( m_File ) != TIBMSG_OK )
		return false;
	char buff[128];
	char * ptr = tibMsg.PrintTib( buff, sizeof(buff) );
	TibField field;
	if( tibMsg.Get( "SUBJECT", field ) != TIBMSG_OK )
		return false;
	field.Convert( buff, 128 );
	szSubject = buff;
	return true;
}

CDTibMessage * CDATib::factoryOptionQuote( const std::string & subject, const RvDatum &data, void * pClosure )
{
	CDTMOptionQuote *	pOQ = NULL;

	try
	{
		if (&data == NULL || subject.length() == 0 || subject.find('.') <= 0)
			return pOQ;

		CDTMOptionQuote *	pOQCurrent = NULL;
		TibMsg				tibMsg;
		TibField			tf;
		Tib_i32				intTib;
		double				dblTib;
		char				buff[101];
		int					i,j;

		// unpack
		if (tibMsg.UnPack((char *)data.data()) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuote() unpack failure for " + subject);
			return pOQ;
		}

		if (tibMsg.Get("REC_STATUS", tf) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuote() no REC_STATUS for " + subject);
			return pOQ;
		}
		else
		{
			if (tf.Convert(intTib) != TIBMSG_OK)
			{
				m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuote() Convert REC_STATUS failure for " + subject);
				return pOQ;
			}

			if(intTib != TIBMSG_OK)
			{
				m_log.write(LOG_DEBUG,  "CDATib::factoryOptionQuote() bad tib data REC_STATUS for " + subject);
				return pOQ;
			}
		}

		pOQ = new CDTMOptionQuote();
		if( !pOQ )
		{
			m_log.write(LOG_ERROR,  "CDATib::factoryOptionQuote() new CDTMOptionQuote() for " + subject);
			return NULL;
		}
		pOQ->setSubject(subject);

		i = subject.find_last_of('.');
		if( i == -1 || i == subject.length()-1 )
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuote() bad subject: " + subject);
			delete pOQ;
			return NULL;
		}

		pOQ->setExchange( subject.substr( i+1, -1 ) );
		j = subject.substr(0,i).find_last_of('.') + 1;
		if( j == -1 || i <= j )
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuote() bad subject: " + subject);
			delete pOQ;
			return NULL;
		}
		pOQ->setSymbol( subject.substr(j,i-j) );
		pOQ->setRic( pOQ->getSymbol() );

		if (tibMsg.Get("ASK", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_ask = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_ASK;
			}
		}

		if (tibMsg.Get("ASKSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_askSize = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_ASKSIZE;
			}
		}

		if (tibMsg.Get("BID", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_bid = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_BID;
			}
		}

		if (tibMsg.Get("BIDSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_bidSize = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_BIDSIZE;
			}
		}

		if (tibMsg.Get("TRDPRC_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_last = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_LAST;
			}
		}

		if (tibMsg.Get("TRDVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_lastSize = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_LASTSIZE;
			}
		}

		if (tibMsg.Get("NETCHNG_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_change = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_NET;
			}
		}

		if (tibMsg.Get("ACVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pOQ->m_volume = dblTib;
				pOQ->m_changedFields |= FIELD_QUOTE_VOLUME;
			}
		}

		if (tibMsg.Get("wQuoteQualifier", tf) == TIBMSG_OK)
		{
			memset(buff, 0, sizeof(buff));
			if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
			{
				pOQ->setExStatus( buff );
			}
		}

		if (pOQ->m_changedFields == 0)
		{
			try
			{
				delete pOQ;
			}
			catch (...)
			{
				m_log.write(LOG_WARNING, "factoryStockQuote delete pOQ error for subject " + subject);
			}
			pOQ = NULL;
		}
	}
	catch (...)
	{
		m_log.write(LOG_WARNING, "factoryOptionQuote error for subject " + subject);
		try
		{
			if ( pOQ != NULL)
			{
				delete pOQ;
			}
		}
		catch (...)
		{
			m_log.write(LOG_WARNING, "factoryOptionQuote delete pOQ error for subject " + subject);
		}
		pOQ = NULL;
	}

	return (pOQ);
}

CDTibMessage * CDATib::factoryOptionQuoteBest( const std::string & subject, const RvDatum &data, void * pClosure )
{
	CDTMOptionQuoteBest *	pQOB = NULL;
	TibMsg					tibMsg;

	try
	{
		if (&data == NULL || subject.length() == 0 || subject.find('.') <= 0)
			return pQOB;

		CDTMOptionQuoteBest *	pOQCurrent = NULL;
		TibField				tf;
		Tib_i32					intTib;
		double					dblTib;
		char					buff[101];
		int						i, j;

		// unpack
		if (tibMsg.UnPack((char *)data.data()) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuoteBest() unpack error for " + subject);
			return pQOB;
		}

		if (tibMsg.Get("REC_STATUS", tf) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuoteBest() no REC_STATUS for " + subject);
			return pQOB;
		}
		else
		{
			if (tf.Convert(intTib) != TIBMSG_OK)
			{
				m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuoteBest() convert error for " + subject);
				return pQOB;
			}

			if(intTib != TIBMSG_OK)
			{
				m_log.write(LOG_DEBUG,  "CDATib::factoryOptionQuoteBest() bad tib data REC_STATUS for " + subject);
				return pQOB;
			}
		}

		pQOB = new CDTMOptionQuoteBest();
		if( !pQOB )
		{
			m_log.write(LOG_ERROR, "CDATib::factoryOptionQuoteBest() new CDTMOptionQuoteBest() for " + subject);
			return NULL;
		}
		pQOB->setSubject(subject);
		try
		{
			i = subject.find_last_of('.');
			if( i == -1 || i == subject.length()-1 )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryOptionQuoteBest() bad subject " + subject);
				delete pQOB;
				return NULL;
			}
			std::string szExch = subject.substr( i+1, -1 );
			if( szExch.empty() )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryOptionQuoteBest() bad subject " + subject);
				delete pQOB;
				return NULL;
			}
			j = subject.substr(0,i).find_last_of('.') + 1;
			if( j == -1 || i <= j )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryOptionQuoteBest() bad subject " + subject);
				delete pQOB;
				return NULL;
			}
			pQOB->setSymbol( subject.substr(j,i-j) );
			pQOB->setRic( pQOB->getSymbol() );
		}
		catch(...)
		{
			LogTibMsg( subject, tibMsg );
			m_log.write(LOG_ERROR, "CDATib::factoryOptionQuoteBest() parsing error for " + subject);
			return NULL;
		}
		
		if (tibMsg.Get("ASK", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_ask = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_ASK;
			}
		}

		if (tibMsg.Get("ASKSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_askSize = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_ASKSIZE;

				if (dblTib > 0 && tibMsg.Get("wAskPartId", tf) == TIBMSG_OK)
				{
					memset(buff, 0, sizeof(buff));
					if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
					{
						if (strlen(buff) > 0)
						{
							buff[1] = '\0';
							pQOB->setAskEx( buff );
						}
					}
				}

			}
		}

		if (tibMsg.Get("BID", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_bid = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_BID;
			}
		}

		if (tibMsg.Get("BIDSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_bidSize = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_BIDSIZE;

				if (dblTib > 0 && tibMsg.Get("wBidPartId", tf) == TIBMSG_OK)
				{
					memset(buff, 0, sizeof(buff));
					if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
					{
						if (strlen(buff) > 0)
						{
							buff[1] = '\0';
							pQOB->setBidEx( buff );
						}
					}
				}
			}
		}

		if (tibMsg.Get("TRDPRC_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_last = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_LAST;
			}
		}

		if (tibMsg.Get("TRDVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_lastSize = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_LASTSIZE;
			}
		}

		if (tibMsg.Get("NETCHNG_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_change = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_NET;
			}
		}

		if (tibMsg.Get("ACVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQOB->m_volume = dblTib;
				pQOB->m_changedFields |= FIELD_QUOTE_VOLUME;
			}
		}

		if (tibMsg.Get("RDN_EXCHID", tf) == TIBMSG_OK)
		{
			memset(buff, 0, sizeof(buff));
			if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
			{
				if (strlen(buff) > 0)
					pQOB->setLastEx( buff );
			}
		}

		if (pQOB->m_changedFields == 0)
		{
			try
			{
				delete pQOB;
			}
			catch (...)
			{
				m_log.write(LOG_WARNING, "CDATib::factoryOptionQuoteBest() delete pQOB error for subject " + subject);
			}
			pQOB = NULL;
		}
	}
	catch (...)
	{
		LogTibMsg( subject, tibMsg );
		m_log.write(LOG_WARNING, "factoryOptionQuoteBest() error for subject " + subject);
		try
		{
			if ( pQOB != NULL)
			{
				delete pQOB;
			}
		}
		catch (...)
		{
			m_log.write(LOG_WARNING, "factoryOptionQuoteBest() delete pQOB error for subject " + subject);
		}
		pQOB = NULL;
	}

	return (pQOB);
}

CDTibMessage * CDATib::factoryStockQuote( const std::string & subject, const RvDatum &data, void * pClosure )
{
	CDTMStockQuote *	pEQ = NULL;
	TibMsg				tibMsg;
	try
	{
		if (&data == NULL || subject.length() == 0 || subject.find(".") <= 0)
			return pEQ;

		TibField			tf;
		Tib_i32				intTib;
		double				dblTib;

		// unpack
		if (tibMsg.UnPack((char *)data.data()) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryStockQuote() unpack error for " + subject);
			return pEQ;
		}

		if (tibMsg.Get("REC_STATUS", tf) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryStockQuote() no REC_STATUS for " + subject);
			return pEQ;
		}
		else
		{
			if (tf.Convert(intTib) != TIBMSG_OK)
			{
				m_log.write(LOG_WARNING,  "CDATib::factoryStockQuote() convert error for " + subject);
				return pEQ;
			}

			if(intTib != TIBMSG_OK)
			{
				m_log.write(LOG_DEBUG,  "CDATib::factoryStockQuote() bad tib data REC_STATUS for " + subject);
				return pEQ;
			}
		}

		pEQ = new CDTMStockQuote();
		if( !pEQ )
		{
			m_log.write(LOG_ERROR,  "CDATib::factoryStockQuote() new CDTMStockQuote() for " + subject);
			return NULL;
		}
		pEQ->setSubject(subject);
		// The user symbol, message symbol and subject are different for
		// index symbols, but the same for stock symbols.
		// since we don't know what we have, parse the subject and try to
		// translate.
		try
		{
			int i = subject.find_last_of('.');
			if( i == -1 )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryStockQuote() bad subject " + subject);
				delete pEQ;
				return NULL;
			}
			int j = subject.substr(0,i).find_last_of('.') + 1;
			if( j == -1 || i <= j )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryStockQuote() bad subject " + subject);
				delete pEQ;
				return NULL;
			}
			pEQ->setSymbol( subject.substr(j,-1) );
			//pEQ->setBase( subject.substr(j,i-j) );
			pEQ->setRic( pEQ->getSymbol() );
		}
		catch(...)
		{
			m_log.write(LOG_ERROR, "CDATib::factoryStockQuote() error parsing subject " + subject);
		}
		if( pClosure )
		{
			try
			{
				CMask * pMask = (CMask *)pClosure;
				pEQ->setExchange( pMask->GetExchange() );
				pEQ->setBase( pMask->GetBase() );
				if( pMask->GetSecType() == "FT" )
				{
					if( pMask->GetSymbol() != pEQ->getRic() )
						pEQ->setRic( pMask->GetSymbol()+"."+pMask->GetExchange() );
					if( pMask->GetBase() != pEQ->getBase() )
						pEQ->setBase( pMask->GetBase() );
				}
			}
			catch(...)
			{
				m_log.write(LOG_ERROR, "CDATib::factoryStockQuote() CMask exception" + subject);
			}
		}
		if (tibMsg.Get("ASK", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_ask = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_ASK;
			}
		}

		if (tibMsg.Get("ASKSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_askSize = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_ASKSIZE;
			}
		}

		if (tibMsg.Get("BID", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_bid = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_BID;
			}
		}

		if (tibMsg.Get("BIDSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_bidSize = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_BIDSIZE;
			}
		}

		if (tibMsg.Get("TRDPRC_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_last = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_LAST;
			}
		}

		if (tibMsg.Get("TRDVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_lastSize = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_LASTSIZE;
			}
		}

		if (tibMsg.Get("NETCHNG_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_change = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_NET;
			}
		}

		if (tibMsg.Get("ACVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_volume = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_VOLUME;
			}
		}

		if (tibMsg.Get("OPEN_PRC", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_open = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_OPEN;
			}
		}

		if (tibMsg.Get("HST_CLOSE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_close = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_CLOSE;
			}
		}

		if (tibMsg.Get("HIGH_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_high = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_HIGH;
			}
		}

		if (tibMsg.Get("LOW_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_low = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_LOW;
			}
		}

		char	buff[64];
		if (tibMsg.Get("PRCTCK_1", tf) == TIBMSG_OK)
		{
			memset(buff, 0, sizeof(buff));
			if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
			{
				pEQ->m_szPrcTick = buff;
				pEQ->m_changedFields |= FIELD_QUOTE_PRCTICK;
			}
		}

		if (tibMsg.Get("DSPLY_NAME", tf) == TIBMSG_OK)
		{
			memset(buff, 0, sizeof(buff));
			if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
			{
				pEQ->m_szDisplay = buff;
				pEQ->m_changedFields |= FIELD_QUOTE_DISPLAY;
			}
		}

		if (pEQ->m_changedFields == 0)
		{
			try
			{
				delete pEQ;
			}
			catch (...)
			{
				m_log.write(LOG_WARNING, "factoryStockQuote delete pEQ error for subject " + subject);
			}
			pEQ = NULL;
		}
	}
	catch (...)
	{
		LogTibMsg( subject, tibMsg );
		m_log.write(LOG_WARNING, "factoryStockQuote error for subject " + subject);
		try
		{
			if ( pEQ != NULL)
			{
				delete pEQ;
			}
		}
		catch (...)
		{
			m_log.write(LOG_WARNING, "factoryStockQuote delete pEQ error for subject " + subject);
		}
		pEQ = NULL;
	}
	return (pEQ);
}

CDTibMessage * CDATib::factoryStock2Quote(const std::string & subject, const RvDatum &data, void * pClosure )
{
	CDTMStock2Quote *	pEQ = NULL;

	try
	{
		if (&data == NULL || subject.length() == 0 || subject.find(".") <= 0)
			return pEQ;

		TibMsg				tibMsg;
		TibField			tf;
		Tib_i32				intTib;
		double				dblTib;

		// unpack
		if (tibMsg.UnPack((char *)data.data()) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryStockQuote() unpack error for " + subject);
			return pEQ;
		}

		if (tibMsg.Get("REC_STATUS", tf) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryStockQuote() no REC_STATUS for " + subject);
			return pEQ;
		}
		else
		{
			if (tf.Convert(intTib) != TIBMSG_OK)
			{
				m_log.write(LOG_WARNING,  "CDATib::factoryStockQuote() convert error for " + subject);
				return pEQ;
			}

			if(intTib != TIBMSG_OK)
			{
				m_log.write(LOG_DEBUG,  "CDATib::factoryStockQuote() bad tib data REC_STATUS for " + subject);
				return pEQ;
			}
		}

		pEQ = new CDTMStock2Quote();
		if( !pEQ )
		{
			m_log.write(LOG_ERROR,  "CDATib::factoryStock2Quote() new CDTMStock2Quote() for " + subject);
			return NULL;
		}
		pEQ->setSubject(subject);
		// The user symbol, message symbol and subject are different for
		// index symbols, but the same for stock symbols.
		// since we don't know what we have, parse the subject and try to
		// translate.

		// RSF feed subject: RSF.REC.SKILGROW.O
		// Direct feed subject: STN.REC.SKIL.GROW
		if( subject.substr(0,subject.find('.')) == "RSF" )
		{
			
			int i = subject.find_last_of('.');
			if( i == -1 )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryStock2Quote() bad subject " + subject);
				delete pEQ;
				return NULL;
			}
			int j = subject.substr(0,i).find_last_of('.') + 1;
			if( j == -1 )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryStock2Quote() bad subject " + subject);
				delete pEQ;
				return NULL;
			}
			//pEQ->m_symbol = subject.substr(j,-1); ???
			pEQ->setSymbol( subject.substr(j,i-j) );
			pEQ->setRic( pEQ->getSymbol() );
			if( pEQ->getSymbol().length() < 5 )
			{
				m_log.write(LOG_ERROR, "CDATib::factoryStock2Quote() bad subject " + subject);
				delete pEQ;
				return NULL;
			}
			if (tibMsg.Get("MKT_MKR_ID", tf) != TIBMSG_OK)
			{
				pEQ->setMM( pEQ->getRic().substr( pEQ->getRic().length()-4, -1 ) ); 
			}
			else
			{
				pEQ->setMM( (char *)tf.Data() );
			}
			pEQ->setBase( pEQ->getSymbol().substr(0,pEQ->getSymbol().find(pEQ->getMM())));
		}
		else
		{
			
			int i = subject.find( "REC." );
			std::string symbol = subject.substr( i+4, -1 );
			pEQ->setSymbol( symbol );
			pEQ->setRic( symbol );
			pEQ->setMM(symbol.substr( symbol.find('.')+1, -1 ));
			pEQ->setBase(symbol.substr( 0, symbol.find('.')));
		}

		if (tibMsg.Get("ASK", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_ask = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_ASK;
			}
		}

		if (tibMsg.Get("ASKSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_askSize = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_ASKSIZE;
			}
		}

		if (tibMsg.Get("BID", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_bid = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_BID;
			}
		}

		if (tibMsg.Get("BIDSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pEQ->m_bidSize = dblTib;
				pEQ->m_changedFields |= FIELD_QUOTE_BIDSIZE;
			}
		}
	}
	catch(...)
	{
		delete pEQ;
		m_log.write(LOG_ERROR, "CDATib::factoryStock2Quote() exception"+subject);
	}
	return (pEQ);
}


CDTibMessage * CDATib::factoryGreeks( const std::string & subject, const RvDatum &data, void * pClosure)
{
	CDTMGreeks *		pG = NULL;
	TibMsg				tibMsg;
	try
	{
		if (&data == NULL || subject.length() == 0 || subject.find(".") <= 0)
			return pG;

		TibField			tf;
		Tib_i32				intTib;
		char				buff[101];
		int					i = 0;
		int					j = 0;
		int					k = 0;
		std::string			expiration;

		// unpack
		if (tibMsg.UnPack((char *)data.data()) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryGreeks() unpack error for subject " + subject);
			return pG;
		}
		if (tibMsg.Get("REC_STATUS", tf) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryGreeks() no REC_STATUS for subject " + subject);
			return pG;

		}
		else
		{
			if (tf.Convert(intTib) != TIBMSG_OK)
			{
				m_log.write(LOG_WARNING,  "CDATib::factoryGreeks() convert error for subject " + subject);
				return pG;
			}

			if(intTib != TIBMSG_OK)
			{
				m_log.write(LOG_DEBUG,  "CDATib::factoryGreeks() bad REC_STATUS for subject " + subject);
				return pG;
			}
		}

		pG = new CDTMGreeks();
		if( !pG )
		{
			m_log.write(LOG_ERROR,  "CDATib::factoryGreeks() new CDTMGreeks() for " + subject);
			return NULL;
		}
		pG->setSubject(subject);

		if(tibMsg.Get("BASE", tf) == TIBMSG_OK)
		{
			memset(buff, 0, sizeof(buff));
			if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
			{
				pG->setRic( buff ); 
				pG->setBase( buff );
			}
		}

		if(tibMsg.Get("EXPDT", tf) == TIBMSG_OK)
		{
			memset(buff, 0, sizeof(buff));
			if (tf.Convert(buff, sizeof(buff)) == TIBMSG_OK)
			{
				expiration = buff;
				pG->setExpiration( expiration.substr(2,2) +
									expiration.substr(5,2) +
									expiration.substr(8,2) );
			}
		}

		if(tibMsg.Get("NK", tf) == TIBMSG_OK)
		{
			if (tf.Convert(intTib) == TIBMSG_OK)
				i = intTib;
		}

		if(tibMsg.Get("NSET", tf) == TIBMSG_OK)
		{
			if (tf.Convert(intTib) == TIBMSG_OK)
				j = intTib;
		}

		char base[50];
		char fld[100];

		if(j > 0)
		{
			sprintf(base, "SET_%d", j);

			TibField			tfExp;
			TibField			tfPut;
			TibField			tfCall;
			Tib_f32 *			fStrike;
			Tib_f32 *			fPut;
			Tib_f32 *			fCall;

			// expirations
			sprintf(fld, "%s.KS", base);
			if (tibMsg.Get(fld, tfExp) == TIBMSG_OK)
			{
				fStrike = (Tib_f32 *)tfExp.Data();
				// put deltas
				sprintf(fld,"%s.DELTAP", base);
				if (tibMsg.Get(fld, tfPut) == TIBMSG_OK)
				{
					fPut = (Tib_f32 *) tfPut.Data();

					// call deltas
					sprintf(fld, "%s.DELTAC", base);
					if (tibMsg.Get(fld, tfCall) == TIBMSG_OK)
					{
						fCall = (Tib_f32 *)tfCall.Data();

						for(i--; i >= 0; i--)
						{
							CStrike strike;
							std::string szStrike = CDateConverter::f2s(fStrike[i]);
							strike.m_putDelta = -1.0 * fPut[i];	// puts always have negative deltas. we need only the value.
							strike.m_callDelta = fCall[i];
							pG->m_greekMap.insert( std::make_pair<std::string,CStrike>(szStrike,strike) );
						}
					}
				}
			}
		}
	}
	catch(...)
	{
		LogTibMsg( subject, tibMsg );
		m_log.write(LOG_WARNING, "factoryGreeks() error for subject " + subject);
		try
		{
			if ( pG != NULL)
			{
				delete pG;
			}
		}
		catch (...)
		{
			m_log.write(LOG_WARNING, "factoryGreeks delete pG error for subject " + subject);
		}
		pG = NULL;
	}
	return (pG);
}

CDTibMessage * CDATib::factoryFutureQuote( const std::string & subject, const RvDatum &data, void * pClosure)
{
	CDTMFutureQuote *	pQF = NULL;
	try
	{
		if (&data == NULL || subject.length() == 0 || subject.find('.') <= 0)
			return pQF;

		CDTMFutureQuote *	pQFCurrent = NULL;
		TibMsg				tibMsg;
		TibField			tf;
		Tib_i32				intTib;
		double				dblTib;
		int					i,j;

		// unpack
		if (tibMsg.UnPack((char *)data.data()) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryOptionQuote() unpack failure for " + subject);
			return pQF;
		}

		if (tibMsg.Get("REC_STATUS", tf) != TIBMSG_OK)
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryFutureQuote() no REC_STATUS for " + subject);
			return pQF;
		}
		else
		{
			if (tf.Convert(intTib) != TIBMSG_OK)
			{
				m_log.write(LOG_WARNING,  "CDATib::factoryFutureQuote() Convert REC_STATUS failure for " + subject);
				return pQF;
			}

			if(intTib != TIBMSG_OK)
			{
				m_log.write(LOG_DEBUG,  "CDATib::factoryFutureQuote() bad tib data REC_STATUS for " + subject);
				return pQF;
			}
		}

		pQF = new CDTMFutureQuote();
		if( !pQF )
		{
			m_log.write(LOG_ERROR,  "CDATib::factoryOptionQuote() new CDTMFutureQuote() for " + subject);
			return NULL;
		}
		pQF->setSubject(subject);

		i = subject.find_last_of('.');
		if( i == -1 || i == subject.length()-1 )
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryFutureQuote() bad subject: " + subject);
			delete pQF;
			return NULL;
		}

		j = subject.substr(0,i).find_last_of('.') + 1;
		if( j == -1 || i <= j )
		{
			m_log.write(LOG_WARNING,  "CDATib::factoryFutureQuote() bad subject: " + subject);
			delete pQF;
			return NULL;
		}
		pQF->setSymbol( subject.substr(j,i-j) );
		pQF->setRic( pQF->getSymbol() );

		if (tibMsg.Get("ASK", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_ask = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_ASK;
			}
		}

		if (tibMsg.Get("ASKSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_askSize = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_ASKSIZE;
			}
		}

		if (tibMsg.Get("BID", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_bid = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_BID;
			}
		}

		if (tibMsg.Get("BIDSIZE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_bidSize = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_BIDSIZE;
			}
		}

		if (tibMsg.Get("TRDPRC_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_last = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_LAST;
			}
		}

		if (tibMsg.Get("TRDVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_lastSize = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_LASTSIZE;
			}
		}

		if (tibMsg.Get("NETCHNG_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_change = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_NET;
			}
		}

		if (tibMsg.Get("OPEN_PRC", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_open = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_OPEN;
			}
		}

		if (tibMsg.Get("HST_CLOSE", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_close = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_CLOSE;
			}
		}

		if (tibMsg.Get("HIGH_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_high = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_HIGH;
			}
		}

		if (tibMsg.Get("LOW_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_low = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_LOW;
			}
		}

		if (tibMsg.Get("ACVOL_1", tf) == TIBMSG_OK)
		{
			if (tf.Convert(dblTib) == TIBMSG_OK)
			{
				pQF->m_volume = dblTib;
				pQF->m_changedFields |= FIELD_QUOTE_VOLUME;
			}
		}

		if (pQF->m_changedFields == 0)
		{
			try
			{
				delete pQF;
			}
			catch (...)
			{
				m_log.write(LOG_WARNING, "factoryFutureQuote delete pQF error for subject " + subject);
			}
			pQF = NULL;
		}
	}
	catch (...)
	{
		m_log.write(LOG_WARNING, "factoryFutureQuote error for subject " + subject);
		try
		{
			if ( pQF != NULL)
			{
				delete pQF;
			}
		}
		catch (...)
		{
			m_log.write(LOG_WARNING, "factoryFutureQuote delete pQF error for subject " + subject);
		}
		pQF = NULL;
	}

	return pQF;
}