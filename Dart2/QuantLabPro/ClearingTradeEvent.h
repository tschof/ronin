#pragma once
#include "DataEvent.h"
class ClearingTradeEvent :
	public DataEvent
{
public:
	ClearingTradeEvent()
		:timeStamp(), executedQty(), price(), symbol(), clearingAccount(), liquidityCode()
	{	
	}

	inline int absorbeIfValid(std::string& rawData)
	{
		std::vector <std::string> tokens;

		// stringstream class check1 
		std::stringstream check1(rawData);

		std::string intermediate;

		// Tokenizing w.r.t. space ' ' 
		while (std::getline(check1, intermediate, ','))
		{
			tokens.push_back(intermediate);
		}
		if (tokens[14].compare("1") == 0 ||
			tokens[14].compare("2") == 0) {
			executedQty = std::stol(tokens[42]);
			clearingAccount = tokens[12];
		}
		else {
			return -1;
		}
	}
	inline const std::string& getSymbol()
	{
		return symbol;
	}
	inline const uint64_t getTimeStamp()
	{
		return timeStamp;
	}
	inline virtual const uint32_t getSize()
	{
		return executedQty;
	}
	inline const uint32_t getPrice()
	{
		return price;
	}

private:
	uint64_t timeStamp;
	uint32_t executedQty;
	uint32_t price;
	std::string symbol;
	std::string clearingAccount;
	std::string liquidityCode;
};


