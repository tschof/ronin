#include "stdafx.h"
#include "Csv.h"


int CCsv::Bust(CString& Message)
{
	int	Index;
	int	FieldCnt;

	for (FieldCnt = 0; ((Index = Message.Find(',')) >= 0); FieldCnt++)
	{
		
		if (Index == 0)
			m_Fields.SetAtGrow(FieldCnt, CString());
		else
			m_Fields.SetAtGrow(FieldCnt, Message.Left(Index));

		Message = Message.Mid(Index+1);
	}

	if (Message == "\n")
	{
		return (FieldCnt);
	}

	if (Message != "")
		m_Fields.SetAtGrow(FieldCnt++, Message);

	return(FieldCnt);
}


CCsv::CCsv(CString& Message, int Size)		
{ 
	m_Fields.SetSize(Size); 
	Bust(Message); 

	if (Size > CSV_EXEC_TIME &&
		Length(CSV_EXEC_TIME) == 0)
	{
		m_Fields[CSV_EXEC_TIME] = m_Fields[CSV_TIME];
	}
}