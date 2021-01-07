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

		public void Update(RDSEx.WEB.Execution trade, ROCOrder order)
		{
			setField(TradedFieldIDs.ROC.omTag, trade.omTag);
			setField(TradedFieldIDs.ROC.omExecTag, trade.omExecTag);
			setField(TradedFieldIDs.ROC.execTime, trade.execTime.ToLocalTime().ToOADate());
			setField(TradedFieldIDs.ROC.execQty, (long)trade.execQty);
			setField(TradedFieldIDs.ROC.execPrice, trade.execPrice);

			// Attach some of the order info with the trade
			setField(TradedFieldIDs.ROC.symbol, order.Symbol);
			setField(TradedFieldIDs.ROC.underlying, order.Underlying);
			setField(TradedFieldIDs.ROC.expDate, order.ExpDate);
			setField(TradedFieldIDs.ROC.strikePrice, order.StrikePrice);
			setField(TradedFieldIDs.ROC.callPut, order.CallPut);
			setField(TradedFieldIDs.ROC.secType, order.SecType);
			setField(TradedFieldIDs.ROC.trader, order.Trader);
			setField(TradedFieldIDs.ROC.localAcct, order.LocalAcct);
			setField(TradedFieldIDs.ROC.account, order.ClearingAcct);
			setField(TradedFieldIDs.ROC.openClose, order.OpenClose);
			setField(TradedFieldIDs.ROC.side, (long)order.Side);
			setField(TradedFieldIDs.ROC.destID, order.DestID);
		}

		protected void Update(RDSEx.WEB.TposTrade tposTrade)
		{
			setField(TradedFieldIDs.TPOS.activeState, tposTrade.m_ActiveState);
			setField(TradedFieldIDs.TPOS.clearingAccount, tposTrade.m_ClearingAccount);
			setField(TradedFieldIDs.TPOS.clearingStatus, tposTrade.m_ClearingStatus);
			setField(TradedFieldIDs.TPOS.commission, tposTrade.m_Commission);
			setField(TradedFieldIDs.TPOS.contraBroker, tposTrade.m_ContraBroker);
			setField(TradedFieldIDs.TPOS.contraFirm, tposTrade.m_ContraFirm);
			setField(TradedFieldIDs.TPOS.exchange, tposTrade.m_Exchange);
			setField(TradedFieldIDs.TPOS.expDate, tposTrade.m_ExpDate);
			setField(TradedFieldIDs.TPOS.extDescription, tposTrade.m_ExtDescription);
			setField(TradedFieldIDs.TPOS.extTradeID, tposTrade.m_ExtTradeID);
			setField(TradedFieldIDs.TPOS.lastModDate, tposTrade.m_LastModDate);
			setField(TradedFieldIDs.TPOS.lastModeReason, tposTrade.m_LastModReason);
			if (tposTrade.m_LastModReason.Contains("Deleted")) {
				setField(TradedFieldIDs.TPOS.lastModReasonID, 3);
			} else {
				setField(TradedFieldIDs.TPOS.lastModReasonID, tposTrade.m_LastModReasonID);
			}
			setField(TradedFieldIDs.TPOS.lastModTime, tposTrade.m_LastModTime);
			setField(TradedFieldIDs.TPOS.note, tposTrade.m_Note);
			setField(TradedFieldIDs.TPOS.portfolio, tposTrade.m_Portfolio);
			setField(TradedFieldIDs.TPOS.putCall, tposTrade.m_PutCall);
			if (ModReasonID == 3) {
				setField(TradedFieldIDs.TPOS.qty, 0);
				setField(TradedFieldIDs.TPOS.price, 0);
			} else {
				setField(TradedFieldIDs.TPOS.qty, tposTrade.m_Qty);
				setField(TradedFieldIDs.TPOS.price, tposTrade.m_Price);
			}
			setField(TradedFieldIDs.TPOS.secType, tposTrade.m_SecType);
			setField(TradedFieldIDs.TPOS.settleDate, tposTrade.m_SettleDate);
			setField(TradedFieldIDs.TPOS.shortSaleFlag, tposTrade.m_ShortSaleFlag);
			setField(TradedFieldIDs.TPOS.strike, tposTrade.m_Strike);
			setField(TradedFieldIDs.TPOS.symbol, tposTrade.m_Symbol);
			setField(TradedFieldIDs.TPOS.ticker, tposTrade.m_Ticker);
			setField(TradedFieldIDs.TPOS.tradeDate, tposTrade.m_TradeDate);
			setField(TradedFieldIDs.TPOS.tradeGroup, tposTrade.m_TradeGroup);
			setField(TradedFieldIDs.TPOS.tradeID, tposTrade.m_TradeID);
			setField(TradedFieldIDs.TPOS.traderAcronym, tposTrade.m_TraderAcronym);
			setField(TradedFieldIDs.TPOS.tradeSource, tposTrade.m_TradeSource);
			setField(TradedFieldIDs.TPOS.tradeTime, tposTrade.m_TradeTime);
			setField(TradedFieldIDs.TPOS.undSecType, tposTrade.m_UndSecType);
			setField(TradedFieldIDs.TPOS.undSymbol, tposTrade.m_UndSymbol);
			setField(TradedFieldIDs.TPOS.version, tposTrade.m_Version);

			if (SymbolDetail == "") {
				// Force a Symbol Reload
				_data.Remove(TradedFieldIDs.symbolDetail);
				setField(TradedFieldIDs.symbolDetail, SymbolDetail);
			}
		}

		protected void setField<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		// 3 means deleted
		public long ModReasonID => _data.TryGet(TradedFieldIDs.TPOS.lastModReasonID, out long value) ? value : 0;

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
				DateTime when;

				if (!_data.TryGet(TradedFieldIDs.symbolDetail, out string value)) {
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
