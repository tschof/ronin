#include "stdafx.h"
#include "Csv.h"

int CCsv::Bust(CString& Message)
{
	int	Index;
	int	FieldCnt;

	for (FieldCnt = 0; ((Index = Message.Find(',')) >= 0); FieldCnt++)
		{
		
		if (Index == 0)
			m_Fields.SetAtGrow(FieldCnt, "");
		else
			m_Fields.SetAtGrow(FieldCnt, Message.Left(Index));

		Message = Message.Mid(Index+1);
		}

	if (Message != "")
		m_Fields.SetAtGrow(FieldCnt++, Message);

return(FieldCnt);

}

