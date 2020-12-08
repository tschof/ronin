#pragma once
#include <string>
#include <vector>
#include "TradeEvent.h"

using namespace std;

class PerSymbolDailyTrades
{
public:
	PerSymbolDailyTrades(const string& sym)
		:symbol(sym), weightedAvgPrice(), maxTimeGap(),
		totalShares(), maxPrice(), currentTime(), priceTimesShares()
	{
	}
	~PerSymbolDailyTrades() {}
	
	/*It would probably be better to throw an exception but I feel that rewinding the stack may be 
	  overkill. The parameters of the test stated that time values would always get greater.*/
	template<typename Te>
	int addTrade(unique_ptr<Te>& tradeEvent);
	

	const string& getSymbol() const { return symbol; }
	/* There may be a better way to do this since "friends" break encapsulation but i
	   justified it by thinking: well it is an operator.*/
	friend ostream& operator<<(ostream& os, const PerSymbolDailyTrades& psdt);

	bool operator<(const PerSymbolDailyTrades& psdt) const {
		return symbol < psdt.getSymbol();
	}

	void calcAvgPrice() 
	{
		for (auto s : priceTimesShares) {
			weightedAvgPrice += s;
		}
		weightedAvgPrice /= totalShares;
	}

private:
	string symbol;
	uint32_t weightedAvgPrice;
	uint64_t maxTimeGap;
	uint64_t currentTime;
	uint32_t totalShares;
	uint32_t maxPrice;
	vector<uint32_t>priceTimesShares;
};

inline ostream& operator<<(ostream& os, const PerSymbolDailyTrades& psdt)
{
	os << psdt.symbol.c_str() << ',' << psdt.maxTimeGap << ',' 
		<< psdt.totalShares << ',' << psdt.weightedAvgPrice << ',' << psdt.maxPrice <<"\n";
	return os;
}


template<typename Te>
inline int PerSymbolDailyTrades::addTrade(unique_ptr<Te>& tradeEvent)
{
	int worked = 1;
	if (symbol.compare(tradeEvent->getSymbol()) == 0) {
		if (tradeEvent->getTimeStamp() > currentTime) {
			uint64_t gap = tradeEvent->getTimeStamp() - currentTime;
			if (currentTime > 0 && gap > maxTimeGap) {
				maxTimeGap = gap;
			}
			currentTime = tradeEvent->getTimeStamp();
			if (tradeEvent->getPrice() > maxPrice)
			{
				maxPrice = tradeEvent->getPrice();
			}
			totalShares += tradeEvent->getSize();
			priceTimesShares.push_back((tradeEvent->getPrice() * tradeEvent->getSize()));
		}
		else {
			worked = 0;
		}
	}
	else {
		worked = 0;
	}
	return worked;
}
