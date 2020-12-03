using System;
using System.Collections.Generic;

using DictionaryEx;
using System.Diagnostics;
using CSVEx;
using System.Runtime.Serialization;

namespace RDSEx
{
	public abstract class BaseTypedAccountMap : MutiTypedDictionary
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
	public abstract class BaseExecution : MutiTypedDictionary
	{
		public abstract string TradeID
		{
			get;
		}

		public string PositionKey
		{
			get
			{
				if (!Strings.ContainsKey(TradedFieldIDs.positionKey))
				{
					string key;
					if (ClearingAcct.Length > 5)
					{
						key = String.Concat(new object[] {
							SymbolDetail, "|",
							Trader,	"|",
							ClearingAcct.Substring(0, 5) });
					}
					else
					{
						key = String.Concat(new object[] {
							SymbolDetail, "|",
							Trader,	"|",
							ClearingAcct });
					}
					Strings.Add(TradedFieldIDs.positionKey, key);
				}

				return Strings[TradedFieldIDs.positionKey];
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
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						if (Underlying != "")
						{
							int num = 0;
							if (Int32.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out num))
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
					case CSVFieldIDs.SecutrityTypes.Future:
						if (Symbol != "")
						{
							int num = 0;
							if (Int32.TryParse(Symbol.Substring(Symbol.Length - 1, 1), out num))
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
				if (!Strings.ContainsKey(TradedFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									detail = String.Concat(new object[] {
										Underlying,	" ",
										tDT.AddMonths(1).ToString("yy MMM dd").ToUpper(), " ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									if (Underlying.Length == Symbol.Length - 1 && Underlying == Symbol.Substring(0, Symbol.Length - 1))
									{
										detail = String.Concat(new object[] {
											Symbol,	" ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
									else
									{
										detail = String.Concat(new object[] {
											Underlying,	" ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						case CSVFieldIDs.SecutrityTypes.Future:
							if (IsTPOS && GLOBAL.Future.PlusOne.Contains(BaseSymbol))
							{
								tOk = false;
								tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
								tDT = tDT.AddMonths(1);
								if (GLOBAL.MonthCodes.Map.ContainsKey(tDT.Month))
								{
									detail = BaseSymbol + GLOBAL.MonthCodes.Map[tDT.Month];
									detail = detail + Symbol.Substring(detail.Length);
									break;
								}
							}
							detail = Symbol;
							break;
						default:
							detail = Symbol;
							break;
					}

					Strings.Add(TradedFieldIDs.symbolDetail, detail);
				}

				return Strings[TradedFieldIDs.symbolDetail];
			}
		}

		public string SymbolDisplay
		{
			get
			{
				if (!Strings.ContainsKey(TradedFieldIDs.symbolDisplay))
				{
					string display = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									display = String.Concat(new object[] {
										Underlying,	" ",
										tDT.AddMonths(1).ToString("yy MMM").ToUpper(), 
										"(",
										tDT.ToString("yy MMM dd").ToUpper(), ") ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									display = SymbolDetail;
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							display = SymbolDetail;
							break;
					}

					if (display == "")
					{
						return "";
					}
					Strings.Add(TradedFieldIDs.symbolDisplay, display);
				}

				return Strings[TradedFieldIDs.symbolDisplay];
			}
		}

		public bool IsTPOS
		{
			get
			{
				if (Bools.ContainsKey(TradedFieldIDs.TPOS.isTPOS))
				{
					return Bools[TradedFieldIDs.TPOS.isTPOS];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(TradedFieldIDs.TPOS.isTPOS, value);
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				if (!Strings.ContainsKey(OrderFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								detail = String.Concat(new object[] {
									Underlying,	" ",
									tDT.ToString("yy MMM dd").ToUpper(), " ",
									StrikePrice.Replace(".00", ""), " ",
									CallPut });
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							detail = Symbol;
							break;
					}

					Strings.Add(OrderFieldIDs.symbolDetail, detail);
				}

				return Strings[OrderFieldIDs.symbolDetail];
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

		public BaseExecution(SerializationInfo info, StreamingContext ctxt)
			: base(info, ctxt)
		{
		}
	}

	[Serializable]
	public abstract class BaseOrder : MutiTypedDictionary
	{
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
				if (!Strings.ContainsKey(OrderFieldIDs.positionKey))
				{
					string key;
					if (ClearingAcct.Length > 5)
					{
						key = String.Concat(new object[] {
							SymbolDetail, "|",
							Trader,	"|",
							ClearingAcct.Substring(0, 5) });
					}
					else
					{
						key = String.Concat(new object[] {
							SymbolDetail, "|",
							Trader,	"|",
							ClearingAcct });
					}
					Strings.Add(OrderFieldIDs.positionKey, key);
				}

				return Strings[OrderFieldIDs.positionKey];
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
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						if (Underlying != "")
						{
							int num = 0;
							if (Int32.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out num))
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
				if (!Strings.ContainsKey(OrderFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									detail = String.Concat(new object[] {
										Underlying,	" ",
										tDT.AddMonths(1).ToString("yy MMM dd").ToUpper(), " ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									if (Underlying.Length == Symbol.Length - 1 && Underlying == Symbol.Substring(0, Symbol.Length - 1))
									{
										detail = String.Concat(new object[] {
											Symbol,	" ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
									else
									{
										detail = String.Concat(new object[] {
											Underlying,	" ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							detail = Symbol;
							break;
					}

					Strings.Add(OrderFieldIDs.symbolDetail, detail);
				}

				return Strings[OrderFieldIDs.symbolDetail];
			}
		}

		public string SymbolDisplay
		{
			get
			{
				if (!Strings.ContainsKey(OrderFieldIDs.symbolDisplay))
				{
					string display = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									display = String.Concat(new object[] {
										Underlying,	" ", 
										tDT.AddMonths(1).ToString("yy MMM").ToUpper(), 
										"(",
										tDT.ToString("yy MMM dd").ToUpper(), ") ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									display = SymbolDetail;
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							display = SymbolDetail;
							break;
					}

					if (display == "")
					{
						return "";
					}
					Strings.Add(OrderFieldIDs.symbolDisplay, display);
				}

				return Strings[OrderFieldIDs.symbolDisplay];
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				if (!Strings.ContainsKey(OrderFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								detail = String.Concat(new object[] {
									Underlying,	" ",
									tDT.ToString("yy MMM dd").ToUpper(), " ",
									StrikePrice.Replace(".00", ""), " ",
									CallPut });
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							detail = Symbol;
							break;
					}

					Strings.Add(OrderFieldIDs.symbolDetail, detail);
				}

				return Strings[OrderFieldIDs.symbolDetail];
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

		public BaseOrder(SerializationInfo info, StreamingContext ctxt)
			: base(info, ctxt)
		{
		}
	}

	public abstract class BasePosition : MutiTypedDictionary
	{
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
				if (!Strings.ContainsKey(PositionFieldIDs.positionKey))
				{
					string key;
					if (ClearingAcct.Length > 5)
					{
						key = String.Concat(new object[] {
							SymbolDetail, "|",
							Trader,	"|",
							ClearingAcct.Substring(0, 5) });
					}
					else
					{
						key = String.Concat(new object[] {
							SymbolDetail, "|",
							Trader,	"|",
							ClearingAcct });
					}
					Strings.Add(PositionFieldIDs.positionKey, key);
				}

				return Strings[PositionFieldIDs.positionKey];
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
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						if (Underlying != "")
						{
							int num = 0;
							if (Int32.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out num))
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
					case CSVFieldIDs.SecutrityTypes.Future:
						if (Symbol != "")
						{
							int num = 0;
							if (Int32.TryParse(Symbol.Substring(Symbol.Length - 1, 1), out num))
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
				if (!Strings.ContainsKey(TradedFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case "O":
						case "I":
						case "M":
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									detail = String.Concat(new object[] {
										Underlying,	" ",
										tDT.AddMonths(1).ToString("yy MMM dd").ToUpper(), " ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									if (Underlying.Length == Symbol.Length - 1 && Underlying == Symbol.Substring(0, Symbol.Length - 1))
									{
										detail = String.Concat(new object[] {
											Symbol,	" ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
									else
									{
										detail = String.Concat(new object[] {
										Underlying,	" ",
										tDT.ToString("yy MMM dd").ToUpper(), " ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
									}
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						case CSVFieldIDs.SecutrityTypes.Future:
							if (IsTPOS && GLOBAL.Future.PlusOne.Contains(BaseSymbol))
							{
								tOk = false;
								tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
								tDT = tDT.AddMonths(1);
								if (GLOBAL.MonthCodes.Map.ContainsKey(tDT.Month))
								{
									detail = BaseSymbol + GLOBAL.MonthCodes.Map[tDT.Month];
									detail = detail + Symbol.Substring(detail.Length);
									break;
								}
							}
							detail = Symbol;
							break;
						default:
							detail = Symbol;
							break;
					}

					Strings.Add(TradedFieldIDs.symbolDetail, detail);
				}

				return Strings[TradedFieldIDs.symbolDetail];
			}
			set
			{
				Update(TradedFieldIDs.symbolDetail, value);
			}
		}

		public string SymbolDisplay
		{
			get
			{
				if (!Strings.ContainsKey(TradedFieldIDs.symbolDisplay))
				{
					string display = "";
					switch (SecType)
					{
						case "O":
						case "I":
						case "M":
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									display = String.Concat(new object[] {
										Underlying,	" ",
										tDT.AddMonths(1).ToString("yy MMM").ToUpper(), 
										"(",
										tDT.AddMonths(1).ToString("yy MMM dd").ToUpper(), ") ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									display = SymbolDetail;
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							display = SymbolDetail;
							break;
					}

					if (display == "")
					{
						return "";
					}
					Strings.Add(TradedFieldIDs.symbolDisplay, display);
				}

				return Strings[TradedFieldIDs.symbolDisplay];
			}
			set
			{
				Update(TradedFieldIDs.symbolDisplay, value);
			}
		}

		public bool IsTPOS
		{
			get
			{
				if (Bools.ContainsKey(PositionFieldIDs.TPOS.isTPOS))
				{
					return Bools[PositionFieldIDs.TPOS.isTPOS];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(PositionFieldIDs.TPOS.isTPOS, value);
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				if (!Strings.ContainsKey(OrderFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								detail = String.Concat(new object[] {
									Underlying,	" ",
									tDT.ToString("yy MMM dd").ToUpper(), " ",
									StrikePrice.Replace(".00", ""), " ",
									CallPut });
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							detail = Symbol;
							break;
					}

					Strings.Add(OrderFieldIDs.symbolDetail, detail);
				}

				return Strings[OrderFieldIDs.symbolDetail];
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

	public abstract class BaseSecurityInfo : MutiTypedDictionary
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

	public abstract class BaseOptionInfo : MutiTypedDictionary
	{
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
					case CSVFieldIDs.SecutrityTypes.Option:
					case CSVFieldIDs.SecutrityTypes.OptionFuture:
					case CSVFieldIDs.SecutrityTypes.OptionIndex:
						if (Underlying != "")
						{
							int num = 0;
							if (Int32.TryParse(Underlying.Substring(Underlying.Length - 1, 1), out num))
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
					case CSVFieldIDs.SecutrityTypes.Future:
						if (MDSymbol != "")
						{
							int num = 0;
							if (Int32.TryParse(MDSymbol.Substring(MDSymbol.Length - 1, 1), out num))
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
				if (!Strings.ContainsKey(SecurityFieldIDs.Option.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol))
								{
									detail = String.Concat(new object[] {
										Underlying,	" ",
										tDT.AddMonths(1).ToString("yy MMM dd").ToUpper(), " ",
										StrikePrice.Replace(".00", ""), " ",
										CallPut });
								}
								else
								{
									if (Underlying.Length == OptionSymbol.Length - 1 && Underlying == OptionSymbol.Substring(0, OptionSymbol.Length - 1))
									{
										detail = String.Concat(new object[] {
											OptionSymbol, " ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
									else
									{
										detail = String.Concat(new object[] {
											Underlying,	" ",
											tDT.ToString("yy MMM dd").ToUpper(), " ",
											StrikePrice.Replace(".00", ""), " ",
											CallPut });
									}
								}
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						case CSVFieldIDs.SecutrityTypes.Future:
							if (GLOBAL.Future.PlusOne.Contains(BaseSymbol))
							{
								tOk = false;
								tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
								tDT = tDT.AddMonths(1);
								if (GLOBAL.MonthCodes.Map.ContainsKey(tDT.Month))
								{
									detail = BaseSymbol + GLOBAL.MonthCodes.Map[tDT.Month];
									detail = detail + MDSymbol.Substring(detail.Length);
									break;
								}
							}
							detail = MDSymbol;
							break;
						default:
							detail = MDSymbol;
							break;
					}

					Strings.Add(SecurityFieldIDs.Option.symbolDetail, detail);
				}

				return Strings[SecurityFieldIDs.Option.symbolDetail];
			}
		}

		// ROC to IM, Use this to check againist SecInfo
		public string IMSymbolDetail
		{
			get
			{
				if (!Strings.ContainsKey(OrderFieldIDs.symbolDetail))
				{
					string detail = "";
					switch (SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Option:
						case CSVFieldIDs.SecutrityTypes.OptionFuture:
						case CSVFieldIDs.SecutrityTypes.OptionIndex:
							bool tOk = false;
							DateTime tDT;
							tOk = DateTime.TryParseExact(ExpDate, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
							if (tOk)
							{
								detail = String.Concat(new object[] {
									Underlying,	" ",
									tDT.ToString("yy MMM dd").ToUpper(), " ",
									StrikePrice.Replace(".00", ""), " ",
									CallPut });
							}
							else
							{
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
						default:
							detail = MDSymbol;
							break;
					}

					Strings.Add(OrderFieldIDs.symbolDetail, detail);
				}

				return Strings[OrderFieldIDs.symbolDetail];
			}
		}
	}

	public abstract class BaseSSFutureInfo : MutiTypedDictionary
	{
		public string SecType
		{
			get
			{
				return CSVEx.CSVFieldIDs.SecutrityTypes.Future;
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
