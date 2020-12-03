#include <map>
#include <msclr/lock.h>

using namespace msclr;

public class TypedMap
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

public ref class TypedFieldID
{
public:
	static const int SubscriptionSoruce = 7001;
	static const int SubscriptionSymbol = 7002;

	static const int IssueSymbol = 305;
	static const int IndexSymbol = 293;			// Backup

	//static const int uSubscriptionSoruce = -101;
	//static const int uSubscriptionSymbol = -102;
	static const int uSymbol = -103;
	static const int uParticipentSymbol = -104;
	static const int uParticipentName = -105;

	static const int InstrumentType = 298;
	static const int LineTime = 1174;

	static const int uOnMsgTime = -501;
	static const int uDecodedTime = -502;
	static const int uServerSentTime = -503;
	static const int uClientRecivedTime = -504;
	static const int uDisplayTime = -505;

	static const int NetChange = 341;
	static const int PctChange = 431;
	static const int TotalVolume = 475;
	static const int HighPrice = 278;
	static const int IndexHigh = 286;		// Backup
	static const int LowPrice = 327;
	static const int IndexLow = 287;		// Backup
	static const int OpenPrice = 407;
	static const int PrevClosePrice = 436;
	static const int PrevCloseDate = 435;
	static const int ClosePrice = 242;
	static const int TotalValue = 474;
	static const int Vwap = 494;

	static const int BidPrice = 237;
	static const int IndexBid = 280;		// Backup
	static const int BidSize = 238;
	static const int BidLow = 234;
	static const int BidHigh = 233;
	static const int BidPartId = 236;

	static const int AskPrice = 109;
	static const int IndexAsk = 279;		// Backup
	static const int AskSize = 110;
	static const int AskLow = 106;
	static const int AskHigh = 105;
	static const int AskPartId = 108;

	static const int QuoteDate = 519;
	static const int QuoteTime = 442;
	static const int QuoteCount = 1034;

	static const int TradePrice = 481;
	static const int IndexValue = 295;		// Backup
	static const int TradeDate = 476;
	static const int TradeTime = 485;
	static const int TradeTick = 484;
	static const int TradeVolume = 488;
	static const int TradePartId = 480;
	static const int TradeCount = 901;

	// Stock L1
	static const int PrimExch = 645;

	// Future L1
	static const int ProductInd = 1520;
	static const int DisplayConversionFactor = 1518;
	static const int OrderMinQty = 513;
	static const int OrderMaxQty = 514;
	static const int MaxPriceVariation = 3341;
	static const int Maturity = 1525;
	static const int BookDepth = 2276;
	static const int TicketIncrement = 515;
	static const int InstrumentGroup = 510;
	static const int MinPriceIncrementAmount = 3345;
	static const int MainFraction = 3342;
	static const int HightLimit = 277;
	static const int LowLimit = 326;
	static const int SettlePrice = 458;
	static const int SettleDate = 456;
	static const int LastBestAskPrice = 4026;
	static const int LastBestBidPrice = 4025;
	static const int ExchTotalVol = 2516;

	// Option L1
	static const int PutCall = 439;
	static const int ExpirationDate = 262;
	static const int StrikePrice = 469;
	static const int OpenInterest = 406;
	static const int PrevBidClose = 433;
	static const int PrevBidCloseDate = 434;
	static const int BidClose = 504;
	static const int BidCloseDate = 505;
	static const int PrevAskClose = 506;
	static const int PrevAskCloseDate = 507;
	static const int AskClose = 502;
	static const int AskClosedate = 503;
};
public ref class ManagedTypedMap
{
public:
	System::Collections::Generic::Dictionary<int, double>^ Doubles;
	System::Collections::Generic::Dictionary<int, __int64>^ Longs;
	System::Collections::Generic::Dictionary<int, System::String^>^ Strings;

	System::Collections::Generic::Dictionary<double,__int64>^ BidBook;
	System::Collections::Generic::Dictionary<double,__int64>^ AskBook;

	ManagedTypedMap() 
	{
		Doubles = gcnew System::Collections::Generic::Dictionary<int, double>();
		Longs = gcnew System::Collections::Generic::Dictionary<int, __int64>();
		Strings = gcnew System::Collections::Generic::Dictionary<int, System::String^>();
		BidBook = gcnew System::Collections::Generic::Dictionary<double, __int64>();
		AskBook = gcnew System::Collections::Generic::Dictionary<double, __int64>();
	}
	ManagedTypedMap(ManagedTypedMap %rhs)
	{
		Doubles = gcnew System::Collections::Generic::Dictionary<int, double>(rhs.Doubles);
		Longs =  gcnew System::Collections::Generic::Dictionary<int, __int64>(rhs.Longs);
		Strings = gcnew System::Collections::Generic::Dictionary<int, System::String^>(rhs.Strings);
		BidBook = gcnew System::Collections::Generic::Dictionary<double, __int64>(rhs.BidBook);
		AskBook = gcnew System::Collections::Generic::Dictionary<double, __int64>(rhs.AskBook);
	}

    ~ManagedTypedMap()
	{
		delete Doubles;
		delete Longs;
		delete Strings;
		delete BidBook;
		delete AskBook;
	}

	void Clear()
	{
		Doubles->Clear();
		Longs->Clear();
		Strings->Clear();
		BidBook->Clear();
		AskBook->Clear();
	}

	void Update(int key, double val)
	{
		if (Doubles->ContainsKey(key))
		{
			Doubles[key] = val;
		}
		else
		{
			Doubles->Add(key, val);
		}
	}
	void Update(int key, long val)
	{
		if (Longs->ContainsKey(key))
		{
			Longs[key] = val;
		}
		else
		{
			Longs->Add(key, val);
		}
	}
	void Update(int key, __int64 val)
	{
		if (Longs->ContainsKey(key))
		{
			Longs[key] = val;
		}
		else
		{
			Longs->Add(key, val);
		}
	}
	void Update(int key, const char* val)
	{
		if (Strings->ContainsKey(key))
		{
			Strings[key] = gcnew System::String(val);
		}
		else
		{
			Strings->Add(key, gcnew System::String(val));
		}
	}
	void Update(int key, std::string val)
	{
		Update(key, val.c_str());
	}
	void Update(int key, System::String^ val)
	{
		if (Strings->ContainsKey(key))
		{
			Strings[key] = val;
		}
		else
		{
			Strings->Add(key, val);
		}
	}
	void UpdateBidBook(double key, __int64 val)
	{
		if (BidBook->ContainsKey(key))
		{
			BidBook[key] = val;
		}
		else
		{
			BidBook->Add(key, val);
		}
	}
	void UpdateAskBook(double key, __int64 val)
	{
		if (AskBook->ContainsKey(key))
		{
			AskBook[key] = val;
		}
		else
		{
			AskBook->Add(key, val);
		}
	}

	void Update(ManagedTypedMap^ rhs)
	{
		for each(int key in rhs->Doubles->Keys)
		{
			Update(key, rhs->Doubles[key]);
		}

		for each(int key in rhs->Longs->Keys)
		{
			Update(key, rhs->Longs[key]);
		}

		for each(int key in rhs->Strings->Keys)
		{
			Update(key, rhs->Strings[key]);
		}

		for each(double key in rhs->BidBook->Keys)
		{
			UpdateBidBook(key, rhs->BidBook[key]);
		}

		for each(double key in rhs->BidBook->Keys)
		{
			UpdateBidBook(key, rhs->BidBook[key]);
		}

		for each(double key in rhs->AskBook->Keys)
		{
			UpdateAskBook(key, rhs->AskBook[key]);
		}
	}

	#pragma region - User Info -

	property System::String^ uSubscriptionSymbol
	{
		System::String^ get()
		{
			if (Strings->ContainsKey(TypedFieldID::SubscriptionSymbol))
			{
				return Strings[TypedFieldID::SubscriptionSymbol];
			}
			return "";
		}
	}
	property System::String^ uSubscriptionSoruce
	{
		System::String^ get()
		{
			if (Strings->ContainsKey(TypedFieldID::SubscriptionSoruce))
			{
				System::String^ source = Strings[TypedFieldID::SubscriptionSoruce];
				if (source->Contains("CME-"))
				{
					return "CME";
				}
				else if (source->Contains("OPRA") && !uSubscriptionSymbol->Contains("."))
				{
					return "OPRANBBO";
				}
				else
				{
					return Strings[TypedFieldID::SubscriptionSoruce];
				}
			}
			return "";
		}
	}
	property System::String^ uSubscriptionKey
	{
		System::String^ get()
		{
			if (uSubscriptionSymbol!="" && uSubscriptionSoruce!="")
			{
				return uSubscriptionSymbol + "," + uSubscriptionSoruce;
			}
			return "";
		}
	}
	property System::String^ uParticipentSymbol
	{
		System::String^ get()
		{
			if (uSubscriptionSymbol->Contains("."))
			{
				return uSubscriptionSymbol->Substring(uSubscriptionSymbol->IndexOf("."), 1);
			}
			else
			{
				return uSubscriptionSymbol;
			}
		}
	}
	#pragma endregion

	#pragma region - Info -

	property System::String^ InstrumentType
	{
		System::String^ get()
		{
			if (uSubscriptionSoruce->Contains("CME") || uSubscriptionSoruce->Contains("ICE"))
			{
				return "F";
			}
			else if (uSubscriptionSoruce->Contains("OPRA"))
			{
				return "O";
			}
			return "E";
		}
	}
	property System::String^ LineTime
	{
		System::String^ get()
		{
			if (Strings->ContainsKey(TypedFieldID::LineTime))
			{
				return Strings[TypedFieldID::LineTime];
			}
			return "";
		}
	}
	#pragma endregion

	#pragma region - Bid -

	property bool HasBid
	{
		bool get()
		{
			if (Doubles->ContainsKey(TypedFieldID::BidPrice) || Longs->ContainsKey(TypedFieldID::BidSize))
			{
				return true;
			}
			return false;
		}
	}
	property double BidPrice
	{
		double get()
		{
			if (Doubles->ContainsKey(TypedFieldID::BidPrice))
			{
				return Doubles[TypedFieldID::BidPrice];
			}
			return 0;
		}
	}
	property __int64 BidSize
	{
		__int64 get()
		{
			if (Longs->ContainsKey(TypedFieldID::BidSize))
			{
				return Longs[TypedFieldID::BidSize];
			}
			return 0;
		}
	}
	property System::String^ BidPartId
	{
		System::String^ get()
		{
			if (Strings->ContainsKey(TypedFieldID::BidPartId))
			{
				return Strings[TypedFieldID::BidPartId];
			}
			return "";
		}
	}

	#pragma endregion

	#pragma region - Ask -

	property bool HasAsk
	{
		bool get()
		{
			if (Doubles->ContainsKey(TypedFieldID::AskPrice) || Longs->ContainsKey(TypedFieldID::AskSize))
			{
				return true;
			}
			return false;
		}
	}
	property double AskPrice
	{
		double get()
		{
			if (Doubles->ContainsKey(TypedFieldID::AskPrice))
			{
				return Doubles[TypedFieldID::AskPrice];
			}
			return 0;
		}
	}
	property __int64 AskSize
	{
		__int64 get()
		{
			if (Longs->ContainsKey(TypedFieldID::AskSize))
			{
				return Longs[TypedFieldID::AskSize];
			}
			return 0;
		}
	}
	property System::String^ AskPartId
	{
		System::String^ get()
		{
			if (Strings->ContainsKey(TypedFieldID::AskPartId))
			{
				return Strings[TypedFieldID::AskPartId];
			}
			return "";
		}
	}
	#pragma endregion

	#pragma region - Traded -

	property bool HasTrade
	{
		bool get()
		{
			if (Doubles->ContainsKey(TypedFieldID::TradePrice) || Longs->ContainsKey(TypedFieldID::TradeVolume))
			{
				return true;
			}
			return false;
		}
	}
	property double TradePrice
	{
		double get()
		{
			if (Doubles->ContainsKey(TypedFieldID::TradePrice))
			{
				return Doubles[TypedFieldID::TradePrice];
			}
			return 0;
		}
	}
	property __int64 TradeVolume
	{
		__int64 get()
		{
			if (Longs->ContainsKey(TypedFieldID::TradeVolume))
			{
				return Longs[TypedFieldID::TradeVolume];
			}
			return 0;
		}
	}
	property System::String^ TradePartId
	{
		System::String^ get()
		{
			if (Strings->ContainsKey(TypedFieldID::TradePartId))
			{
				return Strings[TypedFieldID::TradePartId];
			}
			return "";
		}
	}

	#pragma endregion

	#pragma region - General Info -

	property System::Nullable<double> ClosePrice
	{
		System::Nullable<double> get()
		{
			if (Doubles->ContainsKey(TypedFieldID::ClosePrice))
			{
				return Doubles[TypedFieldID::ClosePrice];
			}
			return System::Nullable<double>();
		}
	}
	property System::Nullable<double> PrevClosePrice
	{
		System::Nullable<double> get()
		{
			if (Doubles->ContainsKey(TypedFieldID::PrevClosePrice))
			{
				return Doubles[TypedFieldID::PrevClosePrice];
			}
			return System::Nullable<double>();
		}
	}

	#pragma endregion
protected:
	// finalizer (non-deterministic destructor) (former destructor syntax => virtual void Finalize())
    !ManagedTypedMap()
	{
		delete Doubles;
		delete Longs;
		delete Strings;
		delete BidBook;
		delete AskBook;
	}
};
