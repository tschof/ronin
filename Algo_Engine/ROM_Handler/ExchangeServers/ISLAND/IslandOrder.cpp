// IslandOrder.cpp: implementation of the CIslandOrder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IslandOrder.h"
#include "Island.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIslandOrder::CIslandOrder(const COrder& Order)
{
	m_Fields.AddField('D');

	if (Order.Instructions() == "N" ||
		Order.Instructions() == "S")
	{
		m_Fields.AddField('N');
	}
	else
	{
		m_Fields.AddField('Y');
	}

	m_Fields.AddField(Order.Capacity());

	CIslandApp* pApp = (CIslandApp*)AfxGetApp();

	m_Fields.AddField(4, pApp->MMID());

	switch (Order.TIF())
	{
		case COrder::IC:
		case COrder::FOK:		
			m_Fields.AddField(5, 1);
			break;
		default:
			m_Fields.AddField(5, 99999);
			break;
	};

	m_Fields.AddField(9,	10,Order.dPrice());
	m_Fields.AddField(6,	Order.Symbol());
	m_Fields.AddField(9,	Order.MinQty());
	m_Fields.AddField(9,	Order.Shares());
	m_Fields.AddField(		(char)ToExchangeSide(Order.Side()));
	m_Fields.AddField(10,	Order.OrderTag());
    CString lcsTradeFor = Order.TradeFor();
    if(lcsTradeFor.GetLength() > 4) {
        lcsTradeFor = Order.TradeFor().Left(4);
    }
	m_Fields.AddField(4,lcsTradeFor);//Order.Owner());
	m_Fields.AddField('O');
}

