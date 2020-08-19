#include "Executor.h"
#include "Orders.h"

inline bool compare_forward(const COrder* left, const COrder* right)
{
    return left->Price() < right->Price();
}

inline bool compare_backward(const COrder* left, const COrder* right)
{
    return left->Price() > right->Price();
}

std::string COrders::genOrID()
{
    std::stringstream stream;
    stream << ++(m_pOrdersMemStruct->ordID);
    return stream.str();
}
std::string COrders::genExecID()
{
    std::stringstream stream;
    stream << ++(m_pOrdersMemStruct->execID);
    return stream.str();
}

void COrders::ClearMaps()
{
    m_ClOrdIdToOrderMap.clear();
    m_OrdIdToOrderMap.clear();

    tBookIT it1;
    tBook* pBook = NULL;
    tOrderList* pOrderList = NULL;

    tLoginToBookIT it2 = m_LoginToBuyBook.begin();
    while (it2 != m_LoginToBuyBook.end())
    {
        pBook = it2->second;
        it1 = pBook->begin();
        while (it1 != pBook->end())
        {
            pOrderList = it1->second;
            pOrderList->clear();
            delete pOrderList;
            it1++;
        }
        delete pBook;
        it2++;
    }

    m_LoginToBuyBook.clear();

    it2 = m_LoginToSellBook.begin();
    while (it2 != m_LoginToSellBook.end())
    {
        pBook = it2->second;
        it1 = pBook->begin();
        while (it1 != pBook->end())
        {
            pOrderList = it1->second;
            pOrderList->clear();
            delete pOrderList;
            it1++;
        }
        delete pBook;
        it2++;
    }

    m_LoginToSellBook.clear();
}

void COrders::AllocateMap()
{
    unsigned long bytesToAdd = m_AllocateStep * sizeof(COrder);

    if (m_hd != -1)
    {
        munmap((caddr_t)m_pOrdersMemStruct, m_numOrderSlots * sizeof(COrder));
        close(m_hd);
        ClearMaps();
    }

    m_hd = open("SimulatorOrders.map", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    long offset = 0;
    offset = lseek(m_hd, 0, SEEK_END);
    unsigned int curOrderNum = offset / sizeof(COrder);

    if (m_realloc)
    {
        m_numOrderSlots += m_AllocateStep;
        char* buffer = new char[bytesToAdd];
        memset(buffer, 0, bytesToAdd);
        write(m_hd, buffer, bytesToAdd);
        delete[] buffer;
    }
    else
    {
        m_realloc = true;
        m_numOrderSlots = curOrderNum;
        if (!offset)
        {
            m_numOrderSlots = m_AllocateStep;
            char* buffer = new char[bytesToAdd];
            memset(buffer, 0, bytesToAdd);
            write(m_hd, buffer, bytesToAdd);
            delete[] buffer;
        }
    }

    if (m_hd > 0)
    {
        m_pOrdersMemStruct = (OrdersMemStruct*)mmap(0, m_numOrderSlots * sizeof(COrder), PROT_READ | PROT_WRITE, MAP_SHARED, m_hd, 0);
    }

    int i = 0;
    COrder* pOrder = NULL;

    for (i = 0; i < curOrderNum; i++)
    {
        pOrder = &((m_pOrdersMemStruct->Orders)[i]);
        if (pOrder->Used())
        {
            m_ClOrdIdToOrderMap.insert(std::make_pair<std::string, COrder*>(pOrder->Login() + pOrder->ClOrdID(), pOrder));
            m_OrdIdToOrderMap.insert(std::make_pair<std::string, COrder*>(pOrder->Login() + pOrder->OrdID(), pOrder));
            if (pOrder->ExecInst() != CExecutor::SPECIALORDER)
            {
                AddToBook(pOrder);
            }
        }
        else
        {
            m_FreeList.push_back(pOrder);
        }
    }

    for (i = curOrderNum; i < m_numOrderSlots; i++)
    {
        pOrder = &((m_pOrdersMemStruct->Orders)[i]);
        pOrder->Reset();
        m_FreeList.push_back(pOrder);
    }
}


COrders::COrders() :
m_mapFile("SimulatorOrders.map")
{
    m_AllocateStep = 10000;
    m_numOrderSlots = 0;
    m_realloc = false;
    m_hd = -1;
    struct stat st;
    if (stat(m_mapFile.c_str(), &st))
    {
        time_t t;
        time(&t);
        struct tm Tm1, Tm2;
        localtime_r(&t, &Tm1);
        localtime_r(&(st.st_ctim.tv_sec), &Tm2);
        if (Tm1.tm_year != Tm2.tm_year ||
            Tm1.tm_mon != Tm2.tm_mon ||
            Tm1.tm_mday != Tm2.tm_mday)
        {
            if (unlink(m_mapFile.c_str()) == -1)
            {
                printf("unlink SimulatorOrders.map failed\n");
            }
        }
    }

    AllocateMap();
}

COrder* COrders::GetNewOrder()
{
    if (m_FreeList.empty())
    {
        AllocateMap();
    }

    COrder* pOrder = m_FreeList.front();
    m_FreeList.pop_front();
    pOrder->SetUsed(true);
    pOrder->SetnText(-1);

    return pOrder;
}

void COrders::Add(COrder* pOrder)
{
    m_ClOrdIdToOrderMap.insert(std::make_pair<std::string, COrder*>(pOrder->Login() + pOrder->ClOrdID(), pOrder));
    m_OrdIdToOrderMap.insert(std::make_pair<std::string, COrder*>(pOrder->Login() + genOrID(), pOrder));
}

void COrders::DestroyOrder(COrder* pOrder)
{
    m_ClOrdIdToOrderMap.erase(pOrder->Login() + pOrder->ClOrdID());
    m_OrdIdToOrderMap.erase(pOrder->Login() + pOrder->OrdID());

    if (pOrder->ExecInst() != CExecutor::SPECIALORDER)
    {
        RemoveFromBook(pOrder);
    }

    memset(pOrder, 0, sizeof(COrder));
    pOrder->SetUsed(false);
    m_FreeList.push_back(pOrder);
}

COrders::~COrders()
{
    //m_MemMap.sync();
    //m_MemMap.close();

    if (m_hd != -1)
    {
        munmap((caddr_t)m_pOrdersMemStruct, m_numOrderSlots * sizeof(COrder));
        close(m_hd);
    }
}

void COrders::ZapAllDayOrders()
{
    if (m_hd != -1)
    {
        COrder* pOrder = NULL;
        for (int i = 0; i < m_numOrderSlots; i++)
        {
            pOrder = &((m_pOrdersMemStruct->Orders)[i]);
            if (pOrder->Used() == true &&
                pOrder->Tif() != FIX::TimeInForce_GOODTILLCANCEL &&
                pOrder->Tif() != FIX::TimeInForce_GOODTILLDATE)
            {
                pOrder->Reset();
            }
        }
    }
}

COrder* COrders::FindOrderFromClOrdID(const std::string& ClOrdID)
{
    COrder* pOrder = NULL;
    tTagToOrderMapIterator it = m_ClOrdIdToOrderMap.find(ClOrdID);
    if (it != m_ClOrdIdToOrderMap.end())
    {
        pOrder = it->second;
    }
    return pOrder;
}

COrder* COrders::FindOrderFromOrdID(const std::string& OrdID)
{
    COrder* pOrder = NULL;
    tTagToOrderMapIterator it = m_OrdIdToOrderMap.find(OrdID);
    if (it != m_OrdIdToOrderMap.end())
    {
        pOrder = it->second;
    }
    return pOrder;
}

void COrders::ReplaceClOrdID(const std::string& oldClOrdID, const std::string& newClOrdID)
{
    COrder* pOrder = FindOrderFromClOrdID(oldClOrdID);
    if (pOrder)
    {
        m_ClOrdIdToOrderMap.erase(oldClOrdID);
        m_ClOrdIdToOrderMap.insert(std::make_pair<std::string, COrder*>(newClOrdID, pOrder));
    }
}

void COrders::AddToBook(COrder* pOrder)
{
    tLoginToBookIT it1;
    tBook* pBook = NULL;
    tBookIT it2;
    tOrderList* pOrderList = NULL;


    if (pOrder->Side() == FIX::Side_BUY)
    {
        it1 = m_LoginToBuyBook.find(pOrder->Login());
        if (it1 != m_LoginToBuyBook.end())
        {
            pBook = it1->second;
        }
        else
        {
            pBook = new tBook;
            m_LoginToBuyBook.insert(std::make_pair<std::string, tBook*>(pOrder->Login(), pBook));
        }

        it2 = pBook->find(pOrder->Symbol());
        if (it2 != pBook->end())
        {
            pOrderList = it2->second;
        }
        else
        {
            pOrderList = new tOrderList;
            pBook->insert(std::make_pair<std::string, tOrderList*>(pOrder->Symbol(), pOrderList));
        }

        pOrderList->sort(compare_backward);
    }
    else
    {
        it1 = m_LoginToSellBook.find(pOrder->Login());
        if (it1 != m_LoginToSellBook.end())
        {
            pBook = it1->second;
        }
        else
        {
            pBook = new tBook;
            m_LoginToSellBook.insert(std::make_pair<std::string, tBook*>(pOrder->Login(), pBook));
        }

        it2 = pBook->find(pOrder->Symbol());
        if (it2 != pBook->end())
        {
            pOrderList = it2->second;
        }
        else
        {
            pOrderList = new tOrderList;
            pBook->insert(std::make_pair<std::string, tOrderList*>(pOrder->Symbol(), pOrderList));
        }

        pOrderList->sort(compare_forward);
    }

    if (pOrderList)
    {
        pOrderList->push_back(pOrder);
    }
}

void COrders::RemoveFromBook(COrder* pOrder)
{
    tLoginToBookIT it1;
    tBook* pBook = NULL;
    tBookIT it2;
    tOrderList* pOrderList = NULL;

    if (pOrder->Side() == FIX::Side_BUY)
    {
        it1 = m_LoginToBuyBook.find(pOrder->Login());
        if (it1 != m_LoginToBuyBook.end())
        {
            pBook = it1->second;
        }
    }
    else
    {
        it1 = m_LoginToSellBook.find(pOrder->Login());
        if (it1 != m_LoginToSellBook.end())
        {
            pBook = it1->second;
        }
    }

    if (pBook != NULL)
    {
        pBook = it1->second;
        it2 = pBook->find(pOrder->Symbol());
        if (it2 != pBook->end())
        {
            pOrderList = it2->second;
        }
    }

    if (pOrderList)
    {
        tOrderListIT itt = pOrderList->begin();
        while (itt != pOrderList->end())
        {
            if (*itt == pOrder)
            {
                pOrderList->erase(itt);
                break;
            }

            itt++;
        }
    }
}

COrder* COrders::FindMatch(COrder* pOrder)
{
    COrder* rc = NULL;

    if (pOrder->Side() == FIX::Side_BUY)
    {
        rc = FindBuyMatch(pOrder);
    }
    else
    {
        rc = FindSellMatch(pOrder);
    }

    return rc;
}

COrder* COrders::FindBuyMatch(COrder* pOrder)
{
    COrder* pTargetOrder = NULL;
    COrder* rc = NULL;
    tOrderList* pOrderList = NULL;
    tBook* pBook = NULL;

    tLoginToBookIT it1 = m_LoginToSellBook.find(pOrder->Login());

    if (it1 != m_LoginToSellBook.end())
    {
        pBook = it1->second;
        tBookIT it2 = pBook->find(pOrder->Symbol());

        if (it2 != pBook->end())
        {
            pOrderList = it2->second;
            tOrderListIT itt = pOrderList->begin();

            while (itt != pOrderList->end())
            {
                pTargetOrder = *itt;

                if (pOrder->Type() == FIX::OrdType_MARKET)
                {
                    rc = pTargetOrder;
                    break;
                }
                else
                {
                    if (pOrder->Price() >= pTargetOrder->Price())
                    {
                        rc = pTargetOrder;
                        break;
                    }
                }

                itt++;
            }
        }
    }

    return rc;
}

COrder* COrders::FindSellMatch(COrder* pOrder)
{
    COrder* pTargetOrder = NULL;
    COrder* rc = NULL;
    tOrderList* pOrderList = NULL;
    tBook* pBook = NULL;

    tLoginToBookIT it1 = m_LoginToBuyBook.find(pOrder->Login());

    if (it1 != m_LoginToBuyBook.end())
    {
        pBook = it1->second;
        tBookIT it2 = pBook->find(pOrder->Symbol());

        if (it2 != pBook->end())
        {
            pOrderList = it2->second;
            tOrderListIT itt = pOrderList->begin();

            while (itt != pOrderList->end())
            {
                pTargetOrder = *itt;

                if (pOrder->Type() == FIX::OrdType_MARKET)
                {
                    rc = pTargetOrder;
                    break;
                }
                else
                {
                    if (pOrder->Price() <= pTargetOrder->Price())
                    {
                        rc = pTargetOrder;
                        break;
                    }
                }

                itt++;
            }
        }
    }

    return rc;
}

void COrders::ResortBook(const std::string& login, const std::string& symbol, char side)
{
    tLoginToBookIT it;

    if (side == FIX::Side_BUY)
    {
        it = m_LoginToBuyBook.find(login);
        if (it == m_LoginToBuyBook.end())
        {
            return;
        }
    }
    else
    {
        it = m_LoginToSellBook.find(login);
        if (it == m_LoginToSellBook.end())
        {
            return;
        }
    }

    tBook* pBook = it->second;

    tBookIT it2 = pBook->find(symbol);
    if (it2 != pBook->end())
    {
        tOrderList* pOrderList = it2->second;
        if (side == FIX::Side_BUY)
        {
            pOrderList->sort(compare_backward);
        }
        else
        {
            pOrderList->sort(compare_forward);
        }
    }
}

void COrder::SetOrderQty(unsigned int value)
{
    m_Leave = value - m_Cum;
}


COrder::COrder()
{
    Reset();
}

void COrder::Set(const FIX42::NewOrderSingle& msg, const std::string& login)
{
    FIX::Symbol symbol;
    FIX::Side side;
    FIX::OrdType ordType;
    FIX::OrderQty orderQty;
    FIX::ClOrdID clOrdID;
    FIX::Account account;
    FIX::ExecInst excInst;
    FIX::TimeInForce tif;

    msg.get( ordType );
    msg.get( symbol );
    msg.get( side );
    msg.get( orderQty );
    msg.get( clOrdID );
    msg.get( tif );

    FIX::SecurityType secType = (FIX::SecurityType)FIX::SecurityType_COMMON_STOCK;
    //const char* secType = FIX::SecurityType_COMMON_STOCK;
    if (msg.isSetField(secType))
    {
        msg.get(secType);

        if (secType.getValue().compare(FIX::SecurityType_OPTION) == 0)
        {
            FIX::StrikePrice strike;
            FIX::PutOrCall putcall;
            FIX::MaturityMonthYear mtym;
            FIX::MaturityDay mtd;

            msg.get( strike );
            msg.get( putcall );
            msg.get( mtym );

            std::stringstream stm;

            if (msg.isSetField(mtd))
            {
                msg.get( mtd );
                stm <<  symbol.getValue()
                    <<	mtym.getValue()
                    <<	mtd.getValue()
                    <<  strike.getValue()
                    <<  putcall.getValue();
            }
            else
            {
                stm <<  symbol.getValue()
                    <<	mtym.getValue()
                    <<  strike.getValue()
                    <<  putcall.getValue();
            }

            symbol.setValue(stm.str());
        }
    }

    if (msg.isSetField(excInst))
    {
        msg.get( excInst );
        SetExecInst(excInst.getValue().c_str());
    }
    else
    {
        SetExecInst("NORMAL");
    }

    SetSide(side);
    SetOrderQty(orderQty);
    SetClOrdID(clOrdID.getValue().c_str());
    SetSymbol(symbol.getValue().c_str());
    SetLogin(login.c_str());
    SetType(ordType.getValue());
    SetTif(tif.getValue());

    if (Type() != FIX::OrdType_MARKET)
    {
        FIX::Price price;
        msg.get( price );
        SetPrice(price);
    }
}

void COrder::Reset()
{
    m_Leave = 0;
    m_Cum = 0;
    m_Price = 0.0;
    m_AvgPrice = 0.0;
    m_Side = '1';
    m_Type = '2';
    m_Used = false;
    m_nText = -1;

    strcpy(m_Symbol, "");
    strcpy(m_OrdID, "");
    strcpy(m_ClOrdID, "");
    strcpy(m_Account, "");
}

void COrder::Update(unsigned int lastQty, double lastPrice)
{
    m_Leave -= lastQty;
    m_AvgPrice = (m_Cum * m_AvgPrice + lastQty * lastPrice) / (m_Cum + lastPrice);
    m_Cum += lastQty;
}