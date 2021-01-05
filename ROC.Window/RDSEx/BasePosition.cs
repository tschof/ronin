using System;
using System.Collections.Generic;
using System.Diagnostics;

using CSVEx;
using DictionaryEx;

namespace RDSEx
{
	public abstract class BasePosition
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		protected void Merge(BasePosition other)
		{
			_data.Merge(other._data);
		}

		public abstract bool HasOrders {
			get;
		}

		public abstract List<string> OmTags {
			get;
			set;
		}

		private Dictionary<string, ROCExecution> _trades;
		internal Dictionary<string, ROCExecution> Trades {
			get {
				if (_trades == null) {
					_trades = new Dictionary<string, ROCExecution>();
				}
				return _trades;
			}
			set {
				_trades = value;
			}
		}

		public string PositionKey {
			get {
				string value;
				if (!_data.TryGet(PositionFieldIDs.positionKey, out value)) {
					if (ClearingAcct.Length > 5) {
						value = string.Join("|",
							SymbolDetail,
							Trader,
							ClearingAcct.Substring(0, 5));
					} else {
						value = string.Join("|",
							SymbolDetail,
							Trader,
							ClearingAcct);
					}
					_data.Set(PositionFieldIDs.positionKey, value);
				}

				return value;
			}
		}

		public abstract string Symbol {
			get;
			set;
		}

		private string BaseSymbol {
			get {
				switch (SecType) {
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (Underlying != "") {
							if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out _)) {
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
										return Underlying.Substring(0, Underlying.Length - 2);
									default:
										break;
								}
							}
						}
						break;
					case CSVFieldIDs.SecurityTypes.Future:
						if (Symbol != "") {
							if (int.TryParse(Symbol.Substring(Symbol.Length - 1, 1), out _)) {
								switch (Symbol.Substring(Symbol.Length - 2, 1)) {
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
										return Symbol.Substring(0, Symbol.Length - 2);
									default:
										break;
								}
							}
						}
						break;
				}
				return "";
			}
		}

		public string SymbolDetail {
			get {
				string value;
				DateTime when;

				if (!_data.TryGet(TradedFieldIDs.symbolDetail, out value)) {
					switch (SecType) {
						case "O":
						case "I":
						case "M":
							if (!TimeFormats.TryParse(ExpDate, out when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
								value = string.Join(" ",
									Underlying,
									when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else if (Underlying.Length == Symbol.Length - 1 && Underlying == Symbol.Substring(0, Symbol.Length - 1)) {
								value = string.Join(" ",
									Symbol,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else {
								value = string.Join(" ",
									Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							}
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if (IsTPOS && GLOBAL.Future.PlusOne.Contains(BaseSymbol)) {
								if (TimeFormats.TryParse(ExpDate, out when)) {
									when = when.AddMonths(1);
									if (GLOBAL.MonthCodes.Map.TryGetValue(when.Month, out string name)) {
										value = BaseSymbol + name + Symbol.Substring(value.Length);
										break;
									}
								}
							}
							value = Symbol;
							break;
						default:
							value = Symbol;
							break;
					}

					_data.Set(TradedFieldIDs.symbolDetail, value);
				}

				return value;
			}
			set {
				_data.Set(TradedFieldIDs.symbolDetail, value);
			}
		}

		public string SymbolDisplay {
			get {
				string value;
				if (!_data.TryGet(TradedFieldIDs.symbolDisplay, out value)) {
					switch (SecType) {
						case "O":
						case "I":
						case "M":
							if (TimeFormats.TryParse(ExpDate, out DateTime when)) {
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
									value = string.Format("{0} {1}({2}) {3} {4}",
										Underlying,
										when.AddMonths(1).ToString("yy MMM").ToUpper(),
										when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
										StrikePrice.Replace(".00", ""),
										CallPut);
								} else {
									value = SymbolDetail;
								}
							} else {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							value = SymbolDetail;
							break;
					}

					if (!string.IsNullOrEmpty(value))
						_data.Set(TradedFieldIDs.symbolDisplay, value);
				}

				return value;
			}
			set {
				_data.Set(TradedFieldIDs.symbolDisplay, value);
			}
		}

		public bool IsTPOS {
			get {
				return _data.TryGet(PositionFieldIDs.TPOS.isTPOS, out bool value) ? value : false;
			}
			set {
				_data.Set(PositionFieldIDs.TPOS.isTPOS, value);
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail {
			get {
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDetail, out value)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.TryParse(ExpDate, out DateTime when)) {
								value = string.Join(" ",
									Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							value = Symbol;
							break;
					}

					_data.Set(OrderFieldIDs.symbolDetail, value);
				}

				return value;
			}
		}

		public abstract string Underlying {
			get;
			set;
		}

		public abstract string ExpDate {
			get;
			set;
		}

		public abstract string StrikePrice {
			get;
			set;
		}

		internal string GetStrikFormat(double strikePrice)
		{
			string str = strikePrice.ToString("F7");
			string[] strs = str.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

			if (strs.Length == 2) {
				return "F" + strs[1].TrimEnd(new char[] { '0' }).Length.ToString();
			}
			return "F2";
		}

		public abstract string CallPut {
			get;
			set;
		}

		public abstract string SecType {
			get;
			set;
		}

		public abstract string Trader {
			get;
			set;
		}

		public abstract string ClearingAcct {
			get;
			set;
		}
	}
}
