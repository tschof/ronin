using System;
using System.Collections.Generic;
using System.Text;

namespace MarketDataEx
{
	public static class WombatFieldIDs
	{
		public const int IssueSymbol = 305;

		public const int uSubscriptionSoruce = -101;
		public const int uSubscriptionSymbol = -102;
		public const int uSymbol = -103;
		public const int uParticipentSymbol = -104;
		public const int uParticipentName = -105;

		public const int InstrumentType = 298;
		public const int LineTime = 1174;

		public const int uOnMsgTime = -501;
		public const int uDecodedTime = -502;
		public const int uServerSentTime = -503;
		public const int uClientRecivedTime = -504;
		public const int uDisplayTime = -505;

		public const int NetChange = 341;
		public const int PctChange = 431;
		public const int TotalVolume = 475;
		public const int HighPrice = 278;
		public const int LowPrice = 327;
		public const int OpenPrice = 407;
		public const int ClosePrice = 242;
		public const int PrevClosePrice = 436;
		public const int PrevCloseDate = 435;
		public const int TotalValue = 474;
		public const int Vwap = 494;

		public const int BidPrice = 237;
		public const int BidSize = 238;
		public const int BidLow = 234;
		public const int BidHigh = 233;
		public const int BidPartId = 236;

		public const int AskPrice = 109;
		public const int AskSize = 110;
		public const int AskLow = 106;
		public const int AskHigh = 105;
		public const int AskPartId = 108;

		public const int QuoteDate = 519;
		public const int QuoteTime = 442;
		public const int QuoteCount = 1034;

		public const int TradePrice = 481;
		public const int TradeDate = 476;
		public const int TradeTime = 485;
		public const int TradeTick = 484;
		public const int TradeVolume = 488;
		public const int TradePartId = 480;
		public const int TradeCount = 901;

		// Stock L1
		public const int PrimExch = 645;

		// Future L1
		public const int ProductInd = 1520;
		public const int DisplayConversionFactor = 1518;
		public const int OrderMinQty = 513;
		public const int OrderMaxQty = 514;
		public const int MaxPriceVariation = 3341;
		public const int Maturity = 1525;
		public const int BookDepth = 2276;
		public const int TicketIncrement = 515;
		public const int InstrumentGroup = 510;
		public const int MinPriceIncrementAmount = 3345;
		public const int MainFraction = 3342;
		public const int HightLimit = 277;
		public const int LowLimit = 326;
		public const int SettlePrice = 458;
		public const int SettleDate = 456;
		public const int LastBestAskPrice = 4026;
		public const int LastBestBidPrice = 4025;
		public const int ExchTotalVol = 2516;

		// Option L1
		public const int PutCall = 439;
		public const int ExpirationDate = 262;
		public const int StrikePrice = 469;
		public const int Openlongerest = 406;
		public const int PrevBidClose = 433;
		public const int PrevBidCloseDate = 434;
		public const int BidClose = 504;
		public const int BidCloseDate = 505;
		public const int PrevAskClose = 506;
		public const int PrevAskCloseDate = 507;
		public const int AskClose = 502;
		public const int AskClosedate = 503;

		public const int Book1BidSize = -5001;
		public const int Book1BidPrice = -5101;

		public const int Book1AskSize = -6001;
		public const int Book1AskPrice = -6101;
	}

	public enum FieldTypes
	{
		isDouble,
		isString,
		isLong,
		isDateTime,
	}

	public static class MDFieldDefinitions
	{
		private static Dictionary<int, FieldTypes> _fTypes;
		public static Dictionary<int, FieldTypes> FTypes
		{
			get
			{
				if (_fTypes == null)
				{
					_fTypes = new Dictionary<int, FieldTypes>();
					_fTypes.Add(WombatFieldIDs.IssueSymbol, FieldTypes.isString);

					_fTypes.Add(WombatFieldIDs.uSubscriptionSoruce, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.uSubscriptionSymbol, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.uSymbol, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.uParticipentSymbol, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.uParticipentName, FieldTypes.isString);

					_fTypes.Add(WombatFieldIDs.InstrumentType, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.LineTime, FieldTypes.isString);

					_fTypes.Add(WombatFieldIDs.uOnMsgTime, FieldTypes.isDateTime);
					_fTypes.Add(WombatFieldIDs.uDecodedTime, FieldTypes.isDateTime);
					_fTypes.Add(WombatFieldIDs.uServerSentTime, FieldTypes.isDateTime);
					_fTypes.Add(WombatFieldIDs.uClientRecivedTime, FieldTypes.isDateTime);
					_fTypes.Add(WombatFieldIDs.uDisplayTime, FieldTypes.isDateTime);

					_fTypes.Add(WombatFieldIDs.NetChange, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.PctChange, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.TotalVolume, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.HighPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.LowPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.OpenPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.ClosePrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.PrevClosePrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.PrevCloseDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.TotalValue, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Vwap, FieldTypes.isDouble);

					_fTypes.Add(WombatFieldIDs.BidPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.BidSize, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.BidLow, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.BidHigh, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.BidPartId, FieldTypes.isString);

					_fTypes.Add(WombatFieldIDs.AskPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.AskSize, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.AskLow, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.AskHigh, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.AskPartId, FieldTypes.isString);

					_fTypes.Add(WombatFieldIDs.QuoteDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.QuoteTime, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.QuoteCount, FieldTypes.isLong);

					_fTypes.Add(WombatFieldIDs.TradePrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.TradeDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.TradeTime, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.TradeTick, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.TradeVolume, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.TradePartId, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.TradeCount, FieldTypes.isLong);

					// Stock L1
					_fTypes.Add(WombatFieldIDs.PrimExch, FieldTypes.isString);

					// Future L1
					_fTypes.Add(WombatFieldIDs.ProductInd, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.DisplayConversionFactor, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.OrderMinQty, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.OrderMaxQty, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.MaxPriceVariation, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.Maturity, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.BookDepth, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.TicketIncrement, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.InstrumentGroup, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.MinPriceIncrementAmount, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.MainFraction, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.HightLimit, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.LowLimit, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.SettlePrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.SettleDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.LastBestAskPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.LastBestBidPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.ExchTotalVol, FieldTypes.isLong);

					// Option L1
					_fTypes.Add(WombatFieldIDs.PutCall, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.ExpirationDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.StrikePrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Openlongerest, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.PrevBidClose, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.PrevBidCloseDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.BidClose, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.BidCloseDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.PrevAskClose, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.PrevAskCloseDate, FieldTypes.isString);
					_fTypes.Add(WombatFieldIDs.AskClose, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.AskClosedate, FieldTypes.isString);

					// Book
					// Book
					_fTypes.Add(WombatFieldIDs.Book1BidSize, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 1, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 2, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 3, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 4, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 5, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 6, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 7, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 8, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 9, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 10, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 11, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 12, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 13, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1BidSize - 14, FieldTypes.isLong);

					_fTypes.Add(WombatFieldIDs.Book1BidPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 1, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 2, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 3, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 4, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 5, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 6, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 7, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 8, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 9, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 10, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 11, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 12, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 13, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1BidPrice - 14, FieldTypes.isDouble);

					_fTypes.Add(WombatFieldIDs.Book1AskSize, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 1, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 2, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 3, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 4, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 5, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 6, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 7, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 8, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 9, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 10, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 11, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 12, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 13, FieldTypes.isLong);
					_fTypes.Add(WombatFieldIDs.Book1AskSize - 14, FieldTypes.isLong);

					_fTypes.Add(WombatFieldIDs.Book1AskPrice, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 1, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 2, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 3, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 4, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 5, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 6, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 7, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 8, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 9, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 10, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 11, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 12, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 13, FieldTypes.isDouble);
					_fTypes.Add(WombatFieldIDs.Book1AskPrice - 14, FieldTypes.isDouble);
				}
				return _fTypes;
			}
		}

		private static Dictionary<int, string> _fNames;
		public static Dictionary<int, string> FNames
		{
			get
			{
				if (_fNames == null)
				{
					_fNames = new Dictionary<int, string>();
					_fNames.Add(WombatFieldIDs.IssueSymbol, "IssueSymbol");

					_fNames.Add(WombatFieldIDs.uSubscriptionSoruce, "uSubscriptionSoruce");
					_fNames.Add(WombatFieldIDs.uSubscriptionSymbol, "uSubscriptionSymbol");
					_fNames.Add(WombatFieldIDs.uSymbol, "uSymbol");
					_fNames.Add(WombatFieldIDs.uParticipentSymbol, "uParticipentSymbol");
					_fNames.Add(WombatFieldIDs.uParticipentName, "uParticipentName");

					_fNames.Add(WombatFieldIDs.InstrumentType, "InstrumentType");
					_fNames.Add(WombatFieldIDs.LineTime, "LineTime");

					_fNames.Add(WombatFieldIDs.uOnMsgTime, "uOnMsgTime");
					_fNames.Add(WombatFieldIDs.uDecodedTime, "uDecodedTime");
					_fNames.Add(WombatFieldIDs.uServerSentTime, "uServerSentTime");
					_fNames.Add(WombatFieldIDs.uClientRecivedTime, "uClientRecivedTime");
					_fNames.Add(WombatFieldIDs.uDisplayTime, "uDisplayTime");

					_fNames.Add(WombatFieldIDs.NetChange, "NetChange");
					_fNames.Add(WombatFieldIDs.PctChange, "PctChange");
					_fNames.Add(WombatFieldIDs.TotalVolume, "TotalVolume");
					_fNames.Add(WombatFieldIDs.HighPrice, "HighPrice");
					_fNames.Add(WombatFieldIDs.LowPrice, "LowPrice");
					_fNames.Add(WombatFieldIDs.OpenPrice, "OpenPrice");
					_fNames.Add(WombatFieldIDs.ClosePrice, "ClosePrice");
					_fNames.Add(WombatFieldIDs.PrevClosePrice, "PrevClosePrice");
					_fNames.Add(WombatFieldIDs.PrevCloseDate, "PrevCloseDate");
					_fNames.Add(WombatFieldIDs.TotalValue, "TotalValue");
					_fNames.Add(WombatFieldIDs.Vwap, "Vwap");

					_fNames.Add(WombatFieldIDs.BidPrice, "BidPrice");
					_fNames.Add(WombatFieldIDs.BidSize, "BidSize");
					_fNames.Add(WombatFieldIDs.BidLow, "BidLow");
					_fNames.Add(WombatFieldIDs.BidHigh, "BidHigh");
					_fNames.Add(WombatFieldIDs.BidPartId, "BidPartId");

					_fNames.Add(WombatFieldIDs.AskPrice, "AskPrice");
					_fNames.Add(WombatFieldIDs.AskSize, "AskSize");
					_fNames.Add(WombatFieldIDs.AskLow, "AskLow");
					_fNames.Add(WombatFieldIDs.AskHigh, "AskHigh");
					_fNames.Add(WombatFieldIDs.AskPartId, "AskPartId");

					_fNames.Add(WombatFieldIDs.QuoteDate, "QuoteDate");
					_fNames.Add(WombatFieldIDs.QuoteTime, "QuoteTime");
					_fNames.Add(WombatFieldIDs.QuoteCount, "QuoteCount");

					_fNames.Add(WombatFieldIDs.TradePrice, "TradePrice");
					_fNames.Add(WombatFieldIDs.TradeDate, "TradeDate");
					_fNames.Add(WombatFieldIDs.TradeTime, "TradeTime");
					_fNames.Add(WombatFieldIDs.TradeTick, "TradeTick");
					_fNames.Add(WombatFieldIDs.TradeVolume, "TradeVolume");
					_fNames.Add(WombatFieldIDs.TradePartId, "TradePartId");
					_fNames.Add(WombatFieldIDs.TradeCount, "TradeCount");

					// Stock L1
					_fNames.Add(WombatFieldIDs.PrimExch, "PrimExch");

					// Future L1
					_fNames.Add(WombatFieldIDs.ProductInd, "ProductInd");
					_fNames.Add(WombatFieldIDs.DisplayConversionFactor, "DisplayConversionFactor");
					_fNames.Add(WombatFieldIDs.OrderMinQty, "OrderMinQty");
					_fNames.Add(WombatFieldIDs.OrderMaxQty, "OrderMaxQty");
					_fNames.Add(WombatFieldIDs.MaxPriceVariation, "MaxPriceVariation");
					_fNames.Add(WombatFieldIDs.Maturity, "Maturity");
					_fNames.Add(WombatFieldIDs.BookDepth, "BookDepth");
					_fNames.Add(WombatFieldIDs.TicketIncrement, "TicketIncrement");
					_fNames.Add(WombatFieldIDs.InstrumentGroup, "InstrumentGroup");
					_fNames.Add(WombatFieldIDs.MinPriceIncrementAmount, "MinPriceIncrementAmount");
					_fNames.Add(WombatFieldIDs.MainFraction, "MainFraction");
					_fNames.Add(WombatFieldIDs.HightLimit, "HightLimit");
					_fNames.Add(WombatFieldIDs.LowLimit, "LowLimit");
					_fNames.Add(WombatFieldIDs.SettlePrice, "SettlePrice");
					_fNames.Add(WombatFieldIDs.SettleDate, "SettleDate");
					_fNames.Add(WombatFieldIDs.LastBestAskPrice, "LastBestAskPrice");
					_fNames.Add(WombatFieldIDs.LastBestBidPrice, "LastBestBidPrice");
					_fNames.Add(WombatFieldIDs.ExchTotalVol, "ExchTotalVol");

					// Option L1
					_fNames.Add(WombatFieldIDs.PutCall, "PutCall");
					_fNames.Add(WombatFieldIDs.ExpirationDate, "ExpirationDate");
					_fNames.Add(WombatFieldIDs.StrikePrice, "StrikePrice");
					_fNames.Add(WombatFieldIDs.Openlongerest, "Openlongerest");
					_fNames.Add(WombatFieldIDs.PrevBidClose, "PrevBidClose");
					_fNames.Add(WombatFieldIDs.PrevBidCloseDate, "PrevBidCloseDate");
					_fNames.Add(WombatFieldIDs.BidClose, "BidClose");
					_fNames.Add(WombatFieldIDs.BidCloseDate, "BidCloseDate");
					_fNames.Add(WombatFieldIDs.PrevAskClose, "PrevAskClose");
					_fNames.Add(WombatFieldIDs.PrevAskCloseDate, "PrevAskCloseDate");
					_fNames.Add(WombatFieldIDs.AskClose, "AskClose");
					_fNames.Add(WombatFieldIDs.AskClosedate, "AskClosedate");

					// Book
					_fNames.Add(WombatFieldIDs.Book1BidSize, "Book1BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 1, "Book2BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 2, "Book3BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 3, "Book4BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 4, "Book5BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 5, "Book6BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 6, "Book7BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 7, "Book8BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 8, "Book9BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 9, "Book10BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 10, "Book11BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 11, "Book12BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 12, "Book13BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 13, "Book14BidSize");
					_fNames.Add(WombatFieldIDs.Book1BidSize - 14, "Book15BidSize");

					_fNames.Add(WombatFieldIDs.Book1BidPrice, "Book1BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 1, "Book2BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 2, "Book3BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 3, "Book4BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 4, "Book5BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 5, "Book6BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 6, "Book7BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 7, "Book8BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 8, "Book9BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 9, "Book10BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 10, "Book11BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 11, "Book12BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 12, "Book13BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 13, "Book14BidPrice");
					_fNames.Add(WombatFieldIDs.Book1BidPrice - 14, "Book15BidPrice");

					_fNames.Add(WombatFieldIDs.Book1AskSize, "Book1AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 1, "Book2AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 2, "Book3AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 3, "Book4AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 4, "Book5AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 5, "Book6AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 6, "Book7AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 7, "Book8AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 8, "Book9AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 9, "Book10AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 10, "Book11AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 11, "Book12AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 12, "Book13AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 13, "Book14AskSize");
					_fNames.Add(WombatFieldIDs.Book1AskSize - 14, "Book15AskSize");
					
					_fNames.Add(WombatFieldIDs.Book1AskPrice, "Book1AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 1, "Book2AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 2, "Book3AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 3, "Book4AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 4, "Book5AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 5, "Book6AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 6, "Book7AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 7, "Book8AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 8, "Book9AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 9, "Book10AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 10, "Book11AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 11, "Book12AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 12, "Book13AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 13, "Book14AskPrice");
					_fNames.Add(WombatFieldIDs.Book1AskPrice - 14, "Book15AskPrice");
				}
				return _fNames;
			}
		}

		private static Dictionary<string, int> _fIDs;
		public static Dictionary<string, int> FIDs
		{
			get
			{
				if (_fIDs == null)
				{
					_fIDs = new Dictionary<string, int>();
					foreach (int key in FNames.Keys)
					{
						_fIDs.Add(FNames[key], key);
					}
				}
				return _fIDs;
			}
		}
	}
}
