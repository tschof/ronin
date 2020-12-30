#pragma once
#include <map>
#include <string>
#include <set>
#include "trade.h"

template<class Printer>
class TradeSorter
{
	using LiquidityCodeMap = std::map<std::string, int64_t>;
	using DestinationMap = std::map<int16_t, LiquidityCodeMap>;
	using ClearingMap = std::map<std::string, DestinationMap>;
	using SecTypeMap = std::map<std::string, ClearingMap>;
public:
	TradeSorter()
		:mainMap()
	{}

	void add(Trade& trade);
	void printTrades(Printer& p);
	
private:
	SecTypeMap mainMap;
	std::set<std::string> validClearing = { "X3JH4","MBRA","ATR21209","6004","3FF21209","46148",
											"3JH41209","3KT81209","74X51509","3KT8F","3JF7F",
											"ARR11209","46147","3JF71209","46153","CRN14",
											"6003","CRN11","S4021","X3KT9","3JM81209",
											"3KT91209","46159","31XJ1209", "3FW41209" };
};

template<class Printer>
void TradeSorter<Printer>::add(Trade& trade)
{
	auto secIter = mainMap.find(trade.secType);
	if (secIter != mainMap.end()) {
		ClearingMap& clear = secIter->second;
		auto inner = clear.find(trade.clearingAcc);
		if (inner == clear.end()) {
			DestinationMap inSide;
			LiquidityCodeMap inLiq;
			inLiq.insert(std::make_pair(trade.liqCode, trade.exshares));
			inSide.insert(std::make_pair(trade.dest, inLiq));
			clear.insert(std::make_pair(trade.clearingAcc, inSide));
		} else {
			DestinationMap& dest = inner->second;
			auto tdest = dest.find(trade.dest);
			if (tdest != dest.end()) {
				LiquidityCodeMap& liq = tdest->second;
				auto tLiq = liq.find(trade.liqCode);
				if (tLiq == liq.end()) {
					liq.insert(std::make_pair(trade.liqCode, trade.exshares));
				} else {
					tLiq->second += trade.exshares;
				}
			} else {
				LiquidityCodeMap lliq;
				lliq.insert(std::make_pair(trade.liqCode, trade.exshares));
				dest.insert(std::make_pair(trade.dest, lliq));
			}
		}
	} else {
		ClearingMap lclear;
		DestinationMap inDest;
		LiquidityCodeMap inLiq;
		inLiq.insert(std::make_pair(trade.liqCode, trade.exshares));
		inDest.insert(std::make_pair(trade.dest, inLiq));
		lclear.insert(std::make_pair(trade.clearingAcc, inDest));
		mainMap.insert(std::make_pair(trade.secType, lclear));
	}
}

template<class Printer>
inline void TradeSorter<Printer>::printTrades(Printer& p)
{
	for (auto secType : mainMap) {
		p.printHeader(secType.first);
		for (auto clearing : secType.second) {
			std::string clearAcc = clearing.first;
			for (auto dests : clearing.second) {
				uint16_t destid = dests.first;
				for (auto liqs : dests.second) {
					std::string liqCode = liqs.first;
					uint64_t shares = liqs.second;
					p.printRow(clearAcc, destid, liqCode, shares);
				}
			}
			p.printBlank(4);
		}
		p.printBlank(8);
	}
}

