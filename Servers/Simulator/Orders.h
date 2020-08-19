#ifndef _ORDERS_H__
#define _ORDERS_H__

#include <string>
#include <list>
#include <map>
#include <utility>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "fix42/NewOrderSingle.h"

//#include "Executor.h"


class COrder
{
public:
    COrder();

    void Set(const FIX42::NewOrderSingle& msg, const std::string& login);

    void Update(unsigned int lastQty, double lastPrice);
    void SetOrderQty(unsigned int value);
    void SetLeave(unsigned int value)
    {
        m_Leave = value;
    }
    void SetCum(unsigned int value)
    {
        m_Cum = value;
    }
    void SetSymbol(const char* value)
    {
        strcpy(m_Symbol, value);
    }
    void SetClOrdID(const char* value)
    {
        strcpy(m_ClOrdID, value);
    }
    void SetOrdID(const char* value)
    {
        strcpy(m_OrdID, value);
    }
    void SetAccount(const char* value)
    {
        strcpy(m_Account, value);
    }
    void SetLogin(const char* value)
    {
        strcpy(m_Login, value);
    }
    void SetExecInst(const char* value)
    {
        strcpy(m_ExecInst, value);
    }

    void SetSide(char value)
    {
        m_Side = value;
    }
    void SetType(char value)
    {
        m_Type = value;
    }
    void SetTif(char value)
    {
        m_Tif = value;
    }
    void SetPrice(double value)
    {
        m_Price = value;
        if (m_Price == (double)0)
        {
            SetType('2');
        }
    }

    void SetnText(int value)
    {
        m_nText = value;
    }

    unsigned int Leave()	const
    {
        return m_Leave;
    }
    unsigned int Cum()		const
    {
        return m_Cum;
    }
    unsigned int OrderQty()	const
    {
        return m_Leave + m_Cum;
    }
    double Price()			const
    {
        return m_Price;
    }
    double AvgPrice()		const
    {
        return m_AvgPrice;
    }
    std::string OrdID()		const
    {
        return m_OrdID;
    }
    std::string ClOrdID()	const
    {
        return m_ClOrdID;
    }
    std::string Account()	const
    {
        return m_Account;
    }
    std::string Symbol()	const
    {
        return m_Symbol;
    }
    std::string Login()		const
    {
        return m_Login;
    }
    std::string ExecInst()  const
    {
        return m_ExecInst;
    }

    char Side()				const
    {
        return m_Side;
    }
    char Type()				const
    {
        return m_Type;
    }
    char Tif()				const
    {
        return m_Tif;
    }

    void SetUsed(bool value)
    {
        m_Used = value;
    }

    bool Used()
    {
        return m_Used;
    }
    void Reset();
    int nText()
    {
        return m_nText;
    }

private:
    unsigned int m_Leave;
    unsigned int m_Cum;
    double m_Price;
    double m_AvgPrice;
    char m_Side;
    char m_Type;
    char m_Tif;
    char m_Symbol[36];
    char m_OrdID[128];
    char m_ClOrdID[128];
    char m_Account[36];
    char m_Login[36];
    char m_ExecInst[20];
    bool m_Used;
    bool m_bNormal;
    int m_nText;
};

typedef struct
{
    int ordID;
    int execID;
    COrder Orders[1];
} OrdersMemStruct;

typedef std::list<COrder*> tOrderList;
typedef tOrderList::iterator tOrderListIT;
typedef std::map<std::string, COrder*> tTagToOrderMap;
typedef tTagToOrderMap::iterator tTagToOrderMapIterator;
typedef std::map<std::string, tOrderList*> tBook;
typedef tBook::iterator tBookIT;
typedef std::map<std::string, tBook*> tLoginToBook;
typedef tLoginToBook::iterator tLoginToBookIT;

class COrders
{
public:
    COrders();
    ~COrders();

    std::string genOrID();
    std::string genExecID();

    COrder* GetNewOrder();
    void DestroyOrder(COrder* pOrder);
    COrder* FindOrderFromClOrdID(const std::string& ClOrdID);
    COrder* FindOrderFromOrdID(const std::string& OrdID);
    void ReplaceClOrdID(const std::string& oldClOrdID, const std::string& newClOrdID);
    void Add(COrder* pOrder);
    void AddToBook(COrder* pOrder);
    void ResortBook(const std::string& login, const std::string& symbol, char side);
    void RemoveFromBook(COrder* pOrder);

    COrder* FindMatch(COrder* pOrder);
    void ZapAllDayOrders();

private:

    OrdersMemStruct*	m_pOrdersMemStruct;
    tOrderList			m_FreeList;
    tTagToOrderMap		m_ClOrdIdToOrderMap;
    tTagToOrderMap		m_OrdIdToOrderMap;

    //for normal orders only
    tLoginToBook m_LoginToBuyBook;
    tLoginToBook m_LoginToSellBook;

    unsigned int		m_numOrderSlots;
    unsigned int		m_AllocateStep;
    int m_hd;
    bool m_realloc;
    std::string m_mapFile;

    void AllocateMap();

    COrder* FindBuyMatch(COrder* pOrder);
    COrder* FindSellMatch(COrder* pOrder);
    void ClearMaps();
};

#endif //_ORDERS_H__