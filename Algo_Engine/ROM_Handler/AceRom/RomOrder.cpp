#include "StdAfx.h"
#include "RomOrder.h"
#include <sstream>
#include "Utility.h"

//std::set<short> CRomOrder::s_DeadStatus;
std::set<short> CRomOrder::s_ActionStatus;

void CRomOrder::Init()
{
	//CRomOrder::LoadDeadStatus();
	CRomOrder::LoadActionStatus();
}

void CRomOrder::LoadActionStatus()
{
	s_ActionStatus.insert(ROMVALUE::STATUS_PENDINGCANCEL);
	s_ActionStatus.insert(ROMVALUE::STATUS_REPLACEPENDING);
}



/*
void CRomOrder::LoadDeadStatus()
{
	s_DeadStatus.insert(ROMVALUE::STATUS_CANCELED);
	s_DeadStatus.insert(ROMVALUE::STATUS_FILLED);
	s_DeadStatus.insert(ROMVALUE::STATUS_ORDERREJECTED);
	s_DeadStatus.insert(ROMVALUE::STATUS_EXPIRED);
}

CRomOrder::CRomOrder(CRomCsv& msg, std::string& owner): m_NextExecId(0), m_CurrencyRate(1)
{
    m_pParent = NULL;
    m_LegCount = 0;
	m_pCsv = new CRomCsv(msg);
    m_pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ORDER);
    SetStatus(ROMVALUE::STATUS_NEW);
    SetOwner(owner);
	SetReplacePending(false);
	SetLeave(m_pCsv->nGet(ROMFIELDS::CSV_SHARES));
    SetOriginalShares(m_pCsv->nGet(ROMFIELDS::CSV_SHARES));
	SetCum(0);
	SetStatus(ROMVALUE::STATUS_NEW);
	if (Multiplier() == 0)
	{
		SetMultiplier("1");
	}

    RFIX::CRomMessage<CDummy>::tChild& children = m_pCsv->GetChildren();
    RFIX::CRomMessage<CDummy>::tChildIterator it = children.begin();
    //CRomCsv* pM = NULL;
    RFIX::CRCsv* pM = NULL;
    CRomOrder* pO = NULL;
    
    while (it != children.end())
    {
        pM = *it;
        CRomCsv childCSv(pM->Message());
        pO = new CRomOrder(childCSv, owner);
        //pO->InheritParent(this);
        //pO->SetOriginalShares(pO->Shares() * Shares());
        pO->SetCplxType(ROMVALUE::CPLXTYPE_SINGLE);
        pO->SetLeave(pO->Shares() * Shares());
        m_Children.push_back(pO);
        it++;
    }

    children.clear();
}*/

//CRomOrder::CRomOrder(const CRomOrder& order)
//{
//	m_pCsv = new CRomCsv(*order.csv());
//	SetNextExecIdFromExecTag(order.RomExecTag());
//}


CRomOrder::CRomOrder(CRomCsv& msg): m_NextExecId(0), m_CurrencyRate(1)
{
	Set(msg);
}

CRomOrder::CRomOrder()
{
	memset(this, 0, sizeof(CRomOrder));
}
/*
CRomOrder::~CRomOrder(void)
{
    if (m_pParent)
    {
        m_pParent->RemoveChild(this);
    }
    else if (m_Children)
    {
        tChildrenIT it = m_Children->begin();
        CRomOrder* pO = NULL;

        while (it != m_Children->end())
        {
            pO = *it;
            it++;
            delete pO;
        }

		delete m_Children;
    }
}*/

/*
void CRomOrder::DeleteChildren()
{
	if (m_Children)
	{
		delete m_Children;
		m_Children = NULL;
	}
}

bool CRomOrder::HasChildren() 
{ 
	bool rc = false;

	if (m_Children)
	{
		rc = !(m_Children->empty()); 
	}

	return rc;
}*/



//void CRomOrder::RemoveChild(CRomOrder* pOrder)
//{
  //  m_Children->remove(pOrder);
//}


void CRomOrder::SetNextExecIdFromExecTag(const char* value)
{
	std::string sValue = value;
	DWORD index = sValue.find_last_of('-');
	if (index != std::string::npos)
	{
		std::string s = sValue.substr(index + 1, sValue.length() - index - 1);
		m_NextExecId = atoi(s.c_str()) + 1;
	}
	else
	{
		m_NextExecId = 0;
	}
}


void CRomOrder::NextRomExecTag(std::string& rTag)
{
	char value[100];
	sprintf(value, "%s-%d", RomTag(), m_NextExecId++);
	rTag = value;
}


bool CRomOrder::IsOptionType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_OPTION ||
		secType == ROMVALUE::SECTYPE_OPTIONINDEX ||
		secType == ROMVALUE::SECTYPE_OPTIONFUTURE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRomOrder::IsFutureType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_FUTURE ||
		secType == ROMVALUE::SECTYPE_FUTUREEQUITY ||
		secType == ROMVALUE::SECTYPE_FUTURECURRENCY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRomOrder::IsCurrencyType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_CURRENCY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRomOrder::IsEquityType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_EQUITY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRomOrder::IsLimitType(short orderType)
{
	if (ROMVALUE::TYPE_LIMIT == orderType ||
		ROMVALUE::TYPE_STOPLIMIT == orderType ||
		ROMVALUE::TYPE_LIMITONCLOSE == orderType)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void CRomOrder::UpdateReplacingFields(CRomCsv& msg, short clientType)
{
	//need to define a replaceble fields list!!!
	
	if (msg.IsFieldEmpty(ROMFIELDS::CSV_TYPE) == false)
	{
		m_ReplacingType = msg.nGet(ROMFIELDS::CSV_TYPE);
	}
	else
	{
		m_ReplacingType = Type();
		msg.SetAt(ROMFIELDS::CSV_TYPE, m_ReplacingType);
	}

	switch(m_ReplacingType)
	{
	case ROMVALUE::TYPE_STOP:
	case ROMVALUE::TYPE_STOPLIMIT:
		if (msg.IsFieldEmpty(ROMFIELDS::CSV_STOPPRICE) == false)
		{
			
			SetReplacingStopPrice(msg.Get(ROMFIELDS::CSV_STOPPRICE));
		}
		else
		{
			SetReplacingStopPrice(StopPrice());
			msg.SetAt(ROMFIELDS::CSV_STOPPRICE, ReplacingStopPrice());
		}
		
		if (msg.IsFieldEmpty(ROMFIELDS::CSV_PRICE) == false)
		{
			m_ReplacingPrice = msg.dGet(ROMFIELDS::CSV_PRICE);
		}
		else
		{
			m_ReplacingPrice = Price();
			msg.SetAt(ROMFIELDS::CSV_PRICE, m_ReplacingPrice);
		}
		break;

	case ROMVALUE::TYPE_MARKET:
	case ROMVALUE::TYPE_MARKETONCLOSE:
	case ROMVALUE::TYPE_MARKETLIMIT:
	case ROMVALUE::TYPE_VWAP:
		msg.SetAt(ROMFIELDS::CSV_PRICE, "");
		break;
	default:
		if (msg.IsFieldEmpty(ROMFIELDS::CSV_PRICE) == false)
		{
			m_ReplacingPrice = msg.dGet(ROMFIELDS::CSV_PRICE);
		}
		else
		{
			m_ReplacingPrice = Price();
			msg.SetAt(ROMFIELDS::CSV_PRICE, m_ReplacingPrice);
		}
		break;
	}

	if (msg.IsFieldEmpty(ROMFIELDS::CSV_PEGPRICE_OFFSET) == false)
	{
		SetReplacingPegOffset(msg.Get(ROMFIELDS::CSV_PEGPRICE_OFFSET));
	}
	else
	{
		SetReplacingPegOffset(PegOffset());
		msg.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, ReplacingPegOffset());
	}

	if (msg.IsFieldEmpty(ROMFIELDS::CSV_TIF) == false)
	{
		m_ReplacingTIF = msg.nGet(ROMFIELDS::CSV_TIF);
	}
	else
	{
		m_ReplacingTIF = TIF();
		msg.SetAt(ROMFIELDS::CSV_TIF, m_ReplacingTIF);
	}

	if (ROMVALUE::TIF_GTD == m_ReplacingTIF)
	{
		if (msg.IsFieldEmpty(ROMFIELDS::CSV_GTD_DATE) == false)
		{
			CUtility::StrCpy(m_ReplacingExpTime, sizeof(m_ReplacingExpTime), msg.Get(ROMFIELDS::CSV_GTD_DATE));
		}
		else
		{
			CUtility::StrCpy(m_ReplacingExpTime, sizeof(m_ReplacingExpTime), m_ExpTime);
			msg.SetAt(ROMFIELDS::CSV_GTD_DATE, m_ReplacingExpTime);
		}
	}

	if (msg.IsFieldEmpty(ROMFIELDS::CSV_EXEC_INSTRUCTION) == false)
	{
		m_ReplacingExecInst = msg.nGet(ROMFIELDS::CSV_EXEC_INSTRUCTION);
	}
	else
	{
		m_ReplacingExecInst = ExecInst();
		msg.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ReplacingExecInst);
	}

	if (msg.IsFieldEmpty(ROMFIELDS::CSV_FLOOR) == false)
	{
		SetReplacingMaxFloor(msg.Get(ROMFIELDS::CSV_FLOOR));
	}
	else
	{
		SetReplacingMaxFloor(MaxFloor());
		msg.SetAt(ROMFIELDS::CSV_FLOOR, m_ReplacingMaxFloor);
	}

	if (msg.IsFieldEmpty(ROMFIELDS::CSV_INSTRUCTIONS) == false)
	{
		CUtility::StrCpy(m_ReplacingInstructions, sizeof(m_ReplacingInstructions), msg.Get(ROMFIELDS::CSV_INSTRUCTIONS));
	}
	else
	{
		CUtility::StrCpy(m_ReplacingInstructions, sizeof(m_ReplacingInstructions), m_Instructions);
		msg.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_ReplacingInstructions);
	}

    const char* newtag =    (msg.IsFieldEmpty(ROMFIELDS::CSV_NEW_TAG) == false)? msg.Get(ROMFIELDS::CSV_NEW_TAG) : ClientTag();
    
	SetNewClientTag(newtag);
	    
	if (clientType == ROMVALUE::CLIENTTYPE_FIX )
	{
		long shares = Shares();
		if (msg.IsFieldEmpty(ROMFIELDS::CSV_SHARES) == false)
		{
			shares =  msg.nGet(ROMFIELDS::CSV_SHARES);
		}
        m_ShareChange = shares - Cum() - Leave();
	}
    else
    {
		long shares = Leave();
		if (msg.IsFieldEmpty(ROMFIELDS::CSV_SHARES) == false)
		{
			shares =  msg.nGet(ROMFIELDS::CSV_SHARES);
		}
        m_ShareChange = shares - Leave();
    }
	

	m_PriceChange = m_ReplacingPrice - m_Price;
}

void CRomOrder::UpdateReplacedFields()
{
	if (strlen(NewClientTag()) > 0)
	{
		SetClientTag(NewClientTag());
	}
	SetPrice(m_ReplacingPrice);
	SetPegOffset(m_ReplacingPegOffset);
	SetStopPrice(m_ReplacingStopPrice);
    SetLeave(Leave() + m_ShareChange);
	SetTIF(m_ReplacingTIF);
	SetExecInst(m_ReplacingExecInst);
	SetType(m_ReplacingType);
	SetMaxFloor(m_ReplacingMaxFloor);
	SetExpTime(m_ReplacingExpTime);
	SetInstructions(m_ReplacingInstructions);
	SetShareChange(0);
	SetPriceChange(0);
}

std::string CRomOrder::GetFullOptionSymbol(CRomOrder* pOrder)
{
	if (strcmp(pOrder->ExpYearMonth(), "") == 0)
	{
		return "";
	}

	std::string month = pOrder->ExpYearMonth();
	month = month.substr(4, 2);
	std::string symbol = pOrder->Symbol();

	switch(atoi(month.c_str()))
	{
	case 1:
		symbol.append(" JAN "); break;
	case 2:
		symbol.append(" FEB "); break;
	case 3:
		symbol.append(" MAR "); break;
	case 4:
		symbol.append(" APR "); break;
	case 5:
		symbol.append(" MAY "); break;
	case 6:
		symbol.append(" JUN "); break;
	case 7:
		symbol.append(" JUL "); break;
	case 8:
		symbol.append(" AUG "); break;
	case 9:
		symbol.append(" SEP "); break;
	case 10:
		symbol.append(" OCT "); break;
	case 11:
		symbol.append(" NOV "); break;
	case 12:
		symbol.append(" DEC "); break;
	default:
		return "";
	}

	char value[50];
	sprintf(value, "%s%f %c", symbol.c_str(), pOrder->StrikePrice(), pOrder->CallOrPut());
	std::string rc = value;
	return rc;
}


void CRomOrder::Update(CRomCsv& hoststatus)
{
	short status = hoststatus.nGet(ROMFIELDS::CSV_STATUS);
    
    std::string orderTag = hoststatus.Get(ROMFIELDS::CSV_ORDER_TAG);
	hoststatus.SetAt(ROMFIELDS::CSV_TAG, ClientTag());
	hoststatus.SetAt(ROMFIELDS::CSV_PARENT_TAG, ParentClientTag());
	hoststatus.SetAt(ROMFIELDS::CSV_CLIENT_ECHO, ClientEcho());

	if (status != ROMVALUE::STATUS_FILLED &&
		status != ROMVALUE::STATUS_PARTIALLYFILLED)
	{
		hoststatus.SetAt(ROMFIELDS::CSV_LAST_SHARES, "0");
		hoststatus.SetAt(ROMFIELDS::CSV_LAST_PRICE, "0");
	}

	if ((status == ROMVALUE::STATUS_REPLACED ||
		 status == ROMVALUE::STATUS_OPEN) &&
		orderTag.empty() == false)
	{
		SetRomExchangeTag(orderTag.c_str());
	}

	orderTag = hoststatus.Get(ROMFIELDS::CSV_EXCHANGE_TAG);
	if (orderTag.empty() == false)
	{
		SetExchangeTag(orderTag.c_str());
	}

    if (status == ROMVALUE::STATUS_OPEN)
    {
		if (Type() == ROMVALUE::TYPE_LIMIT &&
			hoststatus.IsFieldEmpty(ROMFIELDS::CSV_PRICE) == false)
		{
			// in case price is improved
			SetPrice(hoststatus.dGet(ROMFIELDS::CSV_PRICE));
		}

		if (Type() == ROMVALUE::TYPE_STOP &&
			hoststatus.nGet(ROMFIELDS::CSV_TYPE) == ROMVALUE::TYPE_STOPLIMIT)
		{
			SetPrice(hoststatus.dGet(ROMFIELDS::CSV_PRICE));
			SetType(ROMVALUE::TYPE_STOPLIMIT);
		}
    }
    else if (status == ROMVALUE::STATUS_FILLED ||
		     status == ROMVALUE::STATUS_PARTIALLYFILLED)
	{
        long lastShares = (hoststatus.IsFieldEmpty(ROMFIELDS::CSV_LAST_SHARES) == false)? hoststatus.nGet(ROMFIELDS::CSV_LAST_SHARES) : hoststatus.nGet(ROMFIELDS::CSV_SHARES);
        double cash = Cum() * AvgPrice() + lastShares * hoststatus.dGet(ROMFIELDS::CSV_LAST_PRICE);
 		SetCum(Cum() + lastShares);
        SetLeave(Leave() - lastShares);
        SetAvgPrice(cash / Cum());
	}
    else if (status == ROMVALUE::STATUS_REPLACED)
	{
 		if (ReplacePending())
        {
			UpdateReplacedFields();
            SetReplacePending(false);
			SetShares(Cum() + Leave()); //current Fix order quantity

			hoststatus.SetAt(ROMFIELDS::CSV_NEW_TAG, ClientTag());
			hoststatus.SetAt(ROMFIELDS::CSV_PRICE, Price());
			hoststatus.SetAt(ROMFIELDS::CSV_STOPPRICE, StopPrice());
			//hoststatus.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, PegOffset());
			hoststatus.SetAt(ROMFIELDS::CSV_TIF, TIF());
			hoststatus.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, ExecInst());
			//hoststatus.SetAt(ROMFIELDS::CSV_TYPE, Type());
			hoststatus.SetAt(ROMFIELDS::CSV_FLOOR, MaxFloor());
        }
 	}
	else if (status == ROMVALUE::STATUS_REPLACEREJECTED)
    {
        SetReplacePending(false);
    }

	if (status == ROMVALUE::STATUS_CANCELED ||
		status == ROMVALUE::STATUS_ORDERREJECTED)
	{
		SetLeave(0);
	}
	else if (status == ROMVALUE::STATUS_EXPIRED ||
			 status == ROMVALUE::STATUS_DONEFORDAY)
	{
		if (TIF() == ROMVALUE::TIF_GTD)
		{
			//if the exptime is not newer than today make it out
			if (CUtility::LaterThanToday(ExpTime()) == false)
			{
				status = ROMVALUE::STATUS_EXPIRED;
				SetLeave(0);
			}
		}
		else if (TIF() != ROMVALUE::TIF_GTC)
		{
			status = ROMVALUE::STATUS_EXPIRED;
			SetLeave(0);
		}
	}

	if (IsActionStatus(status) == false)
	{
		SetStatus(status);
 	}

	//Adding all the fields required by FIX
	if (IsFutureType(SecType()) == true)
	{
		hoststatus.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, ExpYearMonth());
	}
	else if (IsOptionType(SecType()) == true)
	{
		hoststatus.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, ExpYearMonth());
		hoststatus.SetAt(ROMFIELDS::CSV_MATURITYDAY, MaturityDay());
		hoststatus.SetAt(ROMFIELDS::CSV_CALLPUT, CallOrPut());
		hoststatus.SetAt(ROMFIELDS::CSV_STRIKEPRICE, StrikePrice());
		hoststatus.SetAt(ROMFIELDS::CSV_OPENCLOSE, OpenClose());
		hoststatus.SetAt(ROMFIELDS::CSV_UNDERLYING, Underlying());
	}

	if (Type() == ROMVALUE::TYPE_STOP  ||
		Type() == ROMVALUE::TYPE_STOPLIMIT)
	{
		hoststatus.SetAt(ROMFIELDS::CSV_STOPPRICE, StopPrice());
	}

	switch(status)
	{
	case ROMVALUE::STATUS_CANCELED:
	case ROMVALUE::STATUS_CANCELREJECTED:
		hoststatus.SetAt(ROMFIELDS::CSV_NEW_TAG, ActClientTag());
		SetCancelSeq(0);
		break;
	case ROMVALUE::STATUS_REPLACED:
	case ROMVALUE::STATUS_REPLACEPENDING:
	case ROMVALUE::STATUS_REPLACEREJECTED:
		hoststatus.SetAt(ROMFIELDS::CSV_NEW_TAG, NewClientTag());
		SetReplaceSeq(0);
		break;
	}

	hoststatus.SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, ExchangeTag());
	hoststatus.SetAt(ROMFIELDS::CSV_SYMBOL, Symbol());
	hoststatus.SetAt(ROMFIELDS::CSV_AVG_PRICE, AvgPrice());
	hoststatus.SetAt(ROMFIELDS::CSV_CUM_SHARES, Cum());
	hoststatus.SetAt(ROMFIELDS::CSV_EXCHANGE,Exchange());
	hoststatus.SetAt(ROMFIELDS::CSV_SIDE, Side());
	hoststatus.SetAt(ROMFIELDS::CSV_TYPE, Type());
	hoststatus.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, Leave());
	hoststatus.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, SecType());
	hoststatus.SetAt(ROMFIELDS::CSV_SHARES, Shares());
	hoststatus.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, PegOffset());
	hoststatus.SetAt(ROMFIELDS::CSV_SEC_DEF, SecDef());
}

bool CRomOrder::IsDead(const CRomOrder* pOrder)
{
	if (pOrder->Leave() <= 0 && 
		pOrder->ReplacePending() == false)

	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CRomOrder::IsActionStatus(short status)
{
	std::set<short>::iterator it = s_ActionStatus.find(status);
	if (it != s_ActionStatus.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
void CRomOrder::SetLeave(long value)
{
	m_pCsv->SetAt(ROMFIELDS::CSV_LEAVE_SHARES, value);

    if (m_pParent)
    {
        // For simplicity updating more than neccessary does not hirt
        long newParentLeave = value / OriginalShares();
        m_pParent->SetLeave(newParentLeave);
    }
}*/

/*
void CRomOrder::SetCum(long value)
{
    m_pCsv->SetAt(ROMFIELDS::CSV_CUM_SHARES, value);

    if (m_pParent)  // update parent if needed
    {
        long newParentCum = value / OriginalShares();
        long delt = newParentCum - m_pParent->Cum();

        if (delt > 0)
        {
            m_pParent->SetCum(newParentCum);
            //m_pParent->SetLeave(m_pParent->Leave() - delt);
        }
    }
}*/
/*
void CRomOrder::SetPrice(double value)
{
	char buf[20];
	sprintf(buf, "%f", value);
	SetPrice(buf);
}*/

/*
void CRomOrder::AddChild(CRomOrder* pChild)
{
	if (NULL == m_Children)
	{
		m_Children = new tChildren;
	}

    m_Children->push_back(pChild);
}*/


void CRomOrder::InheritParent(CRomOrder* pOrder)
{
    //SetParent(pOrder);
	SetOwner(pOrder->Owner());
    SetTrader(pOrder->Trader());
    SetTradeFor(pOrder->TradeFor());
    SetFirm(pOrder->Firm());
    SetLocalAccount(pOrder->LocalAccount());
    SetClearingAccount(pOrder->ClearingAccount());
    SetClearingId(pOrder->ClearingId());
    SetExchange(pOrder->Exchange());
    SetDest(pOrder->Dest());
    SetExDest(pOrder->ExDest());
    SetTIF(pOrder->TIF());
    //SetSecType(pOrder->SecType());
    SetCMTAAccount(pOrder->CMTAAccount());
    SetGiveUpAccount(pOrder->GiveUpAccount());
    SetMMAccount(pOrder->MMAccount());
    SetMMExchange(pOrder->MMExchange());
    SetCapacity(pOrder->Capacity());
	SetSolicited(pOrder->Solicited());
    SetExecInst(pOrder->ExecInst());
    SetParentClientTag(pOrder->ClientTag());
    SetParentRomTag(pOrder->RomTag());
    SetType(pOrder->Type());
	SetClientEcho(pOrder->ClientEcho());
	
	SetLeave(m_Shares * pOrder->Shares());
	SetShares(Leave());
	SetOriginalShares(Leave());
}

std::string CRomOrder::Message()
{
    std::string order("");
    std::string sep = "!#!";
	CRomCsv csv;

	GetCsv(csv);
	order = csv.Message();

	/*if (HasChildren() == true)
	{
		tChildrenIT it = m_Children->begin();
		CRomOrder* pO = NULL;
		order += sep;

		while (it != m_Children->end())
		{
			pO = *it;
			order += pO->Message() + sep;
			it++;
		}

		CUtility::RemoveMidNewlines(order);
	}*/

	return order;
}

std::string CRomOrder::MessageSingle()
{
	CRomCsv csv;
	GetCsv(csv);
	return csv.Message();
}


void CRomOrder::GetCsv(CRomCsv& csv)
{
	csv.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ORDER);

	if (m_Type != ROMVALUE::TYPE_MARKET ||
		m_Type != ROMVALUE::TYPE_MARKETONCLOSE)
	{
		csv.SetAt(ROMFIELDS::CSV_PRICE, m_Price);
	}

	if (m_StopPrice != 0)
	{
		csv.SetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice);
	}

	csv.SetAt(ROMFIELDS::CSV_ALGOTYPE, m_AlgoType);
	csv.SetAt(ROMFIELDS::CSV_PRICE, m_Price);
	csv.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_PegOffset);
	csv.SetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    csv.SetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, m_OriginalShares);
    csv.SetAt(ROMFIELDS::CSV_CUM_SHARES, m_Cum);
    csv.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
    csv.SetAt(ROMFIELDS::CSV_FLOOR, m_MaxFloor);
	csv.SetAt(ROMFIELDS::CSV_MULTIPLIER, m_Multiplier);
	csv.SetAt(ROMFIELDS::CSV_MINQTY, m_MinQty);

    csv.SetAt(ROMFIELDS::CSV_TIF, m_TIF);
    csv.SetAt(ROMFIELDS::CSV_STATUS, m_Status);
    csv.SetAt(ROMFIELDS::CSV_SIDE, m_Side);
    csv.SetAt(ROMFIELDS::CSV_EXCHANGE, m_Exchange);
    csv.SetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest);
    csv.SetAt(ROMFIELDS::CSV_MM_EXCHANGE, m_MMExchange);
    csv.SetAt(ROMFIELDS::CSV_TYPE, m_Type);
	csv.SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, m_CplxType);

    csv.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ExecInst);
    csv.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType);
    csv.SetAt(ROMFIELDS::CSV_CAPACITY, m_Capacity);
	csv.SetAt(ROMFIELDS::CSV_SOLICITED, m_Solicited);
	csv.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, m_RomTag);
    csv.SetAt(ROMFIELDS::CSV_TAG, m_ClientTag);
	csv.SetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag);
    csv.SetAt(ROMFIELDS::CSV_ORDER_TAG, m_RomExchangeTag);
	csv.SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, m_ExchangeTag);
    csv.SetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag);
	csv.SetAt(ROMFIELDS::CSV_PARENT_STAFFORD_TAG, m_ParentRomTag);
    csv.SetAt(ROMFIELDS::CSV_OWNER, m_Owner);
    csv.SetAt(ROMFIELDS::CSV_TRADER, m_Trader);
    csv.SetAt(ROMFIELDS::CSV_TRADE_FOR, m_TradeFor);
    csv.SetAt(ROMFIELDS::CSV_CURRENCY, m_Currency);
    csv.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth);
    csv.SetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol);
	csv.SetAt(ROMFIELDS::CSV_DBSYMBOL, m_DBSymbol);
	csv.SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
   // csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_NewClientTag);
   // csv.SetAt(ROMFIELDS::CSV_ACTTAG, m_ActClientTag);
    csv.SetAt(ROMFIELDS::CSV_FIRM, m_Firm);
    csv.SetAt(ROMFIELDS::CSV_LOCALACCOUNT, m_LocalAccount);
    csv.SetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, m_ClearingAccount);
    csv.SetAt(ROMFIELDS::CSV_CMTAACCOUNT, m_CMTAAccount);
    csv.SetAt(ROMFIELDS::CSV_GIVEUPACCOUNT, m_GiveUpAccount);
    csv.SetAt(ROMFIELDS::CSV_CLEARINGID, m_ClearingId);
    csv.SetAt(ROMFIELDS::CSV_EXDEST, m_ExDest);
	csv.SetAt(ROMFIELDS::CSV_SHORT_LENDER, m_ShortLender);
	csv.SetAt(ROMFIELDS::CSV_GTD_DATE, m_ExpTime);
	csv.SetAt(ROMFIELDS::CSV_AVG_PRICE, AvgPrice());

	if (strlen(m_RomExchangeTag) == 0)
	{
		csv.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_Instructions);
	}

	csv.SetAt(ROMFIELDS::CSV_SEC_ID, m_SecID);
	csv.SetAt(ROMFIELDS::CSV_SEC_DEF, m_SecDef);

	csv.SetAt(ROMFIELDS::CSV_MM_ACCOUNT, m_MMAccount);

	if (CRomOrder::IsOptionType(m_SecType) == true)
	{
		csv.SetAt(ROMFIELDS::CSV_CALLPUT, m_CallOrPut);
		csv.SetAt(ROMFIELDS::CSV_STRIKEPRICE, m_StrikePrice);
		csv.SetAt(ROMFIELDS::CSV_OPENCLOSE, (short)m_OpenClose);
		csv.SetAt(ROMFIELDS::CSV_MATURITYDAY, MaturityDay());
	}

	csv.SetAt(ROMFIELDS::CSV_TIME, m_TimeStamp);
	csv.SetAt(ROMFIELDS::CSV_OM_TIME, CUtility::GetGmtTimeStrMM());
}

/*
CRomOrder* CRomOrder::GetLegFromSecID(const std::string& secid)
{
    CRomOrder* pOrder = NULL;
    tChildrenIT it = m_Children->begin();

    while (it != m_Children->end())
    {
        if ((*it)->SecID() == secid)
        {
            pOrder = *it;
            break;
        }
		it++;
    }

    return pOrder;
}*/

void CRomOrder::Set(CRomCsv& csv)
{
	SingleSet(csv);
}

void CRomOrder::SingleSet(CRomCsv& csv)
{
	//m_Children = NULL;
	//m_pParent = NULL;
    m_LegCount = 0;
	m_Used = true;

	strcpy(m_RomExchangeTag, "");
	strcpy(m_ActClientTag, "");
	strcpy(m_NewClientTag, "");

	m_Price = csv.dGet(ROMFIELDS::CSV_PRICE);

	SetPegOffset(csv.Get(ROMFIELDS::CSV_PEGPRICE_OFFSET));

    m_StrikePrice = csv.dGet(ROMFIELDS::CSV_STRIKEPRICE);
	m_AvgPrice = 0;
	SetStopPrice(csv.Get(ROMFIELDS::CSV_STOPPRICE));
	SetReplacingStopPrice(StopPrice());

	m_ReplacingPrice = m_Price;
	m_PriceChange = 0;

	m_NextExecId = 0;
    m_Shares = csv.nGet(ROMFIELDS::CSV_SHARES);
    m_OriginalShares = m_Shares;
    m_Cum = 0;
    m_Leave = m_Shares;

    SetMaxFloor(csv.Get(ROMFIELDS::CSV_FLOOR));
	SetReplacingMaxFloor(MaxFloor());

	m_ShareChange = 0;
	
	m_Multiplier = csv.nGet(ROMFIELDS::CSV_MULTIPLIER);
	if (0 == m_Multiplier)
	{
		m_Multiplier = 1;
	}

	m_MinQty = csv.nGet(ROMFIELDS::CSV_MINQTY);

    m_TIF = csv.nGet(ROMFIELDS::CSV_TIF);
    m_Status = 0;
    m_Side = csv.nGet(ROMFIELDS::CSV_SIDE);
    m_Exchange = csv.nGet(ROMFIELDS::CSV_EXCHANGE);
    m_Dest = csv.nGet(ROMFIELDS::CSV_ORDERDEST);
    m_MMExchange = csv.nGet(ROMFIELDS::CSV_MM_EXCHANGE);
    m_Type = csv.nGet(ROMFIELDS::CSV_TYPE);
   	m_ReplacingTIF = m_TIF;
	m_ReplacingType = m_Type;
	m_LegCount;
	m_AlgoType = csv.nGet(ROMFIELDS::CSV_ALGOTYPE);

    m_ExecInst = csv.cGet(ROMFIELDS::CSV_EXEC_INSTRUCTION);

	if (csv.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE))
	{
		m_SecType = 'E';
	}
	else
	{
		m_SecType = csv.cGet(ROMFIELDS::CSV_SECURITY_TYPE);
	}

    m_CallOrPut = csv.cGet(ROMFIELDS::CSV_CALLPUT);
    m_Capacity = csv.cGet(ROMFIELDS::CSV_CAPACITY);
	m_Solicited = csv.cGet(ROMFIELDS::CSV_SOLICITED);

	m_ReplacingExecInst = m_ExecInst;

	SetRomTag(csv.Get(ROMFIELDS::CSV_STAFFORD_TAG)); 
    //SetRomExecTag("");
	SetSecDef(csv.Get(ROMFIELDS::CSV_SEC_DEF));
	SetClientTag(csv.Get(ROMFIELDS::CSV_TAG));
    SetRomExchangeTag("");
	SetParentClientTag(csv.Get(ROMFIELDS::CSV_PARENT_TAG));
    SetParentRomTag(csv.Get(ROMFIELDS::CSV_PARENT_STAFFORD_TAG));
	SetOwner(csv.Get(ROMFIELDS::CSV_OWNER));
	SetTrader(csv.Get(ROMFIELDS::CSV_TRADER));
	SetTradeFor(csv.Get(ROMFIELDS::CSV_TRADE_FOR));
	SetCurrency(csv.Get(ROMFIELDS::CSV_CURRENCY));
	SetExpYearMonth(csv.Get(ROMFIELDS::CSV_EXPYEARMONTH));
	SetSymbol(csv.Get(ROMFIELDS::CSV_SYMBOL));
	SetShortLender(csv.Get(ROMFIELDS::CSV_SHORT_LENDER));
	SetSecDef(csv.Get(ROMFIELDS::CSV_SEC_DEF));
	//SetSecID(csv.Get(ROMFIELDS::CSV_SEC_ID));
	//SetSingleTimeStamp(csv.Get(ROMFIELDS::CSV_TIME));

	if (csv.IsFieldEmpty(ROMFIELDS::CSV_DBSYMBOL) == false)
	{
		SetDBSymbol(csv.Get(ROMFIELDS::CSV_DBSYMBOL));
	}
	else
	{
		SetDBSymbol(m_Symbol);
	}

	SetUnderlying(csv.Get(ROMFIELDS::CSV_UNDERLYING));
    SetNewClientTag("");
    SetActClientTag("");
    SetFirm(csv.Get(ROMFIELDS::CSV_FIRM));
    SetLocalAccount(csv.Get(ROMFIELDS::CSV_LOCALACCOUNT));
    SetClearingAccount(csv.Get(ROMFIELDS::CSV_CLEARINGACCOUNT));
    SetCMTAAccount(csv.Get(ROMFIELDS::CSV_CMTAACCOUNT));
    SetGiveUpAccount(csv.Get(ROMFIELDS::CSV_GIVEUPACCOUNT));
    SetClearingId(csv.Get(ROMFIELDS::CSV_CLEARINGID));
    SetExDest(csv.Get(ROMFIELDS::CSV_EXDEST));
	SetSingleTimeStamp((CUtility::GetTimeHHMMSS()).c_str());
    SetInstructions(csv.Get(ROMFIELDS::CSV_INSTRUCTIONS));
	CUtility::StrCpy(m_ReplacingInstructions, sizeof(m_ReplacingInstructions), m_Instructions);
	SetMMAccount(csv.Get(ROMFIELDS::CSV_MM_ACCOUNT));
	SetSecID(csv.Get(ROMFIELDS::CSV_SEC_ID));
	SetExpTime(csv.Get(ROMFIELDS::CSV_GTD_DATE));
	CUtility::StrCpy(m_ReplacingExpTime, sizeof(m_ReplacingExpTime), m_ExpTime);
	SetClientEcho(csv.Get(ROMFIELDS::CSV_CLIENT_ECHO));
	SetMaturityDay(csv.Get(ROMFIELDS::CSV_MATURITYDAY));

	m_ReplacePending = false;
	m_OpenClose = csv.nGet(ROMFIELDS::CSV_OPENCLOSE);
	m_CplxType = csv.nGet(ROMFIELDS::CSV_CPLXORDER_TYPE);
}

void CRomOrder::SetTimeStamp(const char* value)
{
	SetSingleTimeStamp(value);
	/*if (m_Children)
	{
		CRomOrder* pOrder = NULL;
		tChildrenIT it = m_Children->begin();
		while (it != m_Children->end())
		{
			pOrder = *it;
			if (pOrder)
			{
				pOrder->SetSingleTimeStamp(value);
			}
			it++;
		}
	}*/
}

int CRomOrder::SideSign()const
{
	if (Side() == ROMVALUE::SIDE_BUY)
	{
		return 1;
	}
	else 
	{
		return -1;
	}
}

/*CRomCsv CRomOrder::csv()const
{
}*/
