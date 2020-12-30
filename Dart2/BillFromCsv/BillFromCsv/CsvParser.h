#pragma once
#include <string>
#include <sstream>

template <class ObjFactory, class ObjStore>
class CsvBillingTradeParser
{
public:
	CsvBillingTradeParser() = delete;
	CsvBillingTradeParser(const CsvBillingTradeParser&) = delete;
	CsvBillingTradeParser& operator=(const CsvBillingTradeParser&) = delete;
	CsvBillingTradeParser(ObjStore& store) 
		:store(store), factory()
	{}

	void parse(const std::string& val);
private:
	ObjStore& store;
	ObjFactory factory;
};

template<class ObjFactory, class ObjStore>
inline void CsvBillingTradeParser<ObjFactory, ObjStore>::parse(const std::string& val)
{
	std::string clearingAcc;
	uint16_t destid;
	uint64_t exShares;
	std::string liqCode;
	std::string secType;
	std::stringstream csvParser(val);
	int i = 0;
	std::string field;
	while (std::getline(csvParser, field, ',')) {
		switch (i) {
		default:
			break;
		case 12:
			clearingAcc = field;
			break;
		case 41:
			destid = std::stoi(field);
			break;
		case 14:
			if (field.size() == 1 && (field[0] == '1' || field[0] == '2')){
				//do nothing ;
			} else {
				return;
			}
			break;
		case 35:
			secType = field;
			break;
		case 66:
			if (field.size() >= 1 && (field[0] == '5' || field[0] =='3')) {
				return;
			}
		case 69:
			liqCode = field;
			if (field.size() <= 0) {
				liqCode = "Empty";
			}
			break;
		case 42:
			exShares = std::stoll(field);
			break;
		}
		++i;
	}
	factory.build(clearingAcc, destid, exShares, liqCode, secType, store);
}
