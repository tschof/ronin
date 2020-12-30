#pragma once
#include <string>
#include <sstream>
#include <map>

template<class BuildType>
class NasdaqBuildFactory
{
public:
	NasdaqBuildFactory() {}
	BuildType build(const std::string& s);
};

class RoProNasdaqStore
{
public:
	void add(std::string orderid, std::string fullMessage);
	void findMatch(std::string& orderid, std::string clearingAcc);
	template<typename RoProNasdaqOutput>
	void printMap(RoProNasdaqOutput& rpno);
private:
	std::multimap<std::string, std::string> orders;
};

template<class ObjectStore>
class RoProNasdaqCsvMatcher
{
public:
	RoProNasdaqCsvMatcher() = delete;
	RoProNasdaqCsvMatcher(const RoProNasdaqCsvMatcher&) = delete;
	RoProNasdaqCsvMatcher& operator=(const RoProNasdaqCsvMatcher&) = delete;
	RoProNasdaqCsvMatcher(ObjectStore& s)
		:store(s)
	{}
	void parse(const std::string& val);
private:
	ObjectStore& store;
};

template <class ObjectFactory,class ObjectStore>
class NasdaqBillCsvParser
{
public:
	NasdaqBillCsvParser() = delete;
	NasdaqBillCsvParser(const NasdaqBillCsvParser&) = delete;
	NasdaqBillCsvParser& operator=(const NasdaqBillCsvParser&) = delete;
	NasdaqBillCsvParser(ObjectStore& s)
		:store(s) , factory()
	{}

	void parse(const std::string& val);

private:
	ObjectStore& store;
	ObjectFactory factory;
};

template<class ObjectFactory, class ObjectStore>
inline void NasdaqBillCsvParser<ObjectFactory, ObjectStore>::parse(const std::string& val)
{
	std::stringstream csvParser(val);
	int i = 0;
	std::string orderid;
	std::string field;
	bool use16 = false;
	while (std::getline(csvParser, field, ',')) {
		switch (i) {
		default:
			break;
		case 15:
			orderid = field;
			if (orderid.size() == 1 && orderid[0] == 'Y') {
				use16 = true;
			}
			break;
		case 16:
			if (field.size() > 0 && use16) {
				orderid = field;
			}
		}
		++i;
	}
	if (orderid.size() > 0) {
		store.add(orderid, factory.build(val));
	}
}
template < class ObjectStore>
void RoProNasdaqCsvMatcher<ObjectStore>::parse(const std::string& val)
{
	std::string clearingAcc;
	uint16_t destid;
	std::string liqCode;
	std::string orderid;
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
			if (destid != 146) {
				return;
			}
			break;
		case 14:
			if (field.size() == 2 && (field[0] == '1' && field[1] == '3')) {
				//do nothing ;
			}
			else {
				return;
			}
			break;
		case 15:
			orderid = field;
			break;
		}
		++i;
	}
	store.findMatch(orderid, clearingAcc);
}

template<class BuildType>
inline BuildType NasdaqBuildFactory<BuildType>::build(const std::string& s)
{
	BuildType b(s);
	return b;
}


void RoProNasdaqStore::add(std::string orderid, std::string fullMessage)
{

	orders.insert(std::make_pair(orderid, fullMessage));
}

inline void RoProNasdaqStore::findMatch(std::string& orderid, std::string clearingAcc)
{
	auto found = orders.equal_range(orderid);
	for(auto it = found.first;it != found.second;++it) {
		std::string& fullMess = it->second;
		fullMess.append(",");
		fullMess.append(clearingAcc);
	}
}
template<typename RoProNasdaqOutput>
inline void RoProNasdaqStore::printMap(RoProNasdaqOutput& rpno)
{
	for (auto a : orders) {
		rpno.print(a.second);
	}
}