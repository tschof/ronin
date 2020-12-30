#pragma once
#include <string>
class DataEvent
{
public:
	virtual ~DataEvent() {}
	virtual const std::string& getSymbol() = 0;
	virtual const uint64_t getTimeStamp() = 0;
	virtual const uint32_t getSize() = 0;
};

