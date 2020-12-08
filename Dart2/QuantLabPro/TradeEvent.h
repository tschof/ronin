#pragma once
#include "DataEvent.h"
#include <istream>
#include <sstream>
#include <string>
using namespace std;

class TradeEvent :public DataEvent
{ 
public:
	TradeEvent(const string& rawData)
		:m_timeStamp(), m_size(), m_price()
	{
		stringstream ss(rawData);
		string token;
		for (int i = 0; i < 4; ++i) {
			getline(ss, token, ',');
			switch (i)
			{
			case 0:
				m_timeStamp = stoll(token);
				break;
			case 1:
				m_symbol = token;
				break;
			case 2:
				m_size = stoi(token);
				break;
			case 3:
				m_price = stoi(token);
			default:
				//TODO should we throw an exception for being out of range?
				break;
			}
		}
	}
	virtual ~TradeEvent() {}
	inline const std::string& getSymbol()
	{
		return m_symbol;
	}
	inline const uint64_t getTimeStamp()
	{
		return m_timeStamp;
	}
	inline virtual const uint32_t getSize()
	{
		return m_size;
	}
	inline const uint32_t getPrice()
	{
		return m_price;
	}

private:
	uint64_t m_timeStamp;
	uint32_t m_size;
	uint32_t m_price;
	string m_symbol;
};


