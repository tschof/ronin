using System;
using CSVEx;
using System.Diagnostics;

namespace RDSEx
{
	public class TPOSExecution : BaseExecution
	{
		public override string TradeID => _data.TryGet(TradedFieldIDs.TPOS.tradeID, out string value) ? value : "";
		internal long Version => _data.TryGet(TradedFieldIDs.TPOS.version, out long value) ? value : 0;

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
}
