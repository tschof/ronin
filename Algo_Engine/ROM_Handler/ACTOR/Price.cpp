// Price.cpp: implementation of the CPrice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "TradeManager.h"
#include "Price.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString		CPrice::ToString(double Price, int Base);
const char*	CPrice::ToString(char *buf, double Price, int Base);
double		CPrice::ToDouble(const char *Price);
void			CPrice::Simplify(int &val, int &base);
int			CPrice::gcd(int u,int v);
	
CPrice::CPrice()
{
	m_Double = 0.0;
	m_String = "";
}

CPrice::CPrice(double Price)
{
	m_Double = Price;
	m_String = ToString(Price);
}
CPrice::CPrice(CString& Price)
{
	if(Price.Find('/') != -1)
	{
		m_Double = ToDouble(Price);
		m_String = Price;
	}
	else
		m_String = ToString(m_Double = atof(Price));

}
CPrice::CPrice(CPrice&	Price)
{
	m_Double = Price.dPrice();
	m_String = Price.Price();
}

const char*	CPrice::ToString(char *buf, double Price, int Base)
{
	strcpy(buf, (LPCSTR)ToString(Price, Base));
	return(buf);
}

// portions of ToString taken from stafford devlib (author???)
CString CPrice::ToString(double Price, int Base)
{
	int	intVal, decVal;

	CString fracStr = "";
	CString wholeStr = "";

	intVal = int(Price);
	decVal = abs(int(((Price - double(intVal)) * (double)Base)));

	Simplify(decVal, Base);

 	if (intVal != 0)
		wholeStr.Format("%d ", intVal);

	if	(decVal != 0)
		fracStr.Format("%2d/%-2d", decVal, Base);

	wholeStr += fracStr;

	return wholeStr;
}

double CPrice::ToDouble(const char *Price)
{
	int	Whole = 0;
	int	Num	= 0;
	int	Den	= 1;

	sscanf(Price, "%d %d/%d", &Whole, &Num, &Den);

	return((double)(Whole + ((double)Num / (double)Den))); 
}


// the following functions (Simplify, gcd) were taken from stafford devlib. author???? works????

void CPrice::Simplify(int &val, int &base)
{
	if (val == 0)
		return;

	int	s = val / abs(val);

	val = abs(val);

	if (base % val)
	{
		if (val % 2)
		{	
			// Is odd number
			if (base % 2)
			{
				int gcd_val = gcd(val,base);
				if ((gcd_val % 2) == 0)
				{
					val = val / gcd_val;
					base = base / gcd_val;
				}
			}
		}
		else
		{
			// Is even number
			if ((base % 2) == 0)	// base is also even number
			{
				int gcd_val = gcd(val,base);
				base = base / gcd_val;
				val = val / gcd_val;
			}
		}
	}
	else
	{
		base = base / val;
		val = 1;
	}

	val *= s;

}

int CPrice::gcd(int u,int v)
{
	int t;
	do
	{
		if (u < v)
		{
			t = u;
			u = v;
			v = t;
		}
		u = u - v;
	} while (u != v);
	return (u);
}
