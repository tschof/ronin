using System;
using System.Collections.Generic;
using System.Text;

namespace CSVEx
{
	public sealed class CSVFieldIDs
	{
		public sealed class InstructionCmds
		{
			public const string IOC = "INET-C";
			public const string IOO = "INET-O";
			public const string IOI = "INET-I";
		}

		public sealed class MessageTypes
		{
			public const string EndOfQueuedMsg = "Q";
			public const string HeartBeat = "H";
			public const string LoggedIn = "L";
			public const string LoginFailed = "K";
			public const string Alert = "@";
			public const string InvalidField = "I";
			public const string WorkingOrder = "E";
			public const string OrderStatus = "S";

			private static Dictionary<string, string> _descriptions;
			public static Dictionary<string, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<string, string>();
						_descriptions.Add(EndOfQueuedMsg, "End of Queued ROM Messages");
						_descriptions.Add(HeartBeat, "Heart Beat");
						_descriptions.Add(LoggedIn, "Logged In");
						_descriptions.Add(Alert, "Alert");
						_descriptions.Add(InvalidField, "Invalid Field");
						_descriptions.Add(WorkingOrder, "Working Order");
						_descriptions.Add(OrderStatus, "Order Status");
					}

					return _descriptions;
				}
			}
		}

		public sealed class CommandTypes
		{
			public const string Login = "L";
			public const string EnterOrder = "E";
			public const string CancelSingle = "C";
			public const string CancelMultiple = "c";
			public const string Replace = "R";

			private static Dictionary<string, string> _descriptions;
			public static Dictionary<string, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<string, string>();
						_descriptions.Add(Login, "Login");
						_descriptions.Add(EnterOrder, "Enter Order");
						_descriptions.Add(CancelSingle, "Cancel Single");
						_descriptions.Add(CancelMultiple, "Cancel Multiple");
						_descriptions.Add(Replace, "Replace");
					}

					return _descriptions;
				}
			}
		}

		// Used when Sending Loggin Commands
		public sealed class CancelChoices
		{
			public const int KeepOrdersAlive = 0;
			public const int CancelAllLiveOrderOnDisconnection = 1;
			public const int CancelAllLiveOrderExceptGTD_GTC = 2;

			private static Dictionary<int, string> _descriptions;
			public static Dictionary<int, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<int, string>();
						_descriptions.Add(KeepOrdersAlive, "Keep Orders Alive");
						_descriptions.Add(CancelAllLiveOrderOnDisconnection, "Cancel All Live Order On Disconnection");
						_descriptions.Add(CancelAllLiveOrderExceptGTD_GTC, "Cancel All Live Order Except GTD & GTC");
					}

					return _descriptions;
				}
			}
		}

		// Used when Sending Enter Commands 
		public sealed class SideCodes
		{
			public const long Buy = 1;
			public const long Sell = 2;
			public const long Short = 5;
			public const long SSE = 6;

			private static Dictionary<long, string> _descriptions;
			public static Dictionary<long, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<long, string>();
						_descriptions.Add(Buy, "Buy");
						_descriptions.Add(Sell, "Sell");
						_descriptions.Add(Short, "Short");
						_descriptions.Add(SSE, "SSE");
					}

					return _descriptions;
				}
			}

			private static string _buySideFilter = string.Format("Side = {0}", SideCodes.Buy);
			public static string BuySideFilter
			{
				get
				{
					return _buySideFilter;
				}
			}

			private static string _sellSideFilter = string.Format("Side = {0} Or Side = {1}", SideCodes.Sell, SideCodes.Short);
			public static string SellSideFilter
			{
				get
				{
					return _sellSideFilter;
				}
			}
		}

		public sealed class OrderTypes
		{
			public const long Market = 1;
			public const long Limit = 2;
			public const long Stop = 3;
			public const long StopLimit = 4;
			public const long MarketOnClose = 5;
			public const long WithOrWithout = 6;
			public const long LimitOrBetter = 7;
			public const long LimitWithOrWithout = 8;
			public const long OnBasis = 9;
			public const long OnClose = 10;
			public const long LimitOnClose = 11;
			public const long MarketAtOpen = 12;
			public const long VWAP = 14;
			public const long PegMkt = 90;
			public const long PegPri = 91;
			public const long PegMid = 92;
			public const long VWAPMKT = 93;
			public const long VWAPLIM = 94;
			public const long IMBOC = 95;
			public const long IMBOO = 96;
			public const long IMBOI = 97;
			public const long GVWAP = 98;
			public const long OCO = 99;

			// K = Marketable Limit order on CME/Globex

			private static Dictionary<long, string> _descriptions;
			public static Dictionary<long, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<long, string>();
						_descriptions.Add(Market, "Market");
						_descriptions.Add(Limit, "Limit");
						_descriptions.Add(Stop, "Stop");
						_descriptions.Add(StopLimit, "StopLimit");
						_descriptions.Add(MarketOnClose, "MOC");
						_descriptions.Add(WithOrWithout, "WithOrWithout");
						_descriptions.Add(LimitOrBetter, "LimitOrBetter");
						_descriptions.Add(LimitWithOrWithout, "LimitWithOrWithout");
						_descriptions.Add(OnBasis, "OnBasis");
						_descriptions.Add(OnClose, "OnClose");
						_descriptions.Add(LimitOnClose, "LOC");
						_descriptions.Add(MarketAtOpen, "MOO");
						_descriptions.Add(VWAP, "VWAP");
						_descriptions.Add(PegMkt, "Peg MKT");
						_descriptions.Add(PegPri, "Peg PRI");
						_descriptions.Add(PegMid, "Peg MID");
						_descriptions.Add(IMBOC, "IMBOC");
						_descriptions.Add(IMBOO, "IMBOO");
						_descriptions.Add(IMBOI, "IMBOI");
						_descriptions.Add(VWAPMKT, "VWAP MKT");
						_descriptions.Add(VWAPLIM, "VWAP LIM");
						_descriptions.Add(GVWAP, "GVWAP");
						_descriptions.Add(OCO, "OCO");
					}

					return _descriptions;
				}
			}

			private static string _nonMarketOrderOnlyFilter = string.Format(
				("OrderType <> {0} And OrderType <> {1} And OrderType <> {2}"),
				new object[] {
					OrderTypes.Market,
					OrderTypes.MarketOnClose,
					OrderTypes.MarketAtOpen });
			public static string NonMarketOrderOnlyFilter
			{
				get
				{
					return _nonMarketOrderOnlyFilter;
				}
			}

			private static string _stopOrderOnlyFilter = string.Format(
				("OrderType = {0} Or OrderType = {1}"),
				new object[] {
					OrderTypes.Stop,
					OrderTypes.StopLimit });
			public static string StopOrderOnlyFilter
			{
				get
				{
					return _stopOrderOnlyFilter;
				}
			}
		}

		public sealed class TIFCodes
		{
			public const long Day = 0;
			public const long GTC = 1;		// Good Till Cancel
			public const long OPG = 2;
			public const long IOC = 3;		// Immediate or Cancel
			public const long FOK = 4;		// Fill or Kill
			public const long GTX = 5;
			public const long GTD = 6;		// Good Till Date
			public const long OCL = 7;		// On Close

			private static Dictionary<long, string> _descriptions;
			public static Dictionary<long, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<long, string>();
						_descriptions.Add(Day, "Day");
						_descriptions.Add(GTC, "GTC");
						_descriptions.Add(OPG, "On Open");
						_descriptions.Add(IOC, "IOC");
						_descriptions.Add(FOK, "FOK");
						_descriptions.Add(GTX, "GTX");
						_descriptions.Add(GTD, "GTD");
						_descriptions.Add(OCL, "On Close");
					}

					return _descriptions;
				}
			}

			private static string _tifDayFilter = string.Format(
				("TIF <> {0} And TIF <> {1}"),
				new object[] {
					TIFCodes.GTC,
					TIFCodes.GTD });
			public static string TIFDayFilter
			{
				get
				{
					return _tifDayFilter;
				}
			}
		}

		public sealed class AlgoTypes
		{
			public const long None = 0;
			public const long TWAP = 1;
			public const long VWAP = 2;
			public const long GVWAP = 3;

			private static Dictionary<long, string> _descriptions;
			public static Dictionary<long, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<long, string>();
						_descriptions.Add(None, "None");
						_descriptions.Add(TWAP, "TWAP");
						_descriptions.Add(VWAP, "VWAP");
						_descriptions.Add(GVWAP, "GVWAP");
					}

					return _descriptions;
				}
			}
		}

		public sealed class StatusCodes
		{
			public const long New = 0;
			public const long PartiallyFilled = 1;
			public const long Filled = 2;
			public const long DoneForDay = 3;
			public const long Canceled = 4;
			public const long Replaced = 5;
			public const long PendingCancel = 6;
			public const long Stopped = 7;
			public const long Rejected = 8;
			public const long PendingNew = 10;
			public const long Calculated = 11;
			public const long Expired = 12;
			public const long Open = 13;
			public const long CancelRejected = 14;
			public const long Corrected = 15;
			public const long Busted = 16;
			public const long ReplaceRejected = 20;
			public const long ReplacePending = 21;
			public const long Exposed = 22;
			public const long DirectedThruIML = 23;
			public const long Directed = 24;
			public const long DirectedReject = 25;
			public const long ROMCancelPending = 26;
			public const long ROMReplacePending = 27;
			public const long FilledAndCancelled = 99;
			public const long ReplacedAndFilled = 100;

			private static Dictionary<long, string> _descriptions;
			public static Dictionary<long, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<long, string>();
						_descriptions.Add(New, "Sent");
						_descriptions.Add(PartiallyFilled, "Partially Filled");
						_descriptions.Add(Filled, "Filled");
						_descriptions.Add(DoneForDay, "Done For Day");
						_descriptions.Add(Canceled, "Canceled");
						_descriptions.Add(Replaced, "Replaced");
						_descriptions.Add(PendingCancel, "Pending Cancel");
						_descriptions.Add(Stopped, "Stopped");
						_descriptions.Add(Rejected, "Rejected");
						_descriptions.Add(PendingNew, "Pending New");
						_descriptions.Add(Calculated, "Calculated");
						_descriptions.Add(Expired, "Expired");
						_descriptions.Add(Open, "Open");
						_descriptions.Add(CancelRejected, "Cancel Rejected");
						_descriptions.Add(Corrected, "Corrected");
						_descriptions.Add(Busted, "Busted");
						_descriptions.Add(ReplaceRejected, "Replace Rejected");
						_descriptions.Add(Exposed, "Exposed");
						_descriptions.Add(DirectedThruIML, "Directed Thru IML");
						_descriptions.Add(Directed, "Directed");
						_descriptions.Add(DirectedReject, "Directed Reject");
						_descriptions.Add(ROMCancelPending, "ROM Cancel Pending");
						_descriptions.Add(ROMReplacePending, "ROM Replace Pending");
						_descriptions.Add(FilledAndCancelled, "Fill-Canceled");
						_descriptions.Add(ReplacedAndFilled, "Replace-Filled");
					}

					return _descriptions;
				}
			}

			public static bool IsActive(long status)
			{
				switch (status)
				{
					case StatusCodes.Busted:
					case StatusCodes.Expired:
					case StatusCodes.Rejected:
					case StatusCodes.Canceled:
					case StatusCodes.Filled:
					case StatusCodes.FilledAndCancelled:
					case StatusCodes.ReplacedAndFilled:
						return false;
					default:
						return true;
				}
			}

			private static string _activeStatusFilter = string.Format(
				("Status <> {0} And Status <> {1} And Status <> {2} And Status <> {3} And Status <> {4} And Status <> {5} And Status <> {6}"),
				new object[] {
					StatusCodes.Busted,
					StatusCodes.Expired,
					StatusCodes.Rejected,
					StatusCodes.Canceled, 
					StatusCodes.Filled,
					StatusCodes.FilledAndCancelled,
					StatusCodes.ReplacedAndFilled });
			public static string ActiveStatusFilter
			{
				get
				{
					return _activeStatusFilter;
				}
			}
		}

		public sealed class OptionTypes
		{
			public const string Put = "P";
			public const string Call = "C";

			public const string Close = "0";
			public const string Open = "1";

			private static Dictionary<string, string> _descriptions;
			public static Dictionary<string, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<string, string>();
						_descriptions.Add(Put, "Put");
						_descriptions.Add(Call, "Call");
						_descriptions.Add(Close, "Close");
						_descriptions.Add(Open, "Open");
					}

					return _descriptions;
				}
			}
		}

		public sealed class SecurityTypes
		{
			public const string Equity = "E";
			public const string Future = "F";
			public const string Option = "O";
			public const string OptionIndex = "I";
			public const string SingleStockFuture = "J";
			public const string FutureIndex = "K";
			public const string FutureCurrency = "L";
			public const string OptionFuture = "M";
			public const string Spread = "S";
			public const string Forex = "X";

			private static Dictionary<string, string> _descriptions;
			public static Dictionary<string, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<string, string>();
						_descriptions.Add(Equity, "Equity");
						_descriptions.Add(Future, "Future");
						_descriptions.Add(Option, "Option");
						_descriptions.Add(OptionIndex, "OptionIndex");
						_descriptions.Add(SingleStockFuture, "SingleStockFuture");
						_descriptions.Add(FutureIndex, "FutureIndex");
						_descriptions.Add(FutureCurrency, "FutureCurrency");
						_descriptions.Add(OptionFuture, "OptionFuture");
						_descriptions.Add(Spread, "Spread");
						_descriptions.Add(Forex, "Forex");
					}

					return _descriptions;
				}
			}

			private static string _equityFilter = string.Format("SecType = '{0}'", new object[] { SecurityTypes.Equity });
			public static string EquityFilter
			{
				get
				{
					return _equityFilter;
				}
			}

			private static string _futureFilter = string.Format("SecType = '{0}'", new object[] { SecurityTypes.Future });
			public static string FutureFilter
			{
				get
				{
					return _futureFilter;
				}
			}

			private static string _optionFilter = string.Format("SecType = '{0}'", new object[] { SecurityTypes.Option });
			public static string OptionFilter
			{
				get
				{
					return _optionFilter;
				}
			}
		}

		public sealed class ExecutionInstructions
		{
			public const string NotHeld = "1";
			public const string Work = "2";
			public const string GoAlong = "3";
			public const string OverTheDay = "4";
			public const string Held = "5";
			public const string PartNoInit = "6";
			public const string StrictScale = "7";
			public const string TryScale = "8";
			public const string StayOnBid = "9";
			public const string StayOnOffer = "0";
			public const string NoCross = "A";
			public const string OkToCross = "B";
			public const string CallFirst = "C";
			public const string PctOfVol = "D";
			public const string DNI = "E";
			public const string DNR = "F";
			public const string AON = "G";
			public const string InstOnly = "I";
			public const string LastPeg = "L";
			public const string MidPricePeg = "M";
			public const string NonNegot = "N";
			public const string OpeningPeg = "O";
			public const string MarketPeg = "P";
			public const string PrimaryPeg = "R";
			public const string Suspend = "S";
			public const string FixedPeg = "T";
			public const string CustDisp = "U";
			public const string Netting = "V";
			public const string VWAPPeg = "W";

			private static Dictionary<string, string> _descriptions;
			public static Dictionary<string, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<string, string>();
						_descriptions.Add(NotHeld, "NotHeld");
						_descriptions.Add(Work, "Work");
						_descriptions.Add(GoAlong, "GoAlong");
						_descriptions.Add(OverTheDay, "OverTheDay");
						_descriptions.Add(Held, "Held");
						_descriptions.Add(PartNoInit, "PartNoInit");
						_descriptions.Add(StrictScale, "StrictScale");
						_descriptions.Add(TryScale, "TryScale");
						_descriptions.Add(StayOnBid, "StayOnBid");
						_descriptions.Add(StayOnOffer, "StayOnOffer");
						_descriptions.Add(NoCross, "NoCross");
						_descriptions.Add(OkToCross, "OkToCross");
						_descriptions.Add(CallFirst, "CallFirst");
						_descriptions.Add(PctOfVol, "PctOfVol");
						_descriptions.Add(DNI, "DNI");
						_descriptions.Add(DNR, "DNR");
						_descriptions.Add(AON, "AON");
						_descriptions.Add(InstOnly, "InstOnly");
						_descriptions.Add(LastPeg, "LastPeg");
						_descriptions.Add(MidPricePeg, "MidPricePeg");
						_descriptions.Add(NonNegot, "NonNegot");
						_descriptions.Add(OpeningPeg, "OpeningPeg");
						_descriptions.Add(MarketPeg, "MarketPeg");
						_descriptions.Add(PrimaryPeg, "PrimaryPeg");
						_descriptions.Add(Suspend, "Suspend");
						_descriptions.Add(FixedPeg, "FixedPeg");
						_descriptions.Add(CustDisp, "CustDisp");
						_descriptions.Add(Netting, "Netting");
						_descriptions.Add(VWAPPeg, "VWAPPeg");
					}

					return _descriptions;
				}
			}
		}

		public sealed class CplxOrderTypes
		{
			public const int Container = 5;
			public const int Leg = 6;

			private static Dictionary<int, string> _descriptions;
			public static Dictionary<int, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<int, string>();
						_descriptions.Add(Container, "Continer");
						_descriptions.Add(Leg, "Leg");
					}

					return _descriptions;
				}
			}
		}

		public sealed class PegOrderExecInstructions
		{
			public const string MarketPeg = "P";
			public const string PrimaryPeg = "R";
			public const string MidpointPeg = "M";
		}

		// TO ROM
		public sealed class CMD
		{
			public sealed class L
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int LoginName = 2;					// Login name that is unique to the OrderManager system.  Currently is set up in the OMUser table of the ROM database
				public const int CancelChoice = 3;				// For ROC it should allways be 0
				public const int CancelMinutes = 4;				// Minutes before the client’s orders will be canceled after disconnected from ROM.  Its default value is 0.
				public const int ApplicationName = 5;			// The name of the client application sending the login message.
				public const int ApplicationVersion = 6;		// The version of the client application.  This is used to track the compatibility between OM and client application.
				public const int Password = 7;					// Password.
			}

			public sealed class E
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int Trader = 2;					// Trader acronym that is entering the order.  Can be different from the login name however it has to be defined in ROM database.
				public const int Tag = 3;						// Client defined order tag.  MUST be unique for a given day.
				public const int Side = 4;						// Side of this order:
				public const int Symbol = 5;					// Stock – Underlying, Options – OCC Symbol, Futures – Futures Symbol
				public const int Shares = 6;					// Shares – current order quantity
				public const int Price = 7;						// Price in decimal format
				public const int Type = 8;
				public const int TIF = 9;
				public const int Capacity = 10;
				public const int ClearingID = 11;
				public const int ClearingAcct = 12;
				public const int ExchangeID = 13;
				public const int Status = 14;
				public const int Instructions = 15;
				// 16
				// 17
				public const int MinQty = 18;
				public const int StopPrice = 19;
				public const int Discretionary = 20;			// Y/N
				public const int ProgramTrade = 21;				// Program Trade Flag
				public const int LocalAcct = 22;
				public const int Firm = 23;
				public const int TradeFor = 24;					// Set to the trader acronym who owns the account (usually same as Trader Field(#2))
				// 25
				public const int DBSymbol = 26;					// The symbol for inserting trades to Ronin TPOS database. Required if it is different from Symbol.
				public const int Owner = 27;
				public const int Multiplier = 28;				// OM Exec Tag (Dont sent)
				public const int AlgoExchangeID = 29;			// AlgoExchange destID For Dart Algo
				public const int ExpDate = 30;					// Future or Option YYYYMM
				public const int PutCall = 31;
				public const int StrikePrice = 32;
				public const int Underlying = 33;				// It is required for Option and Future(including single stock future) order only
				public const int Floor = 34;					// Max. shares to be displayed.  This feature is not supported by all exchanges.  
				public const int SecType = 35;
				public const int MarketMakerAccount = 36;
				public const int MarketMakerHomeExchange = 37;
				public const int OpenClose = 38;
				public const int TradingSession = 39;
				public const int GriveUpAccount = 40;
				// 41
				public const int LastShares = 42;				// Filled shares in last transaction(when Status is Partially Filled or Filled)
				public const int OriginalShares = 43;			// Total shares of original order (when Status is Partially Filled or Filled)
				public const int ExecPrice = 44;				// Eecution price of last transaction(when Status is Partially Filled or Filled)
				// 45
				public const int CMTAAccount = 46;
				public const int OrderExpirationDateTime = 47;

				public const int SecurityDefinition = 55;		// The ‘market data’ symbol for options on futures products. Ex: for the ES June 2008 1380 Puts The security definition is ESM8 P1380.
				// 56
				public const int ExecutionInstruction = 57;
				public const int ShortLender = 58;				// Required only for Short Sell and Short Sell Exempt orders. NASD Rule 3370 – identify who has loaned the stock to settle. MPID or 4 digit clearing number.

				public const int MaturityDay = 62;				// Option Maturity Day
				public const int DisplayInstruction = 63;		// Imbalance Only "I"

				public const int PegPriceOffset = 64;			// Pag Orders
				public const int ParentTag = 65;				// Client Parent Tag
				public const int CplxOrderType = 66;			// Complex Order Type

				public const int AlgoType = 73;					// AlgoEngin Type

				public const int ClientEcho = 74;

				public const int BeginTime = 76;				// Begin Time for VWAP (default is time order is entered or 8:30al CT open) UTC format.
				public const int EndTime = 77;					// End Time for VWAP (default is time is 3:00pm CT market close) UTC format.
			}

			public sealed class C
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int Trader = 2;
				public const int Tag = 3;
				public const int Side = 4;
				public const int Symbol = 5;
				public const int OriginalShares = 6;
				public const int Price = 7;
			}

			public sealed class c
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int Trader = 2;					// Trader acronym of the trader who entered the orders.  Can be different from the login name however it has to be defined in ROM database.
				public const int OnlyDayorder = 3;				// If this field is 1 only Day orders will be canceled
				public const int Exchange = 13;					// If this field is empty all your open orders will be canceled.
				public const int LoginName = 27;				// Speical For Old ROM Only
			}

			public sealed class R
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int Trader = 2;
				public const int Tag = 3;
				public const int Side = 4;
				public const int Symbol = 5;
				public const int NewShares = 6;					// NEW open shares
				public const int NewPrice = 7;					// NEW price to replace the order at
				public const int TIF = 9;
				public const int NewStopPrice = 19;				// NEW Stop price for Stop order(Tag 8 = 3 ) or Stop Limit order(Tag 8 = 4)

				public const int TradeFor = 24;

				public const int Floor = 34;					// Optional, ROM will sent the default Floor when nothing is send

				public const int LeaveShares = 49;				// Flag the replace to not change qty (-1)

				public const int SecurityDefinition = 55;		// The ‘market data’ symbol for options on futures products. Ex: for the ES June 2008 1380 Puts The security definition is ESM8 P1380.

				public const int NewPegPrice = 64;
			}
		}

		// FROM ROM
		public sealed class MSG
		{
			public sealed class L
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int LoginName = 2;
			}

			// Login Reject
			public sealed class K
			{
				public const int Command = 0;
				public const int Text = 26;
			}

			// Order Status Message
			public sealed class S
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int Trader = 2;
				public const int Tag = 3;
				public const int Side = 4;
				public const int Symbol = 5;
				public const int Shares = 6;
				public const int Price = 7;
				public const int Type = 8;
				public const int TIF = 9;
				public const int ExchangeID = 13;
				public const int Status = 14;
				// 15
				// 16
				public const int OmTag = 17;

				public const int Contra = 23;
				public const int TradeFor = 24;

				public const int Text = 26;
				public const int Owner = 27;
				public const int OmExecTag = 28;

				public const int SecType = 35;
				public const int TradingSession = 39;

				public const int LastShares = 42;
				public const int OriginalShares = 43;
				public const int ExecPrice = 44;

				public const int CumShares = 48;
				public const int LeavShares = 49;
				public const int ExecutionTime = 50;
				public const int AveragePrice = 51;
				public const int OmTime = 52;

				public const int ExchangeTag = 54;
				public const int SecurityDefinition = 55;		// The ‘market data’ symbol for options on futures products. Ex: for the ES June 2008 1380 Puts The security definition is ESM8 P1380.

				public const int MaturityDay = 62;				// Option Maturity Day

				public const int ParentTag = 65;				// Client Parent Tag
				public const int CplxOrderType = 66;			// Complex Order Type

				public const int EffectiveTime = 68;
				public const int LiquidityIndicator = 69;
				public const int ParentRomTag = 70;
				public const int MsgNumber = 71;				// 1 Based

				public const int ClientEcho = 74;
			}
		}

		public const int Command = 0;
		public const int CurrentTimeStamp = 1;
		public const int Trader = 2;
		public const int Tag = 3;
		public const int Side = 4;
		public const int Symbol = 5;
		public const int Shares = 6;
		public const int Price = 7;
		public const int Type = 8;
		public const int TIF = 9;

		public const int ClearingID = 11;
		public const int ClearingAcct = 12;
		public const int ExchangeID = 13;
		public const int Status = 14;

		public const int Instructions = 15;
		public const int NumHistFields = 16;
		public const int OmTag = 17;

		public const int StopPrice = 19;

		public const int ProgramTrade = 21;
		public const int LocalAcct = 22;
		public const int Firm = 23;
		public const int TradeFor = 24;

		public const int Text = 26;
		public const int Owner = 27;
		public const int OmExecTag = 28;
		public const int AlgoExchangeID = 29;
		// Option
		public const int ExpDate = 30;
		public const int CallPut = 31;
		public const int StrikePrice = 32;
		public const int Underlying = 33;				// It is required for Option and Future(including single stock future) order only

		public const int Floor = 34;
		public const int SecType = 35;

		public const int OpenClose = 38;

		public const int LastShares = 42;
		public const int OriginalShares = 43;
		public const int ExecPrice = 44;

		public const int CMTAAccount = 46;
		public const int OrderExpirationDateTime = 47;
		public const int CumShares = 48;
		public const int LeaveShares = 49;
		public const int ExecutionTime = 50;
		public const int AveragePrice = 51;
		public const int OmTime = 52;

		public const int SecurityDefinition = 55;		// The ‘market data’ symbol for options on futures products. Ex: for the ES June 2008 1380 Puts The security definition is ESM8 P1380.
		
		public const int ExecutionInstruction = 57;

		public const int MaturityDay = 62;				// Option Maturity Day
		public const int DisplayInstruction = 63;		// Imbalance Only "I"
		public const int PegOffsetPrice = 64;
		public const int ParentTag = 65;
		public const int CplxOrderType = 66;

		public const int EffectiveTime = 68;

		public const int MsgNumber = 71;				// 1 Based ROM to Client Only
		public const int AlgoType = 73;

		public const int ClientEcho = 74;

		public const int NumFields = 76;
		public const int EndTime = 77;					// EndTime for Algo

		public const int OriginalPrice = 1000;
	}
}
