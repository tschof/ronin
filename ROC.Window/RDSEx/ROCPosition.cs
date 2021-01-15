using System.Collections.Generic;
using DateTime = System.DateTime;

using CSVEx;
using Book = MarketData.Book;
using Price = Common.Price;

namespace RDSEx
{
	public class ROCPosition : ROCSecurity
	{
		public Price AskPrice { get; private set; }
		public long AskSize { get; private set; }
		public string AsOfDate { get; private set; }
		public double AvgCost { get; private set; }
		public Price BidPrice { get; private set; }
		public long BidSize { get; private set; }
		public double BuyAvg {
			get => _buyAvg;
			private set {
				_buyAvg = value;
				setDirty(DirtyEnum.DayRealizedPnL | DirtyEnum.DayUnrealizedPnL);
			}
		}
		public long BuyQty {
			get => _buyQty;
			private set {
				_buyQty = value;
				setDirty(DirtyEnum.CurrentQty | DirtyEnum.DayRealizedPnL | DirtyEnum.DayUnrealizedPnL);
			}
		}
		public override double ContractSize { // ROCPosition handles ContractSize differently than AssetShared.
			get => _contractSize;
			protected set {
				_contractSize = value;
				setDirty(DirtyEnum.DayRealizedPnL | DirtyEnum.DayUnrealizedPnL | DirtyEnum.MarketValue | DirtyEnum.OpenPnL);
			}
		}
		public long CurrentQty { 
			get => _currentQty;
			private set {
				_currentQty = value;
				setDirty(DirtyEnum.MarketValue);
			}
		}
		public double DayPnL => DayUnrealizedPnL + DayRealizedPnL;
		public double DayRealizedPnL {
			get {
				if (isDirty(DirtyEnum.DayRealizedPnL)) {
					if (BuyQty > SellQty) {
						// Day balance is LONG(BUY)
						_dayRealizedPnL = SellQty * (SellAvg - BuyAvg) * ContractSize;
					} else if (BuyQty < SellQty) {
						//Day balance is SHORT(SELL)
						_dayRealizedPnL = BuyQty * (SellAvg - BuyAvg) * ContractSize;
					} else if ((BuyQty > 0) && (SellQty > 0) && (BuyQty == SellQty)) {
						_dayRealizedPnL = BuyQty * (SellAvg - BuyAvg) * ContractSize;
					} else {
						_dayRealizedPnL = 0;
					}
					setClean(DirtyEnum.DayRealizedPnL);
				}
				return _dayRealizedPnL;
			}
		}
		public double DayUnrealizedPnL {
			get {
				if (isDirty(DirtyEnum.DayUnrealizedPnL)) {
					_dayUnrealizedPnL = 0;

					long dayQty = DayQty;
					if (LastTraded > 0) {
						if (dayQty > 0) {
							// Day balance is LONG(BUY)
							_dayUnrealizedPnL = dayQty * (LastTraded - BuyAvg) * ContractSize;
						} else if (dayQty < 0) {
							//Day balance is SHORT(SELL)
							_dayUnrealizedPnL = -dayQty * (SellAvg - LastTraded) * ContractSize;
						}
					}
					setClean(DirtyEnum.DayUnrealizedPnL);
				}
				return _dayUnrealizedPnL;
			}
		}
		public long DayQty => BuyQty - SellQty;
		public readonly string DisplayFormat;
		public bool HasOrders => (OmTags != null) && (OmTags.Count > 0);
		public readonly string InstrumentID;
		public double LastTraded { 
			get => _lastTraded;
			private set {
				_lastTraded = value;
				setDirty(DirtyEnum.DayUnrealizedPnL | DirtyEnum.MarketValue | DirtyEnum.OpenPnL);
			}
		}
		public double MarketValue {
			get {
				return LastTraded * CurrentQty * ContractSize;
			}
		}
		protected override string MaturityDay => "";
		public readonly double Multiplier;
		public double NetChange { get; private set; }
		public readonly int NotionalAmount;
		public List<string> OmTags { get; private set; } = null;
		public double OpenAvg { 
			get => _openAvg;
			private set {
				_openAvg = value;
				setDirty(DirtyEnum.OpenPnL);
			}
		}
		public double OpenMark { get; private set; }
		public double OpenPnL {
			get {
				if (isDirty(DirtyEnum.OpenPnL)) {
					_openPnL = 0;
					if ((LastTraded > 0) && !Common.Price.IsZeroValue(OpenAvg)) {
						if (OpenQty > 0) {
							// Open balance is LONG(BUY)
							_openPnL = OpenQty * (LastTraded - OpenAvg) * ContractSize;
						} else if (OpenQty < 0) {
							//Open balance is SHORT(SELL)
							_openPnL = -OpenQty * (OpenAvg - LastTraded) * ContractSize;
						}
					}
					setClean(DirtyEnum.OpenPnL);
				}
				return _openPnL;
			}
		}
		public long OpenQty { 
			get => _openQty;
			private set {
				_openQty = value;
				setDirty(DirtyEnum.CurrentQty | DirtyEnum.OpenPnL);
			}
		}
		public double PctChange { get; private set; }
		public readonly string Portfolio;
		public readonly string PutCall;
		public double RealizedPnL { get; private set; }
		public string SecurityStatus { get; private set; }
		public double SellAvg { 
			get => _sellAvg;
			private set {
				_sellAvg = value;
				setDirty(DirtyEnum.DayRealizedPnL | DirtyEnum.DayUnrealizedPnL);
			}
		}
		public long SellQty {
			get => _sellQty;
			private set {
				_sellQty = value;
				setDirty(DirtyEnum.CurrentQty | DirtyEnum.DayRealizedPnL | DirtyEnum.DayUnrealizedPnL);
			}
		}
		public double SettlePrice { get; private set; }
		public double TotalPnL => DayPnL + OpenPnL;
		public long TotalVolume { get; private set; }
		public readonly string TradeGroup;
		public readonly string TraderAcronym;
		public readonly string UndSecType;
		public readonly string ValidTraderKey;
		public readonly int Version;

		public ROCPosition(
			double contractSize, 
			long openQty, double 
			openAvg, long buyQty, 
			double buyAvg, 
			long sellQty, 
			double sellAvg, 
			double lastTraded, 
			double settlePrice, 
			ROCPosition incoming = null) : base()
		{
			BuyAvg = buyAvg;
			BuyQty = buyQty;
			ContractSize = contractSize;
			LastTraded = lastTraded;
			OpenAvg = openAvg;
			OpenQty = openQty;
			SellAvg = sellAvg;
			SellQty = sellQty;
			SettlePrice = settlePrice;

			if (Common.Price.IsZeroValue(LastTraded))
				LastTraded = OpenAvg;

			if (Common.Price.IsZeroValue(openAvg))
				OpenAvg = lastTraded;

			if (incoming != null) {
				if (incoming.OpenQty != 0)
					OpenQty += incoming.OpenQty;

				if (incoming.BuyQty > 0 && incoming.BuyAvg > 0) {
					double cost = (BuyQty * BuyAvg) + (incoming.BuyQty * incoming.BuyAvg);
					BuyQty += incoming.BuyQty;
					BuyAvg = cost / BuyQty;
				}

				if (incoming.SellQty > 0 && incoming.SellAvg > 0) {
					double cost = (SellQty * SellAvg) + (incoming.SellQty * incoming.SellAvg);
					SellQty += incoming.SellQty;
					SellAvg = cost / SellQty;
				}
			}
		}

		public ROCPosition(RDSEx.WEB.TposPosition pos) : base()
		{
			AsOfDate = pos.m_AsOfDate;
			AvgCost = pos.m_AvgCost;
			ClearingAcct = pos.m_ClearingAccount;
			CurrentQty = pos.m_CurQty;
			DisplayFormat = pos.m_DisplayFormat;
			ExpDate = pos.m_ExpDate;
			InstrumentID = pos.m_InstrumentID;
			Multiplier = pos.m_Multiplier;
			NotionalAmount = pos.m_NotionalAmount;
			OpenMark = pos.m_OpenMark;
			OpenQty = pos.m_OpenQty;
			Portfolio = pos.m_Portfolio;
			PutCall = pos.m_PutCall;
			RealizedPnL = pos.m_RealizedPnL;
			SecType = pos.m_SecType;
			Source = SourceEnum.TPOS;
			StrikePrice = pos.m_Strike;
			Symbol = pos.m_Symbol;
			TradeGroup = pos.m_TradeGroup;
			TraderAcronym = pos.m_TraderAcronym;
			UndSecType = pos.m_UndSecType;
			if (pos.m_UndSecType == CSVFieldIDs.SecurityTypes.Equity) {
				Underlying = pos.m_Symbol;
			} else {
				Underlying = pos.m_UndSymbol;
			}
			ValidTraderKey = pos.m_ValidTraderKey;
			Version = pos.m_Version;
		}

		public ROCPosition(ROCPosition other, SourceEnum source = SourceEnum.None) : base(other)
		{
			AsOfDate = other.AsOfDate;
			AvgCost = other.AvgCost;
			BuyAvg = other.BuyAvg;
			BuyQty = other.BuyQty;
			CurrentQty = other.CurrentQty;
			DisplayFormat = other.DisplayFormat;
			InstrumentID = other.InstrumentID;
			Multiplier = other.Multiplier;
			NotionalAmount = other.NotionalAmount;
			OmTags = other.OmTags == null ? null : new List<string>(other.OmTags);
			OpenAvg = other.OpenAvg;
			OpenMark = other.OpenMark;
			OpenQty = other.OpenQty;
			Portfolio = other.Portfolio;
			PutCall = other.PutCall;
			RealizedPnL = other.RealizedPnL;
			SellAvg = other.SellAvg;
			SellQty = other.SellQty;
			if (source != SourceEnum.None)
				Source = source;
			TradeGroup = other.TradeGroup;
			TraderAcronym = other.TraderAcronym;
			UndSecType = other.UndSecType;
			ValidTraderKey = other.ValidTraderKey;
			Version = other.Version;
			CallPut = other.CallPut;
			ClearingAcct = other.ClearingAcct;
			_dirty = other._dirty;
			ExpDate = other.ExpDate;
			SecType = other.SecType;
			Trader = other.Trader;
			Underlying = other.Underlying;
		}

		public ROCPosition(ROCTrade trade) : base()
		{
			CallPut = trade.CallPut;
			ExpDate = trade.ExpDate;
			SecType = trade.SecType;
			Source = trade.Source;
			StrikePrice = trade.StrikePrice;
			Symbol = trade.Symbol;
			Trader = trade.LocalAcct;
			Underlying = trade.Underlying;

			if (trade.Side == CSVEx.CSVFieldIDs.SideCodes.Buy) {
				BuyAvg = trade.ExecPrice.Value;
				BuyQty = trade.Qty;
			} else {
				SellAvg = trade.ExecPrice.Value;
				SellQty = trade.Qty;
			}

			if (trade.ClearingAcct.Length > 5) {
				ClearingAcct = trade.ClearingAcct.Substring(0, 5);
			} else {
				ClearingAcct = trade.ClearingAcct;
			}
			AddTrade(trade);
		}

		public void AddTrade(ROCTrade trade)
		{
			double cost;
			switch (trade.Side) {
				case CSVFieldIDs.SideCodes.Buy:
					cost = (BuyQty * BuyAvg) + (trade.Qty * trade.ExecPrice.Value);
					BuyQty += trade.Qty;
					BuyAvg = cost / BuyQty;
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
				default:
					cost = (SellQty * SellAvg) + (trade.Qty * trade.ExecPrice.Value);
					SellQty += trade.Qty;
					SellAvg = cost / SellQty;
					break;
			}
		}

		public void UpdateFromMarket(Book delta)
		{
			double price;
			long size;

			if (delta.TryGetField(Book.FieldEnum.NetChange, out price)) {
				NetChange = price;
			}
			if (delta.TryGetField(Book.FieldEnum.PctChange, out price)) {
				PctChange = price;
			}
			if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size)) {
				TotalVolume = size;
			}
			if (delta.TryGetField(Book.FieldEnum.BidPrice, out price)) {
				BidPrice = price;
			}
			if (delta.TryGetField(Book.FieldEnum.BidSize, out size)) {
				BidSize = size;
			}
			if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
				AskPrice = price;
			}
			if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
				AskSize = size;
			}

			if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price)) {
				LastTraded = price;
			}

			switch (SecType) {
				case "F":
					if (delta.TryGetNonZero(Book.FieldEnum.SettlePrice, out price)) {
						if (delta.TryGetField(Book.FieldEnum.SettleDateDT, out DateTime when) && (when.Day < DateTime.Today.Day)) {
							// Previous Day SettlePrice
							OpenAvg = price;
							SettlePrice = price;
						} else {
							// Todays SettlePrice
							if (OpenAvg == 0) {
								// Todays First Load
								OpenAvg = price;
							}
							SettlePrice = price;
						}
					} else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
						OpenAvg = price;
					}
					break;
				case "E":
					if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out string status)) {
						switch (status.ToLower()) {
							case "none":
								break;
							case "normal":
								SecurityStatus = "";
								break;
							default:
								SecurityStatus = status;
								break;
						}
					}

					if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
						OpenAvg = price;
						SettlePrice = price;
					} else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
						OpenAvg = price;
					}
					break;
				default:
					if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
						OpenAvg = price;
						SettlePrice = price;
					} else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
						OpenAvg = price;
					}
					break;
			}

			// Need to build PnL
			if (LastTraded == 0) {
				LastTraded = OpenAvg;
			}
		}

		public void UpdateFromSecurity(string symbol, double tickSize, double contractSize)
		{
			Symbol = symbol;
			TickSize = tickSize;
			ContractSize = contractSize;
		}

		public void UpdateFromPosition(ROCPosition other, bool replaceOpen)
		{
			if (other.OpenQty > 0) {
				if (replaceOpen) {
					OpenQty = other.OpenQty;
					OpenAvg = other.OpenAvg;
				} else {
					OpenQty += other.OpenQty;
				}
			}

			if (other.BuyQty > 0 && other.BuyAvg > 0) {
				double cost = (BuyQty * BuyAvg) + (other.BuyQty * other.BuyAvg);
				BuyQty += other.BuyQty;
				BuyAvg = cost / BuyQty;
			}

			if (other.SellQty > 0 && other.SellAvg > 0) {
				double cost = (SellQty * SellAvg) + (other.SellQty * other.SellAvg);
				SellQty += other.SellQty;
				SellAvg = cost / SellQty;
			}
		}

		#region - private members -
		private double _buyAvg = 0;
		private long _buyQty = 0;
		private double _contractSize = 0;
		private long _currentQty = 0;
		private double _dayRealizedPnL = 0;
		private double _dayUnrealizedPnL = 0;
		private double _lastTraded = 0;
		private double _openAvg = 0;
		private long _openQty = 0;
		private double _openPnL = 0;
		private double _sellAvg = 0;
		private long _sellQty = 0;
		#endregion // - private fields -

		#region - derived fields -
		// Some class properties are derived from others.  We use "_dirty"
		// to know when dependent props must be recalculated because their
		// constituent props have changed.  This lets us calculate derived
		// properties only when necessary (ie. when changed + requested).
		// Additionally, some dependent props can be manually assigned. Once
		// that happens, the props are no longer derived from constituent
		// props, and we use "_locked" to track these manually overridden
		// props to preserve them.
		[System.Flags]
		private enum DirtyEnum
		{
			None = 0, 
			CurrentQty = 0x01,
			DayRealizedPnL = 0x02,
			DayUnrealizedPnL = 0x04,
			MarketValue = 0x08,
			OpenPnL = 0x10,
		};
		private DirtyEnum _dirty = DirtyEnum.None;
		private bool isDirty(DirtyEnum which) => ((_dirty & which) != 0);
		private void setDirty(DirtyEnum which) => _dirty |= which;
		private void setClean(DirtyEnum which) => _dirty &= ~which;
		#endregion // - derived fields -
	}
}
