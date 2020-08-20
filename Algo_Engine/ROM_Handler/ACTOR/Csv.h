#if !defined(CCsv_H)
#define CCsv_H


// common to all messages
#define CSV_COMMAND				0
#define CSV_TIME					1
#define CSV_TRADER				2
#define CSV_TAG					3		// client assigned tag to id order
#define CSV_SIDE					4
#define CSV_SYMBOL				5
#define CSV_SHARES				6
#define CSV_PRICE					7
#define CSV_TYPE					8
#define CSV_TIF					9
#define CSV_CAPACITY				10		// principal or agent
#define CSV_CLEARINGID			11		// 4 digit nscc clearing code
#define CSV_CLEARINGACCOUNT	12		// account number a above clearing firm
#define CSV_EXCHANGE				13		// destination
#define CSV_STATUS				14		// current status

// just order entries
#define CSV_INSTRUCTIONS		15		// primarily used by instinet
#define CSV_PRICECHECK			16		// tells exchange whether or not to do price check
#define CSV_FLOOR					17		// display size
#define CSV_MINQTY				18		// min exec size
#define CSV_STOPPRICE			19		
#define CSV_DISCRETIONARY		20
#define CSV_SOLICITED			21
#define CSV_LOCALACCOUNT		22
#define CSV_FIRM					23
#define CSV_TRADE_FOR			24		// trader the trade is being done for (i.e. CSV_TRADER doing trade on CSV_TRADE_FOR behalf)
#define CSV_QUERY_TAG			25		// tag used for communications between OM and the DB Server

// just status messages
#define CSV_ORDER_TAG			15		// tag assigned by lowest client
#define CSV_EXCHANGE_TAG		16		// tag assigned by exchange
#define CSV_STAFFORD_TAG		17		// tag assigned by order manager
#define CSV_CANCEL_TAG			18		// tag assigned to cancel by server connected to exchange
#define CSV_EXEC_TAG				19		// execution tag
#define CSV_EXEC_CONFIRM		20		// confirm from exchane
#define CSV_REFERENCE			21		// execution lookup tag from exchange
#define CSV_REPORT				22		// do act report?
#define CSV_CONTRA				23		// contra
#define CSV_TEXT					24		// reject or status text returned by exchange

class CCsv
{

private:
	CStringArray	m_Fields;
	CString			m_Message;

public:

	int Bust(CString& Message);
		
	const CString& Build()
		{
		int i;
		for (i = 0, m_Message = ""; i < m_Fields.GetSize(); i++)	m_Message += (m_Fields.GetAt(i) + ",");
		m_Message += '\n';
		return m_Message;
		}

	const CString&	Message()				{ return Build();		}
	const CString& Current()		const	{ return m_Message;	}

	const CString Get(int Field) const
		{ 
		if (Field < m_Fields.GetSize())
			return(m_Fields[Field]);
		else
			return "";
		}

	const int	nGet(int Field) const 	
		{ 
		if (Field < m_Fields.GetSize())
			return atoi(m_Fields[Field]);	
		else
			return 0;
		}

	const double	dGet(int Field) const 	
		{ 
		if (Field < m_Fields.GetSize())
			return atof(m_Fields[Field]);
		else
			return 0.0;
		}

	const char cGet(int Field)	const 
		{ 
		if (Field < m_Fields.GetSize())
			{
			if (m_Fields[Field].GetLength() > 0)
				return (m_Fields[Field].GetAt(0));
			else
				return 0;
			}
		else
			return 0;
		}

	void	SetAt(int Field, double Value) 
		{
		CString workString;
		workString.Format("%f", Value);
		m_Fields.SetAtGrow(Field, workString);
		}

	void SetAt(int Field, int Value)
		{
		CString workString;
		workString.Format("%d", Value);
		m_Fields.SetAtGrow(Field, workString);
		}

	void SetAt(int Field, char Value)			{ m_Fields.SetAtGrow(Field, CString(Value));	}
	void SetAt(int Field, CString Value)		{ m_Fields.SetAtGrow(Field, Value);				}
	void SetAt(int Field, const char *Value)	{ m_Fields.SetAtGrow(Field, Value);				}

public:
	CCsv(char Command)				
		{ 
		m_Fields.SetSize(26);
		m_Fields[CSV_COMMAND] = CString(Command);
		m_Fields[CSV_TIME] = CTime::GetCurrentTime().Format("%H%M%S");
		}
	CCsv()												{ m_Fields.SetSize(26); }
	CCsv(CString& Message)							{ m_Fields.SetSize(26); Bust(Message);	}
	virtual ~CCsv()									{ m_Fields.RemoveAll();	}
};

#endif