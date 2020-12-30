#pragma once
#include <unordered_map>
#include "Order.h"
#include <map>
#include <queue>
#include <set>
#include <atomic>
template< typename Printer, typename Disposer>
class MatchingEngine
{
	using TimeOrderedMap = std::map<uint64_t, Order*>;
	using PriceOrderedMap = std::map<int32_t, TimeOrderedMap, std::greater<int32_t>>; 
public:
	MatchingEngine(Printer& printer, Disposer& disp): printer(printer), disposer() {}
	~MatchingEngine() {}
	MatchingEngine() = delete;

	void setFakeTimestamp(Order* ord);

	void processOrd(Order* ord);
	void processModify(Modify* mod);	
	void processCancel(Cancel* can);
	void printCurrentState();

private:
	void processBuy(Order* newOrd);
	void processSell(Order* newOrd);
	void processNew(Order* ord, PriceOrderedMap& mapper);
	void removeFromMap(Order* ord, PriceOrderedMap& map);
	void checkMatch(TimeOrderedMap& map, Order* ord);
	void printMap(PriceOrderedMap& map);
	void removeFromAllOrders(Order* ord);
	std::unordered_map<std::string, Order*> all_orders;
	std::atomic<uint64_t> fakeTimeStamp{ 1 };
	PriceOrderedMap buys;
	PriceOrderedMap sells;
	Printer printer;
	Disposer disposer;
};

template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::setFakeTimestamp(Order* ord)
{
	++fakeTimeStamp;
	ord->timestamp = fakeTimeStamp.load();
}

template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::processOrd(Order* ord)
{
	if (all_orders.find(ord->orderid) == all_orders.end()) {
		setFakeTimestamp(ord);
		if (ord->side == Side::BUY) {
			processBuy(ord);
		}
		else  {
			processSell(ord);
		}
	}
	else {
		disposer.reapPointer(ord);
	}
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::processBuy(Order* newOrd)
{	
	auto sellPrices = sells.rbegin();
	while (sellPrices != sells.rend() && sellPrices->first <= newOrd->price && newOrd->leaves > 0) {
		checkMatch(sellPrices->second, newOrd);
		++sellPrices;
	}
	if (newOrd->leaves > 0 && newOrd->tif == Tif::GFD) {
		processNew(newOrd, buys);
	}
	else {
		disposer.reapPointer(newOrd);
	}
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::processSell(Order* newOrd)
{
	auto buyPrices = buys.begin();
	while (buyPrices != buys.end() && buyPrices->first >= newOrd->price && newOrd->leaves > 0) {
		checkMatch(buyPrices->second, newOrd);
		++buyPrices;
	}
	if (newOrd->leaves > 0 && newOrd->tif == Tif::GFD) {
		processNew(newOrd, sells);
	}
	else {
		disposer.reapPointer(newOrd);
	}
}



template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::checkMatch(TimeOrderedMap& map, Order* ord)
{
	for (auto oSide = map.begin(); oSide != map.end();) {
		Order* cur = oSide->second;
		int32_t diff = cur->leaves - ord->leaves;
		if (diff < 0) {
			int32_t posDiff = cur->leaves;
			ord->cum += posDiff;
			ord->leaves -= cur->leaves;
			cur->leaves = 0;
			printer.printTrade(cur, ord, posDiff);
			auto suto = oSide;
			oSide++;
			map.erase(suto);
			removeFromAllOrders(cur);
			
			disposer.reapPointer(ord);
		}
		else if (diff > 0) {
			int32_t pdiff = ord->leaves;
			cur->leaves -= ord->leaves;
			ord->leaves = 0;
			printer.printTrade(cur, ord, pdiff);
			removeFromAllOrders(ord);
			return;
		}
		else {
			int32_t pdiff = ord->leaves;
			ord->leaves = 0;
			cur->leaves = 0;
			printer.printTrade(cur, ord, pdiff);
			oSide = map.erase(oSide);
			removeFromAllOrders(cur);
			removeFromAllOrders(ord);
			disposer.reapPointer(ord);
			return;
		}
	}
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::processNew(Order* ord, PriceOrderedMap& mapper)
{
	if (all_orders.find(ord->orderid) == all_orders.end()) {
		all_orders[ord->orderid] = ord;
	}
		auto inner = mapper.find(ord->price);
		if (inner == mapper.end()) {
			TimeOrderedMap inSide;
			inSide.insert(std::make_pair(ord->timestamp, ord));
			mapper.insert(std::make_pair(ord->price, inSide));
		}
		else {
			TimeOrderedMap& inSide = inner->second;
			inSide.insert(std::make_pair(ord->timestamp, ord));
		}
	
}

template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::processModify(Modify* mod)
{
	auto ord = all_orders.find(mod->orderid);
	if (ord != all_orders.end()) {
		Order* pOrd = ord->second;
		if (pOrd->side == Side::BUY) {
			removeFromMap(pOrd, buys);
		}
		else {
			removeFromMap(pOrd, sells);
		}
		pOrd->side = mod->side;
		pOrd->price = mod->price;
		int32_t diff = mod->size - pOrd->size;
		pOrd->size += diff;
		pOrd->leaves += diff;
		if (pOrd->leaves == 0 || pOrd->cum == pOrd->leaves) {
			removeFromAllOrders(pOrd);
			disposer.reapPointer(pOrd);
		}
		else {
			setFakeTimestamp(pOrd);
			if (pOrd->side == Side::BUY) {
				processBuy(pOrd);
			}
			else { processSell(pOrd); }
		}
	}
	disposer.reapPointer(mod);
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::removeFromMap(Order* ord, PriceOrderedMap& map)
{
	auto inner = map.find(ord->price);
	if (inner != map.end()) {
		TimeOrderedMap& tatP = inner->second;
		auto ordAtTime = tatP.find(ord->timestamp);
		if (ordAtTime != tatP.end()) {
			ordAtTime = tatP.erase(ordAtTime);
		}
	}
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::processCancel(Cancel* can)
{
	auto ord = all_orders.find(can->orderid);
	if (ord != all_orders.end()) {
		Order* realOrd = ord->second;
		if (realOrd->side == Side::BUY) {
			removeFromMap(realOrd, buys);
		}
		else {
			removeFromMap(realOrd, sells);
		}
		
		ord = all_orders.erase(ord);
		disposer.reapPointer(realOrd);
	}
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::printCurrentState()
{
	printer.printString("SELL:");
	printMap(sells);
	printer.printString("BUY:");
	printMap(buys);
}
template<typename Printer, typename Disposer>
void MatchingEngine<Printer, Disposer>::printMap(PriceOrderedMap& map)
{
	for (auto a : map) {
		TimeOrderedMap& tom = a.second;
		auto b = tom.begin();	
		if (b != tom.end()) {
			int32_t price = a.first;
			int32_t qty = 0;
			while (b != tom.end()) {
				qty += b->second->leaves;
				++b;
			}
			printer.printObject(price, qty);
		}
	}
}

template<typename Printer, typename Disposer>
inline void MatchingEngine<Printer, Disposer>::removeFromAllOrders(Order* ord)
{
	auto fOrd = all_orders.find(ord->orderid);
	if (fOrd != all_orders.end()) {
		fOrd = all_orders.erase(fOrd);
	}
}


