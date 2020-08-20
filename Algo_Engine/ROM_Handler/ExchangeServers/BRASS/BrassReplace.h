#if !defined(CBrassReplace_H)
#define CBrassReplace_H

#include "Message.h"

class CBrassReplace	: public CMessageBase
{
private:
	CString		m_Account;
	CString		m_Instructions;
	CString		m_Destination; 
	CString		m_Capacity;
	CString		m_Symbol;
	CString		m_Trader;
	char		m_Command;
	int			m_Side;
	int			m_TIF;
	char		m_Type;
	long		m_Shares;
	long		m_OriginalShares;
	double		m_OriginalPrice;
	double		m_Price;
	double		m_StopPrice;

public:
	const CString&	Account()			const { return m_Account;			}
	const CString&	Instructions()		const { return m_Instructions;		}
	const CString&	Destination()		const { return m_Destination;		}
	const CString&	Capacity()			const { return m_Capacity;			}
	const CString&	Symbol()			const { return m_Symbol;			}
	const CString&	Trader()			const { return m_Trader;			}
	const int		Side()				const { return m_Side;				}
	const int		TIF()				const { return m_TIF;				}
	const char		Type()				const { return m_Type;				}
	const long		Shares()			const { return m_Shares;			}
	const long		OriginalShares()	const { return m_OriginalShares;	}
	const double	OriginalPrice()		const { return m_OriginalPrice;		}
	const double	Price()				const { return m_Price;				}
	const double	StopPrice()			const { return m_StopPrice;			}
	const char		Command()			const { return m_Command;			}

	void Set(const CCsv&	Message);

	void SetOriginalShares(long Shares) { m_OriginalShares = Shares;	}
	void SetOriginalPrice(double Price) { m_OriginalPrice = Price;		}

public:
	CBrassReplace(const CCsv&	Message);
	virtual ~CBrassReplace()				{ };
};

#endif