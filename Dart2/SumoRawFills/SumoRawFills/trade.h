#pragma once
#include <string>


typedef struct Trade
{
	Trade(std::string& clearingAcc, std::string& liqCode, uint64_t exShares,
			uint16_t destroute, std::string& sectype, std::string& romTime,
		  std::string& symbol, std::string& price, std::string& side,
		std::string& exid)
		:clearingAcc(clearingAcc), liqCode(liqCode), exshares(exShares),
		dest(destroute), secType(sectype), romTime(romTime), symbol(symbol),
		price(price), side(side), exId(exid)
	{}
	std::string clearingAcc;
	std::string liqCode;
	uint64_t exshares;
	uint16_t dest;
	std::string secType;
	std::string romTime;
	std::string symbol;
	std::string price;
	std::string side;
	std::string exId;
}Trade;