#pragma once
#include <string>


typedef struct Trade
{
	Trade(const Trade&) = delete;
	Trade& operator=(const Trade&) = delete;
	Trade() = delete;
	~Trade() {}
	Trade(std::string& clearingAcc, std::string& liqCode, uint64_t exShares,
			uint16_t destroute, std::string& sectype)
		:clearingAcc(clearingAcc), liqCode(liqCode), exshares(exShares),
		dest(destroute), secType(sectype)
	{}
	std::string clearingAcc;
	std::string liqCode;
	uint64_t exshares;
	uint16_t dest;
	std::string secType;
}Trade;