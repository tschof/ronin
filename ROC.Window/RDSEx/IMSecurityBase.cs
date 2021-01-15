using System;
using System.Collections.Generic;

namespace RDSEx
{
	public class IMSecurityBase
	{
		public double ContractSize { get; protected set; } = 1.0;
		public int DecimalPlaces {
			get {
				if (_decimalPlaces < 0) {
					if (TickSize > 0) {
						decimal d = TickSizeDec;
						for (_decimalPlaces = 0; d != 0; ++_decimalPlaces)
							d = (d - (int)d) * 10;
					} else {
						_decimalPlaces = 0;
					}
				}
				return _decimalPlaces;
			}
		}
		public DateTime? Expiration {
			get {
				if (_expiration.HasValue)
					return _expiration.Value;

				if (!string.IsNullOrEmpty(_expirationText)) {
					if (DateTime.TryParseExact(
						_expirationText,
						"yyyyMMdd",
						System.Globalization.CultureInfo.CurrentCulture,
						System.Globalization.DateTimeStyles.None,
						out DateTime when)) {
						_expiration = when;
						return when;
					}
				}
				return null;
			}
			set {
				_expiration = value;
				_expirationText = null;
			}
		}
		public string ExpirationText {
			get {
				if (!string.IsNullOrEmpty(_expirationText))
					return _expirationText;
				if (_expiration.HasValue) {
					_expirationText = _expiration.Value.ToString("yyyyMMdd");
					return _expirationText;
				}
				return "";
			}
			set {
				_expirationText = value;
				_expiration = null;
			}
		}
		public string LongName { get; protected set; } = "";
		public string MDSymbol { get; private set; } = "";
		public string MDSource { get; private set; } = "";
		public string SecType { get; protected set; } = "";
		public double TickSize {
			get => _tickSize;
			set {
				_tickSize = value;
				_decimalPlaces = -1;
				TickSizeDec = new decimal(value);
			}
		}
		public decimal TickSizeDec { get; private set; } = 0;
		public string Underlying { get; private set; } = "";

		public Dictionary<string, IMSSFutureInfo> SSFuturesChain {
			get {
				if (_ssfChain == null)
					_ssfChain = new Dictionary<string, IMSSFutureInfo>();
				return _ssfChain;
			}
		}

		public Dictionary<string, IMOptionInfo> OptionChain {
			get {
				if (_optionChain == null)
					_optionChain = new Dictionary<string, IMOptionInfo>();
				return _optionChain;
			}
		}

		public IMSecurityBase()
		{
		}

		public IMSecurityBase(IMSecurityBase other)
		{
			ContractSize = other.ContractSize;
			ExpirationText = other.ExpirationText;
			LongName = other.LongName;
			MDSymbol = other.MDSymbol;
			MDSource = other.MDSource;
			SecType = other.SecType;
			TickSize = other.TickSize;
			Underlying = other.Underlying;

			if (other.SSFuturesChain.Count > 0)
				_ssfChain = new Dictionary<string, IMSSFutureInfo>(other.SSFuturesChain);
			if (other.OptionChain.Count > 0)
				_optionChain = new Dictionary<string, IMOptionInfo>(other.OptionChain);
		}

		public IMSecurityBase(IMOptionInfo optionInfo)
		{
			ContractSize = (optionInfo.ContractSize > 0) ? optionInfo.ContractSize : 100;
			ExpirationText = optionInfo.ExpDate;
			LongName = optionInfo.SymbolDetail;
			MDSymbol = optionInfo.MDSymbol;
			MDSource = optionInfo.MDSource;
			SecType = optionInfo.SecType;
			TickSize = optionInfo.TickSize;
			Underlying = optionInfo.Underlying;
		}

		private int _decimalPlaces = -1;
		private DateTime? _expiration = null;
		private string _expirationText = null;
		private Dictionary<string, IMOptionInfo> _optionChain;
		private Dictionary<string, IMSSFutureInfo> _ssfChain;
		private double _tickSize = 0.01;
	}
}
