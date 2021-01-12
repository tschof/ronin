using System;
using System.Data;
using System.Collections.Generic;

using RDSEx;
using SerializationEx;
using MarketData;

namespace ROC
{
	// Main Orders Collection
	public class HelperOrdersData : HelperDataBase
	{
		private DataTable _optionTypes;
		public DataTable OptionTypes {
			get {
				if (_optionTypes == null) {
					_optionTypes = CreateSimpleTable("Type", typeof(string), "Call", "Put");
					_optionTypes.PrimaryKey = new DataColumn[] { _optionTypes.Columns[0] };
					_optionTypes.DefaultView.Sort = "Type";
				}
				return _optionTypes;
			}
			set {
				_optionTypes = value;
			}
		}

		private DataTable _baseOrderTypes;
		public DataTable BaseOrderTypes {
			get {
				if (_baseOrderTypes == null) {
					_baseOrderTypes = CreateSimpleTable("Type", typeof(string), "MARKET", "LIMIT", "MOC", "MOO", "LOC", "STOP", "STOPLIMIT");
					_baseOrderTypes.DefaultView.Sort = "Type";
				}
				return _baseOrderTypes;
			}
		}

		private DataTable _stockOrderTypes;
		public DataTable StockOrderTypes {
			get {
				if (_stockOrderTypes == null) {
					_stockOrderTypes = CopySimpleTable(BaseOrderTypes, true, "OCO", "PEG MKT", "PEG PRI", "PEG MID", "IMBOO", "IMBOC", "IMBOI", "VWAP MKT", "VWAP LIM", "GVWAP");
					_stockOrderTypes.TableName = "STK";
				}
				return _stockOrderTypes;
			}
		}

		private DataTable _futureOrderTypes;
		public DataTable FutureOrderTypes {
			get {
				if (_futureOrderTypes == null) {
					_futureOrderTypes = CopySimpleTable(BaseOrderTypes, true, "OCO");
					_futureOrderTypes.TableName = "FUT";
				}
				return _futureOrderTypes;
			}
		}

		private DataTable _autoSpreadOrderTypes;
		public DataTable AutoSpreadOrderTypes {
			get {
				if (_autoSpreadOrderTypes == null) {
					_autoSpreadOrderTypes = CopySimpleTable(BaseOrderTypes, false, "LIMIT");
				}
				return _autoSpreadOrderTypes;
			}
		}

		private DataTable _optionOrderTypes;
		public DataTable OptionOrderTypes {
			get {
				if (_optionOrderTypes == null) {
					_optionOrderTypes = BaseOrderTypes.Copy();
				}
				return _optionOrderTypes;
			}
		}

		private DataTable _durations;
		public DataTable Durations {
			get {
				if (_durations == null) {
					_durations = CreateSimpleTable("Duration", typeof(string), "DAY", "GTC", "IOC", "FOK", "OPG");
					_durations.DefaultView.Sort = "Duration";
				}
				return _durations;
			}
			set {
				_durations = value;
			}
		}

		private DataTable _stockDurations;
		public DataTable StockDurations {
			get {
				if (_stockDurations == null) {
					_stockDurations = Durations.Copy();
				}
				return _stockDurations;
			}
		}

		private DataTable _autoSpreadDurations;
		public DataTable AutoSpreadDurations {
			get {
				if (_autoSpreadDurations == null) {
					_autoSpreadDurations = CopySimpleTable(Durations, false, "DAY");
				}
				return _autoSpreadDurations;
			}
		}

		private DataTable _stokAlgoTypes;
		public DataTable StokAlgoTypes {
			get {
				if (_stokAlgoTypes == null) {
					_stokAlgoTypes = CreateSimpleTable("AlgoType", typeof(string), "", "TWAP", "VWAP", "GVWAP");
					_stokAlgoTypes.PrimaryKey = new DataColumn[] { _durations.Columns["AlgoType"] };
					_stokAlgoTypes.DefaultView.Sort = "AlgoType";
				}
				return _stokAlgoTypes;
			}
		}

		private Dictionary<string, ROCOrder> _rocItems;
		public Dictionary<string, ROCOrder> RocItems {
			get {
				if (_rocItems == null) {
					_rocItems = new Dictionary<string, ROCOrder>();
				}
				return _rocItems;
			}
		}

		protected override string GetTableTitle() => "Orders";

		protected override string GetKeyColumnName() => "Tag";

		protected override (string, Type)[] GetTableColumns()
		{
			return new (string, Type)[] {
				("Tag", typeof(string)),
				("Symbol", typeof(string)),
				("SymbolDetail", typeof(string)),
				("SymbolDisplay", typeof(string)),
				("Status", typeof(long)),
				("Side", typeof(long)),
				("Qty", typeof(long)),
				("Price", typeof(double)),
				("StopPrice", typeof(double)),
				("PegPrice", typeof(double)),
				("LeaveQty", typeof(long)),
				("CumQty", typeof(long)),
				("AvgPrice", typeof(double)),
				("OrderType", typeof(long)),
				("TIF", typeof(long)),
				("AlgoType", typeof(long)),
				("AlgoEndTime", typeof(string)),
				("DestID", typeof(long)),
				("OmTime", typeof(DateTime)),
				("ClearingAcct", typeof(string)),
				("ContractSize", typeof(double)),
				("TickSize", typeof(double)),
				("DisplayFactor", typeof(double)),
				("OMTag", typeof(string)),
				("SecType", typeof(string)),
				("Text", typeof(string)),
				("ParentTag", typeof(string)),
				("Echo", typeof(string)),
				("CplxOrderType", typeof(long))
			};
		}

		protected override void OnCreateTable(DataTable table)
		{
			table.CaseSensitive = true;
		}

		// Used by RDS When it first got all the orders from DB
		public void DisplayOrders(Dictionary<string, ROCOrder> orders)
		{
			if (GLOBAL.HRDS.GotRocOrders) {
				lock (Table) {
					foreach (KeyValuePair<string, ROCOrder> entry in orders)
						DisplayOrder(entry.Value);
					
					string[] keys = new string[orders.Count];
					orders.Keys.CopyTo(keys, 0);
					foreach (string key in keys) {
						orders[key] = DisplayOrder(orders[key]);
					}
				}
			}
		}

		public ROCOrder DisplayOrder(ROCOrder order)
		{
			if (!RocItems.TryGetValue(order.Tag, out ROCOrder found)) {
				RocItems.Add(order.Tag, order);
			} else {
				// Update Only Active Orders
				ROCOrder.Merge(found, order);
			}

			if (TryAddKey(order.Tag)) {
				Table.Rows.Add(new object[] {
					order.Tag,
					order.Symbol,
					order.SymbolDetail,
					order.SymbolDisplay,
					order.Status,
					order.Side,
					order.Qty,
					order.OrderPrice.Value,
					order.StopPrice,
					order.PegPrice,
					order.LeaveQty,
					order.CumQty,
					order.AvgPrice,
					order.OrderType,
					order.TIF,
					order.AlgoType,
					order.EndTime,
					order.DestID,
					order.OmTime,
					order.ClearingAcct,
					order.ContractSize,
					order.TickSize,
					order.DisplayFactor,
					order.OmTag,
					order.SecType,
					order.Text,
					order.ParentTag,
					order.ClientEcho,
					order.CplxOrderType });
			} else {
				DataRowView[] rows = SearchView.FindRows(order.Tag);
				foreach (DataRowView row in rows) {
					row["Status"] = order.Status;
					row["LeaveQty"] = order.LeaveQty;
					row["CumQty"] = order.CumQty;

					if (RocItems[order.Tag].UpdateOrder) {
						row["Qty"] = order.Qty;
						row["Price"] = order.OrderPrice.Value;
						row["StopPrice"] = order.StopPrice;
						row["PegPrice"] = order.PegPrice;
						row["AvgPrice"] = order.AvgPrice;
					}

					row["OrderType"] = order.OrderType;
					row["TIF"] = order.TIF;
					row["AlgoType"] = order.AlgoType;
					row["OmTime"] = order.OmTime;
					row["Text"] = order.Text;
				}
			}

			return order;
		}

		public ROCOrder GetOrderByOrderID(string orderID)
		{
			return GLOBAL.HOrders.RocItems.TryGetValue(orderID, out ROCOrder value) ? value : null;
		}

		#region - Export & Import -

		public void Import(string username)
		{
			try {
				if (GLOBAL.OrderManagers.Status == HelperROM.StatusTypes.Started) {
					string data = HelperFile.Load(Configuration.Path.Default.OrderPath, String.Format(@"{0}_{1:G}{2:G3}.orders", username, DateTime.Today.Year, DateTime.Today.DayOfYear));

					if (data != "") {
						byte[] bytes = System.Convert.FromBase64String(data);
						Dictionary<string, ROCOrder> orders = (Dictionary<string, ROCOrder>)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);

						ClearKeys();
						RocItems.Clear();
						Table.Clear();

						DisplayOrders(orders);
					}
				}
			} catch (Exception ex) {
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public void Export()
		{
			if (GLOBAL.OrderManagers.UserName != "") {
				if (RocItems != null && RocItems.Count > 0) {
					byte[] bytes = new ToBinary().Serialize(RocItems, SerializationTypes.Normal);
					string data = System.Convert.ToBase64String(bytes);

					HelperFile.Save(data, Configuration.Path.Default.OrderPath, String.Format(@"{0}_{1:G}{2:G3}.orders", GLOBAL.OrderManagers.UserName, DateTime.Today.Year, DateTime.Today.DayOfYear));
				}
			}
		}

		#endregion
	}

	// Main Executions Collection
	public class HelperExecutionsData : HelperDataBase
	{
		private Dictionary<string, ROCTrade> _rocItems;
		public Dictionary<string, ROCTrade> RocItems {
			get {
				if (_rocItems == null) {
					_rocItems = new Dictionary<string, ROCTrade>();
				}
				return _rocItems;
			}
			set {
				_rocItems = value;
			}
		}

		private Dictionary<string, ROCTrade> _tposItems;
		public Dictionary<string, ROCTrade> TposItems {
			get {
				if (_tposItems == null) {
					_tposItems = new Dictionary<string, ROCTrade>();
				}
				return _tposItems;
			}
			set {
				_tposItems = value;
			}
		}

		protected override string GetTableTitle() => "Trades";
		protected override string GetKeyColumnName() => "ExecID";
		protected override (string, Type)[] GetTableColumns()
		{
			return new (string, Type)[] {
				("ExecID", typeof(string)),
				("Symbol", typeof(string)),
				("SymbolDetail", typeof(string)),
				("SymbolDisplay", typeof(string)),
				("Side", typeof(long)),
				("Qty", typeof(long)),
				("Price", typeof(double)),
				("TradeValue", typeof(double)),
				("DestID", typeof(long)),
				("OmTime", typeof(DateTime)),
				("ClearingAcct", typeof(string)),
				("ContractSize", typeof(double)),
				("TickSize", typeof(double)),
				("DisplayFactor", typeof(double)),
				("OMTag", typeof(string)),
				("SecType", typeof(string)),
				("Source", typeof(string)),
				("CplxOrderType", typeof(long))
			};
		}

		protected override void OnCreateTable(DataTable table)
		{
			// No additional setup for this table.
		}

		// Used by RDS When it first got all the roc executions from DB
		public void AddRocTrades(Dictionary<string, ROCTrade> trades) // Was Update()
		{
			if (GLOBAL.HRDS.GotRocExecutions) {
				lock (Table) {
					foreach (KeyValuePair<string, ROCTrade> entry in trades) {
						ROCTrade trade = entry.Value;
						if (trade.UpdateTrade) {
							RocItems[trade.TradeID] = trade;
							addTradeToTable(trade);
						}
					}
				}
			}
		}

		// Add a single trade.  Use AddRocTrades() instead for collections of trades.
		public void AddTrade(ROCTrade trade)
		{
			if (trade.UpdateTrade) {
				RocItems[trade.TradeID] = trade;
				lock (Table) {
					addTradeToTable(trade);
				}
			}
		}

		// Used by RDS When it first got all the executions tpos
		public void AddTposTrades(Dictionary<string, ROCTrade> trades)
		{
			if (GLOBAL.HRDS.GotTposExecutions) {
				lock (Table) {
					foreach (KeyValuePair<string, ROCTrade> entry in trades) {
						ROCTrade trade = entry.Value;
						TposItems[trade.TradeID] = trade;
						addTradeToTable(trade);
					}
				}
			}
		}

		private void addTradeToTable(ROCTrade trade)
		{
			if (TryAddKey(trade.TradeID)) {
				Table.Rows.Add(new object[] {
					trade.TradeID,
					trade.Symbol,
					trade.SymbolDetail,
					trade.SymbolDisplay,
					trade.Side,
					trade.Qty,
					trade.ExecPrice.Value,
					trade.TradeValue,
					trade.DestID,
					trade.TradeTime,
					trade.ClearingAcct,
					trade.ContractSize,
					trade.TickSize,
					trade.DisplayFactor,
					trade.OmTag,
					trade.SecType,
					trade.Source,
					trade.CplxOrderType });
			} else {
				DataRowView[] rows = SearchView.FindRows(trade.TradeID);
				foreach (DataRowView row in rows) {
					row["Qty"] = trade.Qty;
					row["Price"] = trade.ExecPrice.Value;
					row["TradeValue"] = trade.TradeValue;
					if (!string.IsNullOrEmpty(trade.TradeTime))
						row["OmTime"] = trade.TradeTime;
				}
			}
		}

		#region - Export & Import -

		public void Import(string username)
		{
			try {
				if (GLOBAL.OrderManagers.Status == HelperROM.StatusTypes.Started) {
					string data = HelperFile.Load(Configuration.Path.Default.TradePath, string.Format(@"{0}_{1:G}{2:G3}.trades", username, DateTime.Today.Year, DateTime.Today.DayOfYear));

					if (data != "") {
						byte[] bytes = System.Convert.FromBase64String(data);
						Dictionary<string, ROCTrade> trades = (Dictionary<string, ROCTrade>)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);

						// Don't clear the Trade Table, because trades are duplicated.
						//Keys.Clear();
						RocItems.Clear();
						//Table.Clear();

						AddRocTrades(trades);

						// Update ROC Trades when loading from local file
						GLOBAL.HPositions.AddRocTrades(trades);
					}
				}
			} catch (Exception ex) {
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public void Export()
		{
			if (GLOBAL.OrderManagers.UserName != "") {
				if (RocItems != null && RocItems.Count > 0) {
					byte[] bytes = new ToBinary().Serialize(RocItems, SerializationTypes.Normal);
					string data = System.Convert.ToBase64String(bytes);

					HelperFile.Save(data, Configuration.Path.Default.TradePath, string.Format(@"{0}_{1:G}{2:G3}.trades", GLOBAL.OrderManagers.UserName, DateTime.Today.Year, DateTime.Today.DayOfYear));
				}
			}
		}

		#endregion
	}

	// Main Positions Collection
	public class HelperPositionsData : HelperDataBase
	{
		private Dictionary<string, ROCPosition> _rocItems;
		public Dictionary<string, ROCPosition> RocItems {
			get {
				if (_rocItems == null) {
					_rocItems = new Dictionary<string, ROCPosition>();
				}
				return _rocItems;
			}
			set {
				_rocItems = value;
			}
		}

		private Dictionary<string, ROCPosition> _tposItems;
		public Dictionary<string, ROCPosition> TposItems {
			get {
				if (_tposItems == null) {
					_tposItems = new Dictionary<string, ROCPosition>();
				}
				return _tposItems;
			}
			set {
				_tposItems = value;
			}
		}

		protected override string GetTableTitle() => "Positions";

		protected override string GetKeyColumnName() => null;

		protected override (string, Type)[] GetTableColumns()
		{
			return new (string, Type)[] {
				("PositionKey", typeof(string)),
				("Symbol", typeof(string)),
				("SymbolDetail", typeof(string)),
				("SymbolDisplay", typeof(string)),
				("BidPrice", typeof(double)),
				("BidSize", typeof(long)),
				("AskPrice", typeof(double)),
				("AskSize", typeof(long)),
				("LastTraded", typeof(double)),
				("NetChange", typeof(double)),
				("PctChange", typeof(double)),
				("Volume", typeof(long)),
				("ClearingAcct", typeof(string)),
				("Trader", typeof(string)),
				("OpenQty", typeof(long)),
				("OpenAvg", typeof(double)),
				("BuyQty", typeof(long)),
				("BuyAvg", typeof(double)),
				("SellQty", typeof(long)),
				("SellAvg", typeof(double)),
				("CurrentQty", typeof(long)),
				("ContractSize", typeof(double)),
				("TickSize", typeof(double)),
				("DisplayFactor", typeof(double)),
				("OpenPnL", typeof(double)),
				("DayPnL", typeof(double)),
				("DayRealizedPnL", typeof(double)),
				("TotalPnL", typeof(double)),
				("MarketValue", typeof(double)),
				("SecType", typeof(string)),
				("SecurityStatus", typeof(string)),
				("Underlying", typeof(string)),
				("ExpDate", typeof(string)),
				("StrikePrice", typeof(string)),
				("CallPut", typeof(string)),
				("Source", typeof(string)),
				("SettlePrice", typeof(double))
			};
		}

		protected override void OnCreateTable(DataTable table)
		{
			// No additional setup for this table.
		}

		#region - TPOS Position -

		// Used by RDS When it first got all positions from TPOS
		public void Update(Dictionary<string, ROCPosition> positions)
		{
			if (GLOBAL.HRDS.GotTposPositions) {
				lock (Table) {
					foreach (ROCPosition position in positions.Values) {
						Update(position);
					}
				}
			}
		}

		public void Update(ROCPosition position)
		{
			lock (TposItems) {
				if (TposItems.TryGetValue(position.PositionKey, out ROCPosition found)) {
					found.UpdateFromPosition(position, true);
				} else {
					TposItems.Add(position.PositionKey, position);
				}
			}
		}

		#endregion

		private void updatePositions(Dictionary<string, ROCPosition> positions, Dictionary<string, ROCTrade> trades, Converter<ROCTrade, bool> filter = null)
		{
			lock (Table) {
				lock (positions) {
					foreach (KeyValuePair<string, ROCTrade> entry in trades) {
						ROCTrade trade = entry.Value;
						if ((filter == null) || filter(trade)) {
							if (positions.TryGetValue(trade.PositionKey, out ROCPosition position)) {
								position.AddTrade(trade);
							} else {
								position = new ROCPosition(trade);
								positions.Add(trade.PositionKey, position);
							}
						}
					}
				}
			}
		}

		public void AddTrade(ROCTrade trade)
		{
			Dictionary<string, ROCPosition> positions = null;
			if (trade != null) {
				if (trade.Source == AssetShared.SourceEnum.ROC) {
					positions = RocItems;
				} else if (trade.Source == AssetShared.SourceEnum.TPOS) {
					// Filter out Auto Balance Trades from TPOS
					if (trade.ExecPrice > 0)
						positions = TposItems;
				}
			}

			if (positions != null) {
				lock (positions) {
					if (!positions.TryGetValue(trade.PositionKey, out ROCPosition position)) {
						position = new ROCPosition(trade);
						positions.Add(trade.PositionKey, position);
					}
					position.AddTrade(trade);
				}
			}
		}

		// Used by RDS When it first got all the roc executions from DB
		public void AddRocTrades(Dictionary<string, ROCTrade> trades)
		{
			if (GLOBAL.HRDS.GotRocExecutions)
				updatePositions(RocItems, trades);
		}

		// Used by RDS When it first got all the executions from Tpos
		public void AddTposTrades(Dictionary<string, ROCTrade> trades)
		{
			if (GLOBAL.HRDS.GotTposExecutions)
				updatePositions(TposItems, trades, n => n.ExecPrice > 0);
		}
	}

	#region - Main Market Data -

	// Main Market Data Collection
	public class HelperMarketData
	{
		public Market Current { get; private set; } = new Market();
	}

	#endregion
}
