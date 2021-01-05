using System;
using System.Diagnostics;

using DictionaryEx;
using CSVEx;

namespace RDSEx
{
	public abstract class BaseOptionInfo
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public abstract string SecType {
			get;
			set;
		}

		public abstract string Underlying {
			get;
		}

		public abstract string MDSymbol {
			get;
		}

		public abstract double ContractSize {
			get;
		}

		public abstract double TickSize {
			get;
			set;
		}

		public abstract string ExpDate {
			get;
		}

		public abstract string StrikePrice {
			get;
		}

		public abstract string CallPut {
			get;
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
						if (MDSymbol != "") {
							if (int.TryParse(MDSymbol.Substring(MDSymbol.Length - 1, 1), out _)) {
								switch (MDSymbol.Substring(MDSymbol.Length - 2, 1)) {
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
										return MDSymbol.Substring(0, MDSymbol.Length - 2);
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

		public abstract string OptionSymbol {
			get;
		}

		public string SymbolDetail {
			get {
				string value;
				DateTime when;

				if (!_data.TryGet(SecurityFieldIDs.Option.symbolDetail, out value)) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.TryParse(ExpDate, out when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
								value = string.Join(" ",
									Underlying,
									when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							} else if (Underlying.Length == OptionSymbol.Length - 1 && Underlying == OptionSymbol.Substring(0, OptionSymbol.Length - 1)) {
								value = string.Join(" ",
									OptionSymbol,
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
							if (GLOBAL.Future.PlusOne.Contains(BaseSymbol) && TimeFormats.TryParse(ExpDate, out when)) {
								when = when.AddMonths(1);
								if (GLOBAL.MonthCodes.Map.TryGetValue(when.Month, out string name)) {
									value = BaseSymbol + name + MDSymbol.Substring(value.Length);
									break;
								}
							}
							value = MDSymbol;
							break;
						default:
							value = MDSymbol;
							break;
					}

					_data.Set(SecurityFieldIDs.Option.symbolDetail, value);
				}

				return value;
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
							value = MDSymbol;
							break;
					}

					_data.Set(OrderFieldIDs.symbolDetail, value);
				}

				return value;
			}
		}
	}
}
