#include <map>

#using <mscorlib.dll>

class TypedMap
{
public:
	std::map<int, long> Longs;
	std::map<int, double> Doubles;
	std::map<int, const char*> Strings;

	inline void Update(int key, long value)
	{
		Longs.erase(Longs.find(key));
		Longs.insert(Longs.begin(), std::pair<int, long>(key, value));
	}

	inline void Update(int key, double value)
	{
		Doubles.erase(Doubles.find(key));
		Doubles.insert(Doubles.begin(), std::pair<int, double>(key, value));
	}

	inline void Update(int key, const char* value)
	{
		Strings.erase(Strings.find(key));
		Strings.insert(Strings.begin(), std::pair<int, static const char*>(key, value));
	}
};

public ref class ManagedTypedMap
{
public:
	System::Collections::Generic::Dictionary<int, double>^ Doubles;
	System::Collections::Generic::Dictionary<int, long>^ Longs;
	System::Collections::Generic::Dictionary<int, System::String^>^ Strings;

    ManagedTypedMap()
	{
		Doubles = gcnew System::Collections::Generic::Dictionary<int, double>();
		Longs = gcnew System::Collections::Generic::Dictionary<int, long>();
		Strings = gcnew System::Collections::Generic::Dictionary<int, System::String^>();
	}
	// (deterministic) destructor (turned into IDisposable.Dispose() by the compiler)
    ~ManagedTypedMap()
	{
		delete Doubles;
		delete Longs;
		delete Strings;
	}
protected:
	// finalizer (non-deterministic destructor) (former destructor syntax => virtual void Finalize())
    !ManagedTypedMap()
	{
		delete Doubles;
		delete Longs;
		delete Strings;
	}
};

class TypedFieldID
{
public:
	static const int wIssueSymbol = 305;
	static const int wIndexSymbol = 293;

	// Trade
	static const int wTradePrice = 481;
	static const int wIndexValue = 295;
	static const int wTradeDate = 476;
	static const int wTradeTime = 485;
	static const int wTradeTick = 484;
	static const int wTradeVolume = 488;
	static const int wTradePartId = 480;
	static const int wTradeCount = 901;

	static const int wNetChange = 341;
	static const int wPctChange = 431;
	static const int wTotalVolume = 475;
	static const int wTotalValue = 474;
	static const int wVwap = 494;

	// Ask
	static const int wAskPrice = 109;
	static const int wIndexAsk = 279;
	static const int wAskSize = 110;
	static const int wAskPartId = 108;
	static const int wAskHigh = 105;
	static const int wAskLow = 106;

	// Bid
	static const int wBidPrice = 237;
	static const int wIndexBid = 280;
	static const int wBidSize = 238;
	static const int wBidPartId = 236;
	static const int wBidHigh = 233;
	static const int wBidLow = 234;

	static const int wOpenPrice = 407;
	static const int wPrevClosePrice = 436;
	static const int wPrevCloseDate = 435;
	static const int wClosePrice = 242;

	static const int wHighPrice = 278;
	static const int wIndexHigh = 286;
	static const int wLowPrice = 327;
	static const int wIndexLow = 287;

	static const int wLineTime = 1174;

	static const int wPlAction = 652;
	static const int wPlPrice = 653;
	static const int wPlSide = 654;
	static const int wPlSize = 655;
	static const int wPlTime = 658;
	static const int wPriceLevels = 699;
};