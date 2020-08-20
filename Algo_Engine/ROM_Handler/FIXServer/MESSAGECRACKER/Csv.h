#if !defined(CCsv_H)
#define CCsv_H

#include "Fields.h"


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
		for (i = 0, m_Message = ""; i < m_Fields.GetSize(); i++)
		{		
			m_Message += (m_Fields.GetAt(i) + ",");
		}
		
		//m_Message += m_Fields.GetAt(i); // Make sure to grab the last field
			
		m_Message += '\n';

		m_Message.Replace(NULL, ' ');

		return m_Message;
	}

	const CString& Message()			{ return Build();	}
	const CString& Current()	const	{ return m_Message;	}

	int Length(int Field) 
	{ 
		if(Field < m_Fields.GetSize())
			return(m_Fields[Field].GetLength());
		else
			return 0;
	}

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

	void SetAt(int Field, double Value) 
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

	void SetAt(int Field, unsigned int Value)
	{
		CString workString;
		workString.Format("%u", Value);
		m_Fields.SetAtGrow(Field, workString);
	}

	void SetAt(int Field, char Value)			{ m_Fields.SetAtGrow(Field, CString(Value));	}
	void SetAt(int Field, CString Value)		{ m_Fields.SetAtGrow(Field, Value);				}
	void SetAt(int Field, const char *Value)	{ m_Fields.SetAtGrow(Field, Value);				}

public:
	CCsv(char Command, int Size = CSV_NUM_FIELDS)
	{ 
		m_Fields.SetSize(Size);
		m_Fields[CSV_COMMAND] = CString(Command);
		CTime CurrentTime = CTime::GetCurrentTime();
		CurrentTime.FormatGmt("%Y%m%d-%H:%M:%S");
		m_Fields[CSV_TIME] = CurrentTime.FormatGmt("%Y%m%d-%H:%M:%S");//CTime::GetCurrentTime().Format("%H%M%S");

		if (Size > CSV_EXEC_TIME)
		{
			m_Fields[CSV_EXEC_TIME] = m_Fields[CSV_TIME];
		}
	}

	CCsv(int Size = CSV_NUM_FIELDS)							
	{ 
		m_Fields.SetSize(Size); 
		CTime CurrentTime = CTime::GetCurrentTime();
		CurrentTime.FormatGmt("%Y%m%d-%H:%M:%S");
		m_Fields[CSV_TIME] = CurrentTime.FormatGmt("%Y%m%d-%H:%M:%S");//CTime::GetCurrentTime().Format("%H%M%S");

		if (Size > CSV_EXEC_TIME)
		{
			m_Fields[CSV_EXEC_TIME] = m_Fields[CSV_TIME];
		}
	}

	CCsv(CString& Message, int Size = CSV_NUM_FIELDS);
	virtual ~CCsv()											{ m_Fields.RemoveAll();	}
};

#endif