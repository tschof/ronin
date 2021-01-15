using System.Runtime.Serialization;
using DateTime = System.DateTime;

using CSVEx;

namespace RDSEx
{
	[System.Serializable]
	public abstract class ROCSecurity : ISerializable
	{
		public enum SourceEnum { None, TPOS, ROC };

		private string baseSymbol {
			get {
				if ((_baseSymbol == null) || isDirty(DirtyEnum.BaseSymbol)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!string.IsNullOrEmpty(Underlying) && (Underlying.Length > 2)) {
								if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out _)) {
									if (Common.Constants.FuturesMonthCodes.IsMonthCode(Underlying[Underlying.Length - 2]))
										_baseSymbol = Underlying.Substring(0, Underlying.Length - 2);
								}
							}
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if (!string.IsNullOrEmpty(_symbol) && (_symbol.Length > 2)) {
								if (char.IsDigit(_symbol[_symbol.Length - 1])) {
									if (Common.Constants.FuturesMonthCodes.IsMonthCode(_symbol[Underlying.Length - 2]))
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
		public string CallPut {  // Used in IMSymbolDetail, SymbolDetail, SymbolDisplay, PositionKey
			get => _callPut;
			protected set {
				_callPut = value;
				setDirty(DirtyEnum.IMSymbolDetail | DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string ClearingAcct { // Used in PositionKey
			get => _clearingAcct;
			protected set {
				_clearingAcct = value;
				setDirty(DirtyEnum.PositionKey);
			}
		}
		public virtual double ContractSize {
			get {
				switch (SecType) {
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						return 100;
					case CSVFieldIDs.SecurityTypes.Equity:
						return 1;
					default:
						return _contractSize;
				}
			}
			protected set => _contractSize = value;
		}
		public double DisplayFactor { get => 1.0; }
		public virtual string ExpDate {
			get {
				if ((_expDate == null) || isDirty(DirtyEnum.ExpDate)) {
					if (_expDateBase != null) {
						if ((Underlying != null) && (MaturityDay != null))
							_expDate = _expDateBase + MaturityDay;
						else
							_expDate = _expDateBase;
					}
					setClean(DirtyEnum.ExpDate);
				}
				return _expDate ?? "";
			}
			protected set {
				_expDateBase = value;
			}
		}
		public string IMSymbolDetail {
			// ROC to IM, Use this to check againist SecInfo
			get {
				if ((_imSymbolDetail == null) || isDirty(DirtyEnum.IMSymbolDetail)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.TryParse(ExpDate, out DateTime when)) {
								_imSymbolDetail = string.Join(" ",
									Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice,
									CallPut);
							}
							break;
					}

					if (_imSymbolDetail == null)
						_imSymbolDetail = Symbol;
					setClean(DirtyEnum.IMSymbolDetail);
				}

				return _imSymbolDetail;
			}
		}
		public bool IsOptionOnFuture {
			get {
				if (!_isOptionFuture.HasValue || isDirty(DirtyEnum.IsOptionFuture)) {
					if ((Underlying != null) && (Underlying.Length > 3)) {
						if (char.IsDigit(Underlying[Underlying.Length - 1])) {
							switch (Underlying.Substring(Underlying.Length - 2, 1)) {
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

					if (!_isOptionFuture.HasValue)
						_isOptionFuture = false;
					setClean(DirtyEnum.IsOptionFuture);
				}

				return _isOptionFuture.Value;
			}
		}
		protected abstract string MaturityDay { get; }
		public string PositionKey {
			get {
				if ((_positionKey == null) || isDirty(DirtyEnum.PositionKey)) {
					_positionKey = BuildPositionKey(ClearingAcct, SymbolDetail, Trader);
					setClean(DirtyEnum.PositionKey);
				}
				return _positionKey;
			}
		}
		public string SecType { // Used in PositionKey, SymbolDetail, SymbolDisplay
			get => _secType;
			protected set {
				_secType = value;
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public SourceEnum Source { get; protected set; } = SourceEnum.None;
		public double StrikePrice { // Used in IMSymbolDetail, SymbolDetail, SymbolDisplay, PositionKey
			get => _strikePrice;
			protected set {
				_strikePrice = value;
				setDirty(DirtyEnum.IMSymbolDetail | DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}
		public string Symbol { // Used in PositionKey, SymbolDetail
			get => _symbol;
			protected set {
				_symbol = value;
				setDirty(DirtyEnum.PositionKey | DirtyEnum.SymbolDetail);
			}
		}
		public string SymbolDetail {
			get {
				if ((_symbolDetail == null) || isDirty(DirtyEnum.SymbolDetail)) {
					DateTime when;

					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.TryParse(ExpDate, out when)) {
								string prefix;
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(baseSymbol)) {
									prefix = Underlying;
									when = when.AddMonths(1);
								} else if ((Underlying.Length == Symbol.Length - 1) && (string.Compare(Underlying, 0, _symbol, 0, Underlying.Length) == 0)) {
									prefix = _symbol;
								} else {
									prefix = Underlying;
								}
								_symbolDetail = string.Join(" ", prefix, when.ToString("yy MMM dd").ToUpper(), StrikePrice, CallPut);
							} else {
								System.Diagnostics.Debug.Assert(!System.Diagnostics.Debugger.IsAttached, ExpDate);
							}
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if ((Source == SourceEnum.TPOS) && GLOBAL.Future.PlusOne.Contains(baseSymbol)) {
								if (TimeFormats.TryParse(ExpDate, out when)) {
									when = when.AddMonths(1);
									if (GLOBAL.MonthCodes.Map.TryGetValue(when.Month, out string name))
										_symbolDetail = baseSymbol + name + _symbol;
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
			protected set {
				_symbolDetail = value;
				setLocked(DirtyEnum.SymbolDetail);
			}
		}
		public string SymbolDisplay {
			get {
				if ((_symbolDisplay == null) || isDirty(DirtyEnum.SymbolDisplay)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (GLOBAL.OptionOnFuture.PlusOne.Contains(baseSymbol) &&
								TimeFormats.TryParse(ExpDate, out DateTime when))
							{
								_symbolDisplay = string.Format("{0} {1}({2}) {3} {4}",
									Underlying,
									when.AddMonths(1).ToString("yy MMM").ToUpper(),
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice,
									CallPut);
							}
							break;
					}

					if (_symbolDisplay == null)
						_symbolDisplay = SymbolDetail;
					setClean(DirtyEnum.SymbolDisplay);
				}

				return _symbolDisplay;
			}
		}
		public double TickSize { get; protected set; } = 0.01;
		public string Trader { // Used in PositionKey
			get => _trader;
			protected set {
				_trader = value;
				setDirty(DirtyEnum.PositionKey);
			}
		}
		public string Underlying { // Used in BaseSymbol, ExpDate, IMSymbolDetail, IsOptionFuture, SymbolDetail, SymbolDisplay, PositionKey
			get => _underlying;
			protected set {
				_underlying = value;
				setDirty(DirtyEnum.BaseSymbol | DirtyEnum.ExpDate | DirtyEnum.IMSymbolDetail | DirtyEnum.IsOptionFuture | DirtyEnum.PositionKey | DirtyEnum.SymbolDetail | DirtyEnum.SymbolDisplay);
			}
		}

		protected ROCSecurity()
		{
		}

		protected ROCSecurity(ROCSecurity other)
		{
			_baseSymbol = other._baseSymbol;
			_callPut = other._callPut;
			_clearingAcct = other._clearingAcct;
			_dirty = other._dirty;
			_expDate = other._expDate;
			_expDateBase = other._expDateBase;
			_imSymbolDetail = other._imSymbolDetail;
			_locked = other._locked;
			_positionKey = other._positionKey;
			_secType = other._secType;
			_strikePrice = other._strikePrice;
			_symbolDetail = other._symbolDetail;
			_symbolDisplay = other._symbolDisplay;
			_symbol = other._symbol;
			_trader = other._trader;
			_underlying = other._underlying;
		}

		#region - ISerialization -

		// Deserialization.
		protected ROCSecurity(SerializationInfo info, StreamingContext context)
		{
			_callPut = info.GetString("CallPut");
			_clearingAcct = info.GetString("ClearingAcct");
			_contractSize = info.GetDouble("ContractSize");
			_expDateBase = info.GetString("ExpDateBase");
			_secType = info.GetString("SecType");
			_strikePrice = info.GetDouble("StrikePrice");
			_symbol = info.GetString("Symbol");
			_trader = info.GetString("Trader");
			_underlying = info.GetString("Underlying");
		}

		// Serialization.
		public void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			info.AddValue("CallPut", _callPut);
			info.AddValue("ClearingAcct", _clearingAcct);
			info.AddValue("ContractSize", _contractSize);
			info.AddValue("ExpDateBase", _expDateBase);
			info.AddValue("SecType", _secType);
			info.AddValue("StrikePrice", _strikePrice);
			info.AddValue("Symbol", _symbol);
			info.AddValue("Trader", _trader);
			info.AddValue("Underlying", _underlying);
		}

		#endregion // - ISerialization -

		public static string BuildPositionKey(string clearingAcct, string symbolDetail, string trader)
		{
			string acct = (clearingAcct.Length > 5) ? clearingAcct.Substring(0, 5) : clearingAcct;
			return string.Join("|", symbolDetail, trader, acct);
		}

		#region - private members -

		// Constituent members.
		private string _callPut = "";
		private string _clearingAcct = "";
		private double _contractSize = 1;
		private string _expDateBase = null;
		private string _secType = "";
		private double _strikePrice = 0;
		private string _symbol = "";
		private string _trader = "";
		private string _underlying = "";

		// Derived members.
		private string _baseSymbol = null;
		private string _expDate = null;
		private string _imSymbolDetail = null;
		private bool? _isOptionFuture = null;
		private string _positionKey = null;
		private string _symbolDetail = null;
		private string _symbolDisplay = null;

		#endregion // - private members -

		#region - derived fields -
		// Some class properties are derived from others.  We use "_dirty"
		// to know when dependent props must be recalculated because their
		// constituent props have changed, so we can be maximally
		// efficient in calculating derived properties, instead of updating
		// them at each change to constituent properties.
		[System.Flags]
		private enum DirtyEnum
		{
			None = 0,
			BaseSymbol = 0x01,
			ExpDate = 0x02,
			IMSymbolDetail = 0x04,
			IsOptionFuture = 0x08,
			PositionKey = 0x10,
			SymbolDetail = 0x20,
			SymbolDisplay = 0x40
		};
		private DirtyEnum _dirty = DirtyEnum.None;
		private DirtyEnum _locked = DirtyEnum.None;
		private bool isDirty(DirtyEnum which) => ((_dirty & which) != 0) && ((_locked & which) == 0);
		private void setDirty(DirtyEnum which) => _dirty |= which;
		private void setClean(DirtyEnum which) => _dirty &= ~which;
		private void setLocked(DirtyEnum which) => _locked |= which;

		#endregion // - derived fields -
	}
}