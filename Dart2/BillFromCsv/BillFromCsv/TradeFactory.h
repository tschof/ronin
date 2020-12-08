#pragma once
#include <string>

template<class BuildType, class BuildTypeStore>
class TradeFactory
{	
public:
	TradeFactory() {}
	~TradeFactory() {}
	TradeFactory(const TradeFactory&) = delete;
	TradeFactory& operator=(const TradeFactory&) = delete;

	void build(std::string& clearingAcc, int16_t destid, int64_t shares, std::string& liqCode,
		std::string& secType, BuildTypeStore& bts);

};

template<class BuildType, class BuildTypeStore>
inline void TradeFactory<BuildType, BuildTypeStore>::build(std::string& clearingAcc,
	int16_t destid, int64_t shares, std::string& liqCode, std::string& secType, BuildTypeStore& bts)
{
	BuildType lbt(clearingAcc, liqCode, shares, destid, secType);
	bts.add(lbt);
}
