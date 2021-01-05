using System;
using CSVEx;

namespace RDSEx
{
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
		public long? Side {
			get {
				if (_data.TryGet(TradedFieldIDs.ROC.side, out long value))
					return value;
				return null;
			}
		}

		public long DestID => _data.TryGet(TradedFieldIDs.ROC.destID, out long value) ? value : -1;
		public bool UpdateTrade => _data.TryGet(TradedFieldIDs.ROC.cplxOrderType, out long value) && (value == 5);

		public override string ExpDate {
			get {
				if (_data.TryGet(TradedFieldIDs.ROC.expDate, out string expDate)) {
					if (_data.TryGet(TradedFieldIDs.ROC.maturityDay, out string maturityDay))
						return expDate + maturityDay;
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
}
