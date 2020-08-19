#include "RomOrdersReplay.h"
#include "ThreadRomOrdersNew.h"
#include "DestsManager.h"
#include "BuffStore.h"

std::set<short> CRomOrder::s_aActionStatus;


void CRomOrder::aLoadActionStatus()
{
    s_aActionStatus.insert(ROMVALUE::STATUS_PENDINGCANCEL);
    s_aActionStatus.insert(ROMVALUE::STATUS_REPLACEPENDING);
    s_aActionStatus.insert(ROMVALUE::STATUS_CANCELREJECTED);
    s_aActionStatus.insert(ROMVALUE::STATUS_REPLACEREJECTED);
}

CRomOrder::CRomOrder(CCsv& csv)
{
    memset(this, 0, sizeof(CRomOrder));
    m_Multiplier = 0;  //indicate client did not set it!
    m_NextExecId = 0;
    
    if (csv.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType) == false)
    {
        m_SecType = 'E';
        csv.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, 'E');
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
    //m_sideIndex = CRomOrderNew::GetSideIndexFromSide(m_Side);

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
    SetParentRomTag("");
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
    csv.GetAt(ROMFIELDS::CSV_OPENCLOSE, nbuf);
    m_OpenClose = nbuf;
    csv.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, m_CplxType);

    csv.GetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest, sizeof(m_Dest));
    bool bOptionOnFuture = CDestsManager::getInstance().IsOptionOnFuture(atoi(m_Exchange), m_SecType);
    if (bOptionOnFuture)
    {
        m_secIndex = ROMVALUE::OPTIONONFUTURE;
    }
    else
    {
        m_secIndex = CRomOrderNew::GetSecIndexFromSecType(m_SecType);
    }
    
    //need to set the multiplier correctly here
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
    else if (ROMVALUE::FUTURE != m_secIndex)
    {
        //for future product
        CRomMap<double>& FutureProductMultiplier = CThreadRomOrdersNew::getInstance().ProductMultiplier();
        std::string key;
        size_t ls = get_symbol_len(const_cast<char*>(m_Symbol), strlen(m_Symbol));
        key.assign(m_Symbol, ls);
        double multiplier = 1;
        if (FutureProductMultiplier.Get(key, multiplier))
        {
            m_Multiplier = multiplier;
        }
    }
    
    SetOmExTag(csv);
}

void CRomOrder::SetOmExTag(CCsv& msg)
{
    char omExTag[128];
    sprintf(omExTag, "%s-%ld", m_RomTag, m_NextExecId);
    m_NextExecId++;
    msg.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
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

void CRomOrder::Update1(CCsv& csv)
{
    int status = 0;
    std::string clientTag = m_ClientTag;
    int precision = (m_SecType == ROMVALUE::SECTYPE_EQUITY)? 4 : 8;
    csv.GetAt(ROMFIELDS::CSV_STATUS, status);

    int cplxType = ROMVALUE::CPLXTYPE_SINGLE;
    csv.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxType);
    
    std::string orderTag;
    csv.GetAt(ROMFIELDS::CSV_ORDER_TAG, orderTag);

    if (status != ROMVALUE::STATUS_FILLED &&
        status != ROMVALUE::STATUS_PARTIALLYFILLED)
    {
        csv.SetAt(ROMFIELDS::CSV_LAST_SHARES, "0");
        csv.SetAt(ROMFIELDS::CSV_LAST_PRICE, "0");
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
    csv.GetAt(ROMFIELDS::CSV_EXCHANGE_TAG, orderTag);
    if (orderTag.empty() == false)
    {
        SetExchangeTag(orderTag.c_str());
    }

    switch (status)
    {
    case ROMVALUE::STATUS_OPEN:
        {
            int type = 0;
            csv.GetAt(ROMFIELDS::CSV_TYPE, type);
            if (Type() == ROMVALUE::TYPE_STOP &&
                type == ROMVALUE::TYPE_STOPLIMIT)
            {
                csv.GetAt(ROMFIELDS::CSV_PRICE, m_Price);
                SetType(ROMVALUE::TYPE_STOPLIMIT);
            }
            
            if (ROMVALUE::CPLXTYPE_PRODUCT_SPREAD == cplxType)
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

            if (csv.GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares) == false)
            {
                csv.GetAt(ROMFIELDS::CSV_SHARES, lastShares);
            }

            csv.GetAt(ROMFIELDS::CSV_LAST_PRICE, lastPrice);
            double cash = m_Cum * m_AvgPrice + lastShares * lastPrice;
            m_Cum += lastShares;
            m_Leave -= lastShares;
            SetAvgPrice(cash / m_Cum);
        }
        break;
    case ROMVALUE::STATUS_REPLACED:
        UpdateReplacedFields(csv);
        //trust exchange interface to always pass back the correct leaves for leg!!
        if (m_CplxType == ROMVALUE::CPLXTYPE_LEG)
        {
            csv.GetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
            csv.GetAt(ROMFIELDS::CSV_SHARES, m_Shares);
        }
        csv.SetAt(ROMFIELDS::CSV_NEW_TAG, "");
        break;
    case ROMVALUE::STATUS_REPLACEREJECTED:
        csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_NewClientTag);
        //m_ReplacePending = false;
      
        break;
    case ROMVALUE::STATUS_REPLACEPENDING:
        csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_NewClientTag);
        break;
    case ROMVALUE::STATUS_EXPIRED:
        m_Leave = 0;
        break;
    case ROMVALUE::STATUS_DONEFORDAY:
        if (m_TIF != ROMVALUE::TIF_GTC)
        {
            m_Leave = 0;
        }
        break;
    case ROMVALUE::STATUS_CANCELED:
        csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag);
        m_Leave = 0;
        break;
    case ROMVALUE::STATUS_CANCELREJECTED:
        csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag);
        //m_CancelPending = false;
        break;
    case ROMVALUE::STATUS_PENDINGCANCEL:
        csv.SetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag);
        break;
    case ROMVALUE::STATUS_ORDERREJECTED:
        //m_CancelPending = false;
        m_Leave = 0;
        break;
    }

    if (IsaActionStatus(status) == false)
    {
        m_Status = status;
    }

    //Adding all the fields required by FIX
    if (CUtility::IsFutureType(m_SecType) == true)
    {
        csv.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth);
    }
    else if (CUtility::IsOptionType(m_SecType) == true)
    {
        csv.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, m_ExpYearMonth);
        csv.SetAt(ROMFIELDS::CSV_MATURITYDAY, m_MaturityDay);
        csv.SetAt(ROMFIELDS::CSV_CALLPUT, m_CallOrPut);
        csv.SetAt(ROMFIELDS::CSV_STRIKEPRICE, m_StrikePrice);
        csv.SetAt(ROMFIELDS::CSV_OPENCLOSE, m_OpenClose);
        csv.SetAt(ROMFIELDS::CSV_UNDERLYING, m_Underlying);
    }

    csv.SetAt(ROMFIELDS::CSV_EXCHANGE_TAG, m_ExchangeTag);
    csv.SetAt(ROMFIELDS::CSV_SYMBOL, m_Symbol);
    csv.SetAt(ROMFIELDS::CSV_AVG_PRICE, m_AvgPrice, precision);
    csv.SetAt(ROMFIELDS::CSV_CUM_SHARES, m_Cum);
    csv.SetAt(ROMFIELDS::CSV_EXCHANGE, m_Exchange);
    csv.SetAt(ROMFIELDS::CSV_SIDE, m_Side);
    csv.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, m_Leave);
    csv.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, m_SecType);
    csv.SetAt(ROMFIELDS::CSV_SEC_DEF, m_SecDef);
    csv.SetAt(ROMFIELDS::CSV_TAG, clientTag);
    //csv.SetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag);
    csv.SetAt(ROMFIELDS::CSV_CLIENT_ECHO, m_ClientEcho);
    csv.SetAt(ROMFIELDS::CSV_ORDERDEST, m_Dest);
    csv.SetAt(ROMFIELDS::CSV_OWNER, m_Owner);
    if (m_CplxType)
    {
        csv.SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, m_CplxType);
    }
    //always populate replaceable fields
    if (csv.IsFieldEmpty(ROMFIELDS::CSV_TYPE))
    {
        csv.SetAt(ROMFIELDS::CSV_TYPE, m_Type);
    }

    csv.SetAt(ROMFIELDS::CSV_SHARES, m_Shares);
    
    if (csv.IsFieldEmpty(ROMFIELDS::CSV_PEGPRICE_OFFSET))
    {
        csv.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, m_PegOffset);
    }

    if (IsLimitType(m_Type) == true &&
        csv.IsFieldEmpty(ROMFIELDS::CSV_PRICE))
    {
        csv.SetAt(ROMFIELDS::CSV_PRICE, m_Price, precision);
    }

    if (csv.IsFieldEmpty(ROMFIELDS::CSV_STOPPRICE))
    {
        if (Type() == ROMVALUE::TYPE_STOP  ||
            Type() == ROMVALUE::TYPE_STOPLIMIT)
        {
            csv.SetAt(ROMFIELDS::CSV_STOPPRICE, m_StopPrice);
        }
    }

    if (csv.IsFieldEmpty(ROMFIELDS::CSV_TIF))
    {
        csv.SetAt(ROMFIELDS::CSV_TIF, m_TIF);
    }

    if (csv.IsFieldEmpty(ROMFIELDS::CSV_EXEC_INSTRUCTION))
    {
        csv.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, m_ExecInst);
    }

    if (csv.IsFieldEmpty(ROMFIELDS::CSV_FLOOR))
    {
        csv.SetAt(ROMFIELDS::CSV_FLOOR, m_MaxFloor);
    }

    if (csv.IsFieldEmpty(ROMFIELDS::CSV_MINQTY))
    {
        if (m_MinQty)
        {
            csv.SetAt(ROMFIELDS::CSV_MINQTY, m_MinQty);
        }
    }

    if (csv.IsFieldEmpty(ROMFIELDS::CSV_INSTRUCTIONS))
    {
        csv.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, m_Instructions);
    }

    if (m_ReplacingTIF == ROMVALUE::TIF_GTD &&
        csv.IsFieldEmpty(ROMFIELDS::CSV_GTD_DATE))
    {
        csv.SetAt(ROMFIELDS::CSV_GTD_DATE, m_ReplacingExpTime);
    }
}

void CRomOrder::UpdateReplacedFields(CCsv& msg)
{
    if (strlen(m_NewClientTag) > 0)
    {
        SetClientTag(m_NewClientTag);
    }

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

void CRomOrder::UpdateReplacingFields(CCsv& msg)
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
    
    //char price[50];
    //price[0] = '\0';

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
}

bool CRomOrder::IsaActionStatus(short status)
{
    std::set<short>::iterator it = s_aActionStatus.find(status);
    if (it != s_aActionStatus.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CRomOrder::Update(CCsv& csv)
{
    bool rc = true;
    
    int status = ROMVALUE::STATUS_NEW;
    
    SetOmExTag(csv);
    
    csv.GetAt(ROMFIELDS::CSV_STATUS, status);
    
    if (ROMVALUE::STATUS_ROMCANCELPENDING == status)
    {
        csv.GetAt(ROMFIELDS::CSV_NEW_TAG, m_ActClientTag, sizeof(m_ActClientTag));
    }
    else if (ROMVALUE::STATUS_ROMREPLACEPENDING == status)
    {
        //save replacing values
        UpdateReplacingFields(csv);
    }
    else
    {
        if ((ROMVALUE::STATUS_PARTIALLYFILLED == status ||
             ROMVALUE::STATUS_FILLED == status) &&
             csv.IsFieldEmpty(ROMFIELDS::CSV_EXEC_TAG) &&
             m_CplxType != 3)
        {
            rc = false;
        }
        else
        {
            Update1(csv);
        }
    }
    
    //create oats messages
    return rc;
}

void CRomOrder::GetCsv(CCsv& csv) const
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
    //csv.SetAt(ROMFIELDS::CSV_PARENT_TAG, m_ParentClientTag);
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
            csv.SetAt(ROMFIELDS::CSV_OPENCLOSE, (short)m_OpenClose);
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

void CRomOrder::InheritParent(CRomOrder* pParentOrder)
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
}

CRomOrdersReplay::CRomOrdersReplay()
{
    std::string outfile = "outfile.log";
    std::vector<std::string> infiles;
    /*infiles.push_back("ROM23Log20190605.LOG");
    infiles.push_back("ROM23Log20190605.13.30.01.LOG");
    infiles.push_back("ROM23Log20190605.17.01.31.LOG");
    infiles.push_back("ROM23Log20190605.18.43.45.LOG");
    CRomOrdersReplay replay("ROM23_oatsgoing.log", infiles);*/
    
    infiles.push_back("ROM9Log20190605.LOG");
    infiles.push_back("ROM9Log20190605.13.54.30.LOG");
    infiles.push_back("ROM9Log20190605.14.07.57.LOG");
    infiles.push_back("ROM9Log20190605.14.26.00.LOG");
    infiles.push_back("ROM9Log20190605.14.40.08.LOG");
    infiles.push_back("ROM9Log20190605.14.56.00.LOG");
    infiles.push_back("ROM9Log20190605.15.12.45.LOG");
    infiles.push_back("ROM9Log20190605.15.30.00.LOG");
    infiles.push_back("ROM9Log20190605.15.50.42.LOG");
    infiles.push_back("ROM9Log20190605.16.12.04.LOG");
    infiles.push_back("ROM9Log20190605.16.33.42.LOG");
    infiles.push_back("ROM9Log20190605.16.58.58.LOG");
    infiles.push_back("ROM9Log20190605.17.03.59.LOG");
    infiles.push_back("ROM9Log20190605.18.08.43.LOG");
    infiles.push_back("ROM9Log20190605.19.46.35.LOG");
    infiles.push_back("ROM9Log20190606.00.05.15.LOG");
    CRomOrdersReplay replay("ROM9_oatsgoing.log", infiles);
    Init();
    
    FILE* out_fp = fopen(outfile.c_str(), "w+");
    m_notfoundF = fopen("notfound.log", "w+");
    
    for (int i = 0; i < infiles.size(); i++)
    {
        FILE* in_fp = fopen(infiles[i].c_str(), "r");
        if (in_fp != NULL)
        {
            char buf[11024];
            
            while (fgets(buf, sizeof(buf), in_fp))
            {
                CCsv csv;
                char* p = TheDirtyJob(buf, csv);
                if (p)
                {
                    char* bUserDefinedCplx = strstr(p, "!#!");
                    if (bUserDefinedCplx)
                    {
                        HandleUserDefeinedSpread(p, out_fp);
                    }
                    else
                    {
                        UpdateOrder(csv, out_fp);
                    }
                }
            }
            fclose(in_fp);
        }
    }
    
    if (out_fp)
    {
        fclose(out_fp);
    }
}

void CRomOrdersReplay::HandleUserDefeinedSpread(const char* buffer, FILE* outFile)
{
    std::string l_sub(buffer, strlen(buffer));
    std::size_t idx = 0;
    std::string::size_type pos = 0;
    
    idx = l_sub.find("!#!", pos);
    std::string orderStr = l_sub.substr(pos, idx - pos);
    CCsv* pCsv = CStore<CCsv>::getInstance().Get();
    pCsv->Set(orderStr);
        
    CRomOrder* pOrder = new CRomOrder(*pCsv);
    m_RomTagToOrder.Add(pOrder->RomTag(), pOrder);
    CRomOrder* pParent = pOrder;
    
    CCsv oatsMsg;
    MakeOatsMsg(pOrder, *pCsv, oatsMsg);

    databuf_t* pBuffer = CBuffStore::getInstance().Get();
    oatsMsg.GetMsgNew(pBuffer);
    fwrite(pBuffer->buffer, databuf_unread(pBuffer), 1, outFile);
    CBuffStore::getInstance().ReturnB(pBuffer);
    CStore<CCsv>::getInstance().Return(pCsv);
    
    pos += idx - pos + 3;
    
    while ((idx = l_sub.find("!#!", pos)) != std::string::npos)
    {
        orderStr = l_sub.substr(pos, idx - pos);
        pCsv = CStore<CCsv>::getInstance().Get();
        pCsv->Set(orderStr);
        
        CRomOrder* pOrder = new CRomOrder(*pCsv);
        pCsv->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_NEW);
        
        m_RomTagToOrder.Add(pOrder->RomTag(), pOrder);
        pOrder->InheritParent(pParent);
        
        CCsv oatsMsg;
        MakeOatsMsg(pOrder, *pCsv, oatsMsg);

        pBuffer = CBuffStore::getInstance().Get();
        oatsMsg.GetMsgNew(pBuffer);
        fwrite(pBuffer->buffer, databuf_unread(pBuffer), 1, outFile);
        CBuffStore::getInstance().ReturnB(pBuffer);
        CStore<CCsv>::getInstance().Return(pCsv);
    
        pos += idx - pos + 3;
        if (pos >= strlen(buffer))
        {
            break;
        }
    }
}

void CRomOrdersReplay::UpdateOrder(CCsv& msg, FILE* outFile)
{
    char cmd = ROMVALUE::COMMAND_ORDER;
    int status = ROMVALUE::STATUS_NEW;
    int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
    std::string omTag;
    CRomOrder* pOrder = NULL;
    databuf_t* pBuffer = NULL;
    
    msg.GetAt(ROMFIELDS::CSV_STAFFORD_TAG, omTag);
    msg.GetAt(ROMFIELDS::CSV_COMMAND, cmd);
    msg.GetAt(ROMFIELDS::CSV_STATUS, status);
    msg.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);
    
    switch(cmd)
    {
        case ROMVALUE::COMMAND_CANCEL:
            msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
            cmd = ROMVALUE::COMMAND_STATUS;
            break;
        case ROMVALUE::COMMAND_REPLACE:
            msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEPENDING);
            cmd = ROMVALUE::COMMAND_STATUS;
            break;
    }
    
    
    if (ROMVALUE::COMMAND_ORDER == cmd)
    {
        //need to take care of user defined complex orders!!!!!!!!!!
        //if (ROMVALUE::CPLXTYPE_SPREAD == cplxtype)
        //{
        
        //}
        //else
        //{
            pOrder = new CRomOrder(msg);
            m_RomTagToOrder.Add(omTag, pOrder);

            CCsv oatsMsg;
            MakeOatsMsg(pOrder, msg, oatsMsg);

            pBuffer = CBuffStore::getInstance().Get();
            oatsMsg.GetMsgNew(pBuffer);
            fwrite(pBuffer->buffer, databuf_unread(pBuffer), 1, outFile);
            CBuffStore::getInstance().ReturnB(pBuffer);
        //}
    }
    else if (ROMVALUE::COMMAND_STATUS == cmd)
    {
        if (ROMVALUE::STATUS_ORDERREJECTED == status &&
            m_RomTagToOrder.Get(omTag, pOrder) == false)
        {
            //rejected by ROM,  need to make an order
            std::string errstr;
            msg.GetAt(ROMFIELDS::CSV_TEXT, errstr);
            
            msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_ORDER);
            msg.SetAt(ROMFIELDS::CSV_TEXT, "");
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_NEW);
            
            pOrder = new CRomOrder(msg);
            m_RomTagToOrder.Add(omTag, pOrder);
            
            CCsv oatsMsg;
            MakeOatsMsg(pOrder, msg, oatsMsg);
            
            pBuffer = CBuffStore::getInstance().Get();
            oatsMsg.GetMsgNew(pBuffer);
            fwrite(pBuffer->buffer, databuf_unread(pBuffer), 1, outFile);
            CBuffStore::getInstance().ReturnB(pBuffer);
            
            msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
            msg.SetAt(ROMFIELDS::CSV_TEXT, errstr);
        }
        
        if (m_RomTagToOrder.Get(omTag, pOrder))
        {
            int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
            msg.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);
            bool skip = false;
            
            if (ROMVALUE::CPLXTYPE_LEG == cplxtype)
            {
                if (msg.IsFieldEmpty(ROMFIELDS::CSV_UNDERLYING))
                {
                    msg.SetAt(ROMFIELDS::CSV_UNDERLYING, pOrder->Underlying());
                }

                if (msg.IsFieldEmpty(ROMFIELDS::CSV_SECURITY_TYPE))
                {
                    msg.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, pOrder->SecType());
                }

                msg.SetAt(ROMFIELDS::CSV_CAPACITY, pOrder->Capacity());
                msg.SetAt(ROMFIELDS::CSV_TRADER, pOrder->Trader());
                msg.SetAt(ROMFIELDS::CSV_TRADE_FOR, pOrder->TradeFor());
                msg.SetAt(ROMFIELDS::CSV_TAG, pOrder->ClientTag());
                msg.SetAt(ROMFIELDS::CSV_EXCHANGE, pOrder->Exchange());;
                msg.SetAt(ROMFIELDS::CSV_OWNER, pOrder->Owner());
                pOrder->SetOmExTag(msg);
            }
            else
            {
                skip = !(pOrder->Update(msg));
            }
            
            if (false == skip)
            {
                CCsv oatsMsg;
                MakeOatsMsg(pOrder, msg, oatsMsg);

                pBuffer = CBuffStore::getInstance().Get();
                oatsMsg.GetMsgNew(pBuffer);
                fwrite(pBuffer->buffer, databuf_unread(pBuffer), 1, outFile);
                CBuffStore::getInstance().ReturnB(pBuffer);
            }
            
            if (pOrder->Leave() == 0 && 
                pOrder->CplxType() != ROMVALUE::CPLXTYPE_SPREAD)
            {
                if (m_RomTagToOrder.Remove(omTag, pOrder))
                {
                    delete pOrder;
                }
                
            }
        }
        else //order not found
        {
            pBuffer = CBuffStore::getInstance().Get();
            msg.GetMsgNew(pBuffer);
            fwrite(pBuffer->buffer, databuf_unread(pBuffer), 1, m_notfoundF);
            CBuffStore::getInstance().ReturnB(pBuffer);
        }
    }
}

char* CRomOrdersReplay::TheDirtyJob(const char* buffer, CCsv& csv)
{
    char* rc = NULL;
    char cmd = ' ';
    char* lp = NULL;
    
    char* p = const_cast<char*>(strstr(buffer, m_ToStr.c_str()));
    if (p)
    {
        p += 7;
        lp = strstr(p, "  ");
        if (lp)
        {
            char receiver[128];
            strncpy(receiver, p, lp - p);
            receiver[lp - p] = '\0';
            std::set<std::string>::iterator it = m_DropSet.find(receiver);
            if (it == m_DropSet.end())
            {
                lp += 2;
                if (*lp == ROMVALUE::COMMAND_ORDER ||
                    *lp == ROMVALUE::COMMAND_STATUS ||
                    *lp == ROMVALUE::COMMAND_CANCEL ||
                    *lp == ROMVALUE::COMMAND_REPLACE)
                {
                    csv.Set(lp, strlen(lp) - 1);
                    rc = lp;
                }
            }
        }
    }
    else 
    {
        p = const_cast<char*>(strstr(buffer, m_FromStr.c_str()));
        if (p)
        {
            lp = strstr(p, "  ");
            if (lp)
            {
                lp += 2;
                if (strncmp(lp, m_OrderNotFound.c_str(), m_OrderNotFound.length()) == 0)
                {
                    lp += m_OrderNotFound.length();
                    if (*lp == ROMVALUE::COMMAND_STATUS)
                    {
                        csv.Set(lp, strlen(lp) - 1);
                        rc = lp;
                    }
                }
            }
            
        }
        
    }
    return rc;
}

void CRomOrdersReplay::MakeOatsMsg(CRomOrder* pOrder, CCsv& msg, CCsv& OatsMsg)
{
    pOrder->GetCsv(OatsMsg);
    OatsMsg.SetAt(ROMFIELDS::CSV_OATSMULTIPLIER, pOrder->Multiplier(), 2);

    int cplxType = -1;
    int type = ROMVALUE::TYPE_LIMIT;
    msg.GetAt(ROMFIELDS::CSV_TYPE, type);

    if (msg.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxType) == true)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxType);
    }

    std::string exDest;
    if (msg.GetAt(ROMFIELDS::CSV_EXDEST, exDest) == true)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_EXDEST, exDest);
    }

    std::string omExTag;
    msg.GetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);
    OatsMsg.SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, omExTag);

    char cmd = ROMVALUE::COMMAND_ORDER;
    msg.GetAt(ROMFIELDS::CSV_COMMAND, cmd);
    OatsMsg.SetAt(ROMFIELDS::CSV_COMMAND, cmd);

    int status = -1;
    msg.GetAt(ROMFIELDS::CSV_STATUS, status);

     
    std::string mktAccnt;
    if (msg.GetAt(ROMFIELDS::CSV_MM_ACCOUNT, mktAccnt))
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_MM_ACCOUNT, mktAccnt);
    }

    std::string sTime;
    msg.GetAt(ROMFIELDS::CSV_TIME, sTime);

    if (status == ROMVALUE::STATUS_ROMCANCELPENDING ||
        status == ROMVALUE::STATUS_ROMREPLACEPENDING ||
        cmd == ROMVALUE::COMMAND_ORDER)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_CLIENT_TIME, sTime);
    }

    if (CUtility::IsFutureType(pOrder->SecType()))
    {
        std::string instruction;
        OatsMsg.GetAt(ROMFIELDS::CSV_INSTRUCTIONS, instruction);
        OatsMsg.SetAt(ROMFIELDS::CSV_ACCOUNT, instruction);
    }

    long lastShares = 0;
    msg.GetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
    if (cmd == ROMVALUE::COMMAND_STATUS )
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_COMMAND, cmd);
        OatsMsg.SetAt(ROMFIELDS::CSV_STATUS, status);
        OatsMsg.SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);
        OatsMsg.SetAt(ROMFIELDS::CSV_CLIENT_TIME, sTime);

        std::string execTime;
        msg.GetAt(ROMFIELDS::CSV_EXEC_TIME, execTime);
        OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_TIME, execTime);

        std::string execCfm;
        msg.GetAt(ROMFIELDS::CSV_EXEC_CONFIRM, execCfm);
        OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_CONFIRM, execCfm);

        std::string ref;
        msg.GetAt(ROMFIELDS::CSV_REFERENCE, ref);
        OatsMsg.SetAt(ROMFIELDS::CSV_REFERENCE, ref);

        std::string report;
        msg.GetAt(ROMFIELDS::CSV_REPORT, report);
        OatsMsg.SetAt(ROMFIELDS::CSV_REPORT, report);

        std::string clxTag;
        msg.GetAt(ROMFIELDS::CSV_CANCEL_TAG, clxTag);
        OatsMsg.SetAt(ROMFIELDS::CSV_CANCEL_TAG, clxTag);

        std::string contra;
        msg.GetAt(ROMFIELDS::CSV_CONTRA, contra);
        OatsMsg.SetAt(ROMFIELDS::CSV_CONTRA, contra);

        std::string effTime;
        msg.GetAt(ROMFIELDS::CSV_EFFECTIVE_TIME, effTime);
        OatsMsg.SetAt(ROMFIELDS::CSV_EFFECTIVE_TIME, effTime);

        std::string bilCode;
        msg.GetAt(ROMFIELDS::CSV_BILLING_CODE, bilCode);
        OatsMsg.SetAt(ROMFIELDS::CSV_BILLING_CODE, bilCode);

        std::string lastPrice;
        msg.GetAt(ROMFIELDS::CSV_LAST_PRICE, lastPrice);
        OatsMsg.SetAt(ROMFIELDS::CSV_LAST_PRICE, lastPrice);

        if (status == ROMVALUE::STATUS_FILLED ||
            status == ROMVALUE::STATUS_PARTIALLYFILLED)
        {
            std::string execTag;
            msg.GetAt(ROMFIELDS::CSV_EXEC_TAG, execTag);
            OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_TAG, execTag);
        }

        if (status == ROMVALUE::STATUS_ROMREPLACEPENDING)
        {
            //need to override all the replaceable fields with m_ReplacingXXX
            if (CRomOrderNew::IsLimitType(pOrder->ReplacingType()))
            {
                if (pOrder->SecType() == ROMVALUE::SECTYPE_EQUITY)
                {
                    OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, pOrder->ReplacingPrice(), 4);
                }
                else
                {
                    OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, pOrder->ReplacingPrice(), 8);
                }
            }
            else
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, "");
            }

            OatsMsg.SetAt(ROMFIELDS::CSV_SHARES, pOrder->Leave() + pOrder->Cum() + pOrder->ShareChange());
            std::string reducedQty;
            if (msg.GetAt(ROMFIELDS::CSV_QUERY_TAG, reducedQty))
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_QUERY_TAG, reducedQty);
            }

            OatsMsg.SetAt(ROMFIELDS::CSV_TYPE, pOrder->ReplacingType());
            OatsMsg.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, pOrder->ReplacingPegOffset());
            OatsMsg.SetAt(ROMFIELDS::CSV_STOPPRICE, pOrder->ReplacingStopPrice());
            OatsMsg.SetAt(ROMFIELDS::CSV_TIF, pOrder->ReplacingTIF());
            OatsMsg.SetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, pOrder->ReplacingExecInst());
            OatsMsg.SetAt(ROMFIELDS::CSV_FLOOR, pOrder->ReplacingMaxFloor());
            OatsMsg.SetAt(ROMFIELDS::CSV_INSTRUCTIONS, pOrder->ReplacingInstructions());

            if (pOrder->ReplacingMinQty())
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_MINQTY, pOrder->ReplacingMinQty());
            }

            if (pOrder->ReplacingTIF() == ROMVALUE::TIF_GTD)
            {
                OatsMsg.SetAt(ROMFIELDS::CSV_GTD_DATE, pOrder->ReplacingExpTime());
            }


        }
    }

    std::string omTime;
    msg.GetAt(ROMFIELDS::CSV_OM_TIME, omTime);
    OatsMsg.SetAt(ROMFIELDS::CSV_OM_TIME, omTime);

    std::string text;
    msg.GetAt(ROMFIELDS::CSV_TEXT, text);
    OatsMsg.SetAt(ROMFIELDS::CSV_TEXT, text);

    OatsMsg.SetAt(ROMFIELDS::CSV_PEGPRICE_OFFSET, pOrder->PegOffset());
    OatsMsg.SetAt(ROMFIELDS::CSV_SEC_DEF, pOrder->SecDef());

    if (ROMVALUE::CPLXTYPE_LEG == cplxType)
    {
        std::string symbol;
        msg.GetAt(ROMFIELDS::CSV_SYMBOL, symbol);
        OatsMsg.SetAt(ROMFIELDS::CSV_SYMBOL, symbol);

        std::string side;
        msg.GetAt(ROMFIELDS::CSV_SIDE, side);
        OatsMsg.SetAt(ROMFIELDS::CSV_SIDE, side);

        std::string secid;
        msg.GetAt(ROMFIELDS::CSV_SEC_ID, secid);
        OatsMsg.SetAt(ROMFIELDS::CSV_SEC_ID, secid);

        std::string secdef;
        if (msg.GetAt(ROMFIELDS::CSV_SEC_DEF, secdef))
        {
            OatsMsg.SetAt(ROMFIELDS::CSV_SEC_DEF, secdef);
        }

        std::string expym;
        msg.GetAt(ROMFIELDS::CSV_EXPYEARMONTH, expym);
        OatsMsg.SetAt(ROMFIELDS::CSV_EXPYEARMONTH, expym);

        short cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
        if (msg.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype) == false)
        {
            cplxtype = pOrder->CplxType();
        }
        
        if (pOrder->CplxType() == ROMVALUE::CPLXTYPE_PRODUCT_SPREAD && 
            ROMVALUE::CPLXTYPE_LEG == cplxtype)
        {
            char sectype = ROMVALUE::SECTYPE_EQUITY;
            if (msg.GetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype) == false)
            {
                sectype = pOrder->SecType();
            }
            OatsMsg.SetAt(ROMFIELDS::CSV_SECURITY_TYPE, sectype);

            if (CUtility::IsOptionType(sectype))
            {
                char callput = 'C';
                msg.GetAt(ROMFIELDS::CSV_CALLPUT, callput);
                OatsMsg.SetAt(ROMFIELDS::CSV_CALLPUT, callput);

                std::string strike;
                msg.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);
                OatsMsg.SetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);

                std::string maturityday;
                msg.GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
            }
            else if (CUtility::IsFutureType(sectype))
            {
                std::string maturityday;
                msg.GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
            }
        }
        else
        {
            if (CUtility::IsOptionType(pOrder->SecType()))
            {
                //callput and strike are already populated from GetCsv call

                //cover up the store proc ROM_selOrdersNew bug that
                //expireDate is not retrieved as string and therefor md can not be
                //generated from it
                if (strlen(pOrder->MaturityDay()) == 0 &&
                    msg.IsFieldEmpty(ROMFIELDS::CSV_MATURITYDAY) == false)
                {
                    std::string maturityday;
                    msg.GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                    OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
                }
            }
        }
        
        /*if (CUtility::IsOptionType(sectype))
        {
            char callput = 'C';
            msg.GetAt(ROMFIELDS::CSV_CALLPUT, callput);
            OatsMsg.SetAt(ROMFIELDS::CSV_CALLPUT, callput);

            std::string strike;
            msg.GetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);
            OatsMsg.SetAt(ROMFIELDS::CSV_STRIKEPRICE, strike);

            std::string maturityday;
            msg.GetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
            OatsMsg.SetAt(ROMFIELDS::CSV_MATURITYDAY, maturityday);
        }*/

        //std::string avgPrice;
        //msg.GetAt(ROMFIELDS::CSV_AVG_PRICE, avgPrice);
        //OatsMsg.SetAt(ROMFIELDS::CSV_AVG_PRICE, avgPrice);

        OatsMsg.SetAt(ROMFIELDS::CSV_STATUS, status);
        OatsMsg.SetAt(ROMFIELDS::CSV_LAST_SHARES, lastShares);

        std::string leaveShares;
        msg.GetAt(ROMFIELDS::CSV_LEAVE_SHARES, leaveShares);
        OatsMsg.SetAt(ROMFIELDS::CSV_LEAVE_SHARES, leaveShares);

        std::string cumShares;
        msg.GetAt(ROMFIELDS::CSV_CUM_SHARES, cumShares);
        OatsMsg.SetAt(ROMFIELDS::CSV_CUM_SHARES, cumShares);
    }

    if (CRomOrderNew::IsLimitType(type) == false)
    {
        OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, "");
    }
    else
    {
        std::string sprice;
        if (msg.GetAt(ROMFIELDS::CSV_PRICE, sprice))
        {
            OatsMsg.SetAt(ROMFIELDS::CSV_PRICE, sprice);
        }
    }
    //OatsMsg.GetMsgNew(pBuffer);
}

void CRomOrdersReplay::Init()
{
    m_FromStr = "--From-- ";
    m_ToStr = "--To-- ";
    m_OrderNotFound = "Order Not Found: ";
    m_DropSet.insert("THALES");
    m_DropSet.insert("DREAD");
    m_DropSet.insert("FAIRWAY");
    m_DropSet.insert("ROMNGDROP");
    m_DropSet.insert("MTGDROP");
    m_DropSet.insert("WENTECH");
    m_DropSet.insert("BRACKDROP");
    m_DropSet.insert("WENTECH");
    m_DropSet.insert("SUMODROP");
    m_DropSet.insert("DERIVIXDROP");
    m_DropSet.insert("SUMODROP2");
    m_DropSet.insert("SUMODROP3");
    m_DropSet.insert("SUMODROP4");
    m_DropSet.insert("OATS");
}