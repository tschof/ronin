class CReplace	: public CMessageBase
{
private:
	CString		m_Account;
	CString		m_Instructions;
	CString		m_Destination; 
	CString		m_Capacity;
	CString		m_Symbol;
	CString		m_Trader;
	char			m_Command;
	int			m_Side;
	int			m_TIF;
	char			m_Type;
	long			m_Shares;
	long			m_OriginalShares;
	double		m_Price;
	double		m_StopPrice;

public:
	const CString& Trader()				const { return m_Trader; }
	const CString& Account()			const { return m_Account; }
	const CString& Symbol()				const { return m_Symbol; }
	const int		Side()				const { return m_Side; }
	const long		Shares()				const { return m_Shares; }
	const long		OriginalShares()	const { return m_OriginalShares; }
	const double	Price()				const { return m_Price; }
	const char		Command()			const { return m_Command; }
	
public:
	CReplace(const CCsv&	Message) { };
	virtual ~CReplace()				{ };
};
