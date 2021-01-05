using System;
using CSVEx;
using System.Diagnostics;

namespace RDSEx
{
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
		public long? Side {
			get {
				if (_data.TryGet(OrderFieldIDs.ROC.side, out long value))
					return value;
				return null;
			}
		}
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
	}
}
