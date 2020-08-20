// Message.h: interface for the CMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGE_H__E2B634A6_1FCD_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_MESSAGE_H__E2B634A6_1FCD_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Csv.h"
#include "ActFields.h"

class CLogin;
class CLogout;
class CCsv;

class CFixedString 
{
public:
	char m_String[128];

public:
	const CString GetCString()	const { return CString(m_String);	} 
	const char *Get()				const {	return m_String;				}

	CFixedString& operator=(const CString& str)
		{
		if (str != "")
			{
			strncpy(m_String, str, str.GetLength());
			m_String[str.GetLength()] = '\0';
			}
		else
			m_String[0] = '\0';

		return *this;
		}

	CFixedString& operator=(const char* str)
		{
		strcpy(m_String, str);
		return *this;
		}
	CFixedString& operator=(const int Int)
		{
		sprintf(m_String, "%ld", Int);
		return *this;
		}
	CFixedString& operator+(const char* str)
		{
		sprintf(m_String, "%s%s", m_String, str);
		return *this;
		}
	CFixedString& operator+(const CString& str)
		{
		sprintf(m_String, "%s%s", m_String, str);
		return *this;
		}

	CFixedString& operator+(const long Long)
		{
		sprintf(m_String, "%s%ld", m_String, Long);
		return *this;
		}
	CFixedString& operator+(const int Int)
		{
		sprintf(m_String, "%s%d", m_String, Int);
		return *this;
		}

public:
	CFixedString()	{ m_String[0]		= NULL; }
		
};

class CMessageBase : public CObject
{
protected:
	CFixedString	m_Owner;
	CFixedString	m_SourceId;
	CFixedString	m_OrderTag;
	CFixedString	m_CancelTag;
	CFixedString	m_ExchangeTag;
	CFixedString	m_StaffordTag;

public:
	const char*		lpszOwner()			const		{ return m_Owner.Get();				}
	const char*		lpszSourceId()		const		{ return m_SourceId.Get();			}
	const char*		lpszOrderTag()		const		{ return m_OrderTag.Get();			}
	const char*		lpszCancelTag()		const		{ return m_CancelTag.Get();			}
	const char*		lpszExchangeTag()	const		{ return m_ExchangeTag.Get();		}
	const char*		lpszStaffordTag()	const		{ return m_StaffordTag.Get();		}

	const CString	Owner()				const 	{ return m_Owner.GetCString();			}
	const CString	SourceId()			const 	{ return m_SourceId.GetCString();		}
	const CString	OrderTag()			const 	{ return m_OrderTag.GetCString();		}
	const CString	CancelTag()			const 	{ return m_CancelTag.GetCString();		}
	const CString	ExchangeTag()		const 	{ return m_ExchangeTag.GetCString();	}
	const CString	StaffordTag()		const 	{ return m_StaffordTag.GetCString();	}

	void	SetSourceId(const char*		Value)	{ m_SourceId	= Value;				}
	void	SetSourceId(const CString& Value)	{ m_SourceId	= Value;				}

	void	SetOwner(const char*		Value)	{ m_Owner		=	Value;				}
	void	SetOwner(const CString& Value)		{ m_Owner		=	Value;				}

	void	SetExchangeTag(const CString& Tag)	{ m_ExchangeTag=	Tag;				}
	void	SetExchangeTag(const char* Tag)		{ m_ExchangeTag=	Tag;				}
	
	void	SetCancelTag(const CString& Tag)	{ m_CancelTag	=	Tag;				}
	void	SetCancelTag(const char* Tag)		{ m_CancelTag	=	Tag;				}

	void	SetStaffordTag(const CString& Tag)	{ m_StaffordTag=	Tag;				}
	void	SetStaffordTag(const char* Tag)		{ m_StaffordTag=	Tag;				}

	void	SetOrderTag(const char* Tag)		{ m_OrderTag	=	Tag;				}
	void	SetOrderTag(const CString& Tag)		{ m_OrderTag	=	Tag;				}

public:
	CMessageBase()				{ }
	virtual ~CMessageBase() { }
		
};

class CMessage : public CObject
{
public:
	enum Source
		{
		Socket,
		Rv
		};

	enum Command
		{
		Logon = 'L',
		Logout = 'O',
		Order = CSV_ACT_COMMAND_ENTER
		};

private:
	
	char	m_Type;
	int		m_Source;
	CCsv	*m_pMessage;

public:
	const char	Type()			const { return m_Type;		}
	const int	Source()		const { return m_Source;	}

	void	SetType(char Type)			{ m_Type = Type;	 }
	void	SetSouce(int Source)		{ m_Source = Source; }

	CLogin&	Login()						{ return (CLogin &)*m_pMessage;	}
	CCsv&	Message()				{ return *m_pMessage;			}

public:
	CMessage(int Source, CString& Message);
	CMessage(int Source);
	virtual ~CMessage();

};


class CLogin : public CCsv
{
public:
	const CString Trader()							const	{ return(Get(CSV_TRADER));		}
	void  SetTrader(const CString& Trader)					{ SetAt(CSV_TRADER, Trader);	}

public:
	CLogin(CString& Message)	: CCsv(Message)			{ }
	CLogin()	: CCsv(CMessage::Logon)					{ }
	virtual ~CLogin()									{ }
};

class CLogout : public CCsv
{
public:
	const CString Trader()							const	{ return Get(CSV_TRADER);		}
	void  SetTrader(const CString& Trader)					{ SetAt(CSV_TRADER, Trader);	}

public:
	CLogout(CString& Trader) : CCsv(CMessage::Logout)	{ SetTrader(Trader); }
	CLogout()	: CCsv(CMessage::Logout)				{ }
	virtual ~CLogout()									{ }
};



#endif // !defined(AFX_MESSAGE_H__E2B634A6_1FCD_11D2_8D11_006008CD4894__INCLUDED_)
