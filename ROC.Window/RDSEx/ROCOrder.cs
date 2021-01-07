using System;
using System.Collections.Generic;
using System.Diagnostics;

using CSVEx;

namespace RDSEx
{
	[Serializable]
	public class ROCOrder
	{
		public long AlgoType { get; private set; } = 0;
		public long AlgoDestID { get; private set; } = 0;
		public double AvgPrice { get; private set; } = 0;
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
								}
							}
						}
						break;
				}
				return "";
			}
		}
		public string CallPut { get; private set; } = "";
		public string ClearingAcct { get; private set; } = "";
		public string ClearingID { get; private set; } = "";
		public string ClientEcho { get; private set; } = null;
		public string CMTAFirmID { get; private set; } = "";
		public double ContractSize {
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
			set => _contractSize = value;
		}
		public long CplxOrderType { get; private set; } = 0;
		public long CumQty { get; private set; } = 0;
		public long DestID { get; private set; } = 0;
		public double DisplayFactor { get; private set; } = 1;
		public string EndTime {
			get {
				if (_endTime != DateTime.MinValue)
					return _endTime.ToString("MM-dd HH:mm:ss");
				return "";
			}
		}
		public string ExecInstruction { get; private set; } = "";
		public double ExecPrice { get; private set; } = 0;
		public string ExpDate {
			get {
				if (_expDate == null)
					return "";
				else if ((Underlying != null) && (MaturityDay != null))
					return _expDate + MaturityDay;
				return _expDate;
			}
		}
		public string ExShortName { get; private set; } = "";
		public string Firm { get; private set; } = "";
		public bool HasMaxFloor => !double.IsNaN(MaxFloor);
		public bool HasSide => Side != CSVFieldIDs.SideCodes.None;
		public bool HasTrades => (_trades != null) && (_trades.Count > 0);
		public string IMSymbolDetail {
			// ROC to IM, Use this to check againist SecInfo
			get {
				if (_imSymbolDetail == null) {
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
							} else {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							}
							break;
					}

					if (_imSymbolDetail == null)
						_imSymbolDetail = Symbol;

				}

				return _imSymbolDetail;
			}
		}
		public string Instructions { get; private set; } = "";
		public bool IsActive {
			get {
				switch (Status) {
					case CSVFieldIDs.StatusCodes.Busted:
					case CSVFieldIDs.StatusCodes.Expired:
					case CSVFieldIDs.StatusCodes.Rejected:
					case CSVFieldIDs.StatusCodes.Canceled:
					case CSVFieldIDs.StatusCodes.Filled:
					case CSVFieldIDs.StatusCodes.FilledAndCancelled:
					case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
						return false;
				}
				return true;
			}
		}
		public bool IsMarket {
			get {
				switch (OrderType) {
					case CSVFieldIDs.OrderTypes.Market:
					case CSVFieldIDs.OrderTypes.MarketAtOpen:
					case CSVFieldIDs.OrderTypes.MarketOnClose:
						return true;
				}
				return false;
			}
		}
		public bool IsOptionOnFuture {
			get {
				try {
					if ((Underlying != null) && (Underlying.Length > 3)) {
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
									return true;
								default:
									break;
							}
						}
					}
				} catch (Exception ex) {
					if (Debugger.IsAttached) {
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return false;
			}
		}
		public long LeaveQty { get; private set; } = 0;
		public string LocalAcct { get; private set; } = "";
		public string MaturityDay { get; private set; } = "";
		public double MaxFloor { get; private set; } = double.NaN;
		public string OmTag { get; private set; } = "";
		public DateTime OmTime { get; private set; } = DateTime.Today;
		public string OpenClose { get; private set; } = "";
		public DateTime OrderExpiresDate { get; private set; } = DateTime.MaxValue;
		public long OrderType { get; private set; } = 0;
		public double OriginalPrice { get; private set; } = 0;
		public long OriginalShares { get; private set; } = 0;
		public string Owner { get; private set; } = "";
		public string ParentTag { get; private set; } = null;
		public double PegPrice { get; private set; } = 0;
		public double Price { get; private set; } = 0;
		public string ProgramTrade { get; private set; } = "";
		public string PositionKey {
			get {
				if (_positionKey == null) {
					if (ClearingAcct.Length > 5)
						_positionKey = string.Join("|", SymbolDetail, Trader, ClearingAcct.Substring(0, 5));
					else
						_positionKey = string.Join("|", SymbolDetail, Trader, ClearingAcct);
				}
				return _positionKey;
			}
		}
		public long Qty { get; private set; } = 0;
		public string SecType { get; private set; } = "";
		public string SecurityDefinition { get; private set; } = "";
		public long Side { get; private set; } = CSVFieldIDs.SideCodes.None;
		public long Status { get; private set; } = 0;
		public double StopPrice { get; private set; } = 0;
		public double StrikePrice { get; private set; } = 0;
		public string Symbol { get; private set; } = "";
		public string SymbolDetail {
			get {
				if (_symbolDetail == null) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.TryParse(ExpDate, out DateTime when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else {
								string prefix;
								if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
									prefix = Underlying;
									when = when.AddMonths(1);
								} else if (Underlying.Length == Symbol.Length - 1 && Underlying == Symbol.Substring(0, Symbol.Length - 1)) {
									prefix = Symbol;
								} else {
									prefix = Underlying;
								}

								_symbolDetail = string.Join(" ", prefix,
									when.AddMonths(1).ToString("yy MMM dd").ToUpper(),
									StrikePrice,
									CallPut);
							}

							break;
					}

					if (_symbolDetail == null)
						_symbolDetail = Symbol;
				}

				return _symbolDetail;
			}
		}
		public string SymbolDisplay {
			get {
				if (_symbolDisplay == null) {
					switch (SecType) {
						case CSVFieldIDs.SecurityTypes.Option:
						case CSVFieldIDs.SecurityTypes.OptionFuture:
						case CSVFieldIDs.SecurityTypes.OptionIndex:
							if (!TimeFormats.TryParse(ExpDate, out DateTime when)) {
								Debug.Assert(!Debugger.IsAttached, ExpDate);
							} else if (GLOBAL.OptionOnFuture.PlusOne.Contains(BaseSymbol)) {
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
				}

				return _symbolDisplay;
			}
		}
		public string Tag { get; private set; } = "";
		public string Text { get; private set; } = "";
		public double TickSize { get; private set; } = 0.01;
		public long TIF { get; private set; } = 0;
		public string TradeFor { get; private set; } = "";
		public string Trader { get; private set; } = "";
		public string Underlying { get; private set; } = "";
		public bool UpdateOrder => CplxOrderType == CSVFieldIDs.CplxOrderTypes.Container;

		public ROCOrder(CSV csv) : base()
		{
			string sval;
			if (csv.TryGetValue(CSVFieldIDs.CallPut, out sval))
				CallPut = sval;
			if (csv.TryGetValue(CSVFieldIDs.ClearingAcct, out sval))
				ClearingAcct = sval;
			if (csv.TryGetValue(CSVFieldIDs.ClearingID, out sval))
				ClearingID = sval;
			if (csv.TryGetValue(CSVFieldIDs.ExpDate, out sval))
				_expDate = sval;
			if (csv.TryGetValue(CSVFieldIDs.MaturityDay, out sval))
				MaturityDay = sval;
			if (csv.TryGetValue(CSVFieldIDs.Firm, out sval))
				Firm = sval;
			if (csv.TryGetValue(CSVFieldIDs.Instructions, out sval))
				Instructions = sval;
			if (csv.TryGetValue(CSVFieldIDs.ExecutionInstruction, out sval))
				ExecInstruction = sval;
			if (csv.TryGetValue(CSVFieldIDs.LocalAcct, out sval))
				LocalAcct = sval;
			if (csv.TryGetValue(CSVFieldIDs.OmTag, out sval))
				OmTag = sval;
			if (csv.TryGetValue(CSVFieldIDs.OpenClose, out sval))
				OpenClose = sval;
			if (csv.TryGetValue(CSVFieldIDs.Owner, out sval))
				Owner = sval;
			if (csv.TryGetValue(CSVFieldIDs.SecType, out sval))
				SecType = sval;
			if (csv.TryGetValue(CSVFieldIDs.Symbol, out sval))
				Symbol = sval;
			if (csv.TryGetValue(CSVFieldIDs.Tag, out sval))
				Tag = sval;
			if (csv.TryGetValue(CSVFieldIDs.Text, out sval))
				Text = sval;
			if (csv.TryGetValue(CSVFieldIDs.TradeFor, out sval))
				TradeFor = sval;
			if (csv.TryGetValue(CSVFieldIDs.Trader, out sval))
				Trader = sval;
			if (csv.TryGetValue(CSVFieldIDs.Underlying, out sval))
				Underlying = sval;
			if (csv.TryGetValue(CSVFieldIDs.SecurityDefinition, out sval))
				SecurityDefinition = sval;
			if (csv.TryGetValue(CSVFieldIDs.ParentTag, out sval))
				ParentTag = sval;
			if (csv.TryGetValue(CSVFieldIDs.ClientEcho, out sval))
				ClientEcho = sval;
			if (csv.TryGetValue(CSVFieldIDs.ProgramTrade, out sval))
				ProgramTrade = sval;
			if (csv.TryGetValue(CSVFieldIDs.CMTAAccount, out sval))
				CMTAFirmID = sval;

			double dval;
			if (csv.TryGetValue(CSVFieldIDs.AveragePrice, out dval))
				AvgPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.Price, out dval))
				Price = dval;
			if (csv.TryGetValue(CSVFieldIDs.OriginalPrice, out dval))
				OriginalPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.ExecPrice, out dval))
				ExecPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.StopPrice, out dval))
				StopPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.PegOffsetPrice, out dval))
				PegPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.StrikePrice, out dval))
				StrikePrice = dval;

			DateTime wval;
			if (csv.TryGetValue(CSVFieldIDs.ExecutionTime, out wval))
				OmTime = wval;
			else if (csv.TryGetValue(CSVFieldIDs.OmTime, out wval))
				OmTime = wval;
			if (csv.TryGetValue(CSVFieldIDs.EndTime, out wval))
				_endTime = wval;
			if (csv.TryGetValue(CSVFieldIDs.OrderExpirationDateTime, out wval))
				OrderExpiresDate = wval;

			long lval;
			if (csv.TryGetValue(CSVFieldIDs.CplxOrderType, out lval))
				CplxOrderType = lval;
			if (csv.TryGetValue(CSVFieldIDs.CumShares, out lval))
				CumQty = lval;
			if (csv.TryGetValue(CSVFieldIDs.ExchangeID, out lval))
				DestID = lval;
			if (csv.TryGetValue(CSVFieldIDs.AlgoExchangeID, out lval))
				AlgoDestID = lval;
			if (csv.TryGetValue(CSVFieldIDs.Floor, out lval))
				MaxFloor = lval;
			if (csv.TryGetValue(CSVFieldIDs.LeaveShares, out lval))
				LeaveQty = lval;
			if (csv.TryGetValue(CSVFieldIDs.OriginalShares, out lval))
				OriginalShares = lval;
			if (csv.TryGetValue(CSVFieldIDs.Shares, out lval))
				Qty = lval;
			if (csv.TryGetValue(CSVFieldIDs.Side, out lval))
				Side = lval;
			if (csv.TryGetValue(CSVFieldIDs.Status, out lval))
				Status = lval;
			if (csv.TryGetValue(CSVFieldIDs.TIF, out lval))
				TIF = lval;
			if (csv.TryGetValue(CSVFieldIDs.Type, out lval))
				OrderType = lval;
			if (csv.TryGetValue(CSVFieldIDs.AlgoType, out lval))
				AlgoType = lval;
		}

		public ROCOrder(RDSEx.WEB.Order order) : base()
		{
			Update(order);
		}

		public void Update(RDSEx.WEB.Order order)
		{
			AvgPrice = order.avgPrice;
			CallPut = order.callPut;
			ClearingAcct = order.clearingAccount;
			ClearingID = order.clearingID;
			CumQty = order.cumQty;
			DestID = order.destID;
			ExShortName = order.ex_short_name;
			_expDate = order.expDate;
			Firm = order.firm;
			Instructions = order.instructions;
			LeaveQty = order.leavesQty;
			LocalAcct = order.localAcct;
			if (double.TryParse(order.maxFloor, out double dval))
				MaxFloor = dval;
			OmTag = order.omTag;
			OmTime = order.omTime.ToLocalTime();
			OpenClose = order.openClose;
			OrderExpiresDate = order.orderExpiresDate;
			OrderType = order.orderType;
			OriginalShares = order.originalShares;
			Owner = order.owner;
			Price = order.price;
			Qty = order.qty;
			SecType = order.secType;
			Side = order.side;
			Status = order.status;
			StopPrice = order.stopPrice;
			StrikePrice = order.strikePrice;
			Symbol = order.symbol;
			Tag = order.tag;
			Text = order.text;
			TIF = order.tif;
			TradeFor = order.tradeFor;
			Trader = order.trader;
			Underlying = order.underlying;
		}

		public void UpdateFromSecinfo(string symbol, double tickSize, double contractSize)
		{
			if (string.IsNullOrEmpty(Symbol))
				Symbol = symbol;
			TickSize = tickSize;
			ContractSize = contractSize;
		}

		public void AddTrade(string key, ROCExecution trade)
		{
			if (_trades == null) {
				_trades = new Dictionary<string, ROCExecution>();
				_trades.Add(key, trade);
			} else {
				_trades[key] = trade;
			}
		}

		public bool TryGetTrade(string key, out ROCExecution trade)
		{
			if (_trades == null) {
				trade = null;
				return false;
			}
			return _trades.TryGetValue(key, out trade);
		}

		public void MakeRocStatus()
		{
			if (Status == CSVFieldIDs.StatusCodes.Canceled && (CumQty > 0)) { // Cancelled and had some filled already
				Status = CSVFieldIDs.StatusCodes.FilledAndCancelled;  //Filled & Cancelled
			} else if (Status == CSVFieldIDs.StatusCodes.Replaced && (LeaveQty == 0)) { // Replaced to 0 LeaveQty
				Status = CSVFieldIDs.StatusCodes.ReplacedAndFilled;  //Replaced and Filled
			}
		}

		public static void Merge(ROCOrder original, ROCOrder update)
		{
			if (original.IsActive) {
				original.Status = update.Status;
				original.LeaveQty = update.LeaveQty;
				original.CumQty = update.CumQty;

				if (update.UpdateOrder) {
					original.Qty = update.Qty;
					original.Price = update.Price;
					original.StopPrice = update.StopPrice;
					original.PegPrice = update.PegPrice;
					original.AvgPrice = update.AvgPrice;
				} else {
					// Container back fill
					update.Symbol = original.Symbol;
					if (original.Price != 0) {
						update.Price = original.Price;
					}
					if (original.StopPrice != 0) {
						update.StopPrice = original.StopPrice;
					}
					if (original.PegPrice != 0) {
						update.PegPrice = original.PegPrice;
					}
					if (original.AvgPrice != 0) {
						update.AvgPrice = original.AvgPrice;
					}
				}

				original.OrderType = update.OrderType;
				original.TIF = update.TIF;
				original.OmTime = update.OmTime;
				original.Text = update.Text;
			}
		}

		public static string GetStrikFormat(double strikePrice)
		{
			string str = strikePrice.ToString("F7");
			string[] strs = str.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

			if (strs.Length == 2) {
				return "F" + strs[1].TrimEnd(new char[] { '0' }).Length.ToString();
			}
			return "F2";
		}

		#region - private members -

		private double _contractSize = 0;
		private DateTime _endTime = DateTime.MinValue;
		private string _expDate = null;
		private string _imSymbolDetail = null;
		private string _positionKey = null;
		private string _symbolDetail = null;
		private string _symbolDisplay = null;
		private Dictionary<string, ROCExecution> _trades;

		#endregion // - private members -
	}
}
