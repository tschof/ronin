// Order.h: interface for the COrder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORDER_H__7F191A12_2228_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_ORDER_H__7F191A12_2228_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCsv;
#include "Price.h"

// char arrays are used instead of CString or RWStrings or str strings so that lists of orders can be stored in shared memory.
// their size has to be constant (NO dynamic memory allocation within a COrder)

class COrder : public CMessageBase 
{
public:
	enum Exchange
		{
		TNTO,
		INSTINET,
		NYSE,
		DB,
		ACT
		};

	enum Status
		{
		New,
		PartiallyFilled,
		Filled,
		DoneForDay,
		Canceled,
		Replaced,
		PendingCancel,
		Stopped,
		Rejected,
		Suspended,
		PendingNew,
		Calculated,
		Expired,
		Open,
		CancelRejected
		};

	enum Sides
		{
		FYI,
		Buy,
		Sell,
		BuyMinus,
		SellPlus,
		Short,
		Exempt,
		Undisclosed,
		Cross
		};

	enum OrderType
		{
		FYIType,
		Market,
		Limit,
		Stop,
		StopLimit,
		MarketOnClose,
		WithOrWithout,
		LimitOrBetter,
		LimitWithOrWithout,
		OnBasis,
		OnClose,
		LimitOnClose
		};

	enum Rule80A
		{
		Agency = 'A',
		Principal = 'P',
		Registered = 'E'
		};

	enum TimeInForce
		{
		Day,
		GTC,
		OPG,
		IC,
		FOK,
		GTX,
		GoodTillDate
		};

	static char m_Destinations[4][12];
	static char	m_StatusStrings[20][36];
	static char	m_Sides[12][2];
	static void Init();
	static const char		*StatusString(int Status)	{ return m_StatusStrings[Status];	}
	static const char		*SideString(int Side)		{ return m_Sides[Side];					}
	static const char		*Destination(int Destin)	{ return m_Destinations[Destin];		}
	static const CString	GetHHMMSS()						{ return CTime::GetCurrentTime().Format("%H%M%S");	}
				
protected:
	CFixedString	m_ClearingAccount;	// clearing account number
	CFixedString	m_ClearingId;			// nscc clearing company id
	CFixedString	m_LocalAccount;
	CFixedString	m_Firm;
	CFixedString	m_TradeFor;				// trader doing trade on tradefor's behalf
	char				m_Instructions[36];	// special order instructions
	char				m_Symbol[36];			// symbol
	char				m_FullSymbol[36];		// symbol with reutors extension
	char				m_Trader[36];			// trader acr
	char				m_sPrice[36];			// price represented as a string (99 99/99)
	char				m_Owner[36];			// used by OM to maintain who generated the order
	char				m_SourceId[72];		// used by OM and FIX, comma delimited combination of trader acr and owner
	char				m_Debug[256];			// debug string
	WORD				m_Tag;					// client tag
	char				m_Capacity;				// (P)rincipal, (A)gent
	char				m_Type;					
	int				m_Side;
	int				m_Status;
	int				m_TIF;
	int				m_Floor;
	int				m_MinQty;
	int				m_Exchange;
	int				m_Slot;					// can be used as index in array or grid row
	char				m_PriceCheck;			// tells exchange whether or not to use price checking
	long				m_Shares;
	long				m_OriginalShares;
	long				m_Sequence;
	double			m_Price;
	double			m_ReplacementPrice;
	
	double			m_StopPrice;
	BOOL				m_Discretionary;
	BOOL				m_Solicited;
	BOOL				m_Used;					// used for shared memory array
	BOOL				m_PendingReplace;

public:

	const char*		lpszClearingAccount()const { return m_ClearingAccount.Get();	}
	const char*		lpszTradeFor()			const { return m_ClearingAccount.Get();	}
	const char*		lpszLocalAccount()	const { return m_LocalAccount.Get();		}
	const char*		lpszFirm()				const { return m_Firm.Get();					}
	const char*		lpszClearingId()		const { return m_ClearingId.Get();			}

	const char*		lpszInstructions()	const { return m_Instructions;				}
	const char*		lpszDestination()		const { return m_Destinations[m_Exchange];}
	const char*		lpszSymbol()			const { return m_Symbol;						}
	const char*		lpszFullSymbol()		const { return m_FullSymbol;					}
	const char*		lpszPrice()				const { return m_sPrice;						}
	const char*		lpszTrader()			const { return m_Trader;						}
	const	char*		lpszStatusString()	const { return m_StatusStrings[m_Status]; }
	const char*		lpszSideString()		const { return m_Sides[m_Side];				}
	const char*		lpszDebug()				const { return m_Debug;							}

	const CString	ClearingId()			const { return m_ClearingId.GetCString();				}
	const CString	Firm()					const { return m_Firm.GetCString();						}
	const CString	ClearingAccount()		const { return m_ClearingAccount.GetCString();		}
	const CString	TradeFor()				const { return m_TradeFor.GetCString();				}
	const CString	LocalAccount()			const { return m_LocalAccount.GetCString();			}

	const CString	Debug()					const { return CString(m_Debug);							}
	const CString	Instructions()			const { return CString(m_Instructions);				}
	const CString	Destination()			const { return CString(m_Destinations[m_Exchange]);}
	const CString	Symbol()					const { return CString(m_Symbol);						}
	const CString	FullSymbol()			const { return CString(m_FullSymbol);					}
	const CString	Price()					const { return CString(m_sPrice);						}
	const CString	Trader()					const { return CString(m_Trader);						}
	const CString	StatusString()			const { return CString(m_StatusStrings[m_Status]); }
	const CString	SideString()			const { return CString(m_Sides[m_Side]);				}
	const	long		Sequence()				const { return m_Sequence;									}

	
	const WORD		Tag()						const { return m_Tag;							}
	const char		PriceCheck()			const { return m_PriceCheck;					}
	const BOOL		Type()					const { return m_Type;							}
	const BOOL		Used()					const { return m_Used;							}
	const BOOL		Discretionary()		const { return m_Discretionary;				}
	const BOOL		Solicited()				const { return m_Solicited;					}
	const int		Side()					const { return m_Side;							}
	const int		TIF()						const { return m_TIF;							}
	const int		Floor()					const { return m_Floor;							}
	const int		Status()					const { return m_Status;						}
	const int		MinQty()					const { return m_MinQty;						}
	const int		Slot()					const { return m_Slot;							}
	const int		Exchange()				const { return m_Exchange;						}
	const long		Shares()					const { return m_Shares;						}
	const long		OriginalShares()		const { return m_OriginalShares;				}
	const double	dPrice()					const { return m_Price;							}
	const double	StopPrice()				const { return m_StopPrice;					}
	const double	ReplacementPrice()	const { return m_ReplacementPrice;			}
	const BOOL		PendingReplace()		const { return m_PendingReplace;				}

	void SetClearingId(const char* Value)				{ m_ClearingId			= Value;	}
	void SetClearingId(const CString& Value)			{ m_ClearingId			= Value;	}

	void SetClearingAccount(const char* Value)		{ m_ClearingAccount	= Value;	}
	void SetClearingAccount(const CString& Value)	{ m_ClearingAccount	= Value;	}

	void SetTradeFor(const char* Value)					{ m_TradeFor	= Value;			}
	void SetTradeFor(const CString& Value)				{ m_TradeFor	= Value;			}

	
	void SetLocalAccount(const char* Value)			{ m_LocalAccount		= Value;	}
	void SetLocalAccount(const CString& Value)		{ m_LocalAccount		= Value;	}
	
	void SetFirm(const char* Value)						{ m_Firm					= Value;	}
	void SetFirm(const CString& Value)					{ m_Firm					= Value;	}
		
	void SetTag(WORD Value)							{ m_Tag				= Value;			}
	void SetSlot(int Value)							{ m_Slot				= Value;			}
	
	void SetTrader(const char* Value)			{ strcpy(m_Trader, Value);			}
	void SetTrader(const CString& Value)		{ strcpy(m_Trader, Value);			}

	void SetInstructions(const char* Value)	{ strcpy(m_Instructions, Value); }
	void SetInstructions(const CString& Value){ strcpy(m_Instructions, Value); }

	void SetSymbol(const char* Value)			{ strcpy(m_Symbol, Value);			}
	void SetSymbol(const CString& Value)		{ strcpy(m_Symbol, Value);			}
	void SetFullSymbol(const char* Value)		{ strcpy(m_FullSymbol, Value);	}
	void SetFullSymbol(const CString& Value)	{ strcpy(m_FullSymbol, Value);	}

	const	char		Capacity()				const		{ return m_Capacity;				}
	const int		nCapacity()				const
		{
		if (m_Capacity == 'A')
			return(0);
		else
			return(1);
		}
	void	SetCapacity(int Value)						
		{ 
		if (Value == 0)	m_Capacity	= 'A';	
		else					m_Capacity	= 'P';
		}
	void	SetCapacity(char Value)					{ m_Capacity		= Value;			}

	void SetType(const char Value)				{ m_Type				= Value;			}
	void SetSide(const int Value)					{ m_Side				= Value;			}
	void SetTIF(const int Value)					{ m_TIF				= Value;			}
	void SetStatus(const int Value)				{ m_Status			= Value;			}
	void SetShares(const int Value)				{ m_Shares			= Value;			}
	void SetOriginalShares(const int Value)	{ m_OriginalShares= Value;			}

	void SetPrice(const double Value)			
		{ 
		m_Price	= Value; 
		strcpy(m_sPrice, CPrice::ToString(m_sPrice, Value));
		}
	void SetPrice(const CPrice& Price)
		{
		m_Price = Price.dPrice();
		strcpy(m_sPrice, Price.Price());
		}

	void SetReplacementPrice(const double Value)	{ m_ReplacementPrice = Value; }
	void SetPendingReplace(const BOOL Value)		{ m_PendingReplace	= Value;	}
	void SetStopPrice(const double Value)			{ m_StopPrice			= Value; }
	void SetPriceCheck(const char Value)			{ m_PriceCheck			= Value; }
	void SetFloor(const int Value)					{ m_Floor				= Value; }
	void SetMinQty(const int Value)					{ m_MinQty				= Value; }
	void SetDiscretionary(const BOOL Value)		{ m_Discretionary		= Value; }
	void SetSolicited(const BOOL Value)				{ m_Solicited			= Value; }
	void SetExchange(const int Value)				{ m_Exchange			= Value; }
	void SetUsed(const BOOL Value)					{ m_Used					= Value; }
	void Set(const CCsv& Message);

	const CString& SetOrderTag(long Num) 
		{
		m_Sequence = Num;
		m_OrderTag = CTime::GetCurrentTime().Format("%Y%m%d");
		m_OrderTag = m_OrderTag + Num;
		return(m_OrderTag.GetCString());
		}

	void	SetOrderTag(const char* Tag)			{ m_OrderTag	=	Tag;				}
	void	SetOrderTag(const CString& Tag)		{ m_OrderTag	=	Tag;				}

	
public:
	COrder(CCsv& Message);
	COrder();
	COrder(COrder *);
	virtual ~COrder();

};

#endif // !defined(AFX_ORDER_H__7F191A12_2228_11D2_8D11_006008CD4894__INCLUDED_)
