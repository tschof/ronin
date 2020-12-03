using System;
using System.Collections.Generic;

namespace LBMEx
{
	public static class InstrumentTypes
	{
		public const string Stock = "Stock";
		public const string Future = "Future";
		public const string Option = "Option";
		public const string Spread = "Spread";
	}

	public static class BookSide
	{
		public const string Buy = "B";
		public const string Sell = "S";
	}

	public static class OptionTypes
	{
		public const string Put = "P";
		public const string Call = "C";
	}

	public static class OptionSide
	{
		public const string OPEN = "O";
		public const string CLOSE = "C";
	}

	public static class OptionParticipents
	{
		public const string CBOE = "W";
		public const string BOX = "B";
		public const string AMEX = "A";
		public const string PHLX = "X";
		public const string PCX = "N";
		public const string ISE = "Y";
		public const string NSDQ = "Q";
		public const string NBBO = "NBBO";
	}

	public static class L2Participents
	{
		public static string Translate(string input)
		{
			string output = "";

			switch (input.ToUpper())
			{
				case "C":
					output = "National";
					break;
				case "A":
					output = "American";
					break;
				case "B":
					output = "Boston";
					break;
				case "I":
					output = "ISE";
					break;
				case "Q":
					output = "Nasdq";
					break;
				case "N":
					output = "NYSE";
					break;
				case "P":
					output = "Pacific";
					break;
				case "X":
					output = "Philadelphia";
					break;
				case "Z":
					output = "BATS";
					break;
				case "National":
					output = "C";
					break;
				case "American":
					output = "A";
					break;
				case "Boston":
					output = "B";
					break;
				case "ISE":
					output = "I";
					break;
				case "Nasdq":
					output = "Q";
					break;
				case "NYSE":
					output = "N";
					break;
				case "Pacific":
					output = "P";
					break;
				case "Philadelphia":
					output = "X";
					break;
				case "BATS":
					output = "Z";
					break;
			}

			return output;
		}
	}

	#region - Not Used -

	//public class OPRAData
	//{
	//    private Dictionary<string, OPRAParticipentData> _opraSymbolData;
	//    public Dictionary<string, OPRAParticipentData> OPRASymbolData
	//    {
	//        get
	//        {
	//            if (_opraSymbolData == null)
	//            {
	//                _opraSymbolData = new Dictionary<string,OPRAParticipentData>();
	//            }
	//            return _opraSymbolData;
	//        }
	//    }

	//    public OPRAData(String issueSymbol, L1Data data)
	//    {
	//        string symbol = issueSymbol;
	//        string participent = "NBBO";

	//        int loc = issueSymbol.IndexOf(".");
	//        if (loc > 0)
	//        {
	//            symbol = issueSymbol.Substring(0, loc);
	//            participent = issueSymbol.Substring(loc + 1);
	//        }

	//        switch (participent)
	//        {
	//            case OptionParticipents.AMEX:
	//            case OptionParticipents.BOX:
	//            case OptionParticipents.CBOE:
	//            case OptionParticipents.ISE:
	//            case OptionParticipents.NSDQ:
	//            case OptionParticipents.PCX:
	//            case OptionParticipents.PHLX:
	//            case OptionParticipents.NBBO:
	//                OPRAParticipentData participentData;
	//                if (OPRASymbolData.ContainsKey(participent))
	//                {
	//                    participentData = OPRASymbolData[participent];
	//                }
	//                OPRAParticipentData data = new OPRAParticipentData(participent, participentData);
	//                break;
	//            default:
	//                break;
	//        }
	//    }
	//}
	//public class OPRAParticipentData
	//{
	//    private string _participent = "";
	//    public string Participent
	//    {
	//        get
	//        {
	//            return _participent;
	//        }
	//    }

	//    private L1Data _participentData;
	//    public L1Data ParticipentData
	//    {
	//        get
	//        {
	//            return _participentData;
	//        }
	//    }

	//    public OPRAParticipentData(string participent, L1Data participentData)
	//    {
	//        participent = _participent;
	//        _participentData = participentData;
	//    }
	//}

	//public class L1Data
	//{
	//    // L1
	//    public string IssueSymbol;			//305
	//    public string InstrumentType;		//298
	//    public string LineTime;				//1174

	//    public double NetChange;			//341
	//    public double PctChange;			//431
	//    public int TotalVolume;				//475
	//    public double HighPrice;			//278
	//    public double LowPrice;				//327
	//    public double OpenPrice;			//407
	//    public double ClosePrice;			//242
	//    public double PrevClosePrice;		//436
	//    public string PrevCloseDate;		//435
	//    public double TotalValue;			//474
	//    public double Vwap;					//494

	//    public double BidPrice;				//237
	//    public int BidSize;					//238
	//    public double BidLow;				//234
	//    public double BidHigh;				//233
	//    public string BidPartId;			//236

	//    public double AskPrice;				//109
	//    public int AskSize;					//110
	//    public double AskLow;				//106
	//    public double AskHigh;				//105
	//    public string AskPrarId;			//108

	//    public string QuoteDate;			//519
	//    public string QuoteTime;			//442
	//    public int QuoteCount;				//1034

	//    public double TradePrice;			//481
	//    public string TradeDate;			//476
	//    public string TradeTime;			//485
	//    public string TradeTick;			//484
	//    public int TradeVolume;				//488
	//    public string TradePartId;			//480
	//    public int TradeCount;				//901

	//    // Stock L1
	//    public string PrimExch;						//645

	//    // Future L1
	//    public string ProductInd;					//1520
	//    public double DisplayConversionFactor;		//1518
	//    public int OrderMinQty;						//513
	//    public int OrderMaxQty;						//514
	//    public string MaxPriceVariation;			//3341
	//    public string Maturity;						//1525
	//    public int BookDepth;						//2276
	//    public double TicketIncrement;				//515
	//    public string InstrumentGroup;				//510
	//    public double MinPriceIncrementAmount;		//3345
	//    public string MainFraction;					//3342
	//    public double HightLimit;					//277
	//    public double LowLimit;						//326
	//    public double SettlePrice;					//458
	//    public double SettleDate;					//456
	//    public double LastBestAskPrice;				//4026
	//    public double LastBestBidPrice;				//4025
	//    public int ExchTotalVol;					//2516

	//    // Option L1
	//    public string PutCall;						//439
	//    public string ExpirationDate;				//262
	//    public double StrikePrice;					//469
	//    public int OpenInterest;					//406
	//    public double PrevBidClose;					//433
	//    public string PrevBidCloseDate;				//434
	//    public double BidClose;						//504
	//    public string BidCloseDate;					//505
	//    public double PrevAskClose;					//506
	//    public string PrevAskCloseDate;				//507
	//    public double AskClose;						//502
	//    public string AskClosedate;					//503

	//    public L1Data()
	//    {
	//    }
	//}

	#endregion

	public class Fields
	{
		private Dictionary<int, string> _ids;
		public Dictionary<int, string> IDs
		{
			get
			{
				return _ids;
			}
		}

		public Fields()
		{
			_ids = new Dictionary<int, string>();
		}

		public void Add(int key, string value)
		{
			Modify(key, value);
		}
		public void Remove(int key, string value)
		{
			if (_ids.ContainsKey(key))
			{
				_ids.Remove(key);
			}
		}
		public void Replace(int key, string value)
		{
			Modify(key, value, true);
		}

		private void Modify(int key, string value)
		{
			Modify(key, value, false);
		}
		private void Modify(int key, string value, bool replace)
		{
			if (_ids == null)
			{
				_ids = new Dictionary<int, string>();
			}
			if (_ids.ContainsKey(key))
			{
				if (replace)
				{
					_ids[key] = value;
				}
			}
			else
			{
				_ids.Add(key, value);
			}
		}

		public bool IsUsed(int key)
		{
			return _ids.ContainsKey(key);
		}
		public bool IsUsed(string input)
		{
			return _ids.ContainsValue(input.ToUpper());
		}

		public string GetName(int key)
		{
			if (IsUsed(key))
			{
				return _ids[key];
			}
			return "";
		}

		public int GetKey(string input)
		{
			if (IsUsed(input))
			{
				foreach (int key in _ids.Keys)
				{
					if (input.ToUpper() == _ids[key])
					{
						return key;
					}
				}
			}
			return -1;
		}

		#region - Constant -

		//public const int IssueSymbol = 305;
		//public const int Symbol = -101;
		//public const int ParticipentSymbol = -102;
		//public const int InstrumentType = 298;
		//public const int LineTime = 1174;

		//public const int NetChange = 341;
		//public const int PctChange = 431;
		//public const int TotalVolume = 475;
		//public const int HighPrice = 278;
		//public const int LowPrice = 327;
		//public const int OpenPrice = 407;
		//public const int ClosePrice = 242;
		//public const int PrevClosePrice = 436;
		//public const int PrevCloseDate = 435;
		//public const int TotalValue = 474;
		//public const int Vwap = 494;

		//public const int BidPrice = 237;
		//public const int BidSize = 238;
		//public const int BidLow = 234;
		//public const int BidHigh = 233;
		//public const int BidPartId = 236;

		//public const int AskPrice = 109;
		//public const int AskSize = 110;
		//public const int AskLow = 106;
		//public const int AskHigh = 105;
		//public const int AskPrarId = 108;

		//public const int QuoteDate = 519;
		//public const int QuoteTime = 442;
		//public const int QuoteCount = 1034;

		//public const int TradePrice = 481;
		//public const int TradeDate = 476;
		//public const int TradeTime = 485;
		//public const int TradeTick = 484;
		//public const int TradeVolume = 488;
		//public const int TradePartId = 480;
		//public const int TradeCount = 901;

		//// Stock L1
		//public const int PrimExch = 645;

		//// Future L1
		//public const int ProductInd = 1520;
		//public const int DisplayConversionFactor = 1518;
		//public const int OrderMinQty = 513;
		//public const int OrderMaxQty = 514;
		//public const int MaxPriceVariation = 3341;
		//public const int Maturity = 1525;
		//public const int BookDepth = 2276;
		//public const int TicketIncrement = 515;
		//public const int InstrumentGroup = 510;
		//public const int MinPriceIncrementAmount = 3345;
		//public const int MainFraction = 3342;
		//public const int HightLimit = 277;
		//public const int LowLimit = 326;
		//public const int SettlePrice = 458;
		//public const int SettleDate = 456;
		//public const int LastBestAskPrice = 4026;
		//public const int LastBestBidPrice = 4025;
		//public const int ExchTotalVol = 2516;

		//// Option L1
		//public const int PutCall = 439;
		//public const int ExpirationDate = 262;
		//public const int StrikePrice = 469;
		//public const int OpenInterest = 406;
		//public const int PrevBidClose = 433;
		//public const int PrevBidCloseDate = 434;
		//public const int BidClose = 504;
		//public const int BidCloseDate = 505;
		//public const int PrevAskClose = 506;
		//public const int PrevAskCloseDate = 507;
		//public const int AskClose = 502;
		//public const int AskClosedate = 503;

		//// Book
		//public const int BookBidSize = -1001;
		//public const int BookBidPrice = -2001;

		//public const int BookAskSize = -3001;
		//public const int BookAskPrice = -4001;

		#endregion
	}

	[Serializable]
	public class Changes
	{
		private object _syncObj = new object();

		private Dictionary<int, long> _ints;
		public Dictionary<int, long> Ints
		{
			get
			{
				return _ints;
			}
		}

		private Dictionary<int, double> _doubles;
		public Dictionary<int, double> Doubles
		{
			get
			{
				return _doubles;
			}
		}

		private Dictionary<int, string> _strings;
		public Dictionary<int, string> Strings
		{
			get
			{
				return _strings;
			}
		}

		public Changes()
		{
			Initialize();
		}

		public Changes(int key, string val)
		{
			Initialize();
			Modify(key, val);
		}
		public Changes(int key, double val)
		{
			Initialize();
			Modify(key, val);
		}
		public Changes(int key, long val)
		{
			Initialize();
			Modify(key, val);
		}

		public void Add(int key, string val)
		{
			Modify(key, val);
		}
		public void Add(int key, double val)
		{
			Modify(key, val);
		}
		public void Add(int key, long val)
		{
			Modify(key, val);
		}

		public void Remove(int key)
		{
			lock (_syncObj)
			{
				if (_strings.ContainsKey(key))
				{
					_strings.Remove(key);
				}

				if (_doubles.ContainsKey(key))
				{
					_doubles.Remove(key);
				}

				if (_ints.ContainsKey(key))
				{
					_ints.Remove(key);
				}
			}
		}

		public void Replace(int key, string val)
		{
			Modify(key, val, true);
		}
		public void Replace(int key, double val)
		{
			Modify(key, val, true);
		}
		public void Replace(int key, long val)
		{
			Modify(key, val, true);
		}

		public void Clear()
		{
			Initialize();
		}

		private void Initialize()
		{
			lock (_syncObj)
			{
				_strings = new Dictionary<int,string>();
				_doubles = new Dictionary<int,double>();
				_ints = new Dictionary<int, long>();
			}
		}

		private void Modify(int key, string val)
		{
			Modify(key, val, false);
		}
		private void Modify(int key, string val, bool replace)
		{
			lock (_syncObj)
			{
				if (_strings.ContainsKey(key))
				{
					if (replace)
					{
						_strings[key] = val;
					}
				}
				else
				{
					_strings.Add(key, val);
				}
			}
		}

		private void Modify(int key, double val)
		{
			Modify(key, val, false);
		}
		private void Modify(int key, double val, bool replace)
		{
			lock (_syncObj)
			{
				if (_doubles.ContainsKey(key))
				{
					if (replace)
					{
						_doubles[key] = val;
					}
				}
				else
				{
					_doubles.Add(key, val);
				}
			}
		}

		private void Modify(int key, long val)
		{
			Modify(key, val, false);
		}
		private void Modify(int key, long val, bool replace)
		{
			lock (_syncObj)
			{
				if (_ints.ContainsKey(key))
				{
					if (replace)
					{
						_ints[key] = val;
					}
				}
				else
				{
					_ints.Add(key, val);
				}
			}
		}
	}
}
