using System;
using System.Collections.Generic;

using DictionaryEx;
using System.Diagnostics;
using CSVEx;

namespace RDSEx
{
	public abstract class BaseTypedAccountMap // : MultiTypedDictionary
	{
		private Dictionary<string, AccountMap> _csAccounts;
		public Dictionary<string, AccountMap> CSAccounts
		{
			get
			{
				if (_csAccounts == null)
				{
					_csAccounts = new Dictionary<string, AccountMap>();
				}
				return _csAccounts;
			}
			set
			{
				_csAccounts = value;
			}
		}

		private Dictionary<string, AccountMap> _optAccounts;
		public Dictionary<string, AccountMap> OPTAccounts
		{
			get
			{
				if (_optAccounts == null)
				{
					_optAccounts = new Dictionary<string, AccountMap>();
				}
				return _optAccounts;
			}
			set
			{
				_optAccounts = value;
			}
		}

		private Dictionary<string, AccountMap> _futAccounts;
		public Dictionary<string, AccountMap> FUTAccounts
		{
			get
			{
				if (_futAccounts == null)
				{
					_futAccounts = new Dictionary<string, AccountMap>();
				}
				return _futAccounts;
			}
			set
			{
				_futAccounts = value;
			}
		}
	}

	[Serializable]
	public abstract class BaseExecution
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public abstract string TradeID
		{
			get;
		}

		public string PositionKey
		{
			get
			{
				string value;
				if (!_data.TryGet(TradedFieldIDs.positionKey, out value))
				{
					if (ClearingAcct.Length > 5)
						value = string.Join("|", SymbolDetail, Trader, ClearingAcct.Substring(0, 5));
					else
						value = string.Join("|", SymbolDetail, Trader,	ClearingAcct);
					_data.Set(TradedFieldIDs.positionKey, value);
				}
				return value;
			}
		}

		public abstract string Symbol
		{
			get;
			set;
		}

		public string BaseSymbol
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (Underlying != "")
						{
							if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out int _))
							{
								switch (Underlying.Substring(Underlying.Length - 2, 1))
								{
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
						if (Symbol != "")
						{
							if (int.TryParse(Symbol.Substring(Symbol.Length - 1, 1), out int _))
							{
								switch (Symbol.Substring(Symbol.Length - 2, 1))
								{
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

		public string SymbolDetail
		{
			get
			{
				string value;
				DateTime when;
				bool ok;

				if (!_data.TryGet(TradedFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.ParseDate(ExpDate, out when)) {
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
							if (IsTPOS && GLOBAL.Future.PlusOne.Contains(BaseSymbol))
							{
								if (TimeFormats.ParseDate(ExpDate, out when)) {
									when = when.AddMonths(1);
									if (GLOBAL.MonthCodes.Map.ContainsKey(when.Month)) {
										value = BaseSymbol + GLOBAL.MonthCodes.Map[when.Month] + Symbol.Substring(value.Length);
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

		public string SymbolDisplay
		{
			get
			{
				string value;
				if (!_data.TryGet(TradedFieldIDs.symbolDisplay, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.ParseDate(ExpDate, out DateTime when)) {
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

		public bool IsTPOS
		{
			get
			{
				return _data.TryGet(TradedFieldIDs.TPOS.isTPOS, out bool value) ? value : false;
			}
			set
			{
				_data.Set(TradedFieldIDs.TPOS.isTPOS, value);
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.ParseDate(ExpDate, out DateTime when))
							{
								value = string.Join(" ", Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							}
							else
							{
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

		public abstract string Underlying
		{
			get;
		}

		public abstract string ExpDate
		{
			get;
		}

		public abstract string StrikePrice
		{
			get;
		}

		public string GetStrikFormat(double strikePrice)
		{
			string str = strikePrice.ToString("F7");
			string[] strs = str.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

			if (strs.Length == 2)
			{
				return "F" + strs[1].TrimEnd(new char[] { '0' }).Length.ToString();
			}
			return "F2";
		}

		public abstract string CallPut
		{
			get;
		}

		public abstract string SecType
		{
			get;
		}

		public abstract string Trader
		{
			get;
		}

		public abstract string ClearingAcct
		{
			get;
		}

		public abstract string OmTag
		{
			get;
		}

		public abstract string OpenClose
		{
			get;
		}

		public abstract long Qty
		{
			get;
		}

		public abstract double Price
		{
			get;
		}

		public abstract double TradeValue
		{
			get;
		}

		public abstract DateTime TradeTime
		{
			get;
		}

		public BaseExecution()
		{
		}
	}

	[Serializable]
	public abstract class BaseOrder
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public abstract bool HasTrades
		{
			get;
		}

		public abstract string OmTag
		{
			get;
		}

		private Dictionary<string, ROCExecution> _trades;
		public Dictionary<string, ROCExecution> Trades
		{
			get
			{
				if (_trades == null)
				{
					_trades = new Dictionary<string, ROCExecution>();
				}
				return _trades;
			}
			set
			{
				_trades = value;
			}
		}

		public string PositionKey
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.positionKey, out value))
				{
					if (ClearingAcct.Length > 5)
						value = string.Join("|", SymbolDetail, Trader, ClearingAcct.Substring(0, 5));
					else
						value = string.Join("|", SymbolDetail, Trader, ClearingAcct);
					_data.Set(OrderFieldIDs.positionKey, value);
				}

				return value;
			}
		}

		public abstract string Symbol
		{
			get;
			set;
		}

		public string BaseSymbol
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (Underlying != "")
						{
							if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out int _))
							{
								switch (Underlying.Substring(Underlying.Length - 2, 1))
								{
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
				}
				return "";
			}
		}

		public string SymbolDetail
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.ParseDate(ExpDate, out DateTime when)) {
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
						default:
							value = Symbol;
							break;
					}

					_data.Set(OrderFieldIDs.symbolDetail, value);
				}

				return value;
			}
		}

		public string SymbolDisplay
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDisplay, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.ParseDate(ExpDate, out DateTime when)) {
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
						_data.Set(OrderFieldIDs.symbolDisplay, value);
				}

				return value;
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.ParseDate(ExpDate, out DateTime when))
							{
								value = string.Join(" ",
									Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							}
							else
							{
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

		public abstract string Underlying
		{
			get;
		}

		public abstract string ExpDate
		{
			get;
		}

		public abstract string StrikePrice
		{
			get;
		}

		public string GetStrikFormat(double strikePrice)
		{
			string str = strikePrice.ToString("F7");
			string[] strs = str.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

			if (strs.Length == 2)
			{
				return "F" + strs[1].TrimEnd(new char[] { '0' }).Length.ToString();
			}
			return "F2";
		}

		public abstract string CallPut
		{
			get;
		}

		public abstract string SecType
		{
			get;
		}

		public abstract string Trader
		{
			get;
		}

		public abstract string ClearingAcct
		{
			get;
		}

		public abstract string OpenClose
		{
			get;
		}

		public BaseOrder()
		{
		}
	}

	public abstract class BasePosition
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		protected void Merge(BasePosition other)
		{
			_data.Merge(other._data);
		}

		public abstract bool HasOrders
		{
			get;
		}

		public abstract List<string> OmTags
		{
			get;
			set;
		}

		private Dictionary<string, ROCExecution> _trades;
		public Dictionary<string, ROCExecution> Trades
		{
			get
			{
				if (_trades == null)
				{
					_trades = new Dictionary<string, ROCExecution>();
				}
				return _trades;
			}
			set
			{
				_trades = value;
			}
		}

		public string PositionKey
		{
			get
			{
				string value;
				if (!_data.TryGet(PositionFieldIDs.positionKey, out value))
				{
					if (ClearingAcct.Length > 5)
					{
						value = string.Join("|", 
							SymbolDetail,
							Trader,
							ClearingAcct.Substring(0, 5));
					}
					else
					{
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

		public abstract string Symbol
		{
			get;
			set;
		}

		private string BaseSymbol
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (Underlying != "")
						{
							if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out _))
							{
								switch (Underlying.Substring(Underlying.Length - 2, 1))
								{
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
						if (Symbol != "")
						{
							if (int.TryParse(Symbol.Substring(Symbol.Length - 1, 1), out _))
							{
								switch (Symbol.Substring(Symbol.Length - 2, 1))
								{
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

		public string SymbolDetail
		{
			get
			{
				string value;
				DateTime when;

				if (!_data.TryGet(TradedFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case "O":
						case "I":
						case "M":
							if (!TimeFormats.ParseDate(ExpDate, out when)) {
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
							if (IsTPOS && GLOBAL.Future.PlusOne.Contains(BaseSymbol))
							{
								if (TimeFormats.ParseDate(ExpDate, out when)) {
									when = when.AddMonths(1);
									if (GLOBAL.MonthCodes.Map.ContainsKey(when.Month)) {
										value = BaseSymbol + GLOBAL.MonthCodes.Map[when.Month];
										value = value + Symbol.Substring(value.Length);
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
			set
			{
				_data.Set(TradedFieldIDs.symbolDetail, value);
			}
		}

		public string SymbolDisplay
		{
			get
			{
				string value;
				if (!_data.TryGet(TradedFieldIDs.symbolDisplay, out value))
				{
					switch (SecType)
					{
						case "O":
						case "I":
						case "M":
							if (TimeFormats.ParseDate(ExpDate, out DateTime when))
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									value = string.Format("{0} {1}({2}) {3} {4}",
										Underlying,
										when.AddMonths(1).ToString("yy MMM").ToUpper(), 
										when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
										StrikePrice.Replace(".00", ""),
										CallPut);
								}
								else
								{
									value = SymbolDetail;
								}
							}
							else
							{
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
			set
			{
				_data.Set(TradedFieldIDs.symbolDisplay, value);
			}
		}

		public bool IsTPOS
		{
			get
			{
				return _data.TryGet(PositionFieldIDs.TPOS.isTPOS, out bool value) ? value : false;
			}
			set
			{
				_data.Set(PositionFieldIDs.TPOS.isTPOS, value);
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.ParseDate(ExpDate, out DateTime when))
							{
								value = string.Join(" ",
									Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							}
							else
							{
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

		public abstract string Underlying
		{
			get;
			set;
		}

		public abstract string ExpDate
		{
			get;
			set;
		}

		public abstract string StrikePrice
		{
			get;
			set;
		}

		public string GetStrikFormat(double strikePrice)
		{
			string str = strikePrice.ToString("F7");
			string[] strs = str.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

			if (strs.Length == 2)
			{
				return "F" + strs[1].TrimEnd(new char[] { '0' }).Length.ToString();
			}
			return "F2";
		}

		public abstract string CallPut
		{
			get;
			set;
		}

		public abstract string SecType
		{
			get;
			set;
		}

		public abstract string Trader
		{
			get;
			set;
		}

		public abstract string ClearingAcct
		{
			get;
			set;
		}
	}

	public abstract class BaseSecurityInfo // : MultiTypedDictionary
	{
		public abstract string MDSource
		{
			get;
		}

		public abstract string MDSymbol
		{
			get;
		}

		public abstract double ContractSize
		{
			get;
		}

		public abstract double TickSize
		{
			get;
		}

		public abstract string SecType
		{
			get;
		}

		public abstract string LongName
		{
			get;
		}
	}

	public abstract class BaseOptionInfo // : MultiTypedDictionary
	{
		protected readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		public void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public abstract string SecType
		{
			get;
			set;
		}

		public abstract string Underlying
		{
			get;
		}

		public abstract string MDSymbol
		{
			get;
		}

		public abstract double ContractSize
		{
			get;
		}

		public abstract double TickSize
		{
			get;
			set;
		}

		public abstract string ExpDate
		{
			get;
		}

		public abstract string StrikePrice
		{
			get;
		}

		public abstract string CallPut
		{
			get;
		}

		private string BaseSymbol
		{
			get
			{
				switch (SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
					case CSVFieldIDs.SecurityTypes.OptionIndex:
						if (Underlying != "")
						{
							if (int.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out _))
							{
								switch (Underlying.Substring(Underlying.Length - 2, 1))
								{
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
						if (MDSymbol != "")
						{
							if (int.TryParse(MDSymbol.Substring(MDSymbol.Length - 1, 1), out _))
							{
								switch (MDSymbol.Substring(MDSymbol.Length - 2, 1))
								{
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

		public abstract string OptionSymbol
		{
			get;
		}

		public string SymbolDetail
		{
			get
			{
				string value;
				DateTime when;

				if (!_data.TryGet(SecurityFieldIDs.Option.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.ParseDate(ExpDate, out when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									value = string.Join(" ",
										Underlying,
										when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
										StrikePrice.Replace(".00", ""),
										CallPut);
								}
								else if (Underlying.Length == OptionSymbol.Length - 1 && Underlying == OptionSymbol.Substring(0, OptionSymbol.Length - 1))
									{
										value = string.Join(" ",
											OptionSymbol,
											when.ToString("yy MMM dd").ToUpper(),
											StrikePrice.Replace(".00", ""),
											CallPut);
									}
									else
									{
										value = string.Join(" ",
											Underlying,
											when.ToString("yy MMM dd").ToUpper(),
											StrikePrice.Replace(".00", ""),
											CallPut);
									}
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if (GLOBAL.Future.PlusOne.Contains(BaseSymbol) && TimeFormats.ParseDate(ExpDate, out when)) {
								when = when.AddMonths(1);
								if (GLOBAL.MonthCodes.Map.ContainsKey(when.Month)) {
									value = BaseSymbol + GLOBAL.MonthCodes.Map[when.Month];
									value = value + MDSymbol.Substring(value.Length);
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
		public string IMSymbolDetail
		{
			get
			{
				string value;
				if (!_data.TryGet(OrderFieldIDs.symbolDetail, out value))
				{
					switch (SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (TimeFormats.ParseDate(ExpDate, out DateTime when))
							{
								value = string.Join(" ",
									Underlying,
									when.ToString("yy MMM dd").ToUpper(),
									StrikePrice.Replace(".00", ""),
									CallPut);
							}
							else
							{
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

	public abstract class BaseSSFutureInfo // : MultiTypedDictionary
	{
		public string SecType
		{
			get
			{
				return CSVEx.CSVFieldIDs.SecurityTypes.Future;
			}
		}

		public abstract string MDSource
		{
			get;
		}

		public abstract string MDSymbol
		{
			get;
		}

		public abstract double ContractSize
		{
			get;
		}

		public abstract double TickSize
		{
			get;
			set;
		}

		public abstract string ExpDate
		{
			get;
		}

		public abstract string LongName
		{
			get;
		}
	}
}
