/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * FromIslandExecution.cpp: implementation of the CFromIslandExecution class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 12/28/05    joel        and support for BillingCode
 *
 *******************************************************************************/
#include "stdafx.h"
#include "FromIslandExecution.h"
#include "IslandMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "Order.h"

CFromIslandExecution::CFromIslandExecution(const CString& Message)
{
	char	Type	= Message[16];
	int		Leaves	= 0;
	CString	workString;
	
	if (Type == 'O')
	{
		m_Trader	= CIslandMessage::Get(Message, 17 , 4);
		m_SourceId	= m_Trader;
		m_OrderTag	= CIslandMessage::Get(Message, 21 , 10);
		m_TradeTag	= CIslandMessage::Get(Message, 1 , 10);
		m_TradeTag.TrimLeft();
		
		CString ts = CIslandMessage::Get(Message, 11 , 5);
		ts.TrimLeft();
		m_TransactionTime = GetGMTTransactionTime(ts);

		char Action = Message[31];

		switch (Action)
		{
			case 'A':		// open
				m_ExchangeTag	= CIslandMessage::Get(Message, 32 , 9);
				m_Side			= CIslandMessage::ToStaffordSide(Message[41]);
				m_Shares		= atoi(Message.Mid(42 , 9));
				m_Symbol		= CIslandMessage::Get(Message, 60 , 6);
				m_Price			= atof(Message.Mid(66, 20));
				m_Status		= COrder::Open;

				m_Debug.Format("Report %s %s %d Shares Of %s At %f",
					COrder::StatusString(m_Status),
					COrder::SideString(m_Side),
					m_Shares,
					m_Symbol,
					m_Price);
				break;
			case 'C':		// canceled

				m_Text			= Message.Mid(50 ,4);
				m_Status		= COrder::Canceled;
				m_Text.TrimRight();

				m_Debug.Format("Report %s",
					COrder::StatusString(m_Status));
				break;
			case 'E':		// executed
				m_LastShares	= atoi(Message.Mid(32, 9));
				Leaves			= atoi(Message.Mid(41, 9));
				m_LastPrice		= atof(Message.Mid(50, 20));
				m_ExecBroker	= CIslandMessage::Get(Message, 70 , 4);
				m_BillingCode	= CIslandMessage::Get(Message, 74 , 1);  //joel
				
				if (!Leaves)
					m_Status	= COrder::Filled;
				else
					m_Status	= COrder::PartiallyFilled;

				m_Debug.Format("Report %s %d Shares At %f",
					COrder::StatusString(m_Status),
					m_LastShares,
					m_LastPrice);
				break;
			case 'B':		// broken
				m_Status		= COrder::Busted;
				m_Text			= CIslandMessage::Get(Message, 32 , 4);
				break;
			default:
				break;
		}
					
	}
}


CString CFromIslandExecution::GetGMTTransactionTime(CString& ts)
{
	int hour, minute, second;
	
	int seconds = atoi((LPCTSTR)ts) - 3600;  // Subtracting 3600 is because island uses ET

	minute = (seconds % 3600)/60;
	second = seconds % 60;
	hour = seconds / 3600;

	CTime t = CTime::GetCurrentTime();

	CTime T(t.GetYear(), 
		    t.GetMonth(), 
		    t.GetDay(), 
		    hour,
		    minute,
		    second);

	return T.FormatGmt("%Y%m%d-%H:%M:%S");
}

