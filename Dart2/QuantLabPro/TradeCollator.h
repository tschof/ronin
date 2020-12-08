#pragma once
#include <string>
#include <map>
#include <vector>

using namespace std;

template< typename H, typename T, typename C >
class TradeCollator
{
public:
	TradeCollator(const string& input, const string& output, uint32_t maxRecords)
		:handler(input, output), sortedTrades(), collector(), maxRecords(maxRecords)
	{
	}
	
	void collateTrades();
	void outputData(C& data);


private:
	H handler;
	map <string, C>sortedTrades;
	vector<unique_ptr<T>>collector;
	uint32_t maxRecords;
};

template<typename H, typename T, typename C>
inline void TradeCollator<H, T, C>::collateTrades()
{ 
	handler.getData(collector, maxRecords);
	for (auto& rec : collector) {
		auto iter = sortedTrades.find(rec->getSymbol());
		if (iter == sortedTrades.end()) {
			C psdt(rec->getSymbol());
			psdt.addTrade(rec);
			sortedTrades.insert(std::pair<string, C>(psdt.getSymbol(), psdt));
		}
		else {
			iter->second.addTrade(rec);
		}
	}
	for (auto tradeOut : sortedTrades) {
		tradeOut.second.calcAvgPrice();
		outputData(tradeOut.second);
	}
	sortedTrades.erase(sortedTrades.begin(), sortedTrades.end());
	collector.erase(collector.begin(), collector.end());
}

template<typename H, typename T, typename C>
inline void TradeCollator<H, T, C>::outputData(C& data)
{
	handler.outputData(data);
}
