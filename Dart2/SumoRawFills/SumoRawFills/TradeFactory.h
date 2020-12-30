#pragma once
#include <memory>
#include <string>

template<class BuildType, class BuildTypeStore>
class TradeFactory
{	
public:
	TradeFactory() {}
	~TradeFactory() {}
	TradeFactory(const TradeFactory&) = delete;
	TradeFactory& operator=(const TradeFactory&) = delete;

	void build(std::string& clearingAcc, int16_t destid, int64_t shares,
				std::string& liqCode, std::string& secType, BuildTypeStore& bts,
				std::string& romTime, std::string& symbol,
				std::string& side, std::string& price, std::string& exId);

};

template<class BuildType, class BuildTypeStore>
inline void TradeFactory<BuildType, BuildTypeStore>::build(std::string& clearingAcc,
	int16_t destid, int64_t shares, std::string& liqCode, 
	std::string& secType, BuildTypeStore& bts, std::string& romTime,
	std::string& symbol, std::string& side, std::string& price, std::string& exId)
{
	BuildType *lbt = new BuildType(clearingAcc, liqCode, shares,
		destid, secType, romTime, symbol, price, side, exId);
	bts.add(lbt);
}
