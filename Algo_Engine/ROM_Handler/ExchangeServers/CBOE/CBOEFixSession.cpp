// CBOEFixSession.cpp: implementation of the CCBOEFixSession class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "string.h"
#include "CBOE.h"
#include "CBOEFixSession.h"
#include "fixmsg42\FixAdminMsgs.h"
#include "exchangehandler.h"
#include "fixmsg42\Fix.h"
#include "CBOEMemTagSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCBOEFixSession::CCBOEFixSession()
{
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("ISE","ISX"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("CBOE","CBO"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("AMEX","ASE"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("PHLX","PHO"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("PCX","PSE"));
	m_mapDestToLinkDest.insert(MapDestToLinkDest::value_type("BOX","BOX"));

	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(0,"Broker Option (Admission of Non-Compliance)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(1,"Unknown or invalid or ineligible symbol (for linkage)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(2,"Exchange closed"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(3,"Order exceeds limit"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(4,"Too late to enter"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(5,"Unknown Order"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(6,"Duplicate Order (e.g. dupe ClOrdID)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(8,"Stale Order (Time-to-live of received order has expired.)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(31,"Instrument state is invalid for Linkage (The receiver is in Rotation)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(32,"Instrument state is invalid for Linkage (The receiver is in Non-firm mode)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(33,"Instrument state is invalid for Linkage (The receiver has the instrument halted)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(34,"Not at NBBO"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(35,"Reference Price is out of bound"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(36,"Unknown clearing firm"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(37,"Sub-Account ID missing"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(38,"Invalid Auto-Ex value"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(39,"Account missing"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(40,"TimeInForce missing/invalid"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(41,"OpenClose missing/invalid"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(42,"ExecBroker missing"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(43,"ClearingAccount missing"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(44,"ExecutionInformation missing"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(45,"Order received too soon (does not meet ClassGate requirement)"));
	m_mapLinkageRejReasons.insert(MapLinkageRejReasons::value_type(46,"OrderCapacity missing/invalid"));

	m_SessionString = "W_MAIN"; //initialize string to early morning session string (7:00am-8:15am)

	CMessageApp* l_pApp = (CMessageApp*) AfxGetApp();
	TString l_FileName = l_pApp->DatFile();
	TString l_Buf;
	l_Buf.resize(129);

	GetPrivateProfileString("Fix Information", "UserNamePassword", "",  (char*) l_Buf.c_str(), 128, l_FileName.c_str());
	m_UserNamePassword = l_Buf.c_str();

	GetPrivateProfileString("Fix Information", "ExecBroker", "", (char*) l_Buf.c_str(), 18,l_FileName.c_str()); 
	m_execBroker = l_Buf.c_str();
	m_inCrowdExecBroker = m_execBroker;
	m_linkageExecBroker = m_execBroker;
	/*
	GetPrivateProfileString("Fix Information", "InCrowdExecBroker", "", (char*) l_Buf.c_str(), 18,l_FileName.c_str()); 
	m_inCrowdExecBroker = l_Buf.c_str();

	GetPrivateProfileString("Fix Information", "LinkageExecBroker", "", (char*) l_Buf.c_str(), 18,l_FileName.c_str()); 
	m_linkageExecBroker = l_Buf.c_str();
	*/
	l_pApp->ProcessDebug("CBOE ExecBroker: ",m_execBroker.c_str());
	l_pApp->ProcessDebug("CBOE InCrowdExecBroker: ",m_inCrowdExecBroker.c_str());
	l_pApp->ProcessDebug("CBOE LinkageExecBroker: ",m_linkageExecBroker.c_str());
}

CCBOEFixSession::~CCBOEFixSession()
{

}


BOOL CCBOEFixSession::Logon()
{
	if(!m_LogonInitiated && !m_LogoutInitiated)
	{
		ProcessLogEntry("Sending logon.");
		CFixLogon l_Logon(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr(), m_Encrypt, 
			m_SendHeartbtInt);

		if (!m_TargetSubId.empty())
		{
			l_Logon.TargetSubID(m_TargetSubId.c_str());
		}

		if(m_UserNamePassword.size() > 0)
		{
			l_Logon.SenderSubID(m_UserNamePassword.c_str());
		}
		
		LogonOverride(l_Logon);
		
		if(SendMsg(l_Logon))
		{
			m_LogonInitiated = true;
			return TRUE;
		}
	}

	return FALSE;
}

void CCBOEFixSession::Process(const CFixLogout& a_Logout)
{
	if(!AddIncomingMsgToSequence(a_Logout))
	{
		return;
	}

	TOStrStream l_LogMsg;
	l_LogMsg << "Logout received -- Text: " << a_Logout.Text();
	ProcessLogEntry(l_LogMsg.str().c_str());
	
	if(!m_LogoutInitiated)
	{
		ProcessLogEntry("Sending logout acknowledgement.");
		CFixLogout l_Logout(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr());
		l_Logout.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
		SendMsg(l_Logout);
		m_LogoutInitiated = true;
	}
	else
	{
		m_LogoutInitiated = false;
		Shutdown();
	}
}

long CCBOEFixSession::ProcessEntry(COrder& a_Order)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER , ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing orders until fix session is corrected.");
		CString Msg("IFS:Fix session out of sequence:Can't send order.");
		throw Msg;
	}

	long l_SeqNum = m_OutgoingMsgs.NextMsgSeqNum();
	
	CString clientOrderId = a_Order.OrderTag();

	CFixOrder l_Order(
		l_SeqNum, 
		GetCurrentTimeStr(), 
		clientOrderId, 
		FIX_AutoPublic, //Handling Instructions (Automatic or Manual hndling of order)
		a_Order.Symbol(), 
		(EFixSide) m_Translator.m_ToExchangeSide[a_Order.Side()], 
		(EFixOrdType) m_Translator.m_ToExchangeType[a_Order.Type()]
		);
	
	if (a_Order.Type() == COrder::Stop)
	{
		l_Order.StopPx(a_Order.StopPrice());
	}
	else if (a_Order.Type() == COrder::StopLimit)
	{
		l_Order.StopPx(a_Order.StopPrice());
		l_Order.Price(a_Order.dPrice());
	}
	else if (a_Order.Type() != COrder::Market && a_Order.Type() != COrder::MarketOnClose && a_Order.Type() != COrder::MarketAtOpen)
	{
		l_Order.Price(a_Order.dPrice());
	}
	
	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Order.TargetSubID(m_TargetSubId.c_str());
	}
	//l_Order.TargetSubID(m_TargetSubId.c_str());
	l_Order.SenderSubID(a_Order.SourceId());
	l_Order.OrderQty(a_Order.Shares());
	
	l_Order.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Order.TIF()]);
	
	if (a_Order.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Order.ExecInst());
		l_Order.ExecInst(ExecInst);
	}

	if(a_Order.SecurityType() != COrder::Option)
	{
		l_Order.Rule80A(a_Order.Capacity());
	}
	
	if (a_Order.Floor() != 0)
	{
		l_Order.MaxFloor(a_Order.Floor());
	}
	
	if(a_Order.ExDestination() != "")
		l_Order.ExDestination(a_Order.ExDestination());
	
	if(OrderEntryOverride(a_Order, l_Order) )
		if(!SendMsg(l_Order))
		{
			CString Msg("CFS:PE:Error in SendMsg.");
			throw Msg;
		}
	
	return TRUE;
}

BOOL CCBOEFixSession::OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder)
{
	//Contingency
	//(non-DAY/non-GTC)if(a_Order.TIF() != COrder::Day && a_Order.TIF() != COrder::GTC)
	if(a_Order.TIF() != COrder::Day && a_Order.TIF() != COrder::GTC)
	{
		//(non-Day/non-GTC orders) can only be of (Market/Limit) type or else reject
		if (a_Order.Type() != COrder::Market && a_Order.Type() != COrder::Limit)
		{
			CString Msg;
			Msg.Format("CFS:OEO:Non-Day/Non-GTC orders must be Market or Limit only.");
			((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
			throw Msg;
		}
		//(non-Day/non-GTC orders)(Market/Limit) 
		else if(a_Order.TIF() != COrder::FOK && a_Order.TIF() != COrder::IC && a_Order.TIF() != COrder::OPG)
		{
			CString Msg;
			Msg.Format("CFS:OEO:Market/Limit(non-DAY/non-GTC)orders can only be OPG/IOC/FOK.");
			((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
			throw Msg;
		}
	}
	else
	{
		//(DAY or GTC)(Market or Limit)(AON or Not Held)
		if(a_Order.ExecInst()== COrder::AON || a_Order.ExecInst()== COrder::NotHeld )
		{
			if((a_Order.TIF() != COrder::Day && a_Order.TIF() != COrder::GTC) || (a_Order.Type() != COrder::Market && a_Order.Type() != COrder::Limit))
			{
				CString Msg;
				Msg.Format("CFS:OEO:Wrong Order Type/TIF for AON/NH order.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}
		}	
		else if(a_Order.Type() != COrder::Market && a_Order.Type() != COrder::Limit && a_Order.Type() != COrder::LimitOrBetter
				&& a_Order.Type() != COrder::Stop && a_Order.Type() != COrder::StopLimit && a_Order.Type() != COrder::LimitOnClose 
					&& a_Order.Type() != COrder::MarketOnClose )
		{
			CString Msg;
			Msg.Format("CFS:OEO:Wrong Order type for DAY/GTC order.");
			((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
			throw Msg;
		}
	}
	
	//option specific stuff
	a_FixOrder.SecurityType("OPT");

	//Temp
	// a_FixOrder.Account(a_Order.ClearingId() + "-" + a_Order.ClearingAccount());
	const char* trade4 = a_Order.lpszTradeFor();
	const char* clrAccount = a_Order.lpszClearingAccount();
	const char* subAccount = "";
	if ( clrAccount )
		subAccount = clrAccount;
	if ( trade4 && *trade4 )
		a_FixOrder.SenderSubID(trade4);
	else
		a_FixOrder.SenderSubID(a_Order.SourceId());

	/*
	if ( clrAccount && *clrAccount)
	{
	   subAccount = strchr(clrAccount, (int)':');
	   if ( subAccount)
	   {
		   *subAccount = 0;
		   subAccount++;
	   }
	}
	*/
	CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr = a_FixOrder.CreateNewField((EFixFieldID)440);
	// l_FieldPtr->Value(a_Order.ClearingAccount());
	l_FieldPtr->Value(clrAccount);
	a_FixOrder.InsertInBody(l_FieldPtr);
	a_FixOrder.MaturityMonthYear((LPCTSTR) a_Order.ExpDate());

	if(a_Order.CallOrPut() == 'P')
		a_FixOrder.PutOrCall(0);
	else if(a_Order.CallOrPut() == 'C')
		a_FixOrder.PutOrCall(1);
	else
	{
		CString Msg("CFS:OEO:PutOrCall parameter missing.");
		throw Msg;
	}

	a_FixOrder.StrikePrice(a_Order.StrikePrice());
	// a_FixOrder.ExecBroker(m_execBroker.c_str());
	const char* clrId = a_Order.lpszClearingId();
	if ( clrId==0 || *clrId == 0 )
		a_FixOrder.ExecBroker(m_execBroker.c_str());
	else 
	{
		CString exbroker;
		exbroker.Format("CBOE:%d", atoi(clrId));
		a_FixOrder.ExecBroker(exbroker);
	}

	//CMTA or GIVEUP Setting
	CString GiveupAccount = a_Order.GiveUpAccount();
	CString CMTAAccount = a_Order.CMTAAccount();

	a_FixOrder.TransactTime((LPCTSTR)GetCurrentTimeStr());

	CString strInstructions = a_Order.Instructions();
	strInstructions.MakeUpper();
	
	if( (COrder::PrimaryMM != a_Order.Capacity() && COrder::MarketMaker != a_Order.Capacity() && COrder::MarketMakerFirm != a_Order.Capacity()) && (-1 != strInstructions.Find("INCROWD",0) || -1 != strInstructions.Find("P:",0) ) )
	{
		CString Msg;
		Msg.Format("CFS:OEO:Only Market Maker can send I-Order or Linkage Order.");
		((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
		throw Msg;
	}
	
	char val9324[128];
	::memset(val9324, 0, sizeof(val9324) );
	switch(a_Order.Capacity())
	{
	case COrder::Customer:
	case COrder::Agency://Customer
		a_FixOrder.Rule80A('C');
		if ( clrAccount )
		{
			a_FixOrder.Account(clrAccount);
			sprintf(val9324, "%- 3s        N", clrAccount);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value(val9324);
		}
		// a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value("           R");
		break;

	case COrder::Principal://Firm
	case COrder::BrokerDealerFirm:
		a_FixOrder.Rule80A('F');//will be depricated
		if ( clrAccount )
		{
			a_FixOrder.Account(clrAccount);
			sprintf(val9324, "%- 3s        N", clrAccount);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value(val9324);
		}
		// a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value("           R");
		break;

	case COrder::BrokerDealer://Broker Dealer Customer
		a_FixOrder.Rule80A('X');
		if ( clrAccount )
		{
			a_FixOrder.Account(clrAccount);
			sprintf(val9324, "%- 3s        N", clrAccount);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value(val9324);
		}
		// a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value("           R");
		break;

	//commented out since CBOE converts the following into a customer order
	/*
	case COrder::BrokerDealerFirm: //Broker Dealer Firm
		a_FixOrder.Rule80A('B');
		break;
	*/
	case COrder::MarketMaker://Market Maker Cust
	case COrder::MarketMakerFirm://Market Maker Firm
	case COrder::PrimaryMM: 

		if(-1 != strInstructions.Find("P:",0))//Linkage Order
		{
			if((a_Order.Type() != COrder::Limit) || (a_Order.TIF() != COrder::IC))
			{
				CString Msg;
				Msg.Format("CFS:OEO:Linkage order must be Limit & IOC.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}
			
			//must be CBOE Market Maker
			if(a_Order.MMExchange() != COrder::CBOE)
			{
				CString Msg;
				Msg.Format("CFS:OEO:Only CBOE Market Maker can send Linkage.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}
			
			//must be CBOE Market Maker
			if(a_Order.OpenClose() != 1)
			{
				CString Msg;
				Msg.Format("CFS:OEO:Linkage orders should be OPEN only.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}

			a_FixOrder.Rule80A('P'); //Designate as Linkage Order
				
			MapDestToLinkDest::iterator theIterator;
			theIterator = m_mapDestToLinkDest.find((LPCTSTR)strInstructions.Mid(2));
			if(theIterator != m_mapDestToLinkDest.end())
				a_FixOrder.ExDestination(((*theIterator).second).c_str() );
			else
			{
				CString Msg;
				Msg.Format("CFS:OEO:Not a valid Linkage Destination.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}

			// a_FixOrder.ExecBroker(m_linkageExecBroker.c_str());
			// a_FixOrder.GuaranteedGetCustomField((EFixFieldID)21)->Value("2");
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9465)->Value(a_Order.TradeFor());
			a_FixOrder.Account(a_Order.MMAccount());
		}
		else if(-1 != strInstructions.Find("INCROWD",0))//I Order
		{
			//if((a_Order.TIF() != COrder::Day) || (a_Order.Type() != COrder::Limit))
			if(a_Order.Type() != COrder::Limit)
			{
				CString Msg;
				Msg.Format("CFS:OEO:InCrowd Order can only be Day Limit Order.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}
			
			//must be CBOE Market Maker
			if(a_Order.MMExchange() != COrder::CBOE)
			{
				CString Msg;
				Msg.Format("CFS:OEO:Only CBOE Market Maker can send InCrowd Orders.");
				((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
				throw Msg;
			}
			
			// a_FixOrder.ExecBroker(m_inCrowdExecBroker.c_str());
			a_FixOrder.Rule80A('I'); //Designate as I Order
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Order.MMAccount());						
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9465)->Value(a_Order.TradeFor());
			a_FixOrder.Account(a_Order.MMAccount());
			
			const char* traderAcrn = a_Order.lpszSourceId();
			const char* traderAcct = trade4;
			if ( traderAcct==0 || *traderAcct == 0 )
				traderAcct = traderAcrn;

			// char val9324[128];
			// if ( traderAcrn )
			// 	sprintf(val9324, "           R%- 3s", traderAcrn);	
			// else strcpy(val9324, "           R");
			const char* mmacct = a_Order.lpszMMAccount();
			if ( mmacct == 0)
				mmacct = " ";
			if ( traderAcct )
				sprintf(val9324, "I:%- 3s %- 5sN%- 3s", mmacct, subAccount, traderAcct);			
			else sprintf(val9324, "I:%- 3s %- 5sN", mmacct, subAccount);
			//                    123456789012
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value(val9324);	
		}
		else if(a_Order.MMExchange() == COrder::AMEX || a_Order.MMExchange() == COrder::AMEXOPTION || a_Order.MMExchange() == COrder::PCX || a_Order.MMExchange() == COrder::PHLX || a_Order.MMExchange() == COrder::ISE || a_Order.MMExchange() == COrder::BOX)
		{		
			// a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value("M:" + a_Order.MMAccount());
			const char* traderAcrn = a_Order.lpszSourceId();
			const char* traderAcct = trade4;
			if ( traderAcct==0 || *traderAcct == 0 )
				traderAcct = traderAcrn;
			const char* mmacct = a_Order.lpszMMAccount();
			// char val9324[128];
			// if ( traderAcrn && mmacct && strcmp(traderAcrn, mmacct) != 0 )
			if ( traderAcct )
				sprintf(val9324, "M:%- 3s %- .5sN%- 3s", mmacct, subAccount, traderAcct);			
			else sprintf(val9324, "M:%- 3s %- .5sN", mmacct, subAccount);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value(val9324);	
			a_FixOrder.Rule80A('N');
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Order.MMAccount());
			a_FixOrder.Account(a_Order.MMAccount());
		}
		else if(a_Order.MMExchange() == COrder::CBOE)
		{
			// a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value("M:" + a_Order.MMAccount());		
			const char* traderAcrn = a_Order.lpszSourceId();
			const char* traderAcct = trade4;
			if ( traderAcct==0 || *traderAcct == 0 )
				traderAcct = traderAcrn;
			const char* mmacct = a_Order.lpszMMAccount();
			// char val9324[128];
			// if ( traderAcrn && mmacct && strcmp(traderAcrn, mmacct) != 0 )
			if ( traderAcct )
				sprintf(val9324, "M:%- 3s %- .5sN%- 3s", mmacct, subAccount, traderAcct);			
			else sprintf(val9324, "M:%- 3s %- .5sN", mmacct, subAccount);
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)9324)->Value(val9324);	
			a_FixOrder.Rule80A('M');
			a_FixOrder.GuaranteedGetCustomField((EFixFieldID)440)->Value(a_Order.MMAccount());
			a_FixOrder.Account(a_Order.MMAccount());
		}
		else
		{
			CString Msg;
			Msg.Format("CFS:OEO:Not a valid Home Exchange.");
			((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,((CMessageApp *)AfxGetApp())->Name() +":CFS:OEO:",Msg);
			throw Msg;
		}

		break;
	
	}//	switch(a_Order.Capacity())

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixOrder, a_Order);

	a_FixOrder.GuaranteedGetCustomField((EFixFieldID)386)->Value(1);
	a_FixOrder.GuaranteedGetCustomField((EFixFieldID)336)->Value(m_SessionString.c_str());

	if(-1 == strInstructions.Find("INCROWD",0))//I Order
	{
		if(a_Order.OpenClose() == 1)
			a_FixOrder.OpenClose("O");
		else if(a_Order.OpenClose() == 0)
			a_FixOrder.OpenClose("C");
		else
		{
			CString Msg("CFS:OEO:OpenClose parameter missing.");
			throw Msg;
		}
	}

	CExchangeHandler* l_pHandler =(CExchangeHandler*) &( (CMessageApp *)AfxGetApp() )->Handler();
	CCBOEMemTagSet*& l_MemTag = (CCBOEMemTagSet*&)l_pHandler->GetMemTag();
	if ( l_MemTag)
		l_MemTag->SetExecBroker(a_FixOrder.ExecBroker().c_str());		

	return TRUE;
}

BOOL CCBOEFixSession::ProcessCancel(CCancel& a_Cancel)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing cancel until fix session is corrected.");
		CString Msg("IFS:Fix session out of sequence:Can't send cancel.");
		throw Msg;
	}

	CFixCancel l_Cancel(
		m_OutgoingMsgs.NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Cancel.OrderTag(),
		a_Cancel.CancelTag(),
		a_Cancel.Symbol(),
		(EFixSide) m_Translator.m_ToExchangeSide[a_Cancel.Side()]
		);
	
	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Cancel.TargetSubID(m_TargetSubId.c_str());
	}

	l_Cancel.SenderSubID(a_Cancel.SourceId());

	if(CancelOverride(a_Cancel, l_Cancel))
		if(!SendMsg(l_Cancel))
		{
			CString Msg("CFS:PC:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;
}

BOOL CCBOEFixSession::CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl)
{
	COptFixSession::CancelOverride(a_Cxl, a_FixCxl);
	
	CString strInstructions = a_Cxl.Instructions();
	strInstructions.MakeUpper();

	if(-1 != strInstructions.Find("P:",0))//Linkage Order
	{
			CString Msg("CFS:CO:Linkage orders can't be cancelled.");
			throw Msg;
	}

	/*
	if(-1 != strInstructions.Find("INCROWD",0))//I Order
		a_FixCxl.ExecBroker(m_inCrowdExecBroker.c_str());
	else
		a_FixCxl.ExecBroker(m_execBroker.c_str());	
	*/
	
	CExchangeHandler* l_pHandler =(CExchangeHandler*) &( (CMessageApp *)AfxGetApp() )->Handler();
	CCBOEMemTagSet*& l_MemTag = (CCBOEMemTagSet*&)l_pHandler->GetMemTag();
	if ( l_MemTag )
		a_FixCxl.ExecBroker(l_MemTag->ExecBroker());
	else
	{
		if(-1 != strInstructions.Find("INCROWD",0))//I Order
			a_FixCxl.ExecBroker(m_inCrowdExecBroker.c_str());
		else
			a_FixCxl.ExecBroker(m_execBroker.c_str());	
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "CBOE MAP may be damaged.");
	}

	CString GiveupAccount = a_Cxl.GiveUpAccount();
	CString CMTAAccount = a_Cxl.CMTAAccount();
	
	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixCxl, a_Cxl);

	a_FixCxl.TransactTime(GetCurrentTimeStr());
	
	a_FixCxl.OrderQty((int)a_Cxl.Shares());
	
	return TRUE;
}

BOOL CCBOEFixSession::ProcessReplace(CReplace& a_Replace)
{
	if(m_AwaitingResend)
	{
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing replace until fix session is corrected.");
		a_Replace.SetInstructions("Fix session out of sequence:Can't replace order.");
		return FALSE;
	}

	CFixReplace l_Replace(
		m_OutgoingMsgs.NextMsgSeqNum(),
		GetCurrentTimeStr(),
		a_Replace.OrderTag(),
		a_Replace.CancelTag(),
		FIX_AutoPublic,
		a_Replace.Symbol(),
		(EFixSide) m_Translator.m_ToExchangeSide[a_Replace.Side()],
		(EFixOrdType) m_Translator.m_ToExchangeType[a_Replace.Type()]
		);
	
	if (strcmp(TargetSubId().c_str(), "") != 0)
	{
		l_Replace.TargetSubID(m_TargetSubId.c_str());
	}

	if (strcmp(a_Replace.SourceId(), "") != 0)
	{
		l_Replace.SenderSubID(a_Replace.SourceId());
	}

	if (a_Replace.ExecInst() != COrder::LastExecInst)
	{
		char ExecInst[2];
		sprintf(ExecInst, "%c", a_Replace.ExecInst());
		l_Replace.ExecInst(ExecInst);
	}

	l_Replace.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Replace.TIF()]);

	l_Replace.OrderQty(a_Replace.Shares());

	if (a_Replace.Type() == COrder::Stop)
	{
		l_Replace.StopPx(a_Replace.StopPrice());
	}
	else if (a_Replace.Type() == COrder::StopLimit)
	{
		l_Replace.StopPx(a_Replace.StopPrice());
		l_Replace.Price(a_Replace.Price());
	}
	else if (a_Replace.Type() != COrder::Market && a_Replace.Type() != COrder::MarketOnClose && a_Replace.Type() != COrder::MarketAtOpen)
	{
		l_Replace.Price(a_Replace.Price());
	}

	if (a_Replace.Floor() != 0)
	{
		l_Replace.MaxFloor(a_Replace.Floor());
	}
		
	if(ReplaceOverride(a_Replace, l_Replace))
		if(!SendMsg(l_Replace))
		{
			CString Msg("CFS:PR:Error in SendMsg.");
			throw Msg;
		}

	return TRUE;

}

BOOL CCBOEFixSession::ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace) 
{
	COptFixSession::ReplaceOverride(a_Replace, a_FixReplace);

	CExchangeHandler* l_pHandler =(CExchangeHandler*) &( (CMessageApp *)AfxGetApp() )->Handler();
	CCBOEMemTagSet*& l_MemTag = (CCBOEMemTagSet*&)l_pHandler->GetMemTag();
	if ( l_MemTag == 0 )
	{
		CString Msg("CBOE MAP file is damaged");
		throw Msg;
	}
	a_FixReplace.Rule80A(l_MemTag->Capacity());

	
	CString strInstructions = a_Replace.Instructions();
	strInstructions.MakeUpper();

	const char* clrId = a_Replace.lpszClearingId();
	if ( clrId==0 || *clrId == 0 )
		a_FixReplace.ExecBroker(m_execBroker.c_str());
	else 
	{
		CString exbroker;
		exbroker.Format("CBOE:%d", atoi(clrId));
		a_FixReplace.ExecBroker(exbroker);
	}

	/*
	if(-1 != strInstructions.Find("INCROWD",0))//I Order
		a_FixReplace.ExecBroker(m_inCrowdExecBroker.c_str());
	else
	{
		a_FixReplace.ExecBroker(m_execBroker.c_str());
			
		if(a_Replace.OpenClose() == 1)
			a_FixReplace.OpenClose("O");
		else if(a_Replace.OpenClose() == 0)
			a_FixReplace.OpenClose("C");
		else
		{
			CString Msg("CFS:RO:OpenClose parameter missing.");
			throw Msg;
		}
	}
	*/
	if(a_Replace.OpenClose() == 1)
		a_FixReplace.OpenClose("O");
	else if(a_Replace.OpenClose() == 0)
		a_FixReplace.OpenClose("C");
	else
	{
		CString Msg("CFS:RO:OpenClose parameter missing.");
		throw Msg;
	}

	switch(a_Replace.Capacity())
	{
	case COrder::Customer:
	case COrder::Agency://Customer
		a_FixReplace.Rule80A('C');
		break;

	case COrder::Principal://Firm
	case COrder::BrokerDealerFirm:
		a_FixReplace.Rule80A('F');//will be depricated
		break;

	case COrder::BrokerDealer://Broker Dealer Customer
		a_FixReplace.Rule80A('X');
		break;

	//commented out since CBOE converts the following into a customer order
	/*
	case COrder::BrokerDealerFirm: //Broker Dealer Firm
		a_FixReplace.Rule80A('B');
		break;
	*/
	case COrder::MarketMaker://Market Maker Cust
	case COrder::MarketMakerFirm://Market Maker Firm
	case COrder::PrimaryMM: 

		if(-1 != strInstructions.Find("P:",0))//Linkage Order
		{
			CString Msg("CFS:RO:Linkage orders can't be replaced.");
			throw Msg;
			//a_FixReplace.Rule80A('P'); //Designate as Linkage Order
		}
		else if(-1 != strInstructions.Find("INCROWD",0))//I Order
		{
			a_FixReplace.Rule80A('I'); //Designate as I Order
			a_FixReplace.GuaranteedGetCustomField((EFixFieldID)9465)->Value(l_MemTag->TradeFor());
		}
		else if(a_Replace.MMExchange() == COrder::AMEX || a_Replace.MMExchange() == COrder::AMEXOPTION || a_Replace.MMExchange() == COrder::PCX || a_Replace.MMExchange() == COrder::PHLX || a_Replace.MMExchange() == COrder::ISE || a_Replace.MMExchange() == COrder::BOX)
		{
			a_FixReplace.Rule80A('N');
		}
		else if(a_Replace.MMExchange() == COrder::CBOE)
		{
			a_FixReplace.Rule80A('M');
		}
		break;
	}//	switch(a_Replace.Capacity())

	CString GiveupAccount = a_Replace.GiveUpAccount();
	CString CMTAAccount = a_Replace.CMTAAccount();

	ProcessCMTAGiveup(GiveupAccount, CMTAAccount, a_FixReplace, a_Replace);

	a_FixReplace.TransactTime((LPCTSTR)GetCurrentTimeStr());

	a_FixReplace.GuaranteedGetCustomField((EFixFieldID)386)->Value(1);
	a_FixReplace.GuaranteedGetCustomField((EFixFieldID)336)->Value(m_SessionString.c_str());

	l_MemTag->SetExecBroker(a_FixReplace.ExecBroker().c_str());		

	return TRUE;
}

BOOL CCBOEFixSession::ExecutionOverride(CExecution &Exec, const CFixExecution& a_Exec)
{
	if(!COptFixSession::ExecutionOverride(Exec, a_Exec))
		return FALSE;

	//extract the cancel Qty from the Custom Cancel Qty field
	if(Exec.ExecType() == COrder::Canceled )
	{
		Exec.SetShares(atoi(const_cast<CFixExecution&>(a_Exec).GetCustomFieldValue(84).c_str()));
		if(a_Exec.LeavesQty() > 0)
		{
			Exec.SetExecType(COrder::Replaced); //change status to replace as this represents a partial cancel from the exchange's point of view
		
			CExchangeHandler* l_pHandler =(CExchangeHandler*) &( (CMessageApp *)AfxGetApp() )->Handler();
			CCBOEMemTagSet*& l_MemTag = (CCBOEMemTagSet*&)l_pHandler->GetMemTag();
			if ( l_MemTag)
			{
				//To pre-fill in the rest of the (partial cancel) replace info as so that succeeding blocks of code have data to place correctfully.
				l_MemTag->GetTags(MAP_ORDER_TAG_QUERY, Exec.OrderTag());
				l_MemTag->SetReplaceShares(Exec.Shares());
				l_MemTag->SetReplacePrice(Exec.Price());
				l_MemTag->SetReplaceTag(Exec.OrderTag());
				l_MemTag->SetTimeInForce(Exec.TimeInForce());
				l_MemTag->SetReplaceStopPrice(Exec.StopPrice());
				l_MemTag->SetReplaceOrderType(Exec.OrderType());
				if(!Exec.Instructions().IsEmpty())
					l_MemTag->SetReplaceExecInst(Exec.Instructions().GetAt(0));
			}
		}
	}
	else if(Exec.ExecType() == COrder::DoneForDay)
		Exec.SetShares(atoi(const_cast<CFixExecution&>(a_Exec).GetCustomFieldValue(84).c_str()));

	//extract the OLAOrdRejectReason field
	if(0 == (const_cast<CFixExecution&>(a_Exec).GetCustomFieldValue(FIX_Rule80A)).compare("P"))
	{
		if(Exec.ExecType() == COrder::Canceled || Exec.ExecType() == COrder::Rejected)
		{
			MapLinkageRejReasons::iterator theIterator;
			theIterator = m_mapLinkageRejReasons.find(atoi(const_cast<CFixExecution&>(a_Exec).GetCustomFieldValue(5209).c_str()));
			if(theIterator != m_mapLinkageRejReasons.end())
				Exec.SetReason(((*theIterator).second).c_str() );
			else
				Exec.SetReason(const_cast<CFixExecution&>(a_Exec).GetCustomFieldValue(5209).c_str());
		}
	}
	else if(Exec.ExecType() == COrder::Rejected )
	{
		CString reason;
		reason.Format("Reject Reason:%d", a_Exec.OrdRejReason());
		Exec.SetReason(reason);
	}

	Exec.SetTradeTag(a_Exec.ExecID().c_str());

	return TRUE;
}

void CCBOEFixSession::Process(const CFixCancelReject& a_Reject)
{
	//This message used to reject both cancels and cancel\replace
	
	if(!AddIncomingMsgToSequence(a_Reject))
		return;

	CCancelReject l_Reject;
	l_Reject.SetSourceId(a_Reject.TargetSubID().c_str());
	l_Reject.SetOrderTag(a_Reject.OrigClOrdID().c_str());
	l_Reject.SetExchangeTag(a_Reject.OrderID().c_str());
	l_Reject.SetCancelTag(a_Reject.ClOrdID().c_str());
	l_Reject.SetStatus(m_Translator.m_ToStaffordStatus[a_Reject.OrdStatus()]);

	if(0 == atoi(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(102).c_str()))
		l_Reject.SetReason("Too late to cancel");
	else if(1 == atoi(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(102).c_str()))
		l_Reject.SetReason("Unknown Order");

	l_Reject.SetText(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(9311).c_str());
		
	((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
}

BOOL CCBOEFixSession::SendRejectMsg(const long& a_RefSeqNo, const char* a_pText)
{
	//According to the CBOE FIX 4.2 spec document (fix-03a.pdf), CBOE doesn't accept reject 
	//messages sent to them or else they suspend the session

	TOStrStream l_LogMsg;
	l_LogMsg << "Not allowed to send reject back to CBOE -- RefSeqNo: " << a_RefSeqNo << " Text: " << a_pText;
	ProcessLogEntry(l_LogMsg.str().c_str());
	
	/*CFixReject l_Reject(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr(), a_RefSeqNo);
	l_Reject.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
	l_Reject.Text(a_pText);;

	RejectOverride(l_Reject);
	
	return SendMsg(l_Reject);*/

	return TRUE;
}