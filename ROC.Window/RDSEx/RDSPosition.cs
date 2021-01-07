using System.Collections.Generic;
using DateTime = System.DateTime;

using CSVEx;
using Book = MarketData.Book;

namespace RDSEx
{
	public class RDSPosition
	{
		public enum SourceEnum { None, TPOS, ROC };

		public double AskPrice { get; private set; }
		public long AskSize { get; private set; }
		public string AsOfDate { get; private set; }
		public double AvgCost { get; private set; }
		private string BaseSymbol {
			get {
				if ((_baseSymbol == null) || isDirty(DirtyEnum.BaseSymbol)) {
					switch (_sectype) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!string.IsNullOrEmpty(_underlying) && (_underlying.Length > 2)) {
								if (int.TryParse(_underlying.Substring(_underlying.Length - 1, 1), out _)) {
									if (Common.Constants.FuturesMonthCodes.IsMonthCode(_underlying[_underlying.Length - 2]))
										_baseSymbol = _underlying.Substring(0, _underlying.Length - 2);
								}
							}
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if (!string.IsNullOrEmpty(_symbol) && (_symbol.Length > 2)) {
								if (int.TryParse(_symbol.Substring(_symbol.Length - 1, 1), out _)) {
									if (Common.Constants.FuturesMonthCodes.IsMonthCode(_symbol[_underlying.Length - 2]))
										_baseSymbol = _symbol.Substring(0, _symbol.Length - 2);
								}
							}
							break;
					}

					if (_baseSymbol == null)
						_baseSymbol = _symbol;
					setClean(DirtyEnum.BaseSymbol);
				}

				return _baseSymbol == null ? "" : _baseSymbol;
			}
		}
		public double BidPrice { get; private set; }
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
		public string CallPut {
			get => _callPut;
			set {
				_callPut = value;
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string ClearingAcct {
			get {
				return _clearingAcct;
			}
			private set {
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
				_clearingAcct = value;
			}
		}
		public double ContractSize {
			get => _contractSize;
			private set {
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
		public double DisplayFactor { get; private set; }
		public string DisplayFormat { get; private set; }
		public string ExpDate {
			get { return _expDate; }
			set {
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
				_expDate = value; 
			}
		}
		public bool HasOrders => (OmTags != null) && (OmTags.Count > 0);
		public string InstrumentID { get; private set; }
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
		public double Multiplier { get; private set; }
		public double NetChange { get; private set; }
		public int NotionalAmount { get; private set; }
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
		public string Portfolio { get; private set; }
		public string PositionKey {
			get {
				if ((_positionKey == null) || isDirty(DirtyEnum.PositionKey)) {
					_positionKey = BuildPositionKey(_clearingAcct, SymbolDetail, _trader);
					setClean(DirtyEnum.PositionKey);
				}
				return _positionKey;
			}
		}
		public string PutCall { get; private set; }
		public double RealizedPnL { get; private set; }
		public string SecType {
			get => _sectype;
			set {
				_sectype = value;
				setDirty(DirtyEnum.BaseSymbol | DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
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
		public SourceEnum Source { get; private set; }
		public string Strike { get; private set; }
		public string StrikePrice {
			get => _strikePrice;
			set {
				_strikePrice = value;
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string Symbol {
			get { return _symbol; }
			set {
				_symbol = value;
				setDirty(DirtyEnum.BaseSymbol | DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string SymbolDetail {
			get {
				if ((_symbolDetail == null) || isDirty(DirtyEnum.SymbolDetail)) {
					DateTime when;

					switch (SecType) {
						case "O":
						case "I":
						case "M":
							if (TimeFormats.TryParse(_expDate, out when)) {
								string prefix;
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
									prefix = _underlying;
									when = when.AddMonths(1);
								} else if ((_underlying.Length == Symbol.Length - 1) && (string.Compare(_underlying, 0, _symbol, 0, _underlying.Length) == 0)) {
									prefix = _symbol;
								} else {
									prefix = _underlying;
								}
								_symbolDetail = string.Join(" ", prefix, when.ToString("yy MMM dd").ToUpper(), _strikePrice.Replace(".00", ""), _callPut);
							} else {
								System.Diagnostics.Debug.Assert(!System.Diagnostics.Debugger.IsAttached, _expDate);
							}
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if ((Source == SourceEnum.TPOS) && GLOBAL.Future.PlusOne.Contains(BaseSymbol)) {
								if (TimeFormats.TryParse(ExpDate, out when)) {
									when = when.AddMonths(1);
									if (GLOBAL.MonthCodes.Map.TryGetValue(when.Month, out string name))
										_symbolDetail = BaseSymbol + name + _symbol;
								}
							}

							break;
					}

					if (_symbolDetail == null)
						_symbolDetail = _symbol;
					setClean(DirtyEnum.SymbolDetail);
				}

				return _symbolDetail;
			}
			set {
				_symbolDetail = value;
				setLocked(DirtyEnum.SymbolDetail);
			}
		}
		public string SymbolDisplay {
			get {
				if ((_symbolDisplay == null) || isDirty(DirtyEnum.SymbolDisplay)) {
					switch (SecType) {
						case "O":
						case "I":
						case "M":
							if (TimeFormats.TryParse(_expDate, out DateTime when)) {
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
									_symbolDisplay = string.Format("{0} {1}({2}) {3} {4}",
										_underlying,
										when.AddMonths(1).ToString("yy MMM").ToUpper(),
										when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
										_strikePrice.Replace(".00", ""),
										_callPut);
								} else {
									_symbolDisplay = SymbolDetail;
								}
							} else {
								System.Diagnostics.Debug.Assert(!System.Diagnostics.Debugger.IsAttached, _expDate);
							}
							break;
						default:
							_symbolDisplay = SymbolDetail;
							break;
					}

					if (_symbolDisplay == null)
						_symbolDisplay = SymbolDetail;
					setClean(DirtyEnum.SymbolDisplay);
				}

				return _symbolDisplay;
			}
			set {
				_symbolDisplay = value;
				setLocked(DirtyEnum.SymbolDisplay);
			}
		}
		public double TickSize { get; private set; }
		public double TotalPnL => DayPnL + OpenPnL;
		public long TotalVolume { get; private set; }
		public string TradeGroup { get; private set; }
		public string Trader {
			get {
				return _trader;
			}
			private set {
				_trader = value;
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string TraderAcronym { get; private set; }
		public string UndSecType { get; private set; }
		public string Underlying {
			get { return _underlying; }
			set {
				_underlying = value;
				setDirty(DirtyEnum.BaseSymbol | DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string ValidTraderKey { get; private set; }
		public int Version { get; private set; }

		public RDSPosition(
			double contractSize, 
			long openQty, double 
			openAvg, long buyQty, 
			double buyAvg, 
			long sellQty, 
			double sellAvg, 
			double lastTraded, 
			double settlePrice, 
			RDSPosition incoming = null)
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
					OpenQty = OpenQty + incoming.OpenQty;

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

		public RDSPosition(RDSEx.WEB.TposPosition pos)
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
			Strike = pos.m_Strike.ToString();
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

		public RDSPosition(RDSPosition other, SourceEnum source = SourceEnum.None)
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
			Source = source == SourceEnum.None ? other.Source : source;
			Strike = other.Strike;
			TradeGroup = other.TradeGroup;
			TraderAcronym = other.TraderAcronym;
			UndSecType = other.UndSecType;
			ValidTraderKey = other.ValidTraderKey;
			Version = other.Version;
			_baseSymbol = other._baseSymbol;
			_callPut = other._callPut;
			_clearingAcct = other._clearingAcct;
			_dirty = other._dirty;
			_expDate = other._expDate;
			_locked = other._locked;
			_positionKey = other._positionKey;
			_sectype = other._sectype;
			_strikePrice = other._strikePrice;
			_symbol = other._symbol;
			_symbolDetail = other._symbolDetail;
			_symbolDisplay = other._symbolDisplay;
			_trader = other._trader;
			_underlying = other._underlying;
		}

		public RDSPosition(RDSEx.ROCExecution trade)
		{
			CallPut = trade.CallPut;
			ExpDate = trade.ExpDate;
			SecType = trade.SecType;
			Source = SourceEnum.ROC;
			StrikePrice = trade.StrikePrice;
			Symbol = trade.Symbol;
			Trader = trade.LocalAcct;
			Underlying = trade.Underlying;

			if (trade.Side == CSVEx.CSVFieldIDs.SideCodes.Buy) {
				BuyAvg = trade.Price;
				BuyQty = trade.Qty;
			} else {
				SellAvg = trade.Price;
				SellQty = trade.Qty;
			}

			if (trade.ClearingAcct.Length > 5) {
				ClearingAcct = trade.ClearingAcct.Substring(0, 5);
			} else {
				ClearingAcct = trade.ClearingAcct;
			}
			AddTrade(trade);
		}

		public RDSPosition(TPOSExecution trade)
		{
			CallPut = trade.CallPut;
			ExpDate = trade.ExpDate;
			SecType = trade.SecType;
			Source = SourceEnum.TPOS;
			StrikePrice = trade.StrikePrice;
			Symbol = trade.Symbol;
			Trader = trade.Trader;
			Underlying = trade.Underlying;

			if (trade.Side == CSVEx.CSVFieldIDs.SideCodes.Buy) {
				BuyAvg = trade.Price;
				BuyQty = trade.Qty;
			} else {
				SellAvg = trade.Price;
				SellQty = trade.Qty;
			}

			if (trade.ClearingAcct.Length > 5) {
				ClearingAcct = trade.ClearingAcct.Substring(0, 5);
			} else {
				ClearingAcct = trade.ClearingAcct;
			}
		}

		public void AddTrade(RDSEx.ROCExecution trade)
		{
			double cost;
			switch (trade.Side) {
				case CSVFieldIDs.SideCodes.Buy:
					cost = (BuyQty * BuyAvg) + (trade.Qty * trade.Price);
					BuyQty += trade.Qty;
					BuyAvg = cost / BuyQty;
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
				default:
					cost = (SellQty * SellAvg) + (trade.Qty * trade.Price);
					SellQty += trade.Qty;
					SellAvg = cost / SellQty;
					break;
			}
		}

		public void AddTrade(TPOSExecution trade)
		{
			double cost;
			switch (trade.Side) {
				case CSVFieldIDs.SideCodes.Buy:
					cost = (BuyQty * BuyAvg) + (trade.Qty * trade.Price);
					BuyQty += trade.Qty;
					BuyAvg = cost / BuyQty;
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
				default:
					cost = (SellQty * SellAvg) + (trade.Qty * trade.Price);
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

		public void UpdateFromPosition(RDSPosition other, bool replaceOpen)
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

		public static string BuildPositionKey(string clearingAcct, string symbolDetail, string trader)
		{
			string acct = (clearingAcct.Length > 5) ? clearingAcct.Substring(0, 5) : clearingAcct;
			return string.Join("|", symbolDetail, trader, acct);
		}

		#region - private members -
		private string _baseSymbol = null;
		private double _buyAvg = 0;
		private long _buyQty = 0;
		private string _callPut = null;
		private string _clearingAcct = null;
		private double _contractSize = 0;
		private long _currentQty = 0;
		private double _dayRealizedPnL = 0;
		private double _dayUnrealizedPnL = 0;
		private string _expDate = null;
		private double _lastTraded = 0;
		private double _openAvg = 0;
		private long _openQty = 0;
		private double _openPnL = 0;
		private string _positionKey = null;
		private string _sectype = "";
		private double _sellAvg = 0;
		private long _sellQty = 0;
		private string _strikePrice = null;
		private string _symbol = null;
		private string _symbolDetail = null;
		private string _symbolDisplay = null;
		private string _trader = null;
		private string _underlying = null;
		#endregion // - private fields -

		#region - derived fields -
		// Some class members are derived from other members.  We use "_dirty"
		// to know when dependent members must be recalculated because their
		// constituent members have changed.  This permits us to calculate
		// derived members only when necessary (ie. when changed + requested).
		// Additionally, some dependent members can be manually assigned. Once
		// that happens, the members are no longer derived from constituent
		// members, and we use "_locked" to track these manually overridden
		// members to preserve them.
		[System.Flags]
		private enum DirtyEnum
		{
			None = 0, 
			BaseSymbol = 0x1, 
			CurrentQty = 0x2,
			DayRealizedPnL = 0x4,
			DayUnrealizedPnL = 0x8,
			MarketValue = 0x10,
			OpenPnL = 0x20,
			PositionKey = 0x40, 
			SymbolDetail = 0x80, 
			SymbolDisplay = 0x100
		};
		private DirtyEnum _dirty = DirtyEnum.None;
		private DirtyEnum _locked = DirtyEnum.None;
		private bool isDirty(DirtyEnum which) => ((_dirty & which) != 0) && ((_locked & which) == 0);
		private void setDirty(DirtyEnum which) => _dirty |= which;
		private void setClean(DirtyEnum which) => _dirty &= ~which;
		private void setLocked(DirtyEnum which) => _locked |= which;
		#endregion // - derived fields -
	}
#if OLD
	public class TPOSPositionOld : BasePosition
	{
		public TPOSPositionOld(RDSEx.WEB.TposPosition pos)
		{
			Set(PositionFieldIDs.TPOS.isTPOS, true);
			Set(PositionFieldIDs.TPOS.asOfDate, pos.m_AsOfDate);
			Set(PositionFieldIDs.TPOS.avgCost, pos.m_AvgCost);
			Set(PositionFieldIDs.TPOS.clearingAccount, pos.m_ClearingAccount);
			Set(PositionFieldIDs.TPOS.curQty, pos.m_CurQty);
			Set(PositionFieldIDs.TPOS.displayFormat, pos.m_DisplayFormat);
			Set(PositionFieldIDs.TPOS.expDate, pos.m_ExpDate);
			Set(PositionFieldIDs.TPOS.instrumentID, pos.m_InstrumentID);
			Set(PositionFieldIDs.TPOS.multiplier, pos.m_Multiplier);
			Set(PositionFieldIDs.TPOS.notionalAmount, pos.m_NotionalAmount);
			Set(PositionFieldIDs.TPOS.openMark, pos.m_OpenMark);
			Set(PositionFieldIDs.TPOS.openQty, pos.m_OpenQty);
			Set(PositionFieldIDs.TPOS.portfolio, pos.m_Portfolio);
			Set(PositionFieldIDs.TPOS.putCall, pos.m_PutCall);
			Set(PositionFieldIDs.TPOS.realizedPnL, pos.m_RealizedPnL);
			Set(PositionFieldIDs.TPOS.secType, pos.m_SecType);
			Set(PositionFieldIDs.TPOS.strike, pos.m_Strike.ToString());
			Set(PositionFieldIDs.TPOS.symbol, pos.m_Symbol);
			Set(PositionFieldIDs.TPOS.tradeGroup, pos.m_TradeGroup);
			Set(PositionFieldIDs.TPOS.traderAcronym, pos.m_TraderAcronym);
			//position.Update(PositionFieldIDs.TPOS.traderAcronym, pos.m_Portfolio);
			Set(PositionFieldIDs.TPOS.undSecType, pos.m_UndSecType);
			if (pos.m_UndSecType == CSVFieldIDs.SecurityTypes.Equity) {
				Set(PositionFieldIDs.TPOS.undSymbol, pos.m_Symbol);
			} else {
				Set(PositionFieldIDs.TPOS.undSymbol, pos.m_UndSymbol);
			}
			Set(PositionFieldIDs.TPOS.validTraderKey, pos.m_ValidTraderKey);
			Set(PositionFieldIDs.TPOS.version, pos.m_Version);
		}

		public void Merge(TPOSPosition other)
		{
			base.Merge(other);
		}

		public override string Symbol {
			get => _data.TryGet(PositionFieldIDs.TPOS.symbol, out string value) ? value : "";
			set => _data.Set(PositionFieldIDs.TPOS.symbol, value);
		}

		public double BidPrice {
			get => _data.TryGet(PositionFieldIDs.MD.bidPrice, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.bidPrice, value);
		}

		public long BidSize {
			get => _data.TryGet(PositionFieldIDs.MD.bidSize, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.bidSize, value);
		}

		public double AskPrice {
			get => _data.TryGet(PositionFieldIDs.MD.askPrice, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.askPrice, value);
		}

		public long AskSize {
			get => _data.TryGet(PositionFieldIDs.MD.askSize, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.askSize, value);
		}

		public double LastTraded {
			get => _data.TryGet(PositionFieldIDs.MD.tradePrice, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.tradePrice, value);
		}

		public double NetChange {
			get => _data.TryGet(PositionFieldIDs.MD.netChange, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.netChange, value);
		}

		public double PctChange {
			get => _data.TryGet(PositionFieldIDs.MD.pctChange, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.pctChange, value);
		}

		public long TotalVolume {
			get => _data.TryGet(PositionFieldIDs.MD.totalVolume, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.MD.totalVolume, value);
		}

		public override string ClearingAcct {
			get => _data.TryGet(PositionFieldIDs.TPOS.clearingAccount, out string value) ? value : "";
			set => _data.Set(PositionFieldIDs.TPOS.clearingAccount, value);
		}

		public override string Trader {
			get => _data.TryGet(PositionFieldIDs.TPOS.portfolio, out string value) ? value : "";
			set => _data.Set(PositionFieldIDs.TPOS.portfolio, value);
		}

		public long OpenQty {
			get => _data.TryGet(PositionFieldIDs.TPOS.openQty, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.TPOS.openQty, value);
		}

		public double OpenAvg {
			get => _data.TryGet(PositionFieldIDs.TPOS.avgCost, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.TPOS.avgCost, value);
		}

		public long BuyQty {
			get => _data.TryGet(PositionFieldIDs.buyQty, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.buyQty, value);
		}

		public double BuyAvg {
			get => _data.TryGet(PositionFieldIDs.buyAvg, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.buyAvg, value);
		}

		public long SellQty {
			get => _data.TryGet(PositionFieldIDs.sellQty, out long value) ? value : 0;
			set => _data.Set(PositionFieldIDs.sellQty, value);
		}

		public double SellAvg {
			get => _data.TryGet(PositionFieldIDs.sellAvg, out double value) ? value : 0;
			set => _data.Set(PositionFieldIDs.sellAvg, value);
		}

		public long DayQty {
			get {
				return BuyQty - SellQty;
			}
		}

		public long CurrentQty {
			get {
				return OpenQty + DayQty;
			}
		}

		public double ContractSize {
			get {
				switch (SecType) {
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecurityTypes.Equity:
						return 1;
					default:
						return _data.TryGet(PositionFieldIDs.contractSize, out double value) ? value : 0;
				}
			}
			set {
				_data.Set(PositionFieldIDs.contractSize, value);
			}
		}

		public double TickSize {
			get {
				if (_data.TryGet(PositionFieldIDs.MD.tickSize, out double value)) {
					return value;
				} else {
					return 0.01;
				}
			}
			set {
				_data.Set(PositionFieldIDs.MD.tickSize, value);
			}
		}

		public double DisplayFactor {
			get {
				if (_data.TryGet(PositionFieldIDs.MD.displayFactor, out double value)) {
					return value;
				} else {
					return 1;
				}
			}
			set {
				_data.Set(PositionFieldIDs.MD.displayFactor, value);
			}
		}

		public double OpenPnL {
			get {
				if (LastTraded > 0) {
					if (OpenAvg != 0) {
						if (OpenQty > 0) // Open balance is LONG(BUY)
						{
							return OpenQty * (LastTraded - OpenAvg) * ContractSize;
						} else if (OpenQty < 0) //Open balance is SHORT(SELL)
						  {
							return -OpenQty * (OpenAvg - LastTraded) * ContractSize;
						}
					}
				}

				return 0;
			}
		}

		public double DayUnRealizedPnL {
			get {
				if (LastTraded > 0) {
					if (DayQty > 0) // Day balance is LONG(BUY)
					{
						return DayQty * (LastTraded - BuyAvg) * ContractSize;
					} else if (DayQty < 0)  //Day balance is SHORT(SELL)
					  {
						return -DayQty * (SellAvg - LastTraded) * ContractSize;
					}
				}

				return 0;
			}
		}

		public double DayRealizedPnL {
			get {
				if (BuyQty > SellQty) // Day balance is LONG(BUY)
				{
					return SellQty * (SellAvg - BuyAvg) * ContractSize;
				} else if (BuyQty < SellQty) //Day balance is SHORT(SELL)
				  {
					return BuyQty * (SellAvg - BuyAvg) * ContractSize;
				} else if (BuyQty > 0 && SellQty > 0 && BuyQty == SellQty) {
					return BuyQty * (SellAvg - BuyAvg) * ContractSize;
				} else {
					return 0;
				}
			}
		}

		public double DayPnL {
			get {
				return DayUnRealizedPnL + DayRealizedPnL;
			}
		}

		public double TotalPnL {
			get {
				return DayPnL + OpenPnL;
			}
		}

		public double MarketValue {
			get {
				return LastTraded * CurrentQty * ContractSize;
			}
		}

		public double SettlePrice {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.settlePrice, out double value)) {
					return value;
				} else {
					return 0;
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.settlePrice, value);
			}
		}

		public override string Underlying {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.undSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.undSymbol, value);
			}
		}

		public override string ExpDate {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.expDate, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.expDate, value);
			}
		}

		public override string StrikePrice {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.strike, out string value)) {
					return value;
				} else if (_data.TryGet(PositionFieldIDs.TPOS.strike, out double dval)) {
					_data.Set(PositionFieldIDs.TPOS.strike, dval.ToString());
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.strike, value);
			}
		}

		public override string CallPut {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.putCall, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.putCall, value);
			}
		}

		public override string SecType {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.secType, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.secType, value);
			}
		}

		public string SecurityStatus {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.securityStatus, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.securityStatus, value);
			}
		}

		private List<string> _omTags;
		public override List<string> OmTags {
			get {
				if (_omTags == null) {
					_omTags = new List<string>();
				}
				return _omTags;
			}
			set {
				_omTags = value;
			}
		}

		public override bool HasOrders {
			get {
				if (_omTags == null || _omTags.Count == 0) {
					return false;
				} else {
					return true;
				}
			}
		}

		public long Version {
			get {
				if (_data.TryGet(PositionFieldIDs.TPOS.version, out long value)) {
					return value;
				} else {
					return 0;
				}
			}
		}

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}
	}
#endif // OLD
}
