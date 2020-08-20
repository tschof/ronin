#include "stdafx.h"
#include "IslandMessage.h"

void CFieldCollection::BuildMessage()
{
	POSITION pos;
	CField *pField;

	if (!(m_ValidMessage))
	{
		for (pos = m_Fields.GetHeadPosition(), m_Message = ""; (pos); )
		{
			if ((pField = (CField *)m_Fields.GetNext(pos)))
				m_Message += pField->GetField();
		}
		m_ValidMessage = TRUE;
	}
}

const CString& CFieldCollection::GetMessage() const
{
	return(m_Message);
}

char CFieldCollection::GetCharField(int Offset, int Length) const
{
	CField	*pField = NULL;
	return(0);
}

int CFieldCollection::GetIntField(int Offset, int Length) const
{
	CField	*pField = NULL;
	return(0);
}

double CFieldCollection::GetDoubleField(int Offset, int Length) const
{
	CField	*pField = NULL;
	return(0);
}

const CString& CFieldCollection::GetStringField(int Offset, int Length) const
{
	CField	*pField = NULL;
	return("");
}

CField *CFieldCollection::GetField(int Offset, int Length) const
{
	CField	*pField = NULL;
	return(NULL);
}

void CFieldCollection::AddField(int Length, int Value)
{
	CField *pField = new CField(Length, Value);
	AddField(pField);
}

void CFieldCollection::AddField(char Value)
{
	CField *pField = new CField(Value);
	AddField(pField);
}

void CFieldCollection::AddField(int WholeLength, int DecimelLength, double Value)
{
	CField *pField = new CField(WholeLength, DecimelLength, Value);
	AddField(pField);
}

void CFieldCollection::AddField(int Length, CString Value)
{
	CField *pField = new CField(Length, Value);
	AddField(pField);
}

void CFieldCollection::AddField(CField *pField)
{
	m_Fields.AddHead(pField);
	m_ValidMessage = FALSE;
}

CField::CField(int Length, int Value)
{
	CString	Format;

	m_Int = Value;

	Format.Format("%c%dd", '%', Length);
	m_Field.Format(Format, Value);
}

CField::CField(int WholeLength, int DecimelLength, double Value)
{
	CString	Format;
	CString	workString;

	m_Double = Value;

	Format.Format("%c%d.%df", '%', WholeLength, DecimelLength);
	workString.Format(Format, Value);

	Format.Format("%s%ds", "%", WholeLength+DecimelLength+1);
	m_Field.Format(Format, workString);
}

CField::CField(int Length, const CString& Value)
{
	CString	Format;

	Format.Format("%s%ds", "%-", Length);
	m_Field.Format(Format, Value);
}

CField::CField(char Value)
{
	m_Field = Value;
}