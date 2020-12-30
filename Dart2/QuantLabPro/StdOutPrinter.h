#pragma once
#include <string>
#include <iostream>

template <typename ObjToPrint>
class StdOutPrinter
{
public:
	void printString(const std::string& message);
	void printObject(int32_t price, int32_t leaves);
	void printTrade(ObjToPrint* older, ObjToPrint* newer, int32_t size);
};

template<typename ObjToPrint>
void inline StdOutPrinter<ObjToPrint>::printString(const std::string& message)
{
	std::cout << message << "\n";
}

template<typename ObjToPrint>
inline void StdOutPrinter<ObjToPrint>::printObject(int32_t price, int32_t leaves)
{
	std::cout << price <<" "<<leaves << "\n";
}

template<typename ObjToPrint>
inline void StdOutPrinter<ObjToPrint>::printTrade(ObjToPrint* older, ObjToPrint* newer, int32_t tradeSize)
{
	std::cout << "TRADE " << older->orderid << " " <<
		older->price << " " << tradeSize << " " << newer->orderid <<
		" " << newer->price << " " << tradeSize << "\n";
}
