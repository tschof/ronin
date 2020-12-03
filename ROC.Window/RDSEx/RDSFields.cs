using System;
using System.Collections.Generic;
using System.Text;

namespace RDSEx
{
	public sealed class AccountTypes
	{
		public const string Stock = "CS";
		public const string Option = "OPT";
		public const string Future = "FUT";
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
	}

	public sealed class TradedFieldIDs
	{
		public const int isRom = 100;

		public const int price = 101;
		public const int qty = 102;
		public const int time = 103;
		public const int tradeValue = 104;

		public sealed class ROM
		{
			public const int omTag = 401;
			public const int omExecTag = 402;
			public const int execTime = 403;
			public const int execPrice = 404;
			public const int execQty = 405;
		}

		public sealed class TPOS
		{
			public const int tradeID = 501;
			public const int tradeGroup = 502;
			public const int protfolio = 503;
			public const int traderAcronym = 504;
			public const int clearingAccount = 505;
			public const int version = 506;
			public const int secType = 507;
			public const int symbol = 508;
			public const int undSymbol = 509;
			public const int undSecType = 510;
			public const int expDate = 511;
			public const int strike = 512;
			public const int putCall = 513;
			public const int tradeDate = 514;
			public const int tradeTime = 515;
			public const int extTradeID = 516;
			public const int lastModDate = 517;
			public const int lastModReason = 518;
			public const int lastModeReasonID = 519;
			public const int qty = 520;
			public const int price = 521;
			public const int commission = 522;
		}
	}

	public sealed class PositionFieldIDs
	{
		public const int isRom = 1000;

		// used to linke to multiple of orders
		public const int positionKey = 1001;

		public const int symbol = 1002;

		public const int underlying = 1003;
		public const int expDate = 1004;
		public const int strikePrice = 1005;
		public const int callPut = 1006;

		public const int symbolDetail = 1007;

		public const int secType = 10008;
		
		public const int lastTradedPrice = 1009;
		public const int netChange = 1010;
		public const int pctChange = 1011;
		public const int totalVolume = 1012;

		public const int account = 1013;
		public const int trader = 1014;

		public const int openQty = 1015;
		public const int openAvgPrice = 1016;
		public const int buyQty = 1017;
		public const int buyAvgPrice = 1018;
		public const int sellQty = 1019;
		public const int sellAvgPrice = 1020;
		public const int currentQty = 1021;
		public const int multiplier = 1022;

		public const int openPL = 1023;
		public const int dayPL = 1024;
		public const int totalPL = 1025;

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
			public const int extDate = 5012;
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
		}
	}

	public sealed class OrderFieldIDs
	{
		public const int isRoc = 10000;

		// Link to a single position
		public const int positionKey = 10001;

		public const int symbol = 10002;

		public const int underlying = 10003;
		public const int expDate = 10004;
		public const int strikePrice = 10005;
		public const int callPut = 10006;

		public const int symbolDetail = 10007;

		public const int secType = 10008;

		public const int status = 10009;
		public const int side = 10010;

		public const int exchangeName = 10011;
		public const int destID = 10012;

		public const int account = 1013;
		public const int trader = 1014;

		public const int duration = 10015;
		public const int orderType = 10016;

		public const int qty = 10017;
		public const int leaveQty = 10018;
		public const int filledQty = 10019;

		public const int price = 10020;
		public const int avgPrice = 10021;
		public const int stopPrice = 10022;

		public const int omTime = 10023;
		public const int omTag = 10024;

		public sealed class MSG
		{
			public const int omTag = 30001;
		}

		public sealed class ROM
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
			public const int effectiveTime = 40025;
			public const int instructions = 40026;
			public const int maxFloor = 40027;
			public const int multiplier = 40028;
			public const int orderType = 40029;
			public const int tif = 40030;
			public const int orderExpiresDate = 40031;
			public const int originalShares = 40032;
			public const int cumQty = 40033;
			public const int leavesQty = 40034;
			public const int text = 40035;
			public const int status = 40036;
		}
	}
}
