// Act.cpp: implementation of the CAct class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Act.h"
#include "Price.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CActTrailer::m_seqNumber;

int CAct::m_ActInfo[][2] = // static array to track ACT formatting information
{{ACT_FUNCTION_POS, ACT_FUNCTION_LENGTH},
{ACT_ASOF_POS, ACT_ASOF_LENGTH},
{ACT_SECURITYCLASS_POS, ACT_SECURITYCLASS_LENGTH},
{ACT_BSX_POS, ACT_BSX_LENGTH},
{ACT_REFERENCENUMBER_POS, ACT_REFERENCENUMBER_LENGTH},
{ACT_VOLUME_POS, ACT_VOLUME_LENGTH},
{ACT_SYMBOL_POS, ACT_SYMBOL_LENGTH},
{ACT_PRICEDOLLAR_POS, ACT_PRICEDOLLAR_LENGTH},
{ACT_PRICENUMERATOR_POS, ACT_PRICENUMERATOR_LENGTH},
{ACT_PRICETRADEDIGIT_POS, ACT_PRICETRADEDIGIT_LENGTH},
{ACT_TRADEMODIFIER_POS, ACT_TRADEMODIFIER_LENGTH},
{ACT_PRICEOVERIDE_POS, ACT_PRICEOVERIDE_LENGTH},
{ACT_OEID_POS, ACT_OEID_LENGTH},
{ACT_OEGU_POS, ACT_OEGU_LENGTH},
{ACT_OECLEARNUMBER_POS, ACT_OECLEARNUMBER_LENGTH},
{ACT_MMID_POS, ACT_MMID_LENGTH},
{ACT_MMGU_POS, ACT_MMGU_LENGTH},
{ACT_MMCLEARNUMBER_POS, ACT_MMCLEARNUMBER_LENGTH},
{ACT_MMPAINDICATOR_POS, ACT_MMPAINDICATOR_LENGTH},
{ACT_TRADEREPORTFLAG_POS, ACT_TRADEREPORTFLAG_LENGTH},
{ACT_CLEARINGFLAG_POS, ACT_CLEARINGFLAG_LENGTH},
{ACT_SPECIALTRADEINDICATOR_POS, ACT_SPECIALTRADEINDICATOR_LENGTH},
{ACT_EXECUTIONTIME_POS, ACT_EXECUTIONTIME_LENGTH},
{ACT_MEMO_POS, ACT_MEMO_LENGTH},
{ACT_TRADEDECIMALPRICE_POS, ACT_TRADEDECIMALPRICE_LENGTH},
{ACT_CONTRABRANCHSEQ_POS, ACT_CONTRABRANCHSEQ_LENGTH},
{ACT_TRADEDATE_POS, ACT_TRADEDATE_LENGTH},
{ACT_REVERSALIND_POS, ACT_REVERSALIND_LENGTH},
{ACT_FILLER_POS, ACT_FILLER_LENGTH}};

// CActHeader - Create default ACT header for stafford
CActHeader::CActHeader() : m_Header("STAF\r\n")
{
	m_Header += "\r\n";
	m_Header += "OTHER";
	m_Header += " ";
	m_Header += "ACT";
	m_Header += "\r\n\n";
}

const CString CActHeader::Header()
{
	return m_Header;
}


// increments order number when a new trailer is created
CActTrailer::CActTrailer() : m_Trailer('0', ACT_TRAILER_LENGTH)
{
	//m_SequenceSet.InitMep();
	//m_seqNumber = m_SequenceSet.GetSequence();
}

int CActTrailer::Increment()
{
	char strSeqNumber[8];
	m_seqNumber = ++m_seqNumber%(int)(pow(10, ACT_TRAILER_LENGTH));
	_itoa(m_seqNumber, strSeqNumber, 10);
	m_strSeqNumber = strSeqNumber;

	WritePrivateProfileString("Session Information", "Sequence", strSeqNumber, "c:\\OrderManager\\Act.dat");
	
	return m_seqNumber;
}

int CActTrailer::Decrement()
{
	char strSeqNumber[8];
	m_seqNumber = --m_seqNumber%(int)(pow(10, ACT_TRAILER_LENGTH));
	_itoa(m_seqNumber, strSeqNumber, 10);
	m_strSeqNumber = strSeqNumber;

	WritePrivateProfileString("Session Information", "Sequence", strSeqNumber, "c:\\OrderManager\\Act.dat");

	return m_seqNumber;
}

const CString CActTrailer::Trailer()
{
	return ("-" + m_strSeqNumber);
}

// nothing initiates, all values are invalid
CAct::CAct() : m_Message(' ', ACT_MSG_LENGTH)
{
	m_Message += "\r\n\n";
	for(int i = 0; i < ACT_NUMPARAMS; i++)
		m_ActValid[i] = FALSE;

	CTime ET_time = CTime::GetCurrentTime();
	CString ET_String = ET_time.Format("%H%M%S");
	SetAt(ACT_EXECUTIONTIME, ET_String);
}

// conversion constructer for CCsv class
CAct::CAct(CCsv *CSVSource) : m_Message(' ', ACT_MSG_LENGTH)
{
	m_Message += "\r\n\n";
	m_Source = CSVSource;
	Convert(m_Source);

	CTime ET_time = CTime::GetCurrentTime();
	CString ET_String = ET_time.Format("%H%M%S");
	SetAt(ACT_EXECUTIONTIME, ET_String);
}

CAct::CAct(CString &STRSource) : m_Message(' ', ACT_MSG_LENGTH)
{
	m_Message += "\r\n\n";
	Convert(STRSource);

	CTime ET_time = CTime::GetCurrentTime();
	CString ET_String = ET_time.Format("%H%M%S");
	SetAt(ACT_EXECUTIONTIME, ET_String);
}

CAct::~CAct()
{

}

void CAct::SetAt(int Field, CString Value)	
{ 
	m_Message.GetLength();
	CString strValue = Validate(Field, Value);
	for(int i = 0; i < strValue.GetLength(); i++)
	{
		if(strValue[i] == '\n')
			break;
		m_Message.SetAt((m_ActInfo[Field][ACT_POSITION] + i - 1), strValue[i]);
	}
}

const CString CAct::GetAt(int Field)
{
	CString r_Value(' ',m_ActInfo[Field][ACT_LENGTH]);
	for(int i = 0; i < r_Value.GetLength(); i++)
	{
		r_Value.SetAt(i, m_Message[m_ActInfo[Field][ACT_POSITION] + i - 1]);
	}
	return r_Value;
}

// conversion function for CCsv class
void CAct::Convert(CCsv *CSVSource)
{
	CString* strPass;

	SetAt(ACT_FUNCTION, CSVSource->Get(CSV_ACT_TYPE));
	SetAt(ACT_ASOF, 
		*(strPass = new CString(' ', m_ActInfo[ACT_ASOF][ACT_LENGTH])));
		delete strPass;
	SetAt(ACT_SECURITYCLASS, "N");
	if(CSVSource->Get(CSV_ACT_SIDE) == "B")
		SetAt(ACT_BSX, 'B');
	else
		SetAt(ACT_BSX, 'S');
	SetAt(ACT_REFERENCENUMBER, CSVSource->Get(CSV_ACT_TAG));
	SetAt(ACT_VOLUME, CSVSource->Get(CSV_ACT_SHARES));
	SetAt(ACT_SYMBOL, CSVSource->Get(CSV_ACT_SYMBOL));
	
	CPrice price((CString&)CSVSource->Get(CSV_ACT_PRICE));
	//SetAt(ACT_PRICEDOLLAR, price.GetDollar());
	//SetAt(ACT_PRICENUMERATOR, price.GetNumerator());
	//SetAt(ACT_PRICETRADEDIGIT, price.GetDenominator());
	//SetAt(ACT_PRICETRADEDIGIT, "B");	
	SetAt(ACT_PRICETRADEDIGIT, "A");

	SetAt(ACT_TRADEMODIFIER, 
		*(strPass = new CString(' ', m_ActInfo[ACT_TRADEMODIFIER][ACT_LENGTH])));	
		delete strPass;
	SetAt(ACT_PRICEOVERIDE, "O");
	SetAt(ACT_OEID, CSVSource->Get(CSV_ACT_OEID));
	SetAt(ACT_OEGU, 
		*(strPass = new CString(' ', m_ActInfo[ACT_OEGU][ACT_LENGTH])));	
		delete strPass;
	SetAt(ACT_OECLEARNUMBER, CSVSource->Get(CSV_ACT_OECLEAR));
	SetAt(ACT_MMID, CSVSource->Get(CSV_ACT_MMID));
	SetAt(ACT_MMGU, 
		*(strPass = new CString(' ', m_ActInfo[ACT_MMGU][ACT_LENGTH])));	
		delete strPass;
	SetAt(ACT_MMCLEARNUMBER, CSVSource->Get(CSV_ACT_MMCLEAR));
	SetAt(ACT_MMPAINDICATOR, CSVSource->Get(CSV_ACT_CAPACITY));
	SetAt(ACT_TRADEREPORTFLAG, CSVSource->Get(CSV_ACT_REPORT));
	SetAt(ACT_CLEARINGFLAG, CSVSource->Get(CSV_ACT_CLEAR));
	SetAt(ACT_SPECIALTRADEINDICATOR, 
		*(strPass = new CString(' ', m_ActInfo[ACT_SPECIALTRADEINDICATOR][ACT_LENGTH])));
		delete strPass;
	SetAt(ACT_EXECUTIONTIME, CSVSource->Get(CSV_ACT_EXEC_TIME));
	SetAt(ACT_MEMO, 
		*(strPass = new CString(' ', m_ActInfo[ACT_MEMO][ACT_LENGTH])));
		delete strPass;

	strPass = new CString('0', m_ActInfo[ACT_TRADEDECIMALPRICE][ACT_LENGTH]);
	FormatActPrice(*strPass, CSVSource->Get(CSV_ACT_PRICE));
	SetAt(ACT_TRADEDECIMALPRICE, *strPass);
	delete strPass;

	//	*(strPass = new CString(' ', m_ActInfo[ACT_TRADEDECIMALPRICE][ACT_LENGTH])));
	//	delete strPass;

	CString strExecTime = CSVSource->Get(CSV_ACT_EXEC_TIME);
	int nHour = atol(strExecTime.Left(2));
	int nMinute = atol(strExecTime.Mid(2,2));
	int nSecond = atol(strExecTime.Right(2));

	if((nHour < 9) ||
		(nHour == 9 && nMinute < 30) ||
		//(nHour == 16 && nMinute == 1 && nSecond > 30) ||
		//(nHour == 16 && nMinute > 1) ||
		(nHour >= 16))
	{
		SetAt(ACT_TRADEMODIFIER, "T  ");
	}
}


void CAct::FormatActPrice(CString &Format, CString Price)
{
	CString s = Price;
	int i = Price.Find('.');

	if (i != -1)
	{
		int middle = ACT_TRADEDECIMALPRICE_LENGTH / 2 - 1;
		int start = middle - i + 1;
		int y = 0;

		for (int j = start; j < middle + 3; j++)
		{
			if (Price[y] == '.')
			{
				y++;
			}
			Format.SetAt(j, Price[y++]);
		}
	}
}

void CAct::Convert(CString &STRSource)
{
	m_Type = ValidAct(STRSource);
	int pos;

	switch(m_Type)
	{
	case ACT_T_TCEN:
		pos = STRSource.Find("TCEN");
		m_ControlNumber = STRSource.Mid(pos + 3 + 3, 10);
		m_Message = STRSource.Mid(pos + 3 + 3 + 10, ACT_MSG_LENGTH);
		break;
	case ACT_T_TTEN:
		pos = STRSource.Find("TTEN");
		m_ControlNumber = STRSource.Mid(pos + 3 + 3, 10);
		m_Message = STRSource.Mid(pos + 3 + 3 + 10, ACT_MSG_LENGTH);
		break;
	case ACT_T_TCAL:
		pos = STRSource.Find("TCAL");
		m_ControlNumber = STRSource.Mid(pos + 3 + 3, 10);
		m_Message = STRSource.Mid(pos + 3 + 3 + 10, ACT_MSG_LENGTH);
		break;
	case ACT_T_TTAL:
		pos = STRSource.Find("TTAL");
		m_ControlNumber = STRSource.Mid(pos + 3 + 3, 10);
		m_Message = STRSource.Mid(pos + 3 + 3 + 10, ACT_MSG_LENGTH);
		break;
	case ACT_T_TCLK:
		pos = STRSource.Find("TCLK");
		SetAt(ACT_REFERENCENUMBER, STRSource.Mid(pos + 3 + 3, 6));
		break;
	case ACT_T_REJ:
		pos = STRSource.Find("REJ");
		STRSource = STRSource.Mid(pos + 3 + 3);
		pos = STRSource.Find("\r");
		m_RejectReason = STRSource.Left(pos);
		STRSource = STRSource.Mid(pos + 3);
		pos = STRSource.Find("\r");
		STRSource = STRSource.Mid(pos + 2);
		pos = STRSource.Find("\r");
		STRSource = STRSource.Mid(pos + 4);
		m_Message = STRSource.Left(ACT_MSG_LENGTH);

	default:
		break;
	};	
}

// converts from ACT to CCsv
CCsv* CAct::Convert()
{
	CCsv *CSVDestination = new CCsv();
	CSVDestination->SetAt(CSV_ACT_COMMAND, GetAt(ACT_FUNCTION));
	CSVDestination->SetAt(CSV_ACT_SIDE, GetAt(ACT_BSX));
	CSVDestination->SetAt(CSV_ACT_SHARES, GetAt(ACT_VOLUME));
	CSVDestination->SetAt(CSV_ACT_SYMBOL, GetAt(ACT_SYMBOL));
	if (GetAt(ACT_FUNCTION) == "M")
	{
		CSVDestination->SetAt(CSV_ACT_MMCLEAR, GetAt(ACT_MMCLEARNUMBER));
		CSVDestination->SetAt(CSV_ACT_CAPACITY, GetAt(ACT_MMPAINDICATOR));
	}
	else
		CSVDestination->SetAt(CSV_ACT_OECLEAR, GetAt(ACT_OECLEARNUMBER));
	CSVDestination->SetAt(CSV_ACT_REPORT, GetAt(ACT_TRADEREPORTFLAG));
	CSVDestination->SetAt(CSV_ACT_TIME, GetAt(ACT_EXECUTIONTIME));
	CSVDestination->SetAt(CSV_ACT_PRICE, GetAt(ACT_TRADEDECIMALPRICE));
	return CSVDestination;
}

void CAct::Reference(CCsv* CSVDestination)
{
	CSVDestination->SetAt(CSV_ACT_TAG, GetAt(ACT_REFERENCENUMBER));
}	

// bare validation routines for parameters
CString CAct::Validate(int Field, CString &Value)
{
	short FieldLength = m_ActInfo[Field][ACT_LENGTH];
	CString r_Value(' ', m_ActInfo[Field][ACT_LENGTH]);
	//m_ActValid[Field] = TRUE;
	CString *strPass;
	int decPlace;
	CString lead, trail;

	switch (Field)
	{
	case ACT_FUNCTION:
		if ((Value == "MM") || (Value == "M") || (Value == "U"))
			Value = "V";
		if ((Value == "OE") || (Value == "O"))
			Value = "W";
		//if ((Value != "U") && (Value != "O"))
		//	m_ActValid[Field] = FALSE;
		break;
	case ACT_SYMBOL:
		//if ((Value.GetLength() < 4) || (Value.GetLength() > FieldLength))
		//	m_ActValid[Field] = FALSE;
		if (Value.GetLength() == 4)
			Value += " ";
		break;
	case ACT_VOLUME:
	case ACT_PRICEDOLLAR:
	case ACT_REFERENCENUMBER:
		if (Value.GetLength() > FieldLength)
		{
			Value = *(strPass = new CString('9', FieldLength));
			delete strPass;
		}
		if (Value.GetLength() < FieldLength)
		{
			Value = *(strPass = new CString('0', FieldLength - Value.GetLength())) + Value;
			delete strPass;
		}
		break;
	case ACT_PRICENUMERATOR:
		if (Value.GetLength() > FieldLength)
		{
			Value = *(strPass = new CString("256"));
			delete strPass;
		}
		if (Value.GetLength() < FieldLength)
		{
			Value = *(strPass = new CString('0', FieldLength - Value.GetLength())) + Value;
			delete strPass;
		}
	case ACT_BSX:
		if (Value == "Buy")
			Value = "B";
		if (Value == "Sell")
			Value = "S";
		if (Value == "1")
			Value = "B";
		if (Value == "2" || Value == "5")
			Value = "S";
		//if ((Value != "B") && (Value != "S"))
		//	m_ActValid[Field] = FALSE;
	case ACT_TRADEDECIMALPRICE:
		/*lead = Value.SpanExcluding(".");  // value before decimal point
		if(lead == Value)
			return r_Value = Value;
		trail = Value;
		trail.MakeReverse();
		trail = trail.SpanExcluding(".");
		trail.MakeReverse(); // value after decimal point
		decPlace = Value.Find('.'); // position of decimal point

		if(decPlace == -1)
			decPlace = (int)FieldLength / 2;
		// pad with 0's
		if (lead.GetLength() < (int)FieldLength / 2)
			lead = CString('0', ((int)FieldLength / 2) - lead.GetLength()) + lead;
		if (trail.GetLength() < (int)FieldLength / 2)
			trail += CString('0', ((int)FieldLength / 2) - trail.GetLength());
		Value = lead + trail;*/
		break;
	case ACT_PRICETRADEDIGIT:
		/*if((Value == "2") || (Value == "002"))
			Value = "1";
		else if((Value == "4") || (Value == "004"))
			Value = "2";
		else if((Value == "8") || (Value == "008"))
			Value = "3";
		else if((Value == "16") || (Value == "016"))
			Value = "4";
		else if((Value == "32") || (Value == "002"))
			Value = "5";
		else if((Value == "64") || (Value == "002"))
			Value = "6";
		else if(Value == "128")
			Value = "7";
		else if(Value == "256")
			Value = "8";
		else if(Value == "100")
			Value = "9";
		else
			Value = "1";*/
		Value = "A";

	case ACT_MMCLEARNUMBER:
	case ACT_OECLEARNUMBER:
	case ACT_MMID:
	case ACT_OEID:
	default:
		break;
		//if (Value.GetLength() != FieldLength)
		//	m_ActValid[Field] = FALSE;
	};
	//if (m_ActValid[Field] == TRUE)
		r_Value = Value;
	
	return r_Value;
}

// stamps message as late if 90 seconds after transaction
// and returns the main ACT message body
const CString CAct::Body()
{
	long ET_seconds = strtol(GetAt(ACT_EXECUTIONTIME).Right(2), NULL, 10);
	ET_seconds += strtol(GetAt(ACT_EXECUTIONTIME).Mid(2, 2), NULL, 10) * 60;
	ET_seconds += strtol(GetAt(ACT_EXECUTIONTIME).Left(2), NULL, 10) * 3600;
		
	CTime CT_time = CTime::GetCurrentTime();
	long CT_seconds = CT_time.GetSecond();
	CT_seconds += CT_time.GetMinute() * 60;
	CT_seconds += CT_time.GetHour() * 3600;

	//if((CT_seconds - ET_seconds) > 90)
	//	SetAt(ACT_TRADEMODIFIER, "SLD");

	return m_Message;
}

const CString CAct::Message()
{
	return (m_Header.Header() + Body() + m_Trailer.Trailer());
}

const CString CAct::Send()
{
	//Increment();
	return Message();
}

// writes out entire ACT message, including header and trailer
//BOOL CAct::Send(LPCSTR pFileName)
//{ 
//	CFileException e;
//	if( m_Output.Open( pFileName, CFile::modeCreate | CFile::modeWrite, &e ) )
//	{
//		CString header = m_Header.Header();
//		CString trailer = m_Trailer.Trailer();
//
//		m_Output.Write( header, header.GetLength() );
//		m_Output.Write( m_Message, m_Message.GetLength() );
//		m_Output.Write( trailer, trailer.GetLength() );
//		m_Output.Close();
//		return TRUE;
//	}
//	return FALSE;
//}