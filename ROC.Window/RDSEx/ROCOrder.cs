using System;
using System.Collections.Generic;
using System.Runtime.Serialization;
using MemoryStream = System.IO.MemoryStream;
using BinaryFormatter = System.Runtime.Serialization.Formatters.Binary.BinaryFormatter;

using CSVEx;
using Price = Common.Price;

namespace RDSEx
{
	[Serializable]
	public class ROCOrder : AssetShared, ISerializable
	{
		public readonly long AlgoType = 0;
		public readonly long AlgoDestID = 0;
		public double AvgPrice { get; private set; } = 0;
		public string ClearingID { get; private set; } = "";
		public readonly string ClientEcho = null;
		public readonly string CMTAFirmID = "";
		public readonly long CplxOrderType = 0;
		public long CumQty { get; private set; } = 0;
		public long DestID { get; private set; } = 0;
		public string EndTime {
			get {
				if (_endTime != DateTime.MinValue)
					return _endTime.ToString("MM-dd HH:mm:ss");
				return "";
			}
		}
		public readonly string ExecInstruction = "";
		public string ExShortName { get; private set; } = "";
		public string Firm { get; private set; } = "";
		public bool HasMaxFloor => !double.IsNaN(MaxFloor);
		public bool HasSide => Side != CSVFieldIDs.SideCodes.None;
		public bool HasTrades => (_trades != null) && (_trades.Count > 0);
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
		public long LeaveQty { get; private set; } = 0;
		public string LocalAcct { get; private set; } = "";
		protected override string MaturityDay => _maturityDay;
		public double MaxFloor { get; private set; } = double.NaN;
		public string OmTag { get; private set; } = "";
		public DateTime OmTime { get; private set; } = DateTime.Today;
		public string OpenClose { get; private set; } = "";
		public DateTime OrderExpiresDate { get; private set; } = DateTime.MaxValue;
		public Price OrderPrice { get; private set; } = 0;
		public long OrderType { get; private set; } = 0;
		public readonly double OriginalPrice = 0;
		public long OriginalShares { get; private set; } = 0;
		public string Owner { get; private set; } = "";
		public readonly string ParentTag = null;
		public double PegPrice { get; private set; } = 0;
		public readonly string ProgramTrade = "";
		public long Qty { get; private set; } = 0;
		public readonly string SecurityDefinition = "";
		public long Side { get; private set; } = CSVFieldIDs.SideCodes.None;
		public long Status { get; private set; } = 0;
		public double StopPrice { get; private set; } = 0;
		public string Tag { get; private set; } = "";
		public string Text { get; private set; } = "";
		public long TIF { get; private set; } = 0;
		public string TradeFor { get; private set; } = "";
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
				ExpDate = sval;
			if (csv.TryGetValue(CSVFieldIDs.MaturityDay, out sval))
				_maturityDay = sval;
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
				OrderPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.OriginalPrice, out dval))
				OriginalPrice = dval;
			if (csv.TryGetValue(CSVFieldIDs.ExecPrice, out dval))
				OrderPrice = dval;
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

		public void Update(RDSEx.WEB.Order webOrder)
		{
			AvgPrice = webOrder.avgPrice;
			CallPut = webOrder.callPut;
			ClearingAcct = webOrder.clearingAccount;
			ClearingID = webOrder.clearingID;
			CumQty = webOrder.cumQty;
			DestID = webOrder.destID;
			ExShortName = webOrder.ex_short_name;
			ExpDate = webOrder.expDate;
			Firm = webOrder.firm;
			Instructions = webOrder.instructions;
			LeaveQty = webOrder.leavesQty;
			LocalAcct = webOrder.localAcct;
			if (double.TryParse(webOrder.maxFloor, out double dval))
				MaxFloor = dval;
			OmTag = webOrder.omTag;
			OmTime = webOrder.omTime.ToLocalTime();
			OpenClose = webOrder.openClose;
			OrderExpiresDate = webOrder.orderExpiresDate;
			OrderPrice = webOrder.price;
			OrderType = webOrder.orderType;
			OriginalShares = webOrder.originalShares;
			Owner = webOrder.owner;
			Qty = webOrder.qty;
			SecType = webOrder.secType;
			Side = webOrder.side;
			Status = webOrder.status;
			StopPrice = webOrder.stopPrice;
			StrikePrice = webOrder.strikePrice;
			Symbol = webOrder.symbol;
			Tag = webOrder.tag;
			Text = webOrder.text;
			TIF = webOrder.tif;
			TradeFor = webOrder.tradeFor;
			Trader = webOrder.trader;
			Underlying = webOrder.underlying;
		}

		public void ApplySecinfo(string symbol, double tickSize, double contractSize)
		{
			if (string.IsNullOrEmpty(Symbol))
				Symbol = symbol;
			TickSize = tickSize;
			ContractSize = contractSize;
		}

		public void AddTrade(string key, ROCTrade trade)
		{
			if (_trades == null) {
				_trades = new Dictionary<string, ROCTrade>();
				_trades.Add(key, trade);
			} else {
				_trades[key] = trade;
			}
		}

		public bool TryGetTrade(string key, out ROCTrade trade)
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
					original.OrderPrice = update.OrderPrice;
					original.Qty = update.Qty;
					original.StopPrice = update.StopPrice;
					original.PegPrice = update.PegPrice;
					original.AvgPrice = update.AvgPrice;
				} else {
					// Container back fill
					update.Symbol = original.Symbol;
					if (original.OrderPrice != 0) {
						update.OrderPrice = original.OrderPrice;
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

		#region - ISerialization -

		// Deserialization constructor.
		protected ROCOrder(SerializationInfo info, StreamingContext context) : base(info, context)
		{
			AlgoType = info.GetInt64("AlgoType");
			AlgoDestID = info.GetInt64("AlgoDestID");
			AvgPrice = info.GetDouble("AvgPrice");
			ClearingID = info.GetString("ClearingID");
			ClientEcho = info.GetString("ClientEcho");
			CMTAFirmID = info.GetString("CMTAFirmID");
			CplxOrderType = info.GetInt64("CplxOrderType");
			CumQty = info.GetInt64("CumQty");
			DestID = info.GetInt64("DestID");
			_endTime = info.GetDateTime("EndTime");
			ExecInstruction = info.GetString("ExecInstruction");
			ExShortName = info.GetString("ExShortName");
			Firm = info.GetString("Firm");
			Instructions = info.GetString("Instructions");
			LeaveQty = info.GetInt64("LeaveQty");
			LocalAcct = info.GetString("LocalAcct");
			_maturityDay = info.GetString("MaturityDay");
			MaxFloor = info.GetDouble("MaxFloor");
			OmTag = info.GetString("OmTag");
			OmTime = info.GetDateTime("OmTime");
			OpenClose = info.GetString("OpenClose");
			OrderExpiresDate = info.GetDateTime("OrderExpiresDate");
			OrderPrice = info.GetDouble("OrderPrice");
			OrderType = info.GetInt64("OrderType");
			OriginalPrice = info.GetDouble("OriginalPrice");
			OriginalShares = info.GetInt64("OriginalShares");
			Owner = info.GetString("Owner");
			ParentTag = info.GetString("ParentTag");
			PegPrice = info.GetDouble("PegPrice");
			ProgramTrade = info.GetString("ProgramTrade");
			Qty = info.GetInt64("Qty");
			SecurityDefinition = info.GetString("SecurityDefinition");
			Side = info.GetInt64("Side");
			Status = info.GetInt64("Status");
			StopPrice = info.GetDouble("StopPrice");
			Tag = info.GetString("Tag");
			Text = info.GetString("Text");
			TIF = info.GetInt64("TIF");
			TradeFor = info.GetString("TradeFor");

			int count = info.GetInt32("TradeCount");
			if (count > 0) {
				byte[] data = (byte[])info.GetValue("Trades", typeof(byte[]));
				MemoryStream stream = new MemoryStream(data);
				BinaryFormatter formatter = new BinaryFormatter(null, context);
				_trades = (Dictionary<string, ROCTrade>)formatter.Deserialize(stream);
			}
		}

		// Serialization.
		public new void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context); // Serialize AssetShared first.

			info.AddValue("AlgoType", AlgoType);
			info.AddValue("AlgoDestID", AlgoDestID);
			info.AddValue("AvgPrice", AvgPrice);
			info.AddValue("ClearingID", ClearingID);
			info.AddValue("ClientEcho", ClientEcho);
			info.AddValue("CMTAFirmID", CMTAFirmID);
			info.AddValue("CplxOrderType", CplxOrderType);
			info.AddValue("CumQty", CumQty);
			info.AddValue("DestID", DestID);
			info.AddValue("EndTime", _endTime);
			info.AddValue("ExecInstruction", ExecInstruction);
			info.AddValue("ExShortName", ExShortName);
			info.AddValue("Firm", Firm);
			info.AddValue("Instructions", Instructions);
			info.AddValue("LeaveQty", LeaveQty);
			info.AddValue("LocalAcct", LocalAcct);
			info.AddValue("MaturityDay", _maturityDay);
			info.AddValue("MaxFloor", MaxFloor);
			info.AddValue("OmTag", OmTag);
			info.AddValue("OmTime", OmTime);
			info.AddValue("OpenClose", OpenClose);
			info.AddValue("OrderExpiresDate", OrderExpiresDate);
			info.AddValue("OrderPrice", OrderPrice);
			info.AddValue("OrderType", OrderType);
			info.AddValue("OriginalPrice", OriginalPrice);
			info.AddValue("OriginalShares", OriginalShares);
			info.AddValue("Owner", Owner);
			info.AddValue("ParentTag", ParentTag);
			info.AddValue("PegPrice", PegPrice);
			info.AddValue("ProgramTrade", ProgramTrade);
			info.AddValue("Qty", Qty);
			info.AddValue("SecurityDefinition", SecurityDefinition);
			info.AddValue("Side", Side);
			info.AddValue("Status", Status);
			info.AddValue("StopPrice", StopPrice);
			info.AddValue("Tag", Tag);
			info.AddValue("Text", Text);
			info.AddValue("TIF", TIF);
			info.AddValue("TradeFor", TradeFor);

			// Serialize _trades.
			if ((_trades == null) || (_trades.Count == 0)) {
				info.AddValue("TradeCount", 0, typeof(int));
			} else {
				info.AddValue("TradeCount", _trades.Count);

				int capacity = _trades.Count * 512; // 1/2k should about cover string key + TOCTrade value.
				MemoryStream stream = new MemoryStream(capacity);
				BinaryFormatter formatter = new BinaryFormatter(null, context);
				formatter.Serialize(stream, _trades);
				info.AddValue("Trades", stream.ToArray());
			}
		}

		#endregion // - ISerialization -

		#region - private members -
		private DateTime _endTime = DateTime.MinValue;
		private string _maturityDay= "";
		private Dictionary<string, ROCTrade> _trades = null;

		#endregion // - private members -
	}
}
