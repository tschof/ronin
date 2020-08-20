#if !defined(ISLAND_FIELDS_H)
#define ISLAND_FIELDS_H

class CField : public CObject
{
private:
	int		m_Int;
	double	m_Double;
	char	m_Char;
	CString	m_Field;
	
public:
	int			GetInt()	const { return m_Int;	};
	double		GetDouble()	const { return m_Double;};
	char		GetChar()	const { return m_Char;	};
	const		CString&	GetField()	const { return m_Field;	};

public:
	CField(int Length, int Value);
	CField(int WholeLength, int DecimelLength, double Value);
	CField(int Length, const CString& Value);
	CField(char Value);
};

class CFieldCollection
{

private:
	CObList	m_Fields;
	CString	m_Message;
	BOOL	m_ValidMessage;

private:
	void AddField(CField *pField);

public:
	void	BuildMessage();
	void	Invalidate() { m_ValidMessage = FALSE; }
	void	SetMessage(CString Message)	{ m_Message = Message; }
	const	BOOL Valid() const { return m_ValidMessage;	}

	const	CString& GetMessage() const;
	char	GetCharField(int Offset, int Length) const;
	int		GetIntField(int Offset, int Length) const;
	double	GetDoubleField(int Offset, int Length) const;
	const	CString& GetStringField(int Offset, int Length) const;
			CField *GetField(int Offset, int Length) const;

	void	AddField(int Length, int Value);
	void	AddField(char Value);
	void	AddField(int WholeLength, int DecimelLength, double Value);
	void	AddField(int Length, CString Value);
	
public:
	CFieldCollection() { m_ValidMessage = FALSE; };
	~CFieldCollection()
	{
		CField	*pField;
		while (!(m_Fields.IsEmpty()))
		{
			pField = (CField *)m_Fields.RemoveHead();
			delete pField;
		}
		m_Fields.RemoveAll();
	}
};

#endif