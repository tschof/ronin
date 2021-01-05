using System;
using System.Diagnostics;

using CSVEx;
using DictionaryEx;

namespace RDSEx
{
	[Serializable]
	public abstract class BaseExecution
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public abstract string TradeID {
			get;
		}

		public string PositionKey {
			get {
				string value;
				if (!_data.TryGet(TradedFieldIDs.positionKey, out value)) {
					if (ClearingAcct.Length > 5)
						value = string.Join("|", SymbolDetail, Trader, ClearingAcct.Substring(0, 5));
					else
						value = string.Join("|", SymbolDetail, Trader, ClearingAcct);
					_data.Set(TradedFieldIDs.positionKey, value);
				}
				return value;
			}
		}

		public abstract string Symbol {
			get;
			set;
		}

		public string BaseSymbol {
			get {
				switch (SecType) {
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (Underlying != "") {
							if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out int _)) {
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
							if (int.TryParse(Symbol.Substring(Symbol.Length - 1, 1), out int _)) {
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
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.TryParse(ExpDate, out when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
								value = string.Join(" ", Underlying,
									when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else if (Underlying.Length == Symbol.Length - 1 && Underlying == Symbol.Substring(0, Symbol.Length - 1)) {
								value = string.Join(" ", Symbol,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else {
								value = string.Join(" ", Underlying,
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
		}

		public string SymbolDisplay {
			get {
				string value;
				if (!_data.TryGet(TradedFieldIDs.symbolDisplay, out value)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.TryParse(ExpDate, out DateTime when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
								value = string.Format("{0} {1}({2}) {3} {4}",
									Underlying,
									when.AddMonths(1).ToString("yy MMM").ToUpper(),
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else {
								value = SymbolDetail;
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
		}

		public bool IsTPOS {
			get {
				return _data.TryGet(TradedFieldIDs.TPOS.isTPOS, out bool value) ? value : false;
			}
			set {
				_data.Set(TradedFieldIDs.TPOS.isTPOS, value);
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
								value = string.Join(" ", Underlying,
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
		}

		public abstract string ExpDate {
			get;
		}

		public abstract string StrikePrice {
			get;
		}

		public string GetStrikFormat(double strikePrice)
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
		}

		public abstract string SecType {
			get;
		}

		public abstract string Trader {
			get;
		}

		public abstract string ClearingAcct {
			get;
		}

		public abstract string OmTag {
			get;
		}

		public abstract string OpenClose {
			get;
		}

		public abstract long Qty {
			get;
		}

		public abstract double Price {
			get;
		}

		public abstract double TradeValue {
			get;
		}

		public abstract DateTime TradeTime {
			get;
		}

		public BaseExecution()
		{
		}
	}
}
