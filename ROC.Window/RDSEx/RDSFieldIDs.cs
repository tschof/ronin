using System;
using System.Collections.Generic;
using System.Text;

namespace RDSEx
{
	public static class TimeFormats
	{
		private static string[] _formats = new string[0];
		public static string[] Formats
		{
			get
			{
				if (_formats.Length == 0)
				{
					_formats = new string[3];
					_formats[0] = "yyyyMM";
					_formats[1] = "yyyyMMdd";
					_formats[2] = "yyyy-MM-dd";

					//_formats[0] = "HHmmss";
					//_formats[1] = "yyyyMMdd-HH:mm:ss";
					//_formats[2] = "yyyyMMdd";
					//_formats[3] = "yyyyMM";
					//_formats[3] = "yyyyMMdd-HH:mm:ss.fff";
				}
				return _formats;
			}
		}
	}

	public sealed class AccountTypes
	{
		public const string Stock = "CS";
		public const string Option = "OPT";
		public const string Future = "FUT";
	}

	public sealed class SideCode
	{
		public const int BUY = 1;
		public const int SELL = 2;
		public const int SHORT = 5;
		public const int SSE = 6;
	}

	public sealed class OrderTypeCode
	{
		public const int MARKET = 1;
		public const int LIMIT = 2;
		public const int STOP = 3;
		public const int STOPLIMIT = 4;
		public const int MOC = 5;			// Market On Close TIF must be DAY = 0
		public const int MOO = 12;			// Market On Open TIF must be OPG = 2
	}

	public sealed class TIFCode
	{
		public const int DAY = 0;
		public const int GTC = 1;
		public const int OPG = 2;			// On Open
		public const int IOC = 3;
		public const int FOK = 4;
		public const int GTX = 5;
		public const int GTD = 6;
		public const int ONCLOSE = 7;
	}

	public sealed class TraderFieldIDs
	{
		public const int TradeFor = 1;
		public const int LocalAcAcrn = 2;
	}

	public sealed class AccountFieldIDs
	{
		public const int InterfaceID = 11;
		public const int Account = 12;
		public const int Type = 13;
		public const int ClearingAcID = 14;
		public const int LocalAcAcrn = 15;
		public const int FirmAcr = 16;
		public const int OmAcctType = 17;
		public const int Capacity = 18;
		public const int ClearingFirmID = 19;
		public const int OccAcr = 20;
		public const int HomeExchange = 21;
	}

	public sealed class DestinationFieldIDs
	{
		public const int DestID = 101;
		public const int SecType = 102;
		public const int ShortName = 103;
		public const int GiveupFirmID = 104;
		public const int CMTAFirmID = 105;
		public const int AlgoFlag = 106;
	}

	public sealed class TradedFieldIDs
	{
		// Link to a single position
		public const int positionKey = 101;

		public const int symbolDetail = 102;

		public const int symbolDisplay = 103;

		public sealed class ROC
		{
			// Link to a single position
			public const int positionKey = 401;

			public const int symbol = 402;

			public const int underlying = 403;
			public const int expDate = 404;
			public const int strikePrice = 405;
			public const int callPut = 406;

			public const int symbolDetail = 407;

			public const int secType = 408;

			public const int status = 409;
			public const int side = 410;

			public const int exchangeName = 411;
			public const int destID = 412;

			public const int account = 413;
			public const int trader = 414;
			public const int localAcct = 415;

			public const int openClose = 423;

			public const int omTag = 451;
			public const int omExecTag = 452;
			public const int execTime = 453;
			public const int execPrice = 454;
			public const int execQty = 455;

			public const int contractSize = 456;
			public const int tickSize = 457;
			public const int displayFactor = 458;

			public const int cplxOrderType = 459;

			public const int algoDestID = 460;
			public const int programTrade = 461;
			public const int maturityDay = 462;
		}

		public sealed class TPOS
		{
			public const int activeState = 501;
			public const int clearingAccount = 502;
			public const int clearingStatus = 503;
			public const int commission = 504;
			public const int contraBroker = 505;
			public const int contraFirm = 506;
			public const int exchange = 507;
			public const int expDate = 508;
			public const int extDescription = 509;
			public const int extTradeID = 510;
			public const int lastModDate = 511;
			public const int lastModReasonID = 512;
			public const int lastModTime = 513;
			public const int note = 514;
			public const int portfolio = 515;
			public const int price = 516;
			public const int putCall = 517;
			public const int qty = 518;
			public const int secType = 519;
			public const int settleDate = 520;
			public const int shortSaleFlag = 521;
			public const int strike = 522;
			public const int symbol = 523;
			public const int ticker = 524;
			public const int tradeDate = 525;
			public const int tradeGroup = 526;
			public const int tradeID = 527;
			public const int traderAcronym = 528;
			public const int tradeSource = 529;
			public const int tradeTime = 530;
			public const int undSecType = 531;
			public const int undSymbol = 532;
			public const int version = 533;
			public const int lastModeReason = 534;

			public const int contractSize = 535;
			public const int tickSize = 536;
			public const int displayFactor = 537;
			public const int isTPOS = 538;
		}
	}

	public sealed class OrderFieldIDs
	{
		// Link to a single position
		public const int positionKey = 10001;

		public const int symbolDetail = 10002;

		public const int hasTrades = 10003;

		public const int symbolDisplay = 10004;

		public sealed class ROC
		{
			public const int omTag = 40001;
			public const int tag = 40002;
			public const int omTime = 40003;
			public const int owner = 40004;
			public const int trader = 40005;
			public const int tradeFor = 40006;
			public const int destID = 40007;
			public const int localAcct = 40008;
			public const int firm = 40009;
			public const int clearingAccount = 40010;
			public const int clearingID = 40011;
			public const int side = 40012;
			public const int qty = 40013;
			public const int price = 40014;
			public const int avgPrice = 40015;
			public const int stopPrice = 40016;
			public const int secType = 40017;
			public const int symbol = 40018;
			public const int underlying = 40019;
			public const int strikePrice = 40020;
			public const int expDate = 40021;
			public const int callPut = 40022;
			public const int openClose = 40023;
			public const int ex_short_name = 40024;
			//public const int effectiveTime = 40025;
			public const int instructions = 40026;
			public const int maxFloor = 40027;
			//public const int multiplier = 40028;
			public const int orderType = 40029;
			public const int tif = 40030;
			public const int orderExpiresDate = 40031;
			public const int originalShares = 40032;
			public const int cumQty = 40033;
			public const int leavesQty = 40034;
			public const int text = 40035;
			public const int status = 40036;
			public const int cplxOrderType = 40037;

			public const int contractSize = 40038;
			public const int tickSize = 40039;
			public const int displayFactor = 40040;
			public const int mdsymbol = 40041;
			public const int execPrice = 40042;
			public const int securityDefinition = 40043;

			public const int originalPrice = 40044;
			public const int algoType = 40045;

			public const int parentTag = 40046;
			public const int clientEcho = 40047;

			public const int pegPrice = 40048;
			public const int algoDestID = 40049;
			public const int execInstruction = 40050;
			public const int programTrade = 40051;

			public const int endTime = 40052;
			public const int cmtaFirmID = 40053;
			public const int maturityDay = 40054;
		}
	}

	public sealed class PositionFieldIDs
	{
		// used to linke to multiple of orders
		public const int positionKey = 1001;

		public const int symbolDetail = 1002;

		public const int buyQty = 1003;
		public const int buyAvg = 1004;
		public const int sellQty = 1005;
		public const int sellAvg = 1006;

		public const int contractSize = 1008;

		public const int openPnL = 1009;
		public const int dayPnL = 1010;
		public const int totalPnL = 1011;

		public sealed class TPOS
		{
			public const int tradeGroup = 5001;
			public const int protfolio = 5002;
			public const int clearingAccount = 5003;
			public const int traderAcronym = 5004;
			public const int instrumentID = 5005;
			public const int validTraderKey = 5006;
			public const int asOfDate = 5007;
			public const int secType = 5008;
			public const int symbol = 5009;
			public const int undSymbol = 5010;
			public const int undSecType = 5011;
			public const int expDate = 5012;
			public const int strike = 5013;
			public const int putCall = 5014;
			public const int displayFormat = 5015;
			public const int multiplier = 5016;
			public const int avgCost = 5017;
			public const int realizedPnL = 5018;
			public const int openMark = 5019;
			public const int notionalAmount = 5020;
			public const int version = 5021;
			public const int openQty = 5022;
			public const int curQty = 5023;
			public const int isTPOS = 5024;
			public const int settlePrice = 5025;
			public const int securityStatus = 5026;
		}

		public sealed class MD
		{
			public const int tradePrice = 6001;
			public const int openPrice = 6002;
			public const int closePrice = 6003;
			public const int prevClosePrice = 6004;

			public const int netChange = 6005;
			public const int pctChange = 6006;
			public const int totalVolume = 6007;

			// Option Only
			public const int prevBidClose = 6008;
			public const int bidClose = 6009;
			public const int prevAskClose = 6010;
			public const int askClose = 6011;

			public const int bidPrice = 6012;
			public const int bidSize = 6013;

			public const int askPrice = 6014;
			public const int askSize = 6015;

			public const int tickSize = 6016;
			public const int displayFactor = 6017;
		}
	}

	public sealed class SecurityFieldIDs
	{
		public sealed class Security
		{
			public const int securityType = 1;
			public const int baseSymbol = 2;
			public const int ticker = 3;
			public const int exchange = 4;
			public const int strike = 5;
			public const int expirationDate = 6;
			public const int contractSize = 7;
			public const int putCall = 8;
			public const int tickSize = 9;
			public const int genericMDSymbol = 10;
			public const int dataSourceInfo = 11;
			public const int spcStlFlag = 12;
			public const int curCode = 13;
			public const int exerType = 14;
			public const int instrumentId = 15;
			public const int undInstrumentId = 16;
			public const int longName = 17;
			public const int obsoleteDateTime = 18;
			public const int undExpirationDate = 19;
			public const int fullSymbol = 20;
			public const int issueDate = 21;
			public const int inrstAccuralDate = 22;
			public const int firstCouponDate = 23;
			public const int cusip = 24;
			public const int coupon = 25;
			public const int redemption = 26;
			public const int notionalAmount = 27;
			public const int couponFreq = 28;
			public const int displayFormat = 29;
			public const int expYear = 30;
			public const int expMonth = 31;
			public const int expDay = 32;
			public const int decimalPrecision = 33;
			public const int conversion = 34;

			public const int rocSymbol = 99;
		}

		public sealed class Option
		{
			public const int stockSymbol = 101;
			public const int optionSymbol = 102;
			public const int opraSymbol = 103;
			public const int exchange = 104;
			public const int strikPrice = 105;
			public const int expirationDate = 106;
			public const int contractSize = 107;
			public const int putCall = 108;
			public const int spcStlFlag = 109;
			public const int tickSize = 110;
			public const int curCode = 111;
			public const int exerType = 112;
			public const int instrumentId = 113;
			public const int undInstrumentId = 114;
			public const int longName = 115;
			public const int genericMDSymbol = 116;
			public const int obsoleteDateTime = 117;
			public const int undExpirationDate = 118;
			public const int roninSymbol = 119;

			//public const int opraSymbol = 101;
			//public const int exchange = 102;
			//public const int expirationDate = 103;
			//public const int putCall = 104;
			//public const int tickSize = 105;
			//public const int contractSize = 106;
			//public const int spcStlFlag = 107;
			//public const int strikPrice = 108;

			public const int symbolDetail = 120;
			public const int underlying = 121;

			public const int dataSource = 122;
			public const int secType = 123;
		}

		public sealed class SSFuture
		{
			public const int fullSymbol = 201;
			public const int exchange = 202;
			public const int expirationDate = 203;
			public const int contractSize = 204;
			public const int tickSize = 205;
			public const int curCode = 206;
			public const int longName = 207;
			public const int dataSource = 208;
		}
	}
}
