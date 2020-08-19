#include <sstream>
#include <math.h>
#include <vector>
#include "RomOrderNew.h"
#include "Utility.h"
#include "ClientProfileManagerNew.h"
#include "ClientsManager.h"
#include "Config.h"
#include "Oats.h"
#include "DestsManager.h"
#include "BuffStore.h"
#include "debug_logger.h"
#include "fix_rom_trans.h"
#include "MDSClient.h"
#include "PositionManager.h"


std::set<short> CRomOrderNew::s_ActionStatus;

void CRomOrderNew::Init()
{
    CRomOrderNew::LoadActionStatus();
}

void CRomOrderNew::LoadActionStatus()
{
    s_ActionStatus.insert(ROMVALUE::STATUS_PENDINGCANCEL);
    s_ActionStatus.insert(ROMVALUE::STATUS_REPLACEPENDING);
    s_ActionStatus.insert(ROMVALUE::STATUS_CANCELREJECTED);
    s_ActionStatus.insert(ROMVALUE::STATUS_REPLACEREJECTED);
}

CRomOrderNew::~CRomOrderNew()
{
    if (m_pCoveredSpread)
    {
        delete m_pCoveredSpread;
    }
}

CRomOrderNew::CRomOrderNew()
{
    memset(this, 0, sizeof(CRomOrderNew));
    m_Ratio = 1;
    m_CurrencyRate = 1;
    m_NextExecId = 0;
    m_mixedSpread = false;
    m_needPosMgr = false;
    m_secIndex = ROMVALUE::EQUITY;
    m_pCoveredSpread = NULL;
}

void CRomOrderNew::SetNextExecIdFromExecTag(const char* value)
{
    std::string sValue = value;
    std::string::size_type index = sValue.find_last_of('-');
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


void CRomOrderNew::NextRomExecTag(std::string& tag)
{
    char value[100];
    sprintf(value, "%s-%ld", RomTag(), m_NextExecId++);
    tag = value;
}

void CRomOrderNew::nextRomExecTag(std::string& tag)
{
    char value[100];
    sprintf(value, "%s-%ld", RomTag(), m_NextExecId);
    tag = value;
}

bool CRomOrderNew::IsLimitType(short orderType)
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

void CRomOrderNew::UpdateReplacingFields(CCsv& msg, tKeyedBalanceUpdate& update)
{
    //need to define a replaceble fields list!!!
    int precision = (m_SecType == ROMVALUE::SECTYPE_EQUITY)? 4 : 8;
    
    if (msg.GetAt(ROMFIELDS::CSV_TYPE, m_ReplacingType) == false)
    {
        m_ReplacingType = Type();
        msg.SetAt(ROMFIELDS::CSV_TYPE, m_ReplacingType);
    }

    if (msg.GetAt(ROMFIELDS::CSV_SIDE, m_ReplacingSide) == false)
    {
        m_ReplacingSide = Side();
        msg.SetAt(ROMFIELDS::CSV_SIDE, m_ReplacingSide);
    }
    
    switch(m_ReplacingType)
    {
    case ROMVALUE::TYPE_STOP:
        msg.SetAt(ROMFIELDS::CSV_PRICE, "");
        break;
    case ROMVALUE::TYPE_STOPLIMIT:
        if (msg.GetAt(ROMFIELDS::CSV_STOPPRICE, m_ReplacingStopPrice, sizeof(m_ReplacingStopPrice)) == false)
        {
            SetReplacingStopPrice(StopPrice());
            msg.SetAt(ROMFIELDS::CSV_STOPPRICE, ReplacingStopPrice());
        }
       
        if (msg.GetAt(ROMFIELDS::CSV_PRICE, m_ReplacingPrice) == false)
        {
            m_ReplacingPrice = Price();
            msg.SetAt(ROMFIELDS::CSV_PRICE, m_ReplacingPrice, precision);
        }
        
        break;

    case ROMVALUE::TYPE_MARKET:
    case ROMVALUE::TYPE_MARKETONCLOSE:
    case ROMVALUE::TYPE_MARKETLIMIT:
    case ROMVALUE::TYPE_VWAP:
        msg.SetAt(ROMFIELDS::CSV_PRICE, "");
        break;
    default:
        if (msg.GetAt(ROMFIELDS::CSV_PRICE, m_ReplacingPrice) == false)
        {
            m_ReplacingPrice = Price();
            msg.SetAt(ROMFIELDS::CSV_PRICE, m_ReplacingPrice, precision);
        }
       
        break;
    }

    if (msg.GetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_ReplacingPegOffset, sizeof(m_ReplacingPegOffset)) == false)
    {
        SetReplacingPegOffset(PegOffset());
        msg.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, ReplacingPegOffset());
    }

    if (msg.GetAt(ROMFIELDS::CSV_TIF, m_ReplacingTIF) == false)
    {
        m_ReplacingTIF = TIF();
        msg.SetAt(ROMFIELDS::CSV_TIF, m_ReplacingTIF);
    }

    if (ROMVALUE::TIF_GTD == m_ReplacingTIF)
    {
        if (msg.GetAt(ROMFIELDS::CSV_GTD_DATE, m_ReplacingExpTime, sizeof(m_ReplacingExpTime)) == false)
        {
            CUtility::StrCpy(m_ReplacingExpTime, sizeof(m_ReplacingExpTime), m_ExpTime);
            msg.SetAt(ROMFIELDS::CSV_GTD_DATE, m_ReplacingExpTime);
        }
    }

    if (msg.GetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ReplacingExecInst) == false)
    {
        m_ReplacingExecInst = ExecInst();
        msg.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ReplacingExecInst);
    }

    if (msg.GetAt(ROMFIELDS::CSV_FLOOR, m_ReplacingMaxFloor, sizeof(m_ReplacingMaxFloor)) == false)
    {
        SetReplacingMaxFloor(MaxFloor());
        msg.SetAt(ROMFIELDS::CSV_FLOOR, m_ReplacingMaxFloor);
    }

    if (msg.GetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_ReplacingInstructions, sizeof(m_ReplacingInstructions)) == false)
    {
        CUtility::StrCpy(m_ReplacingInstructions, sizeof(m_ReplacingInstructions), m_Instructions);
        msg.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_ReplacingInstructions);
    }

    if (msg.GetAt(ROMFIELDS::CSV_MINQTY, m_ReplacingMinQty) == false)
    {
        m_ReplacingMinQty = 0;
    }

    std::string newtag;
    if (msg.GetAt(ROMFIELDS::CSV_NEW_TAG, newtag) == false)
    {
        newtag = ClientTag();
    }

    SetNewClientTag(newtag.c_str());

    long shares = 0;
    if (msg.GetAt(ROMFIELDS::CSV_SHARES, shares) == false)
    {
        shares = Shares();
    }
    m_ShareChange = shares - Cum() - Leave();

    double replacingPrice = m_ReplacingPrice;

    if (ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == m_CplxType ||
        ROMVALUE::CPLXTYPE_SPREAD == m_CplxType)
    {
        replacingPrice = fabs(replacingPrice);
        m_PriceChange = replacingPrice - fabs(m_Price);
    }
    else
    {
        m_PriceChange = m_ReplacingPrice - m_Price;
    }

    m_ReplacePending = true;

    update.clearing_sec =m_ClearingSecKey;
    update.update.secIndex = m_secIndex;
    update.username = m_Owner;
    if (ROMVALUE::FUTURE == update.update.secIndex)
    {
        update.product = Product();
    }

    update.update.orderChange = 0;
    update.update.secIndex = m_secIndex;

    update.update.shareChange = m_ShareChange * m_Ratio;
    update.update.dollarChange = m_Multiplier * m_CurrencyRate *  (m_PriceChange * m_Shares + m_ShareChange * replacingPrice);
    m_ReplacingDelt = update.update;
    m_loadedReplacePending = false;

    if (m_pCoveredSpread)
    {
        m_pCoveredSpread->UpdateODeltDollars(m_Price, m_ReplacingPrice, m_Shares, m_Shares + m_ShareChange);
    }
}


void CRomOrderNew::UpdateReplacedFields(CCsv& msg)
{
    if (strlen(m_NewClientTag) > 0)
    {
        SetClientTag(m_NewClientTag);
    }

    double priceChange = m_ReplacingPrice - m_Price;

    if (m_loadedReplacePending == false)
    {
        SetPrice(m_ReplacingPrice);
        SetTIF(m_ReplacingTIF);
        SetType(m_ReplacingType);
        SetMaxFloor(m_ReplacingMaxFloor);
        SetPegOffset(m_ReplacingPegOffset);
        SetStopPrice(m_ReplacingStopPrice);
        SetExecInst(m_ReplacingExecInst);
        SetExpTime(m_ReplacingExpTime);
        SetInstructions(m_ReplacingInstructions);
        SetLeave(Leave() + m_ShareChange);
        m_MinQty = m_ReplacingMinQty;
        SetSide(m_ReplacingSide);
    }
    else
    {
        //use whatever received from exchange server
        msg.GetAt(ROMFIELDS::CSV_PRICE, m_Price);            
        msg.GetAt(ROMFIELDS::CSV_SHARES, m_Shares);
        msg.GetAt(ROMFIELDS::CSV_TYPE, m_Type);
        msg.GetAt(ROMFIELDS::CSV_FLOOR, m_MaxFloor, sizeof(m_MaxFloor));
        msg.GetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ExecInst);
        msg.GetAt(ROMFIELDS::CSV_TIF, m_TIF);
        msg.GetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_PegOffset , sizeof(m_PegOffset));
        msg.GetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_Instructions, sizeof(m_Instructions));
        msg.GetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice, sizeof(m_StopPrice));
        msg.GetAt(ROMFIELDS::CSV_GTD_DATE, m_ExpTime, sizeof(m_ExpTime));
        msg.GetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
        msg.GetAt(ROMFIELDS::CSV_MINQTY, m_MinQty);
        short side = ROMVALUE::SIDE_BUY;
        msg.GetAt(ROMFIELDS::CSV_SIDE, side);
        SetSide(side);
        msg.GetAt(ROMFIELDS::CSV_SIDE, m_Side);
        
        m_loadedReplacePending = false;
    }
    if (m_pCoveredSpread)
    {
        m_pCoveredSpread->UpdateODollars(m_Price);
        m_pCoveredSpread->SetODeltDollars(0);
    }
}


bool CRomOrderNew::IsDead(const CRomOrderNew* pOrder)
{
    if (pOrder->Leave() <= 0 &&
        pOrder->Status() != ROMVALUE::STATUS_REPLACED)// &&
        //pOrder->ReplacePending() == false)// &&
        //pOrder->CplxType() != ROMVALUE::CPLXTYPE_PRODUCT_SPREAD)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CRomOrderNew::IsActionStatus(short status)
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
void CRomOrderNew::SetLeave(long value)
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
void CRomOrderNew::SetCum(long value)
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
void CRomOrderNew::SetPrice(double value)
{
	char buf[20];
	sprintf(buf, "%f", value);
	SetPrice(buf);
}*/

/*
void CRomOrderNew::AddChild(CRomOrderNew* pChild)
{
	if (NULL == m_Children)
	{
		m_Children = new tChildren;
	}

    m_Children->push_back(pChild);
}*/


void CRomOrderNew::InheritParent(CRomOrderNew* pParentOrder)
{
    SetOwner(pParentOrder->Owner());
    SetTrader(pParentOrder->Trader());
    SetTradeFor(pParentOrder->TradeFor());
    SetFirm(pParentOrder->Firm());
    SetLocalAccount(pParentOrder->LocalAccount());
    SetClearingAccount(pParentOrder->ClearingAccount());
    SetClearingId(pParentOrder->ClearingId());
    SetExchange(pParentOrder->Exchange());
    SetDest(pParentOrder->Dest());
    SetExDest(pParentOrder->ExDest());
    SetTIF(pParentOrder->TIF());
    //SetSecType(pOrder->SecType());
    SetCMTAAccount(pParentOrder->CMTAAccount());
    SetGiveUpAccount(pParentOrder->GiveUpAccount());
    SetMMAccount(pParentOrder->MMAccount());
    SetMMExchange(pParentOrder->MMExchange());
    SetCapacity(pParentOrder->Capacity());
    SetSolicited(pParentOrder->Solicited());
    SetExecInst(pParentOrder->ExecInst());
    SetParentClientTag(pParentOrder->ClientTag());
    SetParentRomTag(pParentOrder->RomTag());
    SetType(pParentOrder->Type());
    SetClientEcho(pParentOrder->ClientEcho());
    SetOriginalShares(m_Shares);             //the m_OriginalShares will always keep the ratio,
                                             //it will be used to calculate parent leave.
    SetLeave(m_Shares * pParentOrder->Shares());
    SetShares(Leave());
    m_needPosMgr = false;

    //SetSideIndexFromParentSide(pParentOrder->Side());
}



void CRomOrderNew::GetCsv(CCsv& csv) const
{
    csv.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ORDER);
    int precision = (m_SecType == ROMVALUE::SECTYPE_EQUITY)? 4 : 8;
    
    if (m_Type != ROMVALUE::TYPE_MARKET &&
        m_Type != ROMVALUE::TYPE_MARKETONCLOSE)
    {
        csv.SetAt(ROMFIELDS::CSV_PRICE, m_Price, precision);
    }

    if (strcmp(m_StopPrice, "0") != 0)
    {
        csv.SetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice);
    }

    csv.SetAt(ROMFIELDS::CSV_ALGOTYPE, m_AlgoType);
    //csv.SetAt(ROMFIELDS::CSV_PRICE, m_Price);
    csv.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_PegOffset);
    csv.SetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    csv.SetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, m_OriginalShares);
    csv.SetAt(ROMFIELDS::CSV_CUM_SHARES, m_Cum);
    csv.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
    csv.SetAt(ROMFIELDS::CSV_FLOOR, m_MaxFloor);
    csv.SetAt(ROMFIELDS::CSV_MULTIPLIER, m_Multiplier, precision);

    if (m_MinQty)
    {
        csv.SetAt(ROMFIELDS::CSV_MINQTY, m_MinQty);
    }

    csv.SetAt(ROMFIELDS::CSV_TIF, m_TIF);
    csv.SetAt(ROMFIELDS::CSV_STATUS, m_Status);
    csv.SetAt(ROMFIELDS::CSV_SIDE, m_Side);
    csv.SetAt(ROMFIELDS::CSV_EXCHANGE, m_Exchange);
    csv.SetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest);
    csv.SetAt(ROMFIELDS::CSV_MM_EXCHANGE, m_MMExchange);
    csv.SetAt(ROMFIELDS::CSV_TYPE, m_Type);

    if (m_CplxType)
    {
        csv.SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, m_CplxType);
    }
   
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
    //csv.SetAt(ROMFIELDS::CSV_CURRENCY, m_Currency);
    csv.SetAt(ROMFIELDS::CSV_PRODUCTID, m_ProductID);
    csv.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth);
    csv.SetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol);
    //csv.SetAt(ROMFIELDS::CSV_DBSYMBOL, m_DBSymbol);
    csv.SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
    csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_NewClientTag);
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
    csv.SetAt(ROMFIELDS::CSV_AVG_PRICE, m_AvgPrice, precision);

    if (strlen(m_RomExchangeTag) == 0)
    {
        csv.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_Instructions);
    }

    csv.SetAt(ROMFIELDS::CSV_SEC_ID, m_SecID);
    csv.SetAt(ROMFIELDS::CSV_SEC_DEF, m_SecDef);

    csv.SetAt(ROMFIELDS::CSV_MM_ACCOUNT, m_MMAccount);

    if (CUtility::IsOptionType(m_SecType) == true)
    {
        if (csv.IsFieldEmpty(ROMFIELDS::CSV_CALLPUT))
        {
            csv.SetAt(ROMFIELDS::CSV_CALLPUT, m_CallOrPut);
        }
        if (csv.IsFieldEmpty(ROMFIELDS::CSV_STRIKEPRICE))
        {
            csv.SetAt(ROMFIELDS::CSV_STRIKEPRICE, m_StrikePrice);
        }
        if (csv.IsFieldEmpty(ROMFIELDS::CSV_OPENCLOSE))
        {
            //csv.SetAt(ROMFIELDS::CSV_OPENCLOSE, (short)m_OpenClose);
            csv.SetAt(ROMFIELDS::CSV_OPENCLOSE, m_OpenClose);
        }
        if (csv.IsFieldEmpty(ROMFIELDS::CSV_MATURITYDAY))
        {
            csv.SetAt(ROMFIELDS::CSV_MATURITYDAY, MaturityDay());
        }
    }

    csv.SetAt(ROMFIELDS::CSV_CLIENT_ECHO, m_ClientEcho);
    //csv.SetAt(ROMFIELDS::CSV_TIME, m_TimeStamp);
    //csv.SetAt(ROMFIELDS::CSV_OM_TIME, CUtility::GetGmtTimeStr());
}

/*
CRomOrderNew* CRomOrderNew::GetLegFromSecID(const std::string& secid)
{
    CRomOrderNew* pOrder = NULL;
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

void CRomOrderNew::Reset()
{
    if (m_pCoveredSpread)
    {
        delete m_pCoveredSpread;
        m_pCoveredSpread = NULL;
    }
    memset(this, 0, sizeof(CRomOrderNew));
    m_CurrencyRate = 1;
    m_NextExecId = 0;
    m_Ratio = 1;
}

//void CRomOrderNew::Set(CCsv& csv)
//{
//    SingleSet(csv);
//}

unsigned short CRomOrderNew::GetSecIndexFromSecType(char secType)
{
    unsigned short rc = ROMVALUE::EQUITY;

    if (CUtility::IsEquityType(secType))
    {
        rc = ROMVALUE::EQUITY;
    }
    else if (CUtility::IsOptionType(secType))
    {
        if (ROMVALUE::SECTYPE_OPTIONFUTURE == secType)
        {
            rc = ROMVALUE::OPTIONONFUTURE;
        }
        else
        {
            rc = ROMVALUE::OPTION;
        }
    }
    else if (CUtility::IsFutureType(secType))
    {
        rc = ROMVALUE::FUTURE;
    }

    return rc;
}

unsigned short CRomOrderNew::GetSideIndexFromSide(short side)
{
    if (side == ROMVALUE::SIDE_BUY)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void CRomOrderNew::SingleSet(CCsv& csv)
{
    m_Used = true;
    m_Multiplier = 0;  //indicate client did not set it!
    m_Ratio = 1;

    //strcpy(m_RomExchangeTag, "");
    //strcpy(m_ActClientTag, "");
    //strcpy(m_NewClientTag, "");

    if (csv.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType) == false)
    {
        m_SecType = 'E';
        csv.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, 'E');
    }

    m_secIndex = CRomOrderNew::GetSecIndexFromSecType(m_SecType);

    std::string omExTag;
    if (csv.GetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag))
    {
        SetNextExecIdFromExecTag(omExTag.c_str());
    }

    int nbuf = 0;
    csv.GetAt(ROMFIELDS::CSV_TYPE, nbuf);
    m_Type = nbuf;

    if (CUtility::IsEquityType(m_SecType) && IsLimitType(m_Type))
    {
        csv.GetAt(ROMFIELDS::CSV_PRICE, m_Price);
        csv.GetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice, sizeof(m_StopPrice));
    }
    else
    {
        csv.GetAt(ROMFIELDS::CSV_PRICE, m_Price);
        csv.GetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice, sizeof(m_StopPrice));
        SetReplacingStopPrice(StopPrice());
    }

    //csv.GetAt(ROMFIELDS::CSV_PRICE, m_Price);
    csv.GetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_PegOffset, sizeof(m_PegOffset));

    //double strike = 0;
    //if (csv.GetAt(ROMFIELDS::CSV_STRIKEPRICE,  strike))
    //{
    //    m_StrikePrice.Set(strike);
    //}
    csv.GetAt(ROMFIELDS::CSV_STRIKEPRICE,  m_StrikePrice, sizeof(m_StrikePrice));
    //m_AvgPrice = 0;
    //csv.GetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice, sizeof(m_StopPrice));
    //SetReplacingStopPrice(StopPrice());

    m_ReplacingPrice = m_Price;
    //m_PriceChange = 0;

    //m_NextExecId = 0;
    csv.GetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    m_OriginalShares = m_Shares;
    //m_Cum = 0;
    m_Leave = m_Shares;

    csv.GetAt(ROMFIELDS::CSV_FLOOR, m_MaxFloor, sizeof(m_MaxFloor));
    SetReplacingMaxFloor(MaxFloor());

    //m_ShareChange = 0;

    csv.GetAt(ROMFIELDS::CSV_MINQTY, m_MinQty);
    m_ReplacingMinQty = m_MinQty;

    nbuf = 0;
    csv.GetAt(ROMFIELDS::CSV_TIF, nbuf);
    m_TIF = nbuf;
    //m_Status = 0;
    nbuf = 0;
    csv.GetAt(ROMFIELDS::CSV_SIDE, nbuf);
    m_Side = nbuf;
    m_sideIndex = CRomOrderNew::GetSideIndexFromSide(m_Side);

    nbuf = 0;
    std::string exchange;
    csv.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange);
    SetExchange(exchange.c_str());
    //nbuf = 0;
    std::string dest;
    csv.GetAt(ROMFIELDS::CSV_ORDERDEST, dest);
    SetDest(dest.c_str());
    //nbuf = 0;
    csv.GetAt(ROMFIELDS::CSV_MM_EXCHANGE, nbuf);
    m_MMExchange = nbuf;
    //nbuf = 0;
    //csv.GetAt(ROMFIELDS::CSV_TYPE, nbuf);
    //m_Type = nbuf;
    nbuf = 0;
    m_ReplacingTIF = m_TIF;
    m_ReplacingType = m_Type;
    //m_LegCount;
    csv.GetAt(ROMFIELDS::CSV_ALGOTYPE, nbuf);
    m_AlgoType = nbuf;
    csv.GetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ExecInst);

    //if (csv.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType) == false)
    //{
    //    m_SecType = 'E';
    //    csv.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, 'E');
    //}

    csv.GetAt(ROMFIELDS::CSV_CALLPUT, m_CallOrPut);
    csv.GetAt(ROMFIELDS::CSV_CAPACITY, m_Capacity);
    csv.GetAt(ROMFIELDS::CSV_SOLICITED, m_Solicited);

    m_ReplacingExecInst = m_ExecInst;

    //SetRomTag("");
    csv.GetAt(ROMFIELDS::CSV_STAFFORD_TAG, m_RomTag, sizeof(m_RomTag));
    csv.GetAt(ROMFIELDS::CSV_SEC_DEF, m_SecDef, sizeof(m_SecDef));
    csv.GetAt(ROMFIELDS::CSV_TAG, m_ClientTag, sizeof(m_ClientTag));
    //SetRomExchangeTag("");
    //SetParentClientTag("");
    //SetParentRomTag("");
    csv.GetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag, sizeof(m_ParentClientTag));
    csv.GetAt(ROMFIELDS::CSV_PARENT_STAFFORD_TAG, m_ParentRomTag, sizeof(m_ParentRomTag));
    csv.GetAt(ROMFIELDS::CSV_OWNER, m_Owner, sizeof(m_Owner));
    csv.GetAt(ROMFIELDS::CSV_TRADER, m_Trader, sizeof(m_Trader));
    csv.GetAt(ROMFIELDS::CSV_TRADE_FOR, m_TradeFor, sizeof(m_TradeFor));
    //csv.GetAt(ROMFIELDS::CSV_CURRENCY, m_Currency, sizeof(m_Currency));
    csv.GetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth, sizeof(m_ExpYearMonth));
    csv.GetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol, sizeof(m_Symbol));
    csv.GetAt(ROMFIELDS::CSV_SHORT_LENDER, m_ShortLender, sizeof(m_ShortLender));
    csv.GetAt(ROMFIELDS::CSV_SEC_DEF, m_SecDef, sizeof(m_SecDef));

    if (csv.GetAt(ROMFIELDS::CSV_PRODUCTID, m_ProductID, sizeof(m_ProductID)) == false)
    {
        SetProductID("");
    }
    else
    {
        csv.GetAt(ROMFIELDS::CSV_PRODUCTID, m_ProductID, sizeof(m_ProductID));
    }


    csv.GetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying, sizeof(m_Underlying));
    csv.GetAt(ROMFIELDS::CSV_FIRM, m_Firm, sizeof(m_Firm));
    csv.GetAt(ROMFIELDS::CSV_LOCALACCOUNT, m_LocalAccount, sizeof(m_LocalAccount));
    csv.GetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, m_ClearingAccount, sizeof(m_ClearingAccount));
    csv.GetAt(ROMFIELDS::CSV_CMTAACCOUNT, m_CMTAAccount, sizeof(m_CMTAAccount));
    csv.GetAt(ROMFIELDS::CSV_GIVEUPACCOUNT, m_GiveUpAccount, sizeof(m_GiveUpAccount));
    csv.GetAt(ROMFIELDS::CSV_CLEARINGID, m_ClearingId, sizeof(m_ClearingId));
    csv.GetAt(ROMFIELDS::CSV_EXDEST, m_ExDest, sizeof(m_ExDest));
    //SetSingleTimeStamp((CUtility::GetLocalTimeHHMMSS()).c_str());
    csv.GetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_Instructions, sizeof(m_Instructions));
    CUtility::StrCpy(m_ReplacingInstructions, sizeof(m_ReplacingInstructions), m_Instructions);
    csv.GetAt(ROMFIELDS::CSV_MM_ACCOUNT, m_MMAccount, sizeof(m_MMAccount));
    csv.GetAt(ROMFIELDS::CSV_SEC_ID, m_SecID, sizeof(m_SecID));
    csv.GetAt(ROMFIELDS::CSV_GTD_DATE, m_ExpTime, sizeof(m_ExpTime));
    CUtility::StrCpy(m_ReplacingExpTime, sizeof(m_ReplacingExpTime), m_ExpTime);
    csv.GetAt(ROMFIELDS::CSV_CLIENT_ECHO, m_ClientEcho, sizeof(m_ClientEcho));
    csv.GetAt(ROMFIELDS::CSV_MATURITYDAY, m_MaturityDay, sizeof(m_MaturityDay));

    nbuf = 0;
    //csv.GetAt(ROMFIELDS::CSV_OPENCLOSE, nbuf);
    //m_OpenClose = nbuf;
    csv.GetAt(ROMFIELDS::CSV_OPENCLOSE, m_OpenClose, sizeof(m_OpenClose));
    
    csv.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, m_CplxType);

    csv.GetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest, sizeof(m_Dest));
    bool bOptionOnFuture = CDestsManager::getInstance().IsOptionOnFuture(atoi(m_Exchange), m_SecType);
    if (bOptionOnFuture)
    {
        m_secIndex = ROMVALUE::OPTIONONFUTURE;
    }
    
    csv.GetAt(ROMFIELDS::CSV_MULTIPLIER, m_Multiplier);
    
    if (ROMVALUE::FUTURE != m_secIndex &&
        ROMVALUE::OPTIONONFUTURE != m_secIndex) //future multiplier is retrieved from database
    {
        if (0 == m_Multiplier)
        {
            if (m_SecType == ROMVALUE::SECTYPE_OPTION)
            {
                m_Multiplier = 100;
            }
            else
            {
                m_Multiplier = 1;
            }
        }
    }
    
    MakeClearinSecKey();
    
    SetNeedPosMgr();

    if (ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == m_CplxType &&
        strlen(m_Instructions) > 0)
    {
        CreateCoveredSpread();
    }
}

void CRomOrderNew::SetNeedPosMgr()
{
    if (CConfig::getInstance().PosMgrEnabled())
    {
        m_needPosMgr =  ROMVALUE::SECTYPE_EQUITY == m_SecType &&
                        strlen(m_ParentRomTag) == 0 &&
                        m_CplxType != ROMVALUE::CPLXTYPE_SPREAD &&
                        m_CplxType != ROMVALUE::CPLXTYPE_PRODUCT_SPREAD && 
                        CPositionManager::getInstance().FindClr(m_ClearingAccount);
    }
}

void CRomOrderNew::MakeClearinSecKey()
{
    strcpy(m_ClearingSecKey, m_ClearingAccount);

    if (CUtility::IsEquityType(m_SecType))
    {
        strcpy(m_ClearingSecKey + strlen(m_ClearingAccount), "/CS");
    }
    else if (CUtility::IsFutureType(m_SecType))
    {
        strcpy(m_ClearingSecKey + strlen(m_ClearingAccount), "/FUT");
    }
    else if (CUtility::IsOptionType(m_SecType))
    {
        if (ROMVALUE::SECTYPE_OPTIONFUTURE == m_SecType ||
            ROMVALUE::OPTIONONFUTURE == m_secIndex)
        {
            strcpy(m_ClearingSecKey + strlen(m_ClearingAccount), "/M");
        }
        else
        {
            strcpy(m_ClearingSecKey + strlen(m_ClearingAccount), "/OPT");
        }
    }
    else if (CUtility::IsCurrencyType(m_SecType))
    {
        strcpy(m_ClearingSecKey + strlen(m_ClearingAccount), "/FOR");
    }
}

void CRomOrderNew::SetTimeStamp(const char* value)
{
    SetSingleTimeStamp(value);
}

int CRomOrderNew::SideSign()const
{
    /*if (Side() == ROMVALUE::SIDE_BUY)
    {
        return 1;
    }
    else
    {
        return -1;
    }*/
    return 1;
}

int CRomOrderNew::SideSign(char side)
{
    /*if (side == ROMVALUE::SIDE_BUY)
    {
        return 1;
    }
    else
    {
        return -1;
    }*/
    return 1;
}
void CRomOrderNew::ResetReplacingFields()
{
    m_ReplacePending = false;
}


long CRomOrderNew::UpdateFromChildFill(long last, char& secType, double& dollar)
{
    secType = m_SecType;
    dollar = -last * m_Price * m_Multiplier * SideSign() * CurrencyRate();
    m_Leave -= last;

    double cash = m_Cum * m_AvgPrice + last * m_Price;
    m_Cum += last;
    m_AvgPrice = cash / m_Cum;
    //m_NextExecId++;
    return m_Leave;
}

bool CRomOrderNew::DeadSpread()
{
    bool rc = false;

    if ((ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == m_CplxType) &&
        (ROMVALUE::STATUS_ORDERREJECTED == m_Status ||
         ROMVALUE::STATUS_CANCELED == m_Status ||
         ROMVALUE::STATUS_FILLED == m_Status ||
         ROMVALUE::STATUS_EXPIRED == m_Status))
    {
        rc = true;
    }

    return rc;
}


void CRomOrderNew::GetCancelRequiredFields(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, m_RomTag);
    msg.SetAt(ROMFIELDS::CSV_SIDE, m_Side);
    msg.SetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    msg.SetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol);
    msg.SetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest);
    msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
    msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, m_Cum);
}

void CRomOrderNew::GetReplaceRequiredFields(CCsv& msg)
{
    msg.SetAt(ROMFIELDS::CSV_STAFFORD_TAG, m_RomTag);
    msg.SetAt(ROMFIELDS::CSV_SHARES, m_Leave + m_Cum + m_ShareChange);
    msg.SetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol);
    msg.SetAt(ROMFIELDS::CSV_CAPACITY,m_Capacity);
    msg.SetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, m_Leave + m_Cum + m_ShareChange);
    msg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
    msg.SetAt(ROMFIELDS::CSV_CUM_SHARES, m_Cum);
    msg.SetAt(ROMFIELDS::CSV_SIDE, m_Side);
    msg.SetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest);
}

void CRomOrderNew::QueueOatsMsg(CCsv* pCsv)
{
    std::string romExTag;
    NextRomExecTag(romExTag);
    pCsv->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
    COatsMsg* pOatsMsg = new COatsMsg(pCsv, this, true, true);
    COats::getInstance().AddMessage(pOatsMsg);
}


void CRomOrderNew::DoDrop(CCsv* pCsv)
{
    if (CConfig::getInstance().noDrop() == 0)
    {
        tDropInfo dropInfo;
        int status = ROMVALUE::STATUS_NEW;
        pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);

        if (CClientProfileManagerNew::getInstance().GetDropInfo(m_Trader, dropInfo))
        {
            if ((dropInfo.ReportingFirm).empty() == false &&
                (dropInfo.ReportingFirm).compare("ROMTPOS") != 0)//since current db has this value
            {
                pCsv->SetAt(ROMFIELDS::CSV_CLEARINGID, m_ClearingId);
                pCsv->SetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, m_ClearingAccount);

                if (strlen(m_ClientEcho))
                {
                    pCsv->SetAt(ROMFIELDS::CSV_CLIENT_ECHO, m_ClientEcho);
                }

                if (CUtility::IsOptionType(m_SecType))
                {
                    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
                    {
                        pCsv->SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
                    }

                    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_MATURITYDAY))
                    {
                        pCsv->SetAt(ROMFIELDS::CSV_MATURITYDAY, m_MaturityDay);
                    }
                }

                std::string err;
                char value[200];
                sprintf(value,
                        "%s %s %s %s",
                        m_TradeFor,
                        m_ClearingAccount,
                        m_Firm,
                        m_LocalAccount);

                std::string account;
                pCsv->GetAt(ROMFIELDS::CSV_ACCOUNT, account);
                pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, value);

                bool sendDrop = false;

                if (dropInfo.ReportingType == ROMVALUE::DROPCOPY_FILLONLY)
                {
                    if (status == ROMVALUE::STATUS_FILLED ||
                        status == ROMVALUE::STATUS_PARTIALLYFILLED)
                    {
                        sendDrop = true;
                    }
                }
                else if (dropInfo.ReportingType == ROMVALUE::DROPCOPY_ALL)
                {
                    sendDrop = true;
                }

                if (sendDrop)
                {
                    if (CClientsManager::getInstance().Send(dropInfo.ReportingFirm, *pCsv, err))
                    {
                        //RomLog::LogMessage("DropCopy To " + pData->ReportingFirm(), msg.Message());
                    }
                    else
                    {
                        //RomLog::LogMessage("Can not send DropCopy to " + pData->ReportingFirm(), msg.Message());
                    }
                }

                pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, account);
            }
        }
        else
        {
            //drop data not found
            char msg[128];
            sprintf(msg, "DropInfo not found(trader=%s)\n", m_Trader);
            send_debug_message(msg);
        }
    }
}

void CRomOrderNew::DoTpos(CCsv* pCsv)
{
    if (CConfig::getInstance().noTPOS() == 0)
    {
        tDropInfo dropInfo;

        if (CClientProfileManagerNew::getInstance().GetDropInfo(m_Trader, dropInfo))
        {
            if (dropInfo.RoninMember)
            {
                std::string err;
                char value[200];
                sprintf(value,
                        "%s %s %s %s",
                        m_TradeFor,
                        m_ClearingAccount,
                        m_Firm,
                        m_LocalAccount);

                std::string account;
                pCsv->GetAt(ROMFIELDS::CSV_ACCOUNT, account);
                pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, value);
                CClientsManager::getInstance().Send("ROMTPOS", *pCsv, err);
                pCsv->SetAt(ROMFIELDS::CSV_ACCOUNT, account);
            }
        }
        else
        {
            //drop data not found
            char msg[128];
            sprintf(msg, "DropInfo not found(trader=%s)\n", m_Trader);
            send_debug_message(msg);
        }
    }
}

void CRomOrderNew::ProcessUpdate(CCsv* pCsv)
{
    int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
    pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);
    if (cplxtype != ROMVALUE::CPLXTYPE_LEG)
    {
        cplxtype = m_CplxType;
    }

    switch (cplxtype)
    {
    case ROMVALUE::CPLXTYPE_SINGLE:
        ProcessHostSingleUpdate(pCsv);
        break;
    case ROMVALUE::CPLXTYPE_SPREAD:
        ProcessHostContainerUpdate_UserDefinedSpread(pCsv);
        break;
    case ROMVALUE::CPLXTYPE_PRODUCT_SPREAD:
        ProcessHostContainerUpdate_ProductSpread(pCsv);
        break;
    case ROMVALUE::CPLXTYPE_LEG:
        if (strlen(m_ParentRomTag) != 0)
        {
            ProcessHostLegUpdate_UserDefinedSpread(pCsv);
        }
        else
        {
            ProcessHostLegUpdate_ProductSpread(pCsv);
        }
        break;
    }
}

void CRomOrderNew::ProcessHostLegUpdate_ProductSpread(CCsv* pCsv)
{
    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
    {
        pCsv->SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE))
    {
        pCsv->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType);
    }

    pCsv->SetAt(ROMFIELDS::CSV_CAPACITY, m_Capacity);
    pCsv->SetAt(ROMFIELDS::CSV_TRADER, m_Trader);
    pCsv->SetAt(ROMFIELDS::CSV_TRADE_FOR, m_TradeFor);
    pCsv->SetAt(ROMFIELDS::CSV_TAG, m_ClientTag);
    pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE, m_Exchange);;
    pCsv->SetAt(ROMFIELDS::CSV_OWNER, m_Owner);
    QueueOatsMsg(pCsv);

    std::string err;
    CClientsManager::getInstance().Send(m_Owner, *pCsv, err);

    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);
    if (ROMVALUE::STATUS_FILLED == status ||
        ROMVALUE::STATUS_PARTIALLYFILLED == status)
    {
        DoTpos(pCsv);
    }

    DoDrop(pCsv);
}

void CRomOrderNew::ProcessHostSingleUpdate(CCsv* pCsv)
{
    tKeyedBalanceUpdate balance;
    memset(&(balance.update), 0, sizeof(tBalanceUpdate));
    bool cancelpending = m_CancelPending;
    bool replacepending = m_ReplacePending;
    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);

    Update(pCsv);

    //update balance
    if (GetBalanceUpdate(pCsv, balance.update))
    {
        balance.update.secIndex = m_secIndex;
        balance.username = m_Trader;
        if (ROMVALUE::FUTURE == balance.update.secIndex)
        {
            balance.product = Product();
        }

        balance.clearing_sec = m_ClearingSecKey;
        balance.clract = m_ClearingAccount;
        balance.update.secIndex = m_secIndex;

        CClientProfileManagerNew::getInstance().UpdateBalance(balance);
    }

    std::string err;

    if (status == ROMVALUE::STATUS_FILLED)
    {
        if (cancelpending)
        {
            std::string text = "Too late to cancel:Order has been filled";
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
            QueueOatsMsg(pCsv);
            CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
            DoDrop(pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, status);
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }

        if (replacepending)
        {
            std::string text = "Too late to replace:Order has been filled";
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
            QueueOatsMsg(pCsv);
            CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
            DoDrop(pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, status);
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }
    }

    QueueOatsMsg(pCsv);

    CClientsManager::getInstance().Send(m_Owner, *pCsv, err);

    if (ROMVALUE::STATUS_FILLED == status ||
        ROMVALUE::STATUS_PARTIALLYFILLED == status)
    {
        DoTpos(pCsv);
    }

    DoDrop(pCsv);
    //CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
}


void CRomOrderNew::ProcessHostContainerUpdate_ProductSpread(CCsv* pCsv)
{
    tKeyedBalanceUpdate balance;
    memset(&(balance.update), 0, sizeof(tBalanceUpdate));
    bool cancelpending = m_CancelPending;
    bool replacepending = m_ReplacePending;
    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);
    std::string err;

    Update(pCsv);
    //update balance
    if (GetBalanceUpdate(pCsv, balance.update))
    {
        balance.update.secIndex = m_secIndex;
        balance.username = m_Trader;
        balance.clearing_sec = m_ClearingSecKey;
        balance.clract = m_ClearingAccount;
        if (ROMVALUE::FUTURE == balance.update.secIndex)
        {
            balance.product = Product();
        }

        if (m_pCoveredSpread)
        {
            if (ROMVALUE::STATUS_REPLACEREJECTED == status)
            {
                CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(balance, m_pCoveredSpread, true); 
            }
            else
            {
                CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(balance, m_pCoveredSpread, false); 
            }
        }
        else
        {
            CClientProfileManagerNew::getInstance().UpdateBalance(balance);
        }
    }

    if (status == ROMVALUE::STATUS_FILLED)
    {
        if (cancelpending)
        {
            std::string text = "Too late to cancel:Order has been filled";
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
            QueueOatsMsg(pCsv);
            CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
            DoDrop(pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, status);
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }

        if (replacepending)
        {
            std::string text = "Too late to replace:Order has been filled";
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
            QueueOatsMsg(pCsv);
            CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
            DoDrop(pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, status);
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }
    }

    QueueOatsMsg(pCsv);

    if (ROMVALUE::STATUS_PENDINGCANCEL != status)
    {
        CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
    }

    if (ROMVALUE::STATUS_FILLED != status &&
        ROMVALUE::STATUS_PARTIALLYFILLED != status)
    {
        DoDrop(pCsv);
    }
}

void CRomOrderNew::FlipSide(short newSide)
{
    tKeyedBalanceUpdate balance;
    memset(&(balance.update), 0, sizeof(tBalanceUpdate));
    
    balance.username = m_Trader;
    balance.clearing_sec = m_ClearingSecKey;
    balance.clract = m_ClearingAccount;
    balance.update.shareChange = 0;
    balance.update.orderChange = 0;

    if (NULL == m_pCoveredSpread)
    {
        //back out the dollars with current sideIndex
        balance.update.secIndex = m_secIndex;
        balance.update.sideIndex = m_sideIndex;
        balance.update.dollarChange = -m_Shares * m_Multiplier * fabs(m_Price);
        CClientProfileManagerNew::getInstance().UpdateBalance(balance);

        //flip side
        SetSide(newSide);
        
        //add the dollars with new sideIndex
        balance.update.sideIndex = m_sideIndex;
        balance.update.dollarChange *= -1;
        CClientProfileManagerNew::getInstance().UpdateBalance(balance);
    }
    else
    {
        //back out the dollars with current sideIndex
        balance.update.secIndex = ROMVALUE::EQUITY;;
        balance.update.dollarChange = -m_Shares * m_pCoveredSpread->e_dollars;
        balance.update.sideIndex = m_pCoveredSpread->e_sideIndex;
        CClientProfileManagerNew::getInstance().UpdateBalance(balance);

        balance.update.secIndex = ROMVALUE::OPTION;
        balance.update.dollarChange = -m_Shares * m_pCoveredSpread->o_dollars;
        balance.update.sideIndex = m_pCoveredSpread->o_sideIndex;
        CClientProfileManagerNew::getInstance().UpdateBalance(balance);

        //add the dollars with new sideIndex
        if (ROMVALUE::BUY == m_pCoveredSpread->e_sideIndex)
        {
            m_pCoveredSpread->e_sideIndex = ROMVALUE::SELL;
        }
        else
        {
            m_pCoveredSpread->e_sideIndex = ROMVALUE::BUY;
        }
        
        if (ROMVALUE::BUY == m_pCoveredSpread->o_sideIndex)
        {
            m_pCoveredSpread->o_sideIndex = ROMVALUE::SELL;
        }
        else
        {
            m_pCoveredSpread->o_sideIndex = ROMVALUE::BUY;
        }

        balance.update.secIndex = ROMVALUE::EQUITY;
        balance.update.dollarChange = m_Shares * m_pCoveredSpread->e_dollars;
        balance.update.sideIndex = m_pCoveredSpread->e_sideIndex;
        CClientProfileManagerNew::getInstance().UpdateBalance(balance);

        balance.update.secIndex = ROMVALUE::OPTION;
        balance.update.dollarChange = m_Shares * m_pCoveredSpread->o_dollars;
        balance.update.sideIndex = m_pCoveredSpread->o_sideIndex;
        CClientProfileManagerNew::getInstance().UpdateBalance(balance);
        
        //finally flip side
        SetSide(newSide);
    }
    
}

void CRomOrderNew::ProcessHostContainerUpdate_UserDefinedSpread(CCsv* pCsv)
{
    tKeyedBalanceUpdate balance;
    memset(&(balance.update), 0, sizeof(tBalanceUpdate));

    bool cancelpending = m_CancelPending;
    bool replacepending = m_ReplacePending;
    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);
    
    if (ROMVALUE::STATUS_OPEN == status)
    {
        //Possible side flip here
        short side;
        if (pCsv->GetAt(ROMFIELDS::CSV_SIDE, side) &&
            side != m_Side)
        {
            //if (m_pCoveredSpread)
            //{
             //   SetSide(side);
            //}
            //else
            //{
                //don't call SetSide() because 
                //we don't want to change the sideindex!
                //m_Side = side;
            //}
        }
    }

    Update(pCsv);


    //update balance
    if (GetBalanceUpdate(pCsv, balance.update))
    {
        balance.update.secIndex = m_secIndex;
        balance.username = m_Trader;
        balance.clearing_sec = m_ClearingSecKey;
        balance.clract = m_ClearingAccount;
        if (ROMVALUE::FUTURE == balance.update.secIndex)
        {
            balance.product = Product();
        }

        if (m_pCoveredSpread)
        {
            if (ROMVALUE::STATUS_REPLACEREJECTED == status)
            {
                CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(balance, m_pCoveredSpread, true); 
            }
            else
            {
                CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(balance, m_pCoveredSpread, false); 
            }
        }
        else
        {
            CClientProfileManagerNew::getInstance().UpdateBalance(balance);
        }
    }

    std::string err;

    if (status == ROMVALUE::STATUS_FILLED)
    {
        if (cancelpending)
        {
            std::string text = "Too late to cancel:Order has been filled";
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
            QueueOatsMsg(pCsv);
            CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
            DoDrop(pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, status);
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }

        if (replacepending)
        {
            std::string text = "Too late to replace:Order has been filled";
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
            QueueOatsMsg(pCsv);
            CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
            DoDrop(pCsv);
            pCsv->SetAt(ROMFIELDS::CSV_STATUS, status);
            pCsv->SetAt(ROMFIELDS::CSV_TEXT, text);
        }
    }

    QueueOatsMsg(pCsv);

    if (ROMVALUE::STATUS_FILLED != status &&
        ROMVALUE::STATUS_PARTIALLYFILLED != status &&
        ROMVALUE::STATUS_PENDINGCANCEL != status)
    {
        //CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
        DoDrop(pCsv);
        //CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
    }

    CClientsManager::getInstance().Send(m_Owner, *pCsv, err);
}

void CRomOrderNew::ProcessHostLegUpdate_UserDefinedSpread(CCsv* pCsv)
{
    Update(pCsv);

    std::string err;
    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);
    pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE, m_Exchange);
    pCsv->SetAt(ROMFIELDS::CSV_TAG, m_ClientTag);
    pCsv->SetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag);
    pCsv->SetAt(ROMFIELDS::CSV_TRADER, m_Trader);
    pCsv->SetAt(ROMFIELDS::CSV_TRADE_FOR,m_TradeFor);
    pCsv->SetAt(ROMFIELDS::CSV_CAPACITY, m_Capacity);

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
    {
        pCsv->SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE))
    {
        pCsv->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType);
    }

    QueueOatsMsg(pCsv);

    CClientsManager::getInstance().Send(m_Owner, *pCsv, err);

    if (ROMVALUE::STATUS_FILLED == status ||
        ROMVALUE::STATUS_PARTIALLYFILLED == status)
    {
        DoTpos(pCsv);
    }

    DoDrop(pCsv);
}

void CRomOrderNew::Update(CCsv* pCsv)
{
    int status = 0;
    std::string clientTag = m_ClientTag;
    int precision = (m_SecType == ROMVALUE::SECTYPE_EQUITY)? 4 : 8;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);

    std::string orderTag;
    pCsv->GetAt(ROMFIELDS::CSV_ORDER_TAG, orderTag);

    if (status != ROMVALUE::STATUS_FILLED &&
        status != ROMVALUE::STATUS_PARTIALLYFILLED)
    {
        pCsv->SetAt(ROMFIELDS::CSV_LAST_SHARES, "0");
        pCsv->SetAt(ROMFIELDS::CSV_LAST_PRICE, "0");
    }

    if (orderTag.empty() == false)
    {
        if (status == ROMVALUE::STATUS_REPLACED ||
            status == ROMVALUE::STATUS_OPEN)
        {
            SetRomExchangeTag(orderTag.c_str());
        }
        else if (strlen(m_RomExchangeTag) == 0)
        {
            SetRomExchangeTag(orderTag.c_str());
        }
    }

    orderTag.clear();
    pCsv->GetAt(ROMFIELDS::CSV_EXCHANGE_TAG, orderTag);
    if (orderTag.empty() == false)
    {
        SetExchangeTag(orderTag.c_str());
    }

    switch (status)
    {
    case ROMVALUE::STATUS_OPEN:
        {
            int type = 0;
            pCsv->GetAt(ROMFIELDS::CSV_TYPE, type);
            if (Type() == ROMVALUE::TYPE_STOP &&
                type == ROMVALUE::TYPE_STOPLIMIT)
            {
                pCsv->GetAt(ROMFIELDS::CSV_PRICE, m_Price);
                SetType(ROMVALUE::TYPE_STOPLIMIT);
            }
            
            int cplxType = ROMVALUE::CPLXTYPE_SINGLE;
            if (pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxType) &&
                ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == cplxType)
            {
                m_CplxType = cplxType;
            }
        }
        break;

    case ROMVALUE::STATUS_FILLED:
    case ROMVALUE::STATUS_PARTIALLYFILLED:
        {
            long lastShares = 0;
            double lastPrice = 0;

            if (pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares) == false)
            {
                pCsv->GetAt(ROMFIELDS::CSV_SHARES, lastShares);
            }

            if (m_needPosMgr && ROMVALUE::SIDE_BUY == m_Side)
            {
                //update position possible lending balance too
                UpdatePos(lastShares);
            }
            
            pCsv->GetAt(ROMFIELDS::CSV_LAST_PRICE, lastPrice);
            double cash = m_Cum * m_AvgPrice + lastShares * lastPrice;
            m_Cum += lastShares;
            m_Leave -= lastShares;
            SetAvgPrice(cash / m_Cum);
        }
        break;
    case ROMVALUE::STATUS_REPLACED:
        if (ReplacePending())
        {
            if (m_needPosMgr &&  
                m_ShareChange < 0 &&  //case of m_ShareChange > 0 was already taken care of 
                                      //when replace was sent!!!
                m_Side != ROMVALUE::SIDE_BUY) 
            {
                //update position possible lending balance too
                UpdatePos(-m_ShareChange);
            }
            
            UpdateReplacedFields(*pCsv);
            SetReplacePending(false);
            SetShares(m_Cum + m_Leave); //current Fix order quantity
            pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, m_ClientTag);
            memset(&m_ReplacingDelt, 0, sizeof(tBalanceUpdate));
        }
        else
        {
            //trust exchange interface to always pass back the correct leaves for leg!!
            if (m_CplxType == ROMVALUE::CPLXTYPE_LEG)
            {
                pCsv->GetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
                pCsv->GetAt(ROMFIELDS::CSV_SHARES, m_Shares);
            }
            pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, "");
        }
        break;
    case ROMVALUE::STATUS_REPLACEREJECTED:
        pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, m_NewClientTag);
                
        if (m_ReplacePending)  //only single order or body of spread will have m_ReplacePending==1!!!
        {
            if (m_needPosMgr &&
                m_ShareChange > 0 &&  //case of m_ShareChange < 0 will be taken care
                                      //of when replace is confirmed!!!
                m_Side != ROMVALUE::SIDE_BUY)
            {
                CPositionManager::getInstance().ProcessSSReplaceReject(this);
            }
        }
        
        m_ReplacePending = false;
      
        break;
    case ROMVALUE::STATUS_REPLACEPENDING:
        pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, m_NewClientTag);
        break;
    case ROMVALUE::STATUS_EXPIRED:
        m_Leave = 0;
        break;
    case ROMVALUE::STATUS_DONEFORDAY:
        if (m_TIF != ROMVALUE::TIF_GTC)
        {
            if (m_needPosMgr && m_Side != ROMVALUE::SIDE_BUY)
            {
                UpdatePos(m_Leave);
            }
            m_Leave = 0;
        }
        break;
    case ROMVALUE::STATUS_CANCELED:
        pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag);
        m_CancelPending = false;
        
        if (m_needPosMgr && m_Side != ROMVALUE::SIDE_BUY)
        {
            UpdatePos(m_Leave);
        }
        
        m_Leave = 0;
        break;
    case ROMVALUE::STATUS_CANCELREJECTED:
        pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag);
        m_CancelPending = false;
        break;
    case ROMVALUE::STATUS_PENDINGCANCEL:
        pCsv->SetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag);
        break;
    case ROMVALUE::STATUS_ORDERREJECTED:
        
        if (m_needPosMgr && m_Side != ROMVALUE::SIDE_BUY)
        {
            //CPositionManager::getInstance().ProcessSSOrderReject(this);
            UpdatePos(m_Leave);
        }

        m_CancelPending = false;
        m_Leave = 0;
        break;
    }

    if (IsActionStatus(status) == false)
    {
        m_Status = status;
    }

    //Adding all the fields required by FIX
    if (CUtility::IsFutureType(m_SecType) == true)
    {
        pCsv->SetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth);
    }
    else if (CUtility::IsOptionType(m_SecType) == true)
    {
        pCsv->SetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth);
        pCsv->SetAt(ROMFIELDS::CSV_MATURITYDAY, m_MaturityDay);
        pCsv->SetAt(ROMFIELDS::CSV_CALLPUT, m_CallOrPut);

        //double strike = 0;
        //if (m_StrikePrice.GetValue(strike))
        //{
        //    pCsv->SetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);
        //}
        pCsv->SetAt(ROMFIELDS::CSV_STRIKEPRICE, m_StrikePrice);
        pCsv->SetAt(ROMFIELDS::CSV_OPENCLOSE, m_OpenClose);
        pCsv->SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
    }

    pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, m_ExchangeTag);
    pCsv->SetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol);
    pCsv->SetAt(ROMFIELDS::CSV_AVG_PRICE, m_AvgPrice, precision);
    pCsv->SetAt(ROMFIELDS::CSV_CUM_SHARES, m_Cum);
    pCsv->SetAt(ROMFIELDS::CSV_EXCHANGE, m_Exchange);
    pCsv->SetAt(ROMFIELDS::CSV_SIDE, m_Side);
    pCsv->SetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
    pCsv->SetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType);
    pCsv->SetAt(ROMFIELDS::CSV_SEC_DEF, m_SecDef);
    pCsv->SetAt(ROMFIELDS::CSV_TAG, clientTag);
    pCsv->SetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag);
    pCsv->SetAt(ROMFIELDS::CSV_CLIENT_ECHO, m_ClientEcho);
    pCsv->SetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest);
    pCsv->SetAt(ROMFIELDS::CSV_OWNER, m_Owner);
    if (m_CplxType)
    {
        pCsv->SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, m_CplxType);
    }
    //always populate replaceable fields
    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_TYPE))
    {
        pCsv->SetAt(ROMFIELDS::CSV_TYPE, m_Type);
    }

    pCsv->SetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    //if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_SHARES))
    //{
    //    pCsv->SetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    //}

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_PEGPRICE_OFFSET))
    {
        pCsv->SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_PegOffset);
    }

    if (IsLimitType(m_Type) == true &&
        pCsv->IsFieldEmpty(ROMFIELDS::CSV_PRICE))
    {
        pCsv->SetAt(ROMFIELDS::CSV_PRICE, m_Price, precision);
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_STOPPRICE))
    {
        if (Type() == ROMVALUE::TYPE_STOP  ||
            Type() == ROMVALUE::TYPE_STOPLIMIT)
        {
            pCsv->SetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice);
        }
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_TIF))
    {
        pCsv->SetAt(ROMFIELDS::CSV_TIF, m_TIF);
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_EXEC_INSTRUCTION))
    {
        pCsv->SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ExecInst);
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_FLOOR))
    {
        pCsv->SetAt(ROMFIELDS::CSV_FLOOR, m_MaxFloor);
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_MINQTY))
    {
        if (m_MinQty)
        {
            pCsv->SetAt(ROMFIELDS::CSV_MINQTY, m_MinQty);
        }
    }

    if (pCsv->IsFieldEmpty(ROMFIELDS::CSV_INSTRUCTIONS))
    {
        pCsv->SetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_Instructions);
    }

    if (m_ReplacingTIF == ROMVALUE::TIF_GTD &&
        pCsv->IsFieldEmpty(ROMFIELDS::CSV_GTD_DATE))
    {
        pCsv->SetAt(ROMFIELDS::CSV_GTD_DATE, m_ReplacingExpTime);
    }
}

bool CRomOrderNew::GetBalanceUpdate(CCsv* pCsv, tBalanceUpdate& blUpdate)
{
    bool rc = false;

    blUpdate.sideIndex = m_sideIndex;

    int status = ROMVALUE::STATUS_NEW;
    pCsv->GetAt(ROMFIELDS::CSV_STATUS, status);
    int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
    pCsv->GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);

    if (cplxtype != ROMVALUE::CPLXTYPE_LEG &&
        m_CplxType != ROMVALUE::CPLXTYPE_LEG)
    {
        //double price = m_Price;
        double price = m_Price;
        if (NeedEquityMidPrice())
        {
            price = m_mPrice;
        }

        if (ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == m_CplxType ||
            ROMVALUE::CPLXTYPE_SPREAD == m_CplxType)
        {
            price = fabs(price);
        }

        switch (status)
        {
        case ROMVALUE::STATUS_PARTIALLYFILLED:
        case ROMVALUE::STATUS_FILLED:
            {
                long lastshares = 0;
                pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshares);
                blUpdate.dollarChange = -1 * lastshares * price * m_Multiplier * m_CurrencyRate;
                blUpdate.shareChange = -lastshares * m_Ratio;
                if (ROMVALUE::STATUS_FILLED == status)
                {
                    blUpdate.orderChange = -1;
                    if (m_ReplacePending)
                    {
                        blUpdate.dollarChange -= m_ReplacingDelt.dollarChange;
                        blUpdate.shareChange -= m_ReplacingDelt.shareChange;
                    }
                }
                else
                {
                    blUpdate.orderChange = 0;
                }
            }
            rc = true;
            break;
        case ROMVALUE::STATUS_REPLACEREJECTED:
            blUpdate = m_ReplacingDelt;
            blUpdate.dollarChange *= -1;
            blUpdate.shareChange *= -1;
            blUpdate.orderChange = 0;
            rc = true;
            break;
        case ROMVALUE::STATUS_CANCELED:
        case ROMVALUE::STATUS_ORDERREJECTED:
        case ROMVALUE::STATUS_EXPIRED:
            {
                blUpdate.orderChange = -1;
                long leave = m_Shares - m_Cum; //remember that pOrder->Leaves() has already been set to zero!
                blUpdate.shareChange = -1 * m_Ratio * leave;
                blUpdate.dollarChange = (-1) * leave * price * m_Multiplier * m_CurrencyRate;
                
                if (m_ReplacePending)
                {
                    blUpdate.dollarChange -= m_ReplacingDelt.dollarChange;
                    blUpdate.shareChange -= m_ReplacingDelt.shareChange;  
                }
            }
            rc = true;
            break;
        default:
            rc = false;
            break;
        }
        /*if (ROMVALUE::STATUS_PARTIALLYFILLED == status ||
            ROMVALUE::STATUS_FILLED == status)
        {
            long lastshares = 0;
            pCsv->GetAt(ROMFIELDS::CSV_LAST_SHARES, lastshares);
            blUpdate.dollarChange = -1 * lastshares * price * m_Multiplier * m_CurrencyRate;
            blUpdate.shareChange = -lastshares * m_Ratio;
            if (ROMVALUE::STATUS_FILLED == status)
            {
                blUpdate.orderChange = -1;
            }
            else
            {
                blUpdate.orderChange = 0;
            }

            rc = true;
        }
        else if (CRomOrderNew::IsDead(this))
        {
            blUpdate.orderChange = -1;
            long leave = m_Shares - m_Cum; //remember that pOrder->Leaves() has already been set to zero!
            blUpdate.shareChange = -1 * m_Ratio * leave;
            blUpdate.dollarChange = (-1) * leave * price * m_Multiplier * m_CurrencyRate;
            rc = true;
        }
        else if (ROMVALUE::STATUS_REPLACEREJECTED == status)
        {
            blUpdate = m_ReplacingDelt;
            blUpdate.dollarChange *= -1;
            blUpdate.shareChange *= -1;
            blUpdate.orderChange = 0;
            rc = true;
        }*/
    }

    return rc;
}


bool CRomOrderNew::ProcessCancel(CCsv* pCancel, const std::string& actTag, bool ownercancel, CClientConnection* pCon)
{
    bool rc = true;
    std::string err;
    bool clxrjt_to_oats = true;
    
    if (ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == m_CplxType &&
        m_Leave == 0)
    {
        clxrjt_to_oats = false;
    }
    
    GetCancelRequiredFields(*pCancel);
    std::string sStatus;
    pCancel->GetAt(ROMFIELDS::CSV_STATUS, sStatus);
    pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);

    if (pCon)
    {
        rc = pCon->IncrementMsgCntCheck(m_secIndex, err);
    }

    if (rc)
    {
        if (true == ownercancel &&
            m_CancelPending == true)
        {
            err.assign("Previous cancel is still pending in ROM");
            rc = false;
        }
        else
        {
            if (m_Leave == 0)
            {
                err = "There are not leaves to cancel";
                rc = false;
            }
            else
            {
                if (actTag.empty() == false)
                {
                    SetActClientTag(actTag.c_str());
                }

                if (ownercancel)
                {
                    m_CancelPending = true;
                }
            }
        }
    }

    if (false == rc)
    {
        pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELREJECTED);
        pCancel->SetAt(ROMFIELDS::CSV_TEXT, err);
        
        if (clxrjt_to_oats)
        {
            QueueOatsMsg(pCancel);
        }
    }
    else
    {
        pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
        QueueOatsMsg(pCancel);
        pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
        pCancel->SetAt(ROMFIELDS::CSV_STATUS, sStatus);
        
        pCancel->GetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag, sizeof(m_ActClientTag));
        
        std::string destID;
        pCancel->GetAt(ROMFIELDS::CSV_ORDERDEST, destID);

        databuf_t* pBuffer = CBuffStore::getInstance().Get();
        pCancel->GetMsgNew(pBuffer);

        if (CDestsManager::getInstance().Send(destID, ROMVALUE::COMMAND_CANCEL, pBuffer->buffer, databuf_unread(pBuffer),  err) == false)
        {
            pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
            pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            pCancel->SetAt(ROMFIELDS::CSV_TEXT, err);
            m_CancelPending = false;
            QueueOatsMsg(pCancel);
            rc = false;
        }

        CBuffStore::getInstance().ReturnB(pBuffer);
    }

    return rc;
}

bool CRomOrderNew::ValidateSingleLimitPrice(double price, std::string& err)
{
    bool rc = true;
    if (CRomOrderNew::IsLimitType(m_Type) &&
        ROMVALUE::CPLXTYPE_SPREAD != m_CplxType &&
        ROMVALUE::CPLXTYPE_PRODUCT_SPREAD != m_CplxType)
    {
        if (price <= 0)
        {
            char errStr[256];
            sprintf(errStr, "Invalid price:%f for single Equity order", price);
            err = errStr;
            rc = false;
        }
    }
    return rc;
}

bool CRomOrderNew::ProcessReplace(CCsv* pReplace, CClientConnection* pCon)
{
    bool rc = true;
    std::string err;
    bool rplreject_to_oats = true;
    
    if (0 == m_Leave &&
        ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == m_CplxType)
    {
        rplreject_to_oats = false;
    }
    
    if (pCon)
    {
        rc = pCon->IncrementMsgCntCheck(m_secIndex, err);
    }

    if (false == rc)
    {
        pReplace->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
        pReplace->SetAt(ROMFIELDS::CSV_TEXT, err);
        pReplace->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEREJECTED);
        QueueOatsMsg(pReplace);
        rc = false;
    }
    else
    {
        char excIns;
        int status = ROMVALUE::STATUS_NEW;
        pReplace->GetAt(ROMFIELDS::CSV_STATUS, status);
        std::string text;
        pReplace->GetAt(ROMFIELDS::CSV_TEXT, text);

        if (pReplace->GetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, excIns) &&
            excIns == ROMVALUE::ISO)
        {
            err = "ISO order is not allowed by ROM system";
            rc = false;
        }
        else if (m_ReplacePending == true)
        {
            err = "Previous Replace is still pending in ROM";
            rc = false;
        }

        if (rc)
        {
            tKeyedBalanceUpdate update;
            memset(&(update.update), 0, sizeof(update.update));
            update.update.secIndex = m_secIndex;
            update.clearing_sec = m_ClearingSecKey;
            update.clract = m_ClearingAccount;
            update.update.sideIndex = m_sideIndex;
            update.username = m_Owner;

            if (ROMVALUE::FUTURE == update.update.secIndex)
            {
                update.product = Product();
            }

            UpdateReplacingFields(*pReplace, update);

            if (m_Price != m_ReplacingPrice)
            {
                rc = ValidateSingleLimitPrice(m_ReplacingPrice, err);
                
                if (rc &&
                    CMDSClient::NeedValidatePrice(*this, true))
                {
                    switch(m_secIndex)
                    {
                    case ROMVALUE::EQUITY:
                        {
                            char price[50];
                            price[0] = '\0';

                            if (pReplace->GetAt(ROMFIELDS::CSV_PRICE, price, sizeof(price)))
                            {
                                long intPrice = CUtility::GetIntPrice(price);

                                rc = CMDSClient::getInstance().ValidatePrice(*this, true, intPrice, !pReplace->IsFieldEmpty(ROMFIELDS::CSV_NBBO), err);

                                if (rc)
                                {
                                    rc = CUtility::IsEquityPriceDecimalValid(price, err);
                                    if (rc)
                                    {
                                        m_ReplacingPrice = atof(price);
                                        pReplace->SetAt(ROMFIELDS::CSV_PRICE, price);
                                    }
                                }
                            }
                        }
                        break;
                    case ROMVALUE::OPTION:
                        {
                            std::string nbbo;
                            pReplace->GetAt(ROMFIELDS::CSV_NBBO, nbbo);
                            rc = CMDSClient::getInstance().ValidateOptionPrice(*this, m_ReplacingPrice, nbbo, err);
                        }
                        break;
                    }
                }
            }

            if (rc)
            {
                if (m_ShareChange + m_Leave <= 0)
                {
                    char text[100];
                    sprintf(text, "New Order Share has to be larger than current Cum(%ld)", m_Cum);
                    err = text;
                    rc = false;
                }
                else
                {
                    if (m_ShareChange > 0 && 
                        m_needPosMgr &&
                        m_Side != ROMVALUE::SIDE_BUY)
                    {
                        if (CPositionManager::getInstance().ProcessSSReplace(this, err) == false)
                        {
                            std::string subj = "Position warning";
                            char msg[1024];
                            sprintf(msg, "Replace<romTag=%s, ClearingAccount=%s, share change=%ld>:%s", 
                                    m_RomTag, m_ClearingAccount, m_ShareChange, err.c_str());
                            send_debug_message(msg);
                            CUtility::SendBizEMail(subj, msg, strlen(msg));
                        }
                        //do not handle user defined spread for now
                    }
                    
                    if (m_pCoveredSpread)
                    {
                        rc = CClientProfileManagerNew::getInstance().AllowedToReplaceCoveredSpreadOrder(update, m_Shares, m_pCoveredSpread, err);
                    }
                    else
                    {
                        rc = CClientProfileManagerNew::getInstance().AllowedToReplaceOrder(update, err);
                    }
                }
            }

            pReplace->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);

            if (false == rc) //did not pass the validation
            {
                if (m_ShareChange > 0 && 
                    m_needPosMgr &&
                    m_Side != ROMVALUE::SIDE_BUY)
                {
                    CPositionManager::getInstance().ProcessSSReplaceReject(this);
                }
                
                ResetReplacingFields();
                pReplace->SetAt(ROMFIELDS::CSV_TEXT, err);
                pReplace->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEREJECTED);
                
                if (rplreject_to_oats)
                {
                    QueueOatsMsg(pReplace);
                }
            }
            else
            {
                GetReplaceRequiredFields(*pReplace);
                pReplace->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEPENDING);

                QueueOatsMsg(pReplace);

                pReplace->SetAt(ROMFIELDS::CSV_STATUS, status);
                pReplace->SetAt(ROMFIELDS::CSV_TEXT, text);
                pReplace->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_REPLACE);

                databuf_t* pBuffer = CBuffStore::getInstance().Get();
                pReplace->GetMsgNew(pBuffer);
                rc = CDestsManager::getInstance().Send(m_Dest, ROMVALUE::COMMAND_REPLACE, pBuffer->buffer, databuf_unread(pBuffer), err);
                CBuffStore::getInstance().ReturnB(pBuffer);

                if (false == rc) //was not sent
                {
                    if (m_ShareChange > 0 && 
                        m_needPosMgr &&
                        m_Side != ROMVALUE::SIDE_BUY)
                    {
                        CPositionManager::getInstance().ProcessSSReplaceReject(this);
                    }
                    
                    pReplace->SetAt(ROMFIELDS::CSV_TEXT, err);
                    pReplace->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
                    pReplace->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);

                    QueueOatsMsg(pReplace);

                    update.update.shareChange *= -1;
                    update.update.dollarChange *= -1;

                    if (m_pCoveredSpread)
                    {
                        CClientProfileManagerNew::getInstance().UpdateBalanceCoveredSpread(update, m_pCoveredSpread, true);
                    }
                    else
                    {
                        CClientProfileManagerNew::getInstance().UpdateBalance(update);
                    }

                    ResetReplacingFields();
                }
            }
        }
        else
        {
            pReplace->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            pReplace->SetAt(ROMFIELDS::CSV_TEXT, err);
            pReplace->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEREJECTED);
            if (rplreject_to_oats)
            {
                QueueOatsMsg(pReplace);
            }
        }
    }

    return rc;
}


bool CRomOrderNew::Filter(const CCsv& filter)
{
    char secType;

    if (filter.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, secType) == true )//filter.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE) == false)
    {
        if ((CUtility::IsEquityType(secType) == true &&
             CUtility::IsEquityType(m_SecType) != true) ||
            (CUtility::IsOptionType(secType) == true &&
             CUtility::IsOptionType(m_SecType) != true) ||
            (CUtility::IsFutureType(secType) == true &&
             CUtility::IsFutureType(m_SecType) != true) ||
            (CUtility::IsCurrencyType(secType) == true &&
             CUtility::IsCurrencyType(m_SecType) != true))
        {
            return true;
        }
    }

    std::string exchange;
    if (filter.GetAt(ROMFIELDS::CSV_EXCHANGE, exchange) == true &&
        strcmp(m_Exchange, exchange.c_str()) != 0)
    {
        return true;
    }

    std::string dest;
    if (filter.GetAt(ROMFIELDS::CSV_ORDERDEST, dest) == true &&
        strcmp(m_Dest, dest.c_str()) != 0)
    {
        return true;
    }

    std::string trader;
    if (filter.GetAt(ROMFIELDS::CSV_TRADER, trader) == true &&
        strcmp(m_Trader, trader.c_str()) != 0)
    {
        return true;
    }

    std::string tradefor;
    if (filter.GetAt(ROMFIELDS::CSV_TRADE_FOR, tradefor) == true &&
        strcmp(m_TradeFor, tradefor.c_str()) != 0)
    {
        return true;
    }

    std::string clearingacct;
    if (filter.GetAt(ROMFIELDS::CSV_CLEARINGACCOUNT, clearingacct) == true &&
        strcmp(m_ClearingAccount, clearingacct.c_str()) != 0)
    {
        return true;
    }

    std::string symbol;
    if (filter.GetAt(ROMFIELDS::CSV_SYMBOL, symbol) == true &&
        strcmp(m_Symbol, symbol.c_str()) != 0)
    {
        return true;
    }

    short side = 0;
    if (filter.GetAt(ROMFIELDS::CSV_SIDE, side) == true &&
        m_Side != side)
    {
        return true;
    }

    short type = 0;
    if (filter.GetAt(ROMFIELDS::CSV_TYPE, type) == true &&
        m_Type != type)
    {
        return true;
    }

    double price = 0;
    if (filter.GetAt(ROMFIELDS::CSV_PRICE, price) == true &&
        m_Price != price)
    {
        return true;
    }

    long origshare = 0;
    if (filter.GetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, origshare) == true &&
        m_OriginalShares != origshare)
    {
        return true;
    }

    short tif;
    if (filter.GetAt(ROMFIELDS::CSV_TIF, tif) == true &&
        m_TIF != tif)
    {
        return true;
    }

    if (CUtility::IsOptionType(m_SecType))
    {
        std::string underlying;
        if (filter.GetAt(ROMFIELDS::CSV_UNDERLYING, underlying) == true &&
            strcmp(m_Underlying, underlying.c_str()) != 0)
        {
            return true;
        }

        /*double strike = 0, lstrike = 0;
        if (filter.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike) == true)
        {
            if (GetStrikePrice(lstrike))
            {
                if (lstrike != strike)
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }*/

        std::string strikeprice;
        if (filter.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strikeprice))
        {
            if (strcmp(m_StrikePrice, strikeprice.c_str()))
            {
                return true;
            }
        }

        char callput;
        if (filter.GetAt(ROMFIELDS::CSV_CALLPUT, callput) == true &&
            m_CallOrPut != callput)
        {
            return true;
        }
    }

    if (CUtility::IsEquityType(m_SecType) == false)
    {
        std::string expmonth;
        if (filter.GetAt(ROMFIELDS::CSV_EXPYEARMONTH, expmonth) == true &&
            strcmp(m_ExpYearMonth, expmonth.c_str()) != 0)
        {
            return true;
        }
    }

    return false;
}


void CRomOrderNew::UpdatePos(long qty_change)
{
    tPosUpdate posUpdate;
    posUpdate.clract = m_ClearingAccount;
    posUpdate.qty_change = qty_change;
    posUpdate.side = m_Side;
    posUpdate.symbol = m_Symbol;
    CPositionManager::getInstance().ProcessPositionUpdate(&posUpdate);
}

void CRomOrderNew::UpdatePosOnly(long qty_change)
{
    tPosUpdate posUpdate;
    posUpdate.clract = m_ClearingAccount;
    posUpdate.qty_change = qty_change;
    posUpdate.side = m_Side;
    posUpdate.symbol = m_Symbol;
    CPositionManager::getInstance().UpdatePositionOnly(&posUpdate);
}

void CRomOrderNew::UpdateLendingBalanceOnly(long qty_change)
{
    tPosUpdate posUpdate;
    posUpdate.clract = m_ClearingAccount;
    posUpdate.qty_change = qty_change;
    posUpdate.side = m_Side;
    posUpdate.symbol = m_Symbol;
    CPositionManager::getInstance().UpdateLendingBalanceOnly(&posUpdate);
}

bool CRomOrderNew::NeedEquityMidPrice() const
{
    bool rc = false;
    
    if (m_SecType == ROMVALUE::SECTYPE_EQUITY && 
        CRomOrderNew::IsLimitType(m_Type) == false &&
        m_Type != ROMVALUE::TYPE_MARKETATOPEN)
    {
        rc = true;
    }
    
    return rc;
}

bool CRomOrderNew::GetPosUpdate(tPosUpdate& update)
{
    bool rc = false;
    if (m_needPosMgr)
    {
        rc = true;
        update.clract = m_ClearingAccount;
        update.qty_change = m_Leave;
        update.side = m_Side;
        update.symbol = m_Symbol;
    }
    return rc;
}

void CRomOrderNew::CreateCoveredSpread(const std::vector<CRomOrderNew*>& legs)
{
    m_pCoveredSpread = new tCoveredSpread;
    memset(m_pCoveredSpread, 0, sizeof(tCoveredSpread));
    
    for (size_t i = 0; i < legs.size(); i++)
    {
        if (legs[i]->SecType() == ROMVALUE::SECTYPE_EQUITY)
        {
            m_pCoveredSpread->e_sideIndex = legs[i]->SideIndex();
            m_pCoveredSpread->e_ratio = legs[i]->OriginalShares();
        }
        else
        {
            m_pCoveredSpread->o_ratio += legs[i]->OriginalShares();
        }
    }

    SetDollasAndSideIndexes();
}

void CRomOrderNew::CreateCoveredSpread(const std::vector<CCsv*>& legs)
{
    m_pCoveredSpread = new tCoveredSpread;
    memset(m_pCoveredSpread, 0, sizeof(tCoveredSpread));
   
    for (size_t i = 0; i < legs.size(); i++)
    {
        short side = ROMVALUE::SIDE_BUY;
        if (legs[i]->GetAt(ROMFIELDS::CSV_SIDE, side))
        {
            unsigned short sideIndex = CRomOrderNew::GetSideIndexFromSide(side);
            char sectype = ROMVALUE::SECTYPE_EQUITY;
            if (legs[i]->GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype))
            {
                long originalshares = 0;
                if (legs[i]->GetAt(ROMFIELDS::CSV_ORIGINAL_SHARES, originalshares))
                {
                    if (ROMVALUE::SECTYPE_EQUITY == sectype)
                    {
                        m_pCoveredSpread->e_sideIndex = sideIndex;
                        m_pCoveredSpread->e_ratio = originalshares;
                    }
                    else
                    {
                        m_pCoveredSpread->o_ratio += originalshares;
                    }
                }
            }
        }
    }

    SetDollasAndSideIndexes();
}

void CRomOrderNew::CreateCoveredSpread()
{
    if (strlen(m_Instructions) >= 5 &&
        m_Instructions[0] == 'S' &&
        m_Instructions[1] == ' ' &&
        m_Instructions[3] == ' ')
    {
        char eside[2];
        eside[0] = m_Instructions[2];
        eside[1] = '\0';
        short side = atoi(eside);
        if (ROMVALUE::SIDE_BUY == side ||
            ROMVALUE::SIDE_SELL == side ||
            ROMVALUE::SIDE_SHORT == side)
        {
            char* sratio = &(m_Instructions[4]);
            long ratio = atol(sratio);
            if (ratio > 0)
            {
                m_pCoveredSpread = new tCoveredSpread;
                memset(m_pCoveredSpread, 0, sizeof(tCoveredSpread));
                m_pCoveredSpread->e_ratio = ratio;
                m_pCoveredSpread->o_ratio = 1;
                m_pCoveredSpread->e_sideIndex = CRomOrderNew::GetSideIndexFromSide(side);
               
                SetDollasAndSideIndexes();
            }
        }
    }
}

void CRomOrderNew::SetDollasAndSideIndexes()
{
    short e_sign = 1;
    if (ROMVALUE::SELL == m_pCoveredSpread->e_sideIndex)
    {
        e_sign = -1;
    }
    
    //side of equity leg is given
    //find effective side of option through 
    //finding the sign of effective option price!!!
    double e_dollars = e_sign * m_pCoveredSpread->e_ratio * CMDSClient::getInstance().GetEquityMidPrice(m_Underlying);
    double o_dollars = m_Price * 100 - e_dollars;
    m_pCoveredSpread->o_sideIndex = (o_dollars >= 0)? ROMVALUE::BUY : ROMVALUE::SELL;

    m_pCoveredSpread->e_dollars = fabs(e_dollars);
    m_pCoveredSpread->o_dollars = fabs(o_dollars);

    //update the sideIndex with body's sideIndex
    if (ROMVALUE::SELL == m_sideIndex)
    {
        if (ROMVALUE::BUY == m_pCoveredSpread->e_sideIndex)
        {
            m_pCoveredSpread->e_sideIndex = ROMVALUE::SELL;
        }
        else
        {
            m_pCoveredSpread->e_sideIndex = ROMVALUE::BUY;
        }

        if (ROMVALUE::BUY == m_pCoveredSpread->o_sideIndex)
        {
            m_pCoveredSpread->o_sideIndex = ROMVALUE::SELL;
        }
        else
        {
            m_pCoveredSpread->o_sideIndex = ROMVALUE::BUY;
        }
    }
}
/*void CRomOrderNew::AddLeg(CRomOrderNew* pOrder)
{
    if (NULL == m_Legs)
    {
        m_Legs = new std::vector<CRomOrderNew*>;
    }
    m_Legs->push_back(pOrder);
}*/

/*void CRomOrderNew::FlipSides()
{
    flipside();
    if (m_Legs)
    {
        for (unsigned int i = 0; i < m_Legs->size(); i++)
        {
            (*m_Legs)[i]->flipside();
        }
    }
}*/

/*
void CRomOrderNew::flipside()
{
    switch (m_Side)
    {
        case ROMVALUE::SIDE_BUY:
            m_Side = ROMVALUE::SIDE_SELL;
            break;
        case ROMVALUE::SIDE_SELL:
        case ROMVALUE::SIDE_SHORT:
            m_Side = ROMVALUE::SIDE_BUY;
            break;
    }
}*/
