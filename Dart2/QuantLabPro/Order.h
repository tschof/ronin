#pragma once
#include <string>
#include <istream>
#include <sstream>
#include <ostream>
#include <queue>

enum class Side {BUY, SELL, SHORT};
enum class Tif {IOC, GFD};

class Message {
public:
	Message() : orderid() {}
	virtual ~Message() {}
	Message(const Message&) = delete;
	Message& operator=(const Message&) = delete;
	std::string orderid;
};

class OrdMessage : public Message {
public:
	OrdMessage() : Message(), side(), price(), size() {}
	virtual ~OrdMessage() {}
	OrdMessage(const OrdMessage&) = delete;
	OrdMessage& operator=(const OrdMessage&) = delete;
	Side side;
	int32_t price;
	int32_t size;
};

class Order :public OrdMessage
{
public:
	Order() = delete;
	Order(const Order&) = delete;
	Order& operator=(const Order&) = delete;
	~Order() {}
	Order(const std::string& raw)
		:OrdMessage(),leaves(), tif(), cum(), timestamp()
	{
		timestamp = 0;
		int i = 0;
		std::stringstream ss(raw);
		std::string token;
		while (std::getline(ss, token, ' ')) {
			switch (i) {
			case 0:
				if (token[0] == 'B') {
					side = Side::BUY;
				} else {
				 	side = Side::SELL;
				}
				break;
			case 1:
				if (token[0] == 'G') {
					tif = Tif::GFD;
				} else {
					tif = Tif::IOC;
				}
				break;
			case 2:
				price = std::stol(token);
				break;
			case 3:
				size = std::stol(token);
				leaves = size;
				cum = 0;
				break;
			case 4:
				orderid = token;
				break;
			}
			++i;
		}
	}
	Tif tif;
	int32_t leaves;
	int32_t cum;
	uint64_t timestamp;
};


class Cancel : public Message
{
public:
	Cancel() = delete;
	Cancel(const Cancel&) = delete;
	Cancel& operator=(const Cancel&) = delete;
	~Cancel() {}
	Cancel(const std::string& raw) {
		int i = 0;
		std::stringstream ss(raw);
		std::string token;
		while (std::getline(ss, token, ' ')) {
			switch (i) {
			case 0:
				break;
			case 1:
				orderid = token;
				break;
			}
			++i;
		}
	}	
};

class Modify : public OrdMessage
{
public:
	Modify() = delete;
	Modify(const Modify&) = delete;
	Modify& operator=(const Modify&) = delete;
	~Modify() {}
	Modify(const std::string& raw)
	: OrdMessage() {
		int i = 0;
		std::stringstream ss(raw);
		std::string token;
		while (std::getline(ss, token, ' ')) {
			switch (i) {
			case 0:
				break;
			case 1:
				orderid = token;
				break;
			case 2:
				if (token[0] == 'B') {
					side = Side::BUY;
				}
				else {
					side = Side::SELL;
				}
				break;
			case 3:
				price = std::stol(token);
				break;
			case 4:
				size = std::stol(token);
				break;
			default:
				break;
			}
			++i;
		}
	}
};


