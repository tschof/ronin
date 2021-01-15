using System;

using MarketData;
using CSVEx;

namespace ROC.Tickets.AutoSpread
{
	internal sealed class AutoSpreadItem
	{
		internal AutoSpreadItem(int legNumber, int legBookDepthLimit)
		{
			LegNumber = legNumber;
			LegBookDepthLimit = legBookDepthLimit;
		}

		internal int LegBookDepthLimit = 3; // _legBookDepthLimit
		internal int LegNumber { get; private set; } = 0;
		internal int RowIndex = 0;
		internal string Exchange = "";
		internal string LocalAcctAcrn = "";
		internal string TradeFor = "";
		internal string ShortLender = "";
		internal double Factor = 1;
		internal decimal FactorDec => Convert.ToDecimal(Factor);
		internal double UserRatio = 0;
		internal long TickLimit = 5;
		internal bool IsLean = false;
		internal bool IsNewL1 = true;
		internal bool IsNewBook = true;
		internal bool HasBooks => !MarketData.Bids.Empty && !MarketData.Asks.Empty;
		internal double? BuyStopPrice;
		internal string BidMarketPrice;
		internal double? SellLimitPrice;
		internal double? BuyLimitPrice;
		internal string BuyLimitPriceSTR => toText(BuyLimitPrice);
		internal string BuyStopPriceSTR => toText(BuyStopPrice);
		internal string SellLimitPriceSTR => toText(SellLimitPrice);
		internal double? SellStopPrice;
		internal string SellStopPriceSTR => toText(SellStopPrice);
		internal string AskMarketPrice = "";
		internal double TradedPrice => MarketData.TryGetField(Book.FieldEnum.TradePrice, out double value) ? value : 0;
		internal long TradedVolume => MarketData.TryGetField(Book.FieldEnum.TradeVolume, out long value) ? value : 0;
		internal double BidPrice => MarketData.TryGetField(Book.FieldEnum.BidPrice, out double value) ? value : 0;
		internal string BidPriceSTR => toText(BidPrice);
		internal long BidSize => MarketData.TryGetField(Book.FieldEnum.BidSize, out long value) ? value : 0;
		internal double AskPrice => MarketData.TryGetField(Book.FieldEnum.AskPrice, out double value) ? value : 0;
		internal string AskPriceSTR => toText(AskPrice);
		internal long AskSize => MarketData.TryGetField(Book.FieldEnum.AskSize, out long value) ? value : 0;
		internal string TradedPriceSTR => toText(TradedPrice);
		internal bool HasOriginInfo => (Exchange != "") && (LocalAcctAcrn != "") && (TradeFor != "");

		private RDSEx.IMSecurityBase _secInfo = null;
		internal RDSEx.IMSecurityBase SecInfo {
			get {
				if (_secInfo == null)
					_secInfo = new RDSEx.IMSecurityBase();
				return _secInfo;
			}
		}

		private string _symbolDetail = "";
		internal string SymbolDetail {
			get {
				return _symbolDetail;
			}
			set {
				if (_symbolDetail != value.ToUpper()) {
					_symbolDetail = value.ToUpper();
					if (_symbolDetail != "") {
						_secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(_symbolDetail);
					} else {
						_secInfo = new RDSEx.IMSecurityBase();
					}
				}
			}
		}

		private double _ratio = 0;
		internal double Ratio {
			get => (UserRatio != 0) ? UserRatio : _ratio;
			set => _ratio = value;
		}

		private Book _marketData;
		internal Book MarketData {
			get {
				if (_marketData == null) {
					_marketData = new Book();
				}
				return _marketData;
			}
			set {
				_marketData = value;
			}
		}

		internal void CheckLegBooks()
		{
			MarketData.SortLevels(LegBookDepthLimit, -1, CalculateSyntheticVolume);
		}

		private long CalculateSyntheticVolume(long volume)
		{
			switch (SecInfo.SecType) {
				case CSVFieldIDs.SecurityTypes.Equity:
					return Convert.ToInt64(Math.Floor(volume * 100 / Math.Abs(Factor)));
				default:
					return Convert.ToInt64(Math.Floor(volume / Math.Abs(Factor)));
			}
		}

		internal long? BuyLimitPriceFromBidPrice {
			get {
				if (BuyLimitPrice.HasValue && (SecInfo != null))
					return (long)(Math.Abs(BuyLimitPrice.Value - BidPrice) / SecInfo.TickSize);
				return null;
			}
		}

		internal long? SellLimitPriceFromAskPrice {
			get {
				if (SellLimitPrice.HasValue && (SecInfo != null))
					return (long)(Math.Abs(SellLimitPrice.Value - AskPrice) / SecInfo.TickSize);
				return null;
			}
		}

		private string toText(double value)
		{
			return (SecInfo == null) ? value.ToString() : value.ToString("F" + SecInfo.DecimalPlaces);
		}

		private string toText(double? value)
		{
			if (!value.HasValue)
				return "";
			if (SecInfo != null)
				return value.Value.ToString("F" + SecInfo.DecimalPlaces);
			return value.Value.ToString();
		}
	}
}
