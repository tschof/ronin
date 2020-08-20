// IslandMessage.h: interface for the CIslandMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISLANDMESSAGE_H__AF6E8EBC_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLANDMESSAGE_H__AF6E8EBC_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "IslandFields.h"
#include "Translator.h"

class CIslandHeader
{
public:
	enum Types
	{
		Login		= 'W',
		Goodbye		= 'G',
		Heartbeat	= 'H',
		Sequenced	= 'S',
		Reject		= 'J'
	};

	static void	Init();

private:
	static CMapStringToString m_Reasons;

private:

	char	m_Type;
	int		m_Sequence;
	int		m_Stamp;
	int		m_Version;
	CString	m_Text;


public:
	const char		Type()		const { return m_Type;		}
	const int		Sequence()	const { return m_Sequence;	}
	const int		Stamp()		const { return m_Stamp;		}
	const	int		Version()	const { return m_Version;	}
	const CString&	Text()		const { return m_Text;		}

	

public:
	CIslandHeader(const CString& Message);
	CIslandHeader()		{	}
	~CIslandHeader()	{	}
};

class CIslandMessage  
{
private:

	// translates stafford world to Exchange
	static CTranslator m_Translator;

public:

	enum Types
	{
		Text			= 'T',
		Event			= 'E',
		Status			= 'O',
		Reject			= 'O',
		CancelReject	= 'K',
		Accepted		= 'A',
		Canceled		= 'C',
		Executed		= 'E',
		Broken			= 'B'
	};

	enum Sides
	{
		Buy			= 'B',
		Sell		= 'S',
		Short		= 'T',
		Exempt		= 'E',
	};

public:

	static void	Init();

	// translation of exchange codes to stafford codes
	static const int ToStaffordSide(char ExchangeCode);
	// translation of stafford codes to exchange codes
	static const char ToExchangeSide(int StaffordCode);
	// fetch funcitons
	static CString Get(const CString& Message, int Offset, int Length);

protected:
	CFieldCollection	m_Fields;

	CString				m_Message;

public:
	CString& Message()		
	{ 
		if (!m_Fields.Valid())
			m_Message = Build();
		return m_Message;	
	}

	const CString& Build();

public:
	CIslandMessage(const CIslandHeader& Header, const CString& Message);
	CIslandMessage();
	virtual ~CIslandMessage();

};

#endif // !defined(AFX_ISLANDMESSAGE_H__AF6E8EBC_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
