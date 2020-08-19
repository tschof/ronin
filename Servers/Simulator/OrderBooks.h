#ifndef _ORDERBOOKS_H__
#define _ORDERBOOKS_H__

#include "Orders.h"

class COrderBooks
{
public:
    typedef std::map<std::string, tOrderList*> tStringToOrderListMap;
    typedef tStringToOrderListMap::iterator tStringToOrderListMapIT;
    typedef std::map<std::string, tStringToOrderListMap*> tDestToSymbolOrderListMap;
    typedef tDestToSymbolOrderListMap::iterator tDestToSymbolOrderListMapIT;


    COrderBooks(void);
    ~COrderBooks(void);

    void Add(COrder* pOrder);
    void Remove(COrder* pOrder);
    void Clear();

    COrder* FindMatch(COrder* pOrder);

private:

    tDestToSymbolOrderListMap m_BuyBooks;
    tDestToSymbolOrderListMap m_SellBooks;
};

#endif //_ORDERBOOKS_H__
