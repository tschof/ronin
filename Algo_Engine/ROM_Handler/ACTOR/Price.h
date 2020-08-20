// Price.h: interface for the CPrice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRICE_H__9682C483_10DF_11D2_8D10_006008CD4894__INCLUDED_)
#define AFX_PRICE_H__9682C483_10DF_11D2_8D10_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPrice  
{
public:
	CPrice();
	CPrice(double Price);
	CPrice(CString& Price);
	CPrice(CPrice&	Price);
	virtual ~CPrice() {};

public:
	const double	dPrice()	const { return m_Double; }
	const CString& Price()	const { return m_String; }
	const BOOL		IsSet()	const { return ((((int)(m_Double*1000)) && ((int)m_Double != -999)));	 }

	static	CString		ToString(double Price, int Base = 64);
	static	const char*	ToString(char *buf, double Price, int Base = 64);
	static	double		ToDouble(const char *Price);
	
	CString GetDollar() { int pos = m_String.Find(' ');
						  return m_String.Left(pos);
	}
	CString GetNumerator() { int pos_space = m_String.Find(' ');
							 int pos_div   = m_String.Find('/');
							 if(pos_div == -1)
								 return("0");
							 else
							 {
								 CString rValue = m_String.Mid(pos_space+1, pos_div-pos_space-1);
								 rValue.TrimLeft();
								 return rValue;
							 }
	}
	CString GetDenominator() { int pos = m_String.Find('/');
							   if(pos == -1)
								   return("1");
							   else
							   {
								   CString rValue = m_String.Mid(pos+1);
								   rValue.TrimRight();
								   return rValue;
							   }
	}

// operators
public:
		
	CPrice& operator=(const CPrice& Price)
		{
		if (&Price == this)
			return *this;
		m_String = Price.Price();
		m_Double = Price.dPrice();
		return *this;
		}
			
	CPrice& operator=(const CString& Price)
		{
		m_String = Price;
		m_Double = ToDouble(Price);
		return *this;
		}

	CPrice& operator=(const double Price)
		{
		m_String = ToString(Price);
		m_Double = Price;
		return *this;
		}
	CPrice& operator=(const char *Price)
		{
		m_String = Price;
		m_Double = ToDouble(CString(Price));
		return *this;
		}

	BOOL operator>(const CPrice& Price)		{ return(m_Double > Price.dPrice());	}
	BOOL operator<(const CPrice& Price)		{ return(m_Double < Price.dPrice());	}
	BOOL operator>=(const CPrice& Price)	{ return(m_Double >= Price.dPrice());	}
	BOOL operator<=(const CPrice& Price)	{ return(m_Double <= Price.dPrice());	}
	BOOL operator==(const CPrice& Price)	{ return(m_Double == Price.dPrice());	}


protected:
	CString	m_String;
	double	m_Double;

protected:
	static	void		Simplify(int &val, int &base);
	static	int		gcd(int u,int v);
};

#endif // !defined(AFX_PRICE_H__9682C483_10DF_11D2_8D10_006008CD4894__INCLUDED_)
