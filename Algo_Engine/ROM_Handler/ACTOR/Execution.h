#if !defined(CEXECUTION_H)
#define CEXECUTION_H

#pragma warning(disable:4786)

class CExecution : public CMessageBase
{
protected:
	
	char			m_Report;
	CString		m_TradeTag;
	CString		m_OriginalTag;
	CString		m_ExecBroker;
	CString		m_Confirm;
	CString		m_TransactionType;
	CString		m_ReferenceConfirm;
	CString		m_RejectReason;
	CString		m_Account;
	CString		m_Symbol;
	CString		m_Instructions;
	CString		m_Text;
	CString		m_Debug;
	
	CString		m_Trader;
	CString		m_LocalAccount;
	CString		m_ClearingAccount;
	CString		m_Firm;

	int			m_ExecType;
	int			m_Status;
	int			m_Side;
	int			m_Shares;
	int			m_OrderType;
	int			m_TimeInForce;
	int			m_LastShares;
	int			m_Leaves;
	int			m_Cumulative;
	double		m_Average;
	double		m_Price;
	double		m_StopPrice;
	double		m_LastPrice;

	CTime			m_EntryTime;
	
public:
	
	const CString& Trader()					const { return m_Trader; }
	const CString& LocalAccount()			const { return m_LocalAccount; }
	const CString& ClearingAccount()		const { return m_ClearingAccount; }
	const CString& Firm()					const { return m_Firm; }
	const CString& Debug()						const { return m_Debug;		}

	inline void	SetTrader(const CString& Trader)							
		{ 
		m_Trader = Trader;
		m_Trader.TrimRight();
		}

	inline void	SetLocalAccount(const CString& LocalAccount)
		{ 
		m_LocalAccount = LocalAccount; 
		m_LocalAccount.TrimRight();
		}
	inline void	SetClearingAccount(const CString& ClearingAccount)	
		{ 
		m_ClearingAccount = ClearingAccount; 
		m_ClearingAccount.TrimRight();
		}
	inline void	SetFirm(const CString& Firm)								
		{ 
		m_Firm = Firm; 
		m_Firm.TrimRight();
		}

	inline void SetTradeTag(long Tag)
		{
		m_TradeTag.Format("%ld", Tag);
		}

	const CString& TradeTag()				const { return m_TradeTag; }
	const char		Report()					const { return m_Report; }	
	const CString& Reason()					const { return m_RejectReason; }
	const CString& OriginalTag()			const { return m_OriginalTag; }
	const CString& ExecBroker()			const { return m_ExecBroker; }
	const CString& Confirm()				const { return m_Confirm; }
	const CString& TransactionType()		const { return m_TransactionType; }
	const CString& ReferenceConfirm()	const { return m_ReferenceConfirm; }
	const CString& Account()				const { return m_Account; }
	const CString& Symbol()					const { return m_Symbol; }
	const CString& Instructions()			const { return m_Instructions; }
	const CString& Text()					const { return m_Text; }
	const int		ExecType()				const	{ return m_ExecType; }
	const int		Status()					const { return m_Status; }
	const int		Side()					const { return m_Side; }
	const int		Shares()					const { return m_Shares; }
	const int		OrderType()				const { return m_OrderType; }
	const int		TimeInForce()			const { return m_TimeInForce; }
	const int		LastShares()			const { return m_LastShares; }
	const int		Leaves()					const { return m_Leaves; }
	const int		Cumulative()			const { return m_Cumulative; }
	const double	Average()				const { return m_Average; }
	const double	Price()					const { return m_Price; }
	const double	StopPrice()				const { return m_StopPrice; }
	const double	LastPrice()				const { return m_LastPrice; }
	const CTime		EntryTime()				const { return m_EntryTime; }
	
	void	SetReport(const char Report)				{ m_Report = Report;			}
	void	SetReason(const CString& Reason)			{ m_RejectReason = Reason; }
	void	SetOriginalTag(const CString& Tag)		{ m_OriginalTag = Tag;		}
	void	SetExecBroker(const CString& Broker)	{ m_ExecBroker = Broker;	}
	void	SetConfirm(const CString& Confirm)		{ m_Confirm = Confirm;		}
	void	SetTransactionType(const CString& Type){ m_TransactionType = Type;}
	void	SetReferenceConfirm(const CString& Con){ m_ReferenceConfirm = Con;}
	void	SetAccount(const CString& Account)		{ m_Account = Account;		}
	void	SetSymbol(const CString& Symbol)			{ m_Symbol = Symbol;			}
	void	SetInstructions(const CString& Instruc){ m_Instructions = Instruc;}
	void	SetText(const CString& Text)				{ m_Text = Text;				}
	void	SetExecType(const int Type)				{ m_ExecType = Type;			}
	void	SetStatus(const int Status)				{ m_Status = Status;			}
	void	SetSide(const int Side)						{ m_Side = Side;				}
	void	SetShares(const int Shares)				{ m_Shares = Shares;			}
	void	SetOrderType(const int OrderType)		{ m_OrderType = OrderType; }
	void	SetTimeInForce(const int TIF)				{ m_TimeInForce = TIF;		}
	void	SetLastShares(const int Shares)			{ m_LastShares = Shares;	}
	void	SetLeaves(const int Leaves)				{ m_Leaves = Leaves;			}
	void  SetCumulative(const int CumVol)			{ m_Cumulative = CumVol;	}
	void	SetAverage(const double Average)			{ m_Average = Average;		}
	void	SetPrice(const double Price)				{ m_Price = Price;			}
	void	SetStopPrice(const double Price)			{ m_StopPrice = Price;		}
	void	SetLastPrice(const double Price)			{ m_LastPrice = Price;		}
	void	SetEntryTime(const CTime EntryTime)		{ m_EntryTime = EntryTime; }
	
public:
	CExecution()				{	}
	virtual ~CExecution()	{	}

};
	


#endif