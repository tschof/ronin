using System;
using System.Collections.Generic;

namespace ROMEx
{
	public sealed class CSVFieldIDNews
	{
		public sealed class MessageTypes
		{
			public const string HeartBeat = "Q";
			public const string LoggedIn = "L";
			public const string LoginFailed = "K";
			public const string Alert = "@";
			public const string InvalidField = "I";
			public const string WorkingOrder = "E";
			public const string OrderStatus = "S";
		}

		public sealed class CommandTypes
		{
			public const string Login = "L";
			public const string EnterOrder = "E";
			public const string CancelSingle = "C";
			public const string CancelMultiple = "c";
			public const string Replace = "R";
		}

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

		public sealed class SideCodes
		{
			public const int Buy = 1;
			public const int Sell = 2;
			public const int Short = 5;
			public const int SSE = 6;

			private static Dictionary<int, string> _descriptions;
			public static Dictionary<int, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<int, string>();
						_descriptions.Add(Buy, "Buy");
						_descriptions.Add(Sell, "Sell");
						_descriptions.Add(Short, "Sell Short");
						_descriptions.Add(SSE, "SSE");
					}

					return _descriptions;
				}
			}
		}

		public sealed class OrderTypes
		{
			public const int Market = 1;
			public const int Limit = 2;
			public const int Stop = 3;
			public const int StopLimit = 4;
			public const int MarketOnClose = 5;
			public const int WithOrWithout = 6;
			public const int LimitOrBetter = 7;
			public const int LimitWithOrWithout = 8;
			public const int OnBasis = 9;
			public const int OnClose = 10;
			public const int LimitOnClose = 11;
			public const int MarketAtOpen = 12;
			public const int VWAP = 14;

			// K = Marketable Limit order on CME/Globex

			private static Dictionary<int, string> _descriptions;
			public static Dictionary<int, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<int, string>();
						_descriptions.Add(Market, "Market");
						_descriptions.Add(Limit, "Limit");
						_descriptions.Add(Stop, "Stop");
						_descriptions.Add(StopLimit, "StopLimit");
						_descriptions.Add(MarketOnClose, "MarketOnClose");
						_descriptions.Add(WithOrWithout, "WithOrWithout");
						_descriptions.Add(LimitOrBetter, "LimitOrBetter");
						_descriptions.Add(LimitWithOrWithout, "LimitWithOrWithout");
						_descriptions.Add(OnBasis, "OnBasis");
						_descriptions.Add(OnClose, "OnClose");
						_descriptions.Add(LimitOnClose, "LimitOnClose");
						_descriptions.Add(MarketAtOpen, "MarketAtOpen");
						_descriptions.Add(VWAP, "VWAP");
					}

					return _descriptions;
				}
			}
		}

		public sealed class TIFCodes
		{
			public const int Day = 0;
			public const int GTC = 1;		// Good Till Cancel
			public const int OPG = 2;
			public const int IOC = 3;		// Immediate or Cancel
			public const int FOK = 4;		// Fill or Kill
			public const int GTX = 5;
			public const int GTD = 6;		// Good Till Date
			public const int OCL = 7;		// On Close

			private static Dictionary<int, string> _descriptions;
			public static Dictionary<int, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<int, string>();
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
		}

		public sealed class StatusCodes
		{
			public const int New = 0;
			public const int PartiallyFilled = 1;
			public const int Filled = 2;
			public const int DoneForDay = 3;
			public const int Canceled = 4;
			public const int Replaced = 5;
			public const int PendingCancel = 6;
			public const int Stopped = 7;
			public const int Rejected = 8;
			public const int Calculated = 11;
			public const int Expired = 12;
			public const int Open = 13;
			public const int CancelRejected = 14;
			public const int Corrected = 15;
			public const int Busted = 16;
			public const int ReplaceRejected = 20;
			public const int ReplacePending = 21;
			public const int Exposed = 22;
			public const int DirectedThruIML = 23;
			public const int Directed = 24;
			public const int DirectedReject = 25;
			public const int ROMCancelPending = 26;
			public const int ROMReplacePending = 27;

			private static Dictionary<int, string> _descriptions;
			public static Dictionary<int, string> Descriptions
			{
				get
				{
					if (_descriptions == null)
					{
						_descriptions = new Dictionary<int, string>();
						_descriptions.Add(New, "New");
						_descriptions.Add(PartiallyFilled, "Partially Filled");
						_descriptions.Add(Filled, "Filled");
						_descriptions.Add(DoneForDay, "Done For Day");
						_descriptions.Add(Canceled, "Canceled");
						_descriptions.Add(Replaced, "Replaced");
						_descriptions.Add(PendingCancel, "Pending Cancel");
						_descriptions.Add(Stopped, "Stopped");
						_descriptions.Add(Rejected, "Rejected");
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
					}

					return _descriptions;
				}
			}
		}

		public sealed class OptionTypes
		{
			public const string Put = "P";
			public const string Call = "C";

			public const int Close = "0";
			public const int Open = "1";

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

		public sealed class SecutrityTypes
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

		public sealed class CMD
		{
			public sealed class L
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;			// Current time stamp in UTC (YYYMMDD-HHMMSS format)
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
				public const int CurrentTimeStamp = 1;			// Current time stamp in UTC (YYYMMDD-HHMMSS format)
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
				public const int Solicited = 21;				// Y/N
				public const int LocalAcct = 22;
				public const int Firm = 23;
				public const int TradeFor = 24;					// Set to the trader acronym who owns the account (usually same as Trader Field(#2))
				// 25
				public const int DBSymbol = 26;					// The symbol for inserting trades to Ronin TPOS database. Required if it is different from Symbol.
				// 27
				public const int Multiplier = 28;
				public const int ExecutionDestination = 29;		// Not Used any more?
				public const int ExpDate = 30;					// Future or Option YYYYMM
				public const int CallPut = 31;
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

				//public const int CplxOrderType = 66;			// Complex Order Type

				public const int BeginTime = 76;				// Begin Time for VWAP (default is time order is entered or 8:30al CT open) UTC format.
				public const int EndTime = 77;					// End Time for VWAP (default is time is 3:00pm CT market close) UTC format.
			}

			public sealed class CSingle
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

			public sealed class CMultiple
			{
				public const int Command = 0;
				public const int CurrentTimeStamp = 1;
				public const int Trader = 2;					// Trader acronym of the trader who entered the orders.  Can be different from the login name however it has to be defined in ROM database.
				public const int OnlyDayorder = 3;				// If this field is 1 only Day orders will be canceled
				public const int Exchange = 13;					// If this field is empty all your open orders will be canceled.
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
			}
		}
	}

	public static class CSVFieldIDs
	{
		public const int CSV_NUM_FIELDS = 76;
		public const int CSV_NUM_HIST_FIELDS = 16;

		// common to all messages
		public const int CSV_COMMAND = 0;
		public const int CSV_TIME = 1;
		public const int CSV_TRADER = 2;
		public const int CSV_TAG = 3;					// client assigned tag to id order
		public const int CSV_SIDE = 4;
		public const int CSV_SYMBOL = 5;
		public const int CSV_SHARES = 6;
		public const int CSV_PRICE = 7;
		public const int CSV_TYPE = 8;
		public const int CSV_TIF = 9;
		public const int CSV_CAPACITY = 10;				// principal or agent      'A'
		public const int CSV_CLEARINGID = 11;			// 4 digit nscc clearing code  0551
		public const int CSV_CLEARINGACCOUNT = 12;		// account number a above clearing firm  'ISI table'
		public const int CSV_EXCHANGE = 13;				// destination id
		public const int CSV_STATUS = 14;				// current status

		// just order entries
		public const int CSV_INSTRUCTIONS = 15;			// primarily used by instinet		 'customer Id in ISI table?'
		public const int CSV_PRICECHECK = 16;			// tells exchange whether or not to do price check  'N'
		public const int CSV_DISPLAY_SIZE = 17;				// display size		''
		public const int CSV_MINQTY = 18;				// min exec size	''
		public const int CSV_STOPPRICE = 19;
		public const int CSV_DISCRETIONARY = 20;		//  'N'
		public const int CSV_SOLICITED = 21;			//	'N'
		public const int CSV_LOCALACCOUNT = 22;			//  'ISI table'
		public const int CSV_FIRM = 23;					//  'ISI table'
		public const int CSV_TRADE_FOR = 24;			// trader the trade is being done for (i.e. CSV_TRADER doing trade on CSV_TRADE_FOR behalf) 'trader acronym'
		public const int CSV_QUERY_TAG = 25;			// tag used for communications between OM and the DB Server							  ''
		public const int CSV_DBSYMBOL = 26;				// Database symbol to be used by recorder

		// CSV_OWNER must not be overwritten (field 27)

		public const int CSV_MULTIPLIER = 28;			// multiplier for futures
		public const int CSV_EXDEST = 29;				// ExDestination
		public const int CSV_EXPDATE = 30;				// expiration date for ORDERS option
		public const int CSV_CALLPUT = 31;
		public const int CSV_STRIKEPRICE = 32;
		public const int CSV_UNDERLYING = 33;

		public const int CSV_FLOOR = 34;				// Max Floor Fix tag 111

		public const int CSV_SECURITY_TYPE = 35;		// option, future or equity
		public const int CSV_MM_ACCOUNT = 36;			// market maker account
		public const int CSV_MM_EXCHANGE = 37;			// market maker home exchange
		public const int CSV_OPENCLOSE = 38;			// Open a new position or close existing position, open is 1, close is 0
		public const int CSV_TRADING_SESSION = 39;		// pre-open or primary
		public const int CSV_GIVEUPACCOUNT = 40;
		public const int CSV_ORDERDEST = 41;			// Where to send order
		public const int CSV_ACCOUNT = 45;
		public const int CSV_CMTAACCOUNT = 46;
		public const int CSV_GTD_DATE = 47;
		public const int CSV_CUM_SHARES = 48;
		public const int CSV_LEAVE_SHARES = 49;
		public const int CSV_EXEC_TIME = 50;			// The time the transaction occurred(set by exchange)
		public const int CSV_AVG_PRICE = 51;
		public const int CSV_OM_TIME = 52;				// The time OM sends message to client
		public const int CSV_CLIENT_TIME = 53;			// The time client sends message to OM
		public const int CSV_EXEC_TAG = 54;				// Exchange(not our exchange server) execution tag
		public const int CSV_DBSYMBOL_NEW = 55;			// New field for DBSymbol to resolve the conflict with TEXT field
		public const int CSV_CURRENCY = 56;				// Currency

		public const int CSV_EXEC_INSTRUCTION = 57;		// Fix ExecInst
		public const int CSV_SHORT_LENDER = 58;
		public const int CSV_ACTTAG = 59;
		public const int CSV_NEW_TAG = 60;
		public const int CSV_ORDER_STATUS = 61;
		public const int CSV_TRADE_ID = 62;
		public const int CSV_REJECT_BY = 63;
		public const int CSV_PROGRAM_TRADING = 64;
		public const int CSV_CPLXORDER_TYPE = 66;
		public const int CSV_PARENT_TAG = 65;
		public const int CSV_ECHO = 67;
		public const int CSV_EFFECTIVE_TIME = 68;
		public const int CSV_BILLING_CODE = 69;
		public const int CSV_PARENT_STAFFORD_TAG = 70;

		// For Cross order
		public const int CSV_SETTLEMENT_TERMS = 71;
		public const int CSV_ITEMNUMBER = 72;
		public const int CSV_MGF = 73;
		public const int CSV_REGULATION_ID = 74;
		public const int CSV_PASSTHROUGH = 75;

		// just status messages
		public const int CSV_ORDER_TAG = 15;			// tag assigned by lowest client
		public const int CSV_EXCHANGE_TAG = 16;			// tag assigned by exchange
		public const int CSV_STAFFORD_TAG = 17;			// tag assigned by order manager
		public const int CSV_CANCEL_TAG = 18;			// tag assigned to cancel by server connected to exchange
		public const int CSV_EXEC_CONFIRM = 20;			// confirm from exchane
		public const int CSV_REFERENCE = 21;			// execution lookup tag from exchange
		public const int CSV_REPORT = 22;				// do act report?
		public const int CSV_CONTRA = 23;				// contra
		public const int CSV_TEXT = 26;					// reject or status text returned by exchange
		public const int CSV_OWNER = 27;				// owner of the message
		public const int CSV_OMEXECUTION_TAG = 28;
		public const int CSV_EXEC_SHARES = 42;			// Executed shares
		public const int CSV_LAST_SHARES = 42;			// Filled shares in last transaction(when Status is Partially Filled or Filled)
		public const int CSV_ORIGINAL_SHARES = 43;		// Total shares of original order (when Status is Partially Filled or Filled)
		public const int CSV_EXEC_PRICE = 44;			// Execution price
		public const int CSV_LAST_PRICE = 44;
		public const int CSV_RPT_FIRM = 45;				// Reporting firm  //WXU, C1, 09/19/05

		public const int CSV_OWNERCANCEL = 28;			// Indicate owner canceled this order

		// login messages
		public const int CSV_CANCEL_BOOL = 3;
		public const int CSV_CANCEL_MIN = 4;
		public const int CSV_APP = 5;
		public const int CSV_APP_VERSION = 6;
		public const int CSV_PASSWORD = 7;

		// Modify Fields
		public const int CSV_MOD_MAXSHARES = 3;
		public const int CSV_MOD_BUYINGPOWER = 4;
		public const int CSV_MOD_MAXORDERS = 5;
		public const int CSV_MOD_MAXDOLLARS = 6;
		public const int CSV_MOD_MAXSHARESORDER = 7;

		// Email Fields
		public const int CSV_EMAIL_MESSAGE = 3;
		public const int CSV_EMAIL_LIST = 4;

		// Option Fields
		public const int CSV_CONTRA_MEMBER_ACR = 15;
		public const int CSV_CONTRA_CLEARING_ACCOUNT = 16;
		public const int CSV_EXPIRATION_DATE = 17;
		public const int CSV_STRIKE = 18;
		public const int CSV_CALL_PUT = 19;
		public const int CSV_IS_RESEND = 20;

		// Error Fields
		public const int CSV_ERROR1 = 15;
		public const int CSV_ERROR2 = 16;
		public const int CSV_ERROR3 = 17;

		// Historical Query
		public const int CSV_HIST_TYPE = 3;
		public const int CSV_HIST_SYMBOL = 4;
		public const int CSV_HIST_SECONDS = 5;
		public const int CSV_HIST_BIDTIMES = 6;
		public const int CSV_HIST_BIDPRICES = 7;
		public const int CSV_HIST_BIDSIZES = 8;
		public const int CSV_HIST_ASKTIMES = 9;
		public const int CSV_HIST_ASKPRICES = 10;
		public const int CSV_HIST_ASKSIZES = 11;
		public const int CSV_HIST_LASTTIMES = 12;
		public const int CSV_HIST_LASTPRICES = 13;
		public const int CSV_HIST_LASTSIZES = 14;
		public const int CSV_HIST_REPLYSUBJECT = 15;

		// Seperators
		public const char MAIL_LIST_SEPARATOR = ':';
		public const char HIST_LIST_SEPARATOR = '!';
		public const char SPREAD_ORDER_SEPARATOR = '#';

		// Fleak Fields
		public const int CSV_COMMAND_FLEAK = 2;
		public const int CSV_EXCHANGE_FLEAK = 4;
		public const int CSV_ACTION_FLEAK = 5;
		public const int CSV_TRADER_FLEAK = 3;
		public const int CSV_EXCEPTIONSYMBOL_FLEAK = 3;

		public enum FLEAK_COMMAND
		{
			QUERY_EXCHANGE_STATUS,
			MODIFY_EXCHANGE_ACCESS,
			QUERY_USER_STATUS,
			DISCONNECT_USER,
			RELOAD_DATABASE,
			EXEMPT_TRADER_LIMITS,
			ADD_EXCEPTION_SYMBOLS,
			RELOAD_ORDER_DESTINATIONS
		};
	}
}
