using System;

namespace RDSEx
{
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
					if (TimeFormats.TryParse(value, out DateTime when)) {
						return when.ToString("yyyyMMdd");
					}
				}

				return "";
			}
		}

		public DateTime? ExpDateDT {
			get {
				if (_data.TryGet(SecurityFieldIDs.Option.expirationDate, out string value)) {
					if (TimeFormats.TryParse(value, out DateTime when)) {
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
}
