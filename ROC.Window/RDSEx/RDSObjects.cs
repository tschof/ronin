using System;
using System.Collections.Generic;

using DictionaryEx;
using System.Diagnostics;
using CSVEx;

namespace RDSEx
{
	#region - User Profiles -

	internal static class Utility
	{
		internal static long? GetLong(MultiTypedDictionary data, int key)
		{
			if (data.TryGet(key, out long value))
				return value;
			return null;
		}

		internal static bool TryGet(MultiTypedDictionary data, int key, out string value, bool emptyOk)
		{
			if (data.TryGet(key, out value) && (emptyOk || !string.IsNullOrEmpty(value)))
				return true;
			value = null;
			return false;
		}
	}

	public class DestinationMap
	{
		private readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public long? destID => Utility.GetLong(_data, DestinationFieldIDs.DestID);
		public string secType => _data.TryGet(DestinationFieldIDs.SecType, out string value) ? value : null;
		public string shortName => _data.TryGet(DestinationFieldIDs.ShortName, out string value) ? value : null;
		public string giveupFirmID => _data.TryGet(DestinationFieldIDs.GiveupFirmID, out string value) ? value : null;
		public string cmtaFirmID => _data.TryGet(DestinationFieldIDs.CMTAFirmID, out string value) ? value : null;
		public string algoFlag => _data.TryGet(DestinationFieldIDs.AlgoFlag, out string value) ? value : null;
		public bool isAlgo => _data.TryGet(DestinationFieldIDs.AlgoFlag, out string flag) ? (flag == "1") : false;
	}

	public class AccountMap
	{
		private readonly MultiTypedDictionary _data;

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		private Dictionary<int, DestinationMap> _destinations;
		public Dictionary<int, DestinationMap> Destinations {
			get {
				if (_destinations == null) {
					_destinations = new Dictionary<int, DestinationMap>();
				}
				return _destinations;
			}
			set {
				_destinations = value;
			}
		}

		public long? interfaceID => Utility.GetLong(_data, AccountFieldIDs.InterfaceID);
		public string account => _data.TryGet(AccountFieldIDs.Account, out string value) ? value : null;
		public string type => _data.TryGet(AccountFieldIDs.Type, out string value) ? value : null;
		public string clearingAcID => _data.TryGet(AccountFieldIDs.ClearingAcID, out string value) ? value : null;
		public string localAcAcrn => _data.TryGet(AccountFieldIDs.LocalAcAcrn, out string value) ? value : null;
		public string firmAcr => _data.TryGet(AccountFieldIDs.FirmAcr, out string value) ? value : null;
		public string omAcctType => _data.TryGet(AccountFieldIDs.OmAcctType, out string value) ? value : null;
		public string capacity => _data.TryGet(AccountFieldIDs.Capacity, out string value) ? value : null;
		public string clearingFirmID => _data.TryGet(AccountFieldIDs.ClearingFirmID, out string value) ? value : null;
		public string occAcr => _data.TryGet(AccountFieldIDs.OccAcr, out string value) ? value : null;
		public string homeExchange => _data.TryGet(AccountFieldIDs.HomeExchange, out string value) ? value : null;

		public string clearingAcIDShort {
			get {
				if (!_data.TryGet(AccountFieldIDs.ClearingAcID, out string value)) {
					return null;
				} else if (value.Length > 5) {
					return value.Substring(0, 5);
				} else {
					return value;
				}
			}
		}
	}

	/*
	public class LocalAccountMap : BaseTypedAccountMap
	{
	}
	*/

	public class TraderMap : BaseTypedAccountMap
	{
		private readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public string tradeFor => _data.TryGet(TraderFieldIDs.TradeFor, out string value) ? value : null;
		public string localAcAcrn => _data.TryGet(TraderFieldIDs.LocalAcAcrn, out string value) ? value : null;
	}

	#endregion

	#region - ROC Trades, Orders and Positions - 

	[Serializable]
	public class ROCExecution : BaseExecution
	{
		public override string Symbol {
			get => _data.TryGet(TradedFieldIDs.ROC.symbol, out string value) ? value : null;
			set => _data.Set(TradedFieldIDs.ROC.symbol, value);
		}

		public override string Underlying => _data.TryGet(TradedFieldIDs.ROC.underlying, out string value) ? value : null;
		public string MaturityDay => _data.TryGet(TradedFieldIDs.ROC.maturityDay, out string value) ? value : null;
		public override string CallPut => _data.TryGet(TradedFieldIDs.ROC.callPut, out string value) ? value : null;
		public override string SecType => _data.TryGet(TradedFieldIDs.ROC.secType, out string value) ? value : null;
		public override string Trader => _data.TryGet(TradedFieldIDs.ROC.trader, out string value) ? value : null;
		public string LocalAcct => _data.TryGet(TradedFieldIDs.ROC.localAcct, out string value) ? value : null;
		public override string ClearingAcct => _data.TryGet(TradedFieldIDs.ROC.account, out string value) ? value : null;
		public override string TradeID => _data.TryGet(TradedFieldIDs.ROC.omExecTag, out string value) ? value : null;
		public override long Qty => _data.TryGet(TradedFieldIDs.ROC.execQty, out long value) ? value : 0;
		public override double Price => _data.TryGet(TradedFieldIDs.ROC.execPrice, out double value) ? value : 0;
		public override string OmTag => _data.TryGet(TradedFieldIDs.ROC.omTag, out string value) ? value : null;
		public override string OpenClose => _data.TryGet(TradedFieldIDs.ROC.openClose, out string value) ? value : null;
		public string OmExecTag => _data.TryGet(TradedFieldIDs.ROC.omExecTag, out string value) ? value : "";
		public long? Side => Utility.GetLong(_data, TradedFieldIDs.ROC.side);
		public long DestID => _data.TryGet(TradedFieldIDs.ROC.destID, out long value) ? value : -1;
		public bool UpdateTrade => _data.TryGet(TradedFieldIDs.ROC.cplxOrderType, out long value) && (value == 5);

		public override string ExpDate {
			get {
				if (_data.TryGet(TradedFieldIDs.ROC.expDate, out string expDate)) {
					if (Utility.TryGet(_data, TradedFieldIDs.ROC.underlying, out string underlying, false)) {
						if (Utility.TryGet(_data, TradedFieldIDs.ROC.maturityDay, out string maturityDay, false)) {
							return expDate + maturityDay;
						}
					}
				}
				return "";
			}
		}

		public override string StrikePrice {
			get {
				int key = TradedFieldIDs.ROC.strikePrice;
				string value;

				if (_data.TryGet(key, out value))
					return value;

				if (_data.TryGet(key, out double dval)) {
					value = dval.ToString(GetStrikFormat(dval));
					_data.Set(key, value);
					return value;
				}

				return "0";
			}
		}

		public override double TradeValue {
			get {
				if (_data.TryGet(TradedFieldIDs.ROC.execQty, out long quantity)) {
					if (_data.TryGet(TradedFieldIDs.ROC.execPrice, out double price)) {
						return Math.Round(quantity * price * ContractSize, 7);
					}
				}
				return 0;
			}
		}

		public override DateTime TradeTime {
			get {
				if (_data.TryGet(TradedFieldIDs.ROC.execTime, out double value)) {
					return DateTime.FromOADate(value);
				} else {
					return DateTime.Now;
				}
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
						return _data.TryGet(TradedFieldIDs.ROC.contractSize, out double value) ? value : 0;
				}
			}
			set {
				_data.Set(TradedFieldIDs.ROC.contractSize, value);
			}
		}

		public double TickSize {
			get => _data.TryGet(TradedFieldIDs.ROC.tickSize, out double value) ? value : 0.01;
			set => _data.Set(TradedFieldIDs.ROC.tickSize, value);
		}

		public double DisplayFactor {
			get => _data.TryGet(TradedFieldIDs.ROC.displayFactor, out double value) ? value : 1;
			set => _data.Set(TradedFieldIDs.ROC.displayFactor, value);
		}

		public long CplxOrderType {
			get => _data.TryGet(TradedFieldIDs.ROC.cplxOrderType, out long value) ? value : 0;
			set => _data.Set(TradedFieldIDs.ROC.cplxOrderType, value);
		}

		public ROCExecution()
		{
		}
	}

	[Serializable]
	public class ROCOrder : BaseOrder
	{
		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public override bool HasTrades => _data.TryGet(OrderFieldIDs.hasTrades, out bool value) ? value : false;

		public override string Symbol {
			get => _data.TryGet(OrderFieldIDs.ROC.symbol, out string value) ? value : "";
			set => _data.Set(OrderFieldIDs.ROC.symbol, value);
		}

		public bool IsOptionOnFuture {
			get {
				try {
					if (_data.TryGet(OrderFieldIDs.ROC.underlying, out string value) && (value.Length > 3)) {
						if (int.TryParse(value.Substring(value.Length - 1, 1), out _)) {
							switch (value.Substring(value.Length - 2, 1)) {
								case GLOBAL.MonthCodes.January:
								case GLOBAL.MonthCodes.February:
								case GLOBAL.MonthCodes.March:
								case GLOBAL.MonthCodes.April:
								case GLOBAL.MonthCodes.May:
								case GLOBAL.MonthCodes.June:
								case GLOBAL.MonthCodes.July:
								case GLOBAL.MonthCodes.August:
								case GLOBAL.MonthCodes.September:
								case GLOBAL.MonthCodes.October:
								case GLOBAL.MonthCodes.November:
								case GLOBAL.MonthCodes.December:
									return true;
								default:
									break;
							}
						}
					}
				} catch (Exception ex) {
					if (Debugger.IsAttached) {
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return false;
			}
		}

		public override string Underlying => _data.TryGet(OrderFieldIDs.ROC.underlying, out string value) ? value : "";

		public override string ExpDate {
			get {
				if (_data.TryGet(OrderFieldIDs.ROC.expDate, out string expDate)) {
					if (_data.TryGet(OrderFieldIDs.ROC.underlying, out string _)) {
						if (_data.TryGet(OrderFieldIDs.ROC.maturityDay, out string maturityDay)) {
							return expDate + maturityDay;
						}
					}
					return expDate;
				}
				return "";
			}
		}

		public string MaturityDay => _data.TryGet(OrderFieldIDs.ROC.maturityDay, out string value) ? value : "";

		public override string StrikePrice {
			get {
				string value;

				if (_data.TryGet(OrderFieldIDs.ROC.strikePrice, out value))
					return value;

				if (_data.TryGet(OrderFieldIDs.ROC.strikePrice, out double sp)) {
					value = sp.ToString(GetStrikFormat(sp));
					_data.Set(OrderFieldIDs.ROC.strikePrice, value);
					return value;
				}

				return "0";
			}
		}

		public override string CallPut => _data.TryGet(OrderFieldIDs.ROC.callPut, out string value) ? value : "";
		public override string SecType => _data.TryGet(OrderFieldIDs.ROC.secType, out string value) ? value : "";
		public override string Trader => _data.TryGet(OrderFieldIDs.ROC.trader, out string value) ? value : "";
		public string LocalAcct => _data.TryGet(OrderFieldIDs.ROC.localAcct, out string value) ? value : "";
		public override string ClearingAcct => _data.TryGet(OrderFieldIDs.ROC.clearingAccount, out string value) ? value : "";
		public override string OmTag => _data.TryGet(OrderFieldIDs.ROC.omTag, out string value) ? value : "";
		public override string OpenClose => _data.TryGet(OrderFieldIDs.ROC.openClose, out string value) ? value : "";
		public long? Side => Utility.GetLong(_data, OrderFieldIDs.ROC.side);
		public string Tag => _data.TryGet(OrderFieldIDs.ROC.tag, out string value) ? value : "";
		public long Status {
			get => _data.TryGet(OrderFieldIDs.ROC.status, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.status, value);
		}

		public bool IsActive {
			get {
				if (_data.TryGet(OrderFieldIDs.ROC.status, out long value)) {
					switch (value) {
						case CSVFieldIDs.StatusCodes.Busted:
						case CSVFieldIDs.StatusCodes.Expired:
						case CSVFieldIDs.StatusCodes.Rejected:
						case CSVFieldIDs.StatusCodes.Canceled:
						case CSVFieldIDs.StatusCodes.Filled:
						case CSVFieldIDs.StatusCodes.FilledAndCancelled:
						case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
							return false;
					}
				}
				return true;
			}
		}

		public long Qty {
			get => _data.TryGet(OrderFieldIDs.ROC.qty, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.qty, value);
		}

		public long LeaveQty {
			get => _data.TryGet(OrderFieldIDs.ROC.leavesQty, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.leavesQty, value);
		}

		public double Price {
			get => _data.TryGet(OrderFieldIDs.ROC.price, out double value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.price, value);
		}

		public double OrigionalPrice {
			get => _data.TryGet(OrderFieldIDs.ROC.originalPrice, out double value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.originalPrice, value);
		}

		public double ExecPrice {
			get => _data.TryGet(OrderFieldIDs.ROC.execPrice, out double value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.execPrice, value);
		}

		public double StopPrice {
			get => _data.TryGet(OrderFieldIDs.ROC.stopPrice, out double value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.stopPrice, value);
		}

		public double PegPrice {
			get => _data.TryGet(OrderFieldIDs.ROC.pegPrice, out double value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.pegPrice, value);
		}

		public string MaxFloor => _data.TryGet(OrderFieldIDs.ROC.maxFloor, out string value) ? value : "";

		public long CumQty {
			get => _data.TryGet(OrderFieldIDs.ROC.cumQty, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.cumQty, value);
		}

		public double AvgPrice {
			get => _data.TryGet(OrderFieldIDs.ROC.avgPrice, out double value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.avgPrice, value);
		}

		public long OrderType {
			get => _data.TryGet(OrderFieldIDs.ROC.orderType, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.orderType, value);
		}

		public bool IsMarket {
			get {
				if (_data.TryGet(OrderFieldIDs.ROC.orderType, out long value)) {
					switch (value) {
						case CSVFieldIDs.OrderTypes.Market:
						case CSVFieldIDs.OrderTypes.MarketAtOpen:
						case CSVFieldIDs.OrderTypes.MarketOnClose:
							return true;
					}
				}
				return false;
			}
		}

		public long TIF {
			get => _data.TryGet(OrderFieldIDs.ROC.tif, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.tif, value);
		}

		public long AlgoType {
			get => _data.TryGet(OrderFieldIDs.ROC.algoType, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.algoType, value);
		}

		public string EndTime {
			get {
				if (_data.TryGet(OrderFieldIDs.ROC.endTime, out double value))
					return DateTime.FromOADate(value).ToString("MM-dd HH:mm:ss");
				return "";
			}
		}

		public long DestID {
			get => _data.TryGet(OrderFieldIDs.ROC.destID, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.destID, value);
		}

		public long AlgoDestID => _data.TryGet(OrderFieldIDs.ROC.algoDestID, out long value) ? value : 0;

		public DateTime OmTime {
			get {
				if (_data.TryGet(OrderFieldIDs.ROC.omTime, out double value))
					return DateTime.FromOADate(value);
				return DateTime.Now;
			}

			set => _data.Set(OrderFieldIDs.ROC.omTime, value.ToOADate());
		}

		public string Text {
			get => _data.TryGet(OrderFieldIDs.ROC.text, out string value) ? value : null;
			set => _data.Set(OrderFieldIDs.ROC.text, value);
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
						return _data.TryGet(OrderFieldIDs.ROC.contractSize, out double value) ? value : 0;
				}
			}

			set => _data.Set(OrderFieldIDs.ROC.contractSize, value);
		}

		public double TickSize {
			get => _data.TryGet(OrderFieldIDs.ROC.tickSize, out double value) ? value : 0.01;
			set => _data.Set(OrderFieldIDs.ROC.tickSize, value);
		}

		public double DisplayFactor {
			get => _data.TryGet(OrderFieldIDs.ROC.displayFactor, out double value) ? value : 1;
			set => _data.Set(OrderFieldIDs.ROC.displayFactor, value);
		}

		public string TradeFor {
			get => _data.TryGet(OrderFieldIDs.ROC.tradeFor, out string value) ? value : "";
			set => _data.Set(OrderFieldIDs.ROC.tradeFor, value);
		}

		public long CplxOrderType {
			get => _data.TryGet(OrderFieldIDs.ROC.cplxOrderType, out long value) ? value : 0;
			set => _data.Set(OrderFieldIDs.ROC.cplxOrderType, value);
		}

		public string ParentTag => _data.TryGet(OrderFieldIDs.ROC.parentTag, out string value) ? value : null;
		public string ClientEcho => _data.TryGet(OrderFieldIDs.ROC.clientEcho, out string value) ? value : null;
		public bool UpdateOrder => _data.TryGet(OrderFieldIDs.ROC.cplxOrderType, out long value) && (value == CSVFieldIDs.CplxOrderTypes.Container);
		public string SecurityDefinition => _data.TryGet(OrderFieldIDs.ROC.securityDefinition, out string value) ? value : "";
		public string CMTAFirmID => _data.TryGet(OrderFieldIDs.ROC.cmtaFirmID, out string value) ? value : "";

		public ROCOrder()
		{
		}
	}

	#endregion

	#region - TPOS Trades, Positions - 

	public class TPOSExecution : BaseExecution
	{
		public override string TradeID => _data.TryGet(TradedFieldIDs.TPOS.tradeID, out string value) ? value : "";
		public long Version => _data.TryGet(TradedFieldIDs.TPOS.version, out long value) ? value : 0;

		// 3 means deleted
		public long ModReasonID => _data.TryGet(TradedFieldIDs.TPOS.lastModReasonID, out long value) ? value : 0;

		public override string Symbol {
			get => _data.TryGet(TradedFieldIDs.TPOS.symbol, out string value) ? value : "";
			set => _data.Set(TradedFieldIDs.TPOS.symbol, value);
		}

		public override string Underlying => _data.TryGet(TradedFieldIDs.TPOS.undSymbol, out string value) ? value : "";
		public override string ExpDate => _data.TryGet(TradedFieldIDs.TPOS.expDate, out string value) ? value : "";
		public override string StrikePrice {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.strike, out string value))
					return value;

				if (_data.TryGet(TradedFieldIDs.TPOS.strike, out double sp)) {
					value = sp.ToString(GetStrikFormat(sp));
					_data.Set(TradedFieldIDs.TPOS.strike, value);
					return value;
				}

				return "";
			}
		}

		public override string CallPut => _data.TryGet(TradedFieldIDs.TPOS.putCall, out string value) ? value : "";
		public override string SecType => _data.TryGet(TradedFieldIDs.TPOS.secType, out string value) ? value : "";
		public override string Trader => _data.TryGet(TradedFieldIDs.TPOS.portfolio, out string value) ? value : "";
		public override string ClearingAcct => _data.TryGet(TradedFieldIDs.TPOS.clearingAccount, out string value) ? value : "";
		public override long Qty {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.lastModReasonID, out long reason) && (reason != 3)) // 3 = deleted
				{
					if (_data.TryGet(TradedFieldIDs.TPOS.qty, out long value))
						return value;
				}
				return 0;
			}
		}

		public override double Price {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.lastModReasonID, out long reason) && (reason != 3)) // 3 = deleted
				{
					if (_data.TryGet(TradedFieldIDs.TPOS.price, out double value))
						return value;
				}
				return 0;
			}
		}

		public override double TradeValue {
			get {
				return Qty * Price;
			}
		}

		public override DateTime TradeTime {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.lastModTime, out string value)) {
					try {
						return System.Convert.ToDateTime(value).ToLocalTime();
					} catch (Exception ex) {
						Debug.Assert(false, ex.Message, ex.StackTrace);
						return DateTime.Now;
					}
				} else {
					return DateTime.Now;
				}
			}
		}

		public override string OpenClose {
			get {
				return "";
			}
		}

		public override string OmTag => _data.TryGet(TradedFieldIDs.TPOS.extTradeID, out string value) ? value : "";
		public long? Side {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.qty, out long value))
					return value > 0 ? 1 : 2; // 1 = buy, 2 = sell
				return null;
			}
		}

		public long DestID {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.exchange, out long value))
					return value;

				if (_data.TryGet(TradedFieldIDs.TPOS.exchange, out string text)) {
					if (long.TryParse(text, out long id)) {
						_data.Set(TradedFieldIDs.TPOS.exchange, id);
						return id;
					}
				}

				return -1;
			}
		}

		public double ContractSize {
			get {
				if (_data.TryGet(TradedFieldIDs.TPOS.secType, out string sectype)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							return 100;
						case CSVFieldIDs.SecurityTypes.Equity:
							return 1;
						default:
							return _data.TryGet(TradedFieldIDs.TPOS.contractSize, out double size) ? size : 0;
					}
				}
				return 0;
			}

			set {
				_data.Set(TradedFieldIDs.TPOS.contractSize, value);
			}
		}

		public double TickSize {
			get => _data.TryGet(TradedFieldIDs.TPOS.tickSize, out double value) ? value : 0.01;
			set => _data.Set(TradedFieldIDs.TPOS.tickSize, value);
		}

		public double DisplayFactor {
			get => _data.TryGet(TradedFieldIDs.TPOS.displayFactor, out double value) ? value : 1;
			set => _data.Set(TradedFieldIDs.TPOS.displayFactor, value);
		}
	}

	public class TPOSPosition : BasePosition
	{
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

	#endregion

	#region - IM SecurityInfo, Option Chain, SSFuture Chain -

	public class IMSecurityInfo : BaseSecurityInfo
	{
		private readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public override string MDSource {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.dataSourceInfo, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string MDSymbol {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.genericMDSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override double ContractSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.contractSize, out double value)) {
					return value;
				} else {
					return 1;
				}
			}
		}

		public override double TickSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.tickSize, out double value)) {
					return value;
				} else {
					return 0.01;
				}
			}
		}

		public override string SecType {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.securityType, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string LongName {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.longName, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public string Underlying {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.baseSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public string Expiration {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.expirationDate, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		private Dictionary<string, IMOptionInfo> _optionChain;
		public Dictionary<string, IMOptionInfo> OptionChain {
			get {
				if (_optionChain == null) {
					_optionChain = new Dictionary<string, IMOptionInfo>();
				}
				return _optionChain;
			}
			set {
				_optionChain = value;
			}
		}

		private Dictionary<string, IMSSFutureInfo> _ssfutureChain;
		public Dictionary<string, IMSSFutureInfo> SSFutureChain {
			get {
				if (_ssfutureChain == null) {
					_ssfutureChain = new Dictionary<string, IMSSFutureInfo>();
				}
				return _ssfutureChain;
			}
			set {
				_ssfutureChain = value;
			}
		}
	}

	public class IMOptionInfo : BaseOptionInfo
	{
		public string Exchange {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.exchange, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string SecType {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.secType, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(SecurityFieldIDs.Option.secType, value);
			}
		}

		public string MDSource {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.dataSource, out string value)) {
					return value;
				} else {
					return "";
				}
			}
			set {
				_data.Set(SecurityFieldIDs.Option.dataSource, value);
			}
		}

		public override string MDSymbol {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.genericMDSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override double ContractSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.contractSize, out double value)) {
					return value;
				} else {
					return 100;
				}
			}
		}

		public override double TickSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.tickSize, out double value)) {
					return value;
				} else {
					return 0.01;
				}
			}
			set {
				_data.Set(SecurityFieldIDs.Option.tickSize, value);
			}
		}

		public override string ExpDate {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.expirationDate, out string value)) {
					if (TimeFormats.ParseDate(value, out DateTime when)) {
						return when.ToString("yyyyMMdd");
					}
				}

				return "";
			}
		}

		public DateTime? ExpDateDT {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.expirationDate, out string value)) {
					if (TimeFormats.ParseDate(value, out DateTime when)) {
						return when;
					} else {
						return null;
					}
				} else {
					return null;
				}
			}
		}

		public override string StrikePrice {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.strikPrice, out string value)) {
					return value;
				} else if (_data.TryGet(SecurityFieldIDs.Option.strikPrice, out double dval)) {
					value = dval.ToString();
					_data.Set(SecurityFieldIDs.Option.strikPrice, value);
					return value;
				}

				return "";
			}
		}

		public double? StrikePriceD {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.strikPrice, out double value)) {
					return value;
				} else if (_data.TryGet(SecurityFieldIDs.Option.strikPrice, out string sval)) {
					value = Convert.ToDouble(sval);
					_data.Set(SecurityFieldIDs.Option.strikPrice, value);
					return value;
				}

				return null;
			}
		}

		public override string CallPut {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.putCall, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string Underlying {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.underlying, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string OptionSymbol {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.optionSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}
	}

	public class IMSSFutureInfo : BaseSSFutureInfo
	{
		private readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public override string MDSource {
			get {
				if (_data.TryGet(SecurityFieldIDs.SSFuture.dataSource, out string value)) {
					return value;
				} else {
					return "ONEC";
				}
			}
		}

		public override string MDSymbol {
			get {
				if (_data.TryGet(SecurityFieldIDs.SSFuture.fullSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override double ContractSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.SSFuture.contractSize, out double value)) {
					return value;
				} else {
					return 1;
				}
			}
		}

		public override double TickSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.SSFuture.tickSize, out double value)) {
					return value;
				} else {
					return 0.01;
				}
			}
			set {
				_data.Set(SecurityFieldIDs.SSFuture.tickSize, value);
			}
		}

		public override string ExpDate {
			get {
				if (_data.TryGet(SecurityFieldIDs.SSFuture.expirationDate, out string value)) {
					return value;
				} else {
					return null;
				}
			}
		}

		public override string LongName {
			get {
				if (_data.TryGet(SecurityFieldIDs.SSFuture.longName, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}
	}

	#endregion
}