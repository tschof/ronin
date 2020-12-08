#pragma once
#include <map>
#include <string>
#include <memory>
#include <set>
#include "trade.h"

template<class Printer>
class TradeSorter
{
	using ClearingMap = std::multimap<std::string, Trade*>;
	using SecTypeMap = std::map<std::string, ClearingMap>;
public:
	TradeSorter()
		:mainMap()
	{}

	void add(Trade* trade);
	void printTrades(Printer& p);
	
private:
	/*3FF21209
3FW41209
3CT81209
31XJ1209
3JM81209
3KT81209
3KT91209
ATR21209
*/
	SecTypeMap mainMap;
	std::set<std::string> validClearing = { "ATR21209","3FF21209","3CT81209",
											"3KT81209","3KT8F",
											"X3KT9","3JM81209",
											"3KT91209","31XJ1209", "3FW41209", "3KW61209","ATT01209","X3KW6" };
	uint64_t equityShares = { 0 };
	uint64_t optionContracts = { 0 };
	uint64_t futuresContracts = { 0 };
};


template<class Printer>
inline void TradeSorter<Printer>::add(Trade* trade)
{
	auto clr = validClearing.find(trade->clearingAcc);
	if (clr != validClearing.end()) {
		auto secIter = mainMap.find(trade->secType);
		if (secIter != mainMap.end()) {
			ClearingMap& clear = secIter->second;
			clear.insert(std::make_pair(trade->clearingAcc, std::move(trade)));
		}
		else {
			ClearingMap lclear;
			lclear.insert(std::make_pair(trade->clearingAcc, std::move(trade)));
			mainMap.insert(std::make_pair(trade->secType, lclear));
		}
		if (trade->secType[0] == 'E') {
			equityShares += trade->exshares;
		}
		else if (trade->secType[0] == 'O') {
			optionContracts += trade->exshares;
		}
		else {
			futuresContracts += trade->exshares;
		}
	}
	
}

template<class Printer>
inline void TradeSorter<Printer>::printTrades(Printer& p)
{
	for (auto secType : mainMap) {
		std::string sType = secType.first;
		if (sType[0] == 'E') {
			p.printHeader(sType, equityShares);
		}
		else if (sType[0] == 'O') {
			p.printHeader(sType, optionContracts);
		}
		else {
			p.printHeader(sType, futuresContracts);
		}
		ClearingMap& map = secType.second;
		auto clearing = map.begin();
		for (;clearing != map.end(); ++clearing) {
			p.printRow(clearing->second);			
		}
		p.printBlank(4);
	}
	p.printBlank(8);
}

