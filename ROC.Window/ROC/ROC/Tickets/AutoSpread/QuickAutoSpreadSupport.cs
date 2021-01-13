using System.Collections.Generic;
using System.Data;
using DBNull = System.DBNull;
using OrderCollection = System.Collections.Generic.Dictionary<string, RDSEx.ROCOrder>;

using Common;
using CSVEx;
using RDSEx;

using IndexLegCollection = Common.PairCollection<int, ROC.Tickets.AutoSpread.AutoSpreadItem>;

namespace ROC.Tickets.AutoSpread
{
	internal class QuickAutoSpreadSupport : BaseTicket
	{
		private int _echoID = 0;
		internal int EchoID {
			get {
				return _echoID;
			}
			set {
				_echoID = value;
			}
		}

		private int _primeLeg = 0;
		internal int PrimeLegNumber {
			get {
				return _primeLeg;
			}
			set {
				_primeLeg = value;
			}
		}

		internal double PrimeFactor => _itemByLegs.TryGetValue(PrimeLegNumber, out var found) ? found.Factor : 0;

		private IndexLegCollection _itemByLegs = new IndexLegCollection();

		internal void ReplaceLegCollection(IndexLegCollection other)
		{
			_itemByLegs = other;
		}

		private sealed class FilterType
		{
			internal const int Default = 0;
			internal const int WithMarketOrder = 2;
			internal const int OnlyStopOrder = 3;
		}

		private bool _showSelectedAccountOnly = false;
		internal bool ShowSelectedAccountOnly {
			get {
				return _showSelectedAccountOnly;
			}
			set {
				_showSelectedAccountOnly = value;
			}
		}

		#region - OrderID Generator - 

		internal string GetOrderID(string orderIDBase, TagKeyItems tagItems)
		{
			return GetOrderID(orderIDBase, tagItems.LegNumberText, tagItems.LimitMarketPriceText, tagItems.StopMarketPriceText);
		}

		internal string GetOrderID(string orderIDBase, string legNumber, string limitPrice, string stopPrice)
		{
			return string.Join(";", orderIDBase, legNumber, limitPrice, stopPrice);
		}

		internal string GetParentTag(string orderIDBase, ParentTagKeyItems ptagItems)
		{
			return GetParentTag(orderIDBase, ptagItems.LimitSpreadPriceText, ptagItems.StopSpreadPriceText, ptagItems.MasterSideSpreadText, ptagItems.MasterQuantitySpreadText);
		}
		internal string GetParentTag(string orderIDBase, string limitPrice, string stopPrice, string sideCode, string qty)
		{
			return string.Join(";", orderIDBase, limitPrice, stopPrice, sideCode, qty);
		}

		#endregion

		#region - Orders -

		internal long GetOpenQty(long side)
		{
			long result = 0;

			string filter = GetOpenOrderFilter(side, FilterType.Default);
			if (filter != "") {
				lock (GLOBAL.HOrders.Table) {
					long.TryParse(GLOBAL.HOrders.Table.Compute("Sum(LeaveQty)", filter).ToString(), out result);
				}
			}

			return result;
		}

		internal long GetFilledQty(long side)
		{
			long result = 0;

			string filter = GetFilledOrderFilter(side);
			if (filter != "") {
				lock (GLOBAL.HOrders.Table) {
					long.TryParse(GLOBAL.HOrders.Table.Compute("Sum(CumQty)", filter).ToString(), out result);
				}
			}

			return result;
		}

		internal OrderCollection GetOpenOrder(string side, double price)
		{
			switch (side) {
				case "Buy":
					return GetOpenOrder(CSVFieldIDs.SideCodes.Buy, price);
				case "Sell":
					return GetOpenOrder(CSVFieldIDs.SideCodes.Sell, price);
			}

			return new OrderCollection();
		}
		internal OrderCollection GetOpenOrder(long side, double? price)
		{
			OrderCollection result = new OrderCollection();

			DataRow[] rows = new DataRow[0];
			if (price == null) {
				rows = GetOpenOrderRows(side, FilterType.Default);
			} else {
				rows = GetOpenOrderRows(side, FilterType.Default, (double)price);
			}
			foreach (DataRow row in rows) {
				string tag = "";
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "") {
					tag = row["Tag"].ToString();
					if (GLOBAL.HOrders.RocItems.TryGetValue(tag, out var found) && !result.ContainsKey(tag)) {
						result.Add(tag, found);
					}
				}
			}

			return result;
		}

		internal sealed class OpenOrderItems
		{
			private PairCollection<Price, long> _openOrderPriceQty;
			private List<Price> _openStopOrderPrice;

			internal PairCollection<Price, long> OpenOrderPriceQty {
				get {
					if (_openOrderPriceQty == null) {
						_openOrderPriceQty = new PairCollection<Price, long>();
					}
					return _openOrderPriceQty;
				}
				set {
					_openOrderPriceQty = value;
				}
			}

			internal List<Price> OpenStopOrderPrice {
				get {
					if (_openStopOrderPrice == null) {
						_openStopOrderPrice = new List<Price>();
					}
					return _openStopOrderPrice;
				}
				set {
					_openStopOrderPrice = value;
				}
			}
		}

		internal OpenOrderItems GetOpenPriceQty(string side)
		{
			switch (side) {
				case "Buy":
					return GetOpenPriceQty(CSVFieldIDs.SideCodes.Buy);
				case "Sell":
					return GetOpenPriceQty(CSVFieldIDs.SideCodes.Sell);
			}

			return new OpenOrderItems();
		}

		internal OpenOrderItems GetOpenPriceQty(long side)
		{
			OpenOrderItems result = new OpenOrderItems();

			bool hasPrice = false;
			Price price = 0;
			long qty;

			DataRow[] rows = GetOpenOrderRows(side, FilterType.Default);

			foreach (DataRow row in rows) {
				hasPrice = false;

				ParentTagKeyItems pTagItems = new ParentTagKeyItems();
				if (row["ParentTag"] != DBNull.Value && row["ParentTag"].ToString() != "") {
					pTagItems = new ParentTagKeyItems(row["ParentTag"].ToString());
				}

				TagKeyItems tagItems = new TagKeyItems();
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "") {
					tagItems = new TagKeyItems(row["Tag"].ToString());
				}

				if (tagItems.TryGetLegNumber(out int legNumber) && (legNumber == PrimeLegNumber)) {
					if (row["OrderType"] != DBNull.Value && row["OrderType"].ToString() != "") {
						double value;
						switch ((long)row["OrderType"]) {
							case CSVFieldIDs.OrderTypes.Stop:
							case CSVFieldIDs.OrderTypes.StopLimit:
								if (pTagItems.TryGetStopSpreadPrice(out value)) {
									// Stop Price
									price = value;
									//if (price != (double)row["StopPrice"])
									//{
									//    price = (double)row["StopPrice"];
									//}
									hasPrice = true;
								}
								break;
							default:
								if (pTagItems.TryGetLimitSpreadPrice(out value)) {
									// Limit Price
									price = value;
									//if (price != (double)row["Price"])
									//{
									//    price = (double)row["Price"];
									//}
									hasPrice = true;
								}
								break;
						}
					}

					if (hasPrice) {
						if (pTagItems.TryGetMasterQtySpread(out long value)) {
							// LeaveQty (OriginalQty)
							qty = value;
							if (qty != (long)row["LeaveQty"]) {
								qty = (long)row["LeaveQty"];
							}
							
							if (result.OpenOrderPriceQty.FindOrAdd(price, out long size, () => qty)) {
								result.OpenOrderPriceQty.Set(price, size + qty); // Update existing entry.
							} else {
								// FindOrAdd added an applicable entry using the factory method provided.
							}
						}
					}
				}
			}

			return result;
		}

		private DataRow[] GetOpenOrderRows(long side, int filterType)
		{
			if (CurrentSecInfo != null && EchoID != 0) {
				string filter = GetOpenOrderFilter(side, filterType);
				// Debug
				//GLOBAL.HROC.AddToException("GetOpenOrderRows " + filter);
				if (filter != "") {
					lock (GLOBAL.HOrders.Table) {
						return GLOBAL.HOrders.Table.Select(filter);
					}
				}
			}

			return new DataRow[0];
		}
		private DataRow[] GetOpenOrderRows(long side, int filterType, double price)
		{
			if (CurrentSecInfo != null && EchoID != 0) {
				string filter = GetOpenOrderFilter(side, filterType, price);
				if (filter != "") {
					lock (GLOBAL.HOrders.Table) {
						return GLOBAL.HOrders.Table.Select(filter);
					}
				}
			}

			return new DataRow[0];
		}

		private string GetOpenOrderFilter(long side, int filterType)
		{
			string result = "Echo = '" + EchoID.ToString() + "'";
			switch (side) {
				case CSVFieldIDs.SideCodes.Buy:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.BuySideFilter);
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.SellSideFilter);
					break;
				default:
					return "";
			}
			result = result + string.Format(" And ({0})", CSVFieldIDs.StatusCodes.ActiveStatusFilter);
			result = result + " And LeaveQty > 0";

			if (ShowSelectedAccountOnly && DDAccountDropDown.CurrentAcctountInfo != null) {
				result = result + string.Format(" And ClearingAcct = '{0}'", DDAccountDropDown.CurrentAcctountInfo.clearingAcID);
			}

			switch (filterType) {
				case FilterType.WithMarketOrder:
					break;
				case FilterType.OnlyStopOrder:
					result = result + string.Format(" And ({0})", CSVFieldIDs.OrderTypes.StopOrderOnlyFilter);
					break;
				case FilterType.Default:
					// Non Market Order Only
					result = result + string.Format(" And ({0})", CSVFieldIDs.OrderTypes.NonMarketOrderOnlyFilter);
					break;
			}
			return result;
		}
		private string GetOpenOrderFilter(long side, int filterType, double price)
		{
			string result = GetOpenOrderFilter(side, filterType);

			return result + string.Format(" And (Price = '{0}' Or StopPrice = '{0}')", price);
		}

		private string GetFilledOrderFilter(long side)
		{
			string result = "Echo = '" + EchoID.ToString() + "'";
			switch (side) {
				case CSVFieldIDs.SideCodes.Buy:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.BuySideFilter);
					break;
				case CSVFieldIDs.SideCodes.Sell:
				case CSVFieldIDs.SideCodes.Short:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.SellSideFilter);
					break;
				default:
					return "";
			}
			result = result + " And CumQty > 0";

			if (ShowSelectedAccountOnly && DDAccountDropDown.CurrentAcctountInfo != null) {
				result += string.Format(" And ClearingAcct = '{0}'", DDAccountDropDown.CurrentAcctountInfo.clearingAcID);
			}

			return result;
		}

		#endregion
	}
}
