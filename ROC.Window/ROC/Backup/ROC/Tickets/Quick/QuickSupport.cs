using System;
using System.Collections.Generic;
using System.Text;
using MarketDataEx;
using ArrayEx;
using RDSEx;
using System.Data;
using CSVEx;

namespace ROC
{
	public class QuickSupport : BookSupport
	{
		internal bool IsSpread
		{
			get
			{
				if (CurrentSymbolDetail.Contains("-"))
				{
					return true;
				}
				return false;
			}
		}

		private sealed class FilterType
		{
			public const int Default = 0;
			public const int WithMarketOrder = 2;
			public const int OnlyStopOrder = 3;
		}

		private bool _showSelectedAccountOnly = false;
		public bool ShowSelectedAccountOnly
		{
			get
			{
				return _showSelectedAccountOnly;
			}
			set
			{
				_showSelectedAccountOnly = value;
			}
		}

		#region - Orders -

		internal long GetOpenQty(long side)
		{
			long result = 0;

			string filter = GetOpenOrderFilter(side, FilterType.Default);
			if (filter != "")
			{
				lock (GLOBAL.HOrders.Table)
				{
					Int64.TryParse(GLOBAL.HOrders.Table.Compute("Sum(LeaveQty)", filter).ToString(), out result);
				}
			}

			return result;
		}

		internal long GetFilledQty(long side)
		{
			long result = 0;

			string filter = GetFilledOrderFilter(side);
			if (filter != "")
			{
				lock (GLOBAL.HOrders.Table)
				{
					Int64.TryParse(GLOBAL.HOrders.Table.Compute("Sum(CumQty)", filter).ToString(), out result);
				}
			}

			return result;
		}

		internal Nullable<double> GetStopLimitPrice(string side, double price)
		{
			Nullable<double> stopLimitPrice = null;

			Dictionary<string, ROCOrder> orders = new Dictionary<string, ROCOrder>();

			DataRow[] rows = new DataRow[0];
			switch (side)
			{
				case "Buy":
					rows = GetOpenOrderRows(CSVFieldIDs.SideCodes.Buy, FilterType.OnlyStopOrder, price);
					break;
				case "Sell":
					rows = GetOpenOrderRows(CSVFieldIDs.SideCodes.Sell, FilterType.OnlyStopOrder, price);
					break;
			}

			foreach (DataRow row in rows)
			{
				string tag = "";
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "")
				{
					tag = row["Tag"].ToString();
					if (GLOBAL.HOrders.RocItems.ContainsKey(tag) && !orders.ContainsKey(tag))
					{
						orders.Add(tag, GLOBAL.HOrders.RocItems[tag]);
					}
				}
			}

			switch (side)
			{
				case "Buy":
					foreach (ROCOrder order in orders.Values)
					{
						if (stopLimitPrice == null || (double)stopLimitPrice < order.Price)
						{
							stopLimitPrice = order.Price;
						}
					}
					break;
				case "Sell":
					foreach (ROCOrder order in orders.Values)
					{
						if (stopLimitPrice == null || (double)stopLimitPrice > order.Price)
						{
							stopLimitPrice = order.Price;
						}
					}
					break;
			}

			return stopLimitPrice;
		}

		internal Dictionary<string, ROCOrder> GetOpenOrder(string side, double price)
		{
			switch (side)
			{
				case "Buy":
					return GetOpenOrder(CSVFieldIDs.SideCodes.Buy, price);
				case "Sell":
					return GetOpenOrder(CSVFieldIDs.SideCodes.Sell, price);
			}

			return new Dictionary<string, ROCOrder>();
		}
		internal Dictionary<string, ROCOrder> GetOpenOrder(long side, Nullable<double> price)
		{
			Dictionary<string, ROCOrder> result = new Dictionary<string, ROCOrder>();

			DataRow[] rows = new DataRow[0];
			if (price == null)
			{
				rows = GetOpenOrderRows(side, FilterType.Default);
			}
			else
			{
				rows = GetOpenOrderRows(side, FilterType.Default, (double)price);
			}
			foreach (DataRow row in rows)
			{
				string tag = "";
				if (row["Tag"] != DBNull.Value && row["Tag"].ToString() != "")
				{
					tag = row["Tag"].ToString();
					if (GLOBAL.HOrders.RocItems.ContainsKey(tag) && !result.ContainsKey(tag))
					{
						result.Add(tag, GLOBAL.HOrders.RocItems[tag]);
					}
				}
			}

			return result;
		}

		internal sealed class OpenOrderItems
		{
			private Dictionary<double, long> _openOrderPriceQty;
			private List<double> _openStopOrderPrice;

			public Dictionary<double, long> OpenOrderPriceQty
			{
				get
				{
					if (_openOrderPriceQty == null)
					{
						_openOrderPriceQty = new Dictionary<double, long>();
					}
					return _openOrderPriceQty;
				}
				set
				{
					_openOrderPriceQty = value;
				}
			}

			public List<double> OpenStopOrderPrice
			{
				get
				{
					if (_openStopOrderPrice == null)
					{
						_openStopOrderPrice = new List<double>();
					}
					return _openStopOrderPrice;
				}
				set
				{
					_openStopOrderPrice = value;
				}
			}
		}

		internal OpenOrderItems GetOpenPriceQty(string side)
		{
			switch (side)
			{
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

			Nullable<double> price;
			long qty;
			
			DataRow[] rows = GetOpenOrderRows(side, FilterType.Default);

			foreach (DataRow row in rows)
			{
				price = null;

				if (row["OrderType"] != DBNull.Value && row["OrderType"].ToString() != "")
				{
					switch ((long)row["OrderType"])
					{
						case CSVFieldIDs.OrderTypes.Stop:
						case CSVFieldIDs.OrderTypes.StopLimit:
							if (row["StopPrice"] != DBNull.Value)
							{
								// Use Stop Price for Stop Orders
								price = (double)row["StopPrice"];
								if (price != null && !result.OpenStopOrderPrice.Contains((double)price))
								{
									result.OpenStopOrderPrice.Add((double)price);
								}
							}
							break;
						default:
							if (row["Price"] != DBNull.Value)
							{
								price = (double)row["Price"];
							}
							break;
					}
				}

				if (price != null)
				{
					if (row["LeaveQty"] != DBNull.Value)
					{
						qty = (long)row["LeaveQty"];
						if (result.OpenOrderPriceQty.ContainsKey((double)price))
						{
							result.OpenOrderPriceQty[(double)price] = result.OpenOrderPriceQty[(double)price] + qty;
						}
						else
						{
							result.OpenOrderPriceQty.Add((double)price, qty);
						}
					}
				}
			}

			return result;
		}

		private DataRow[] GetOpenOrderRows(long side, int filterType)
		{
			if (CurrentSecInfo != null && CurrentSymbolDetail != "")
			{
				string filter = GetOpenOrderFilter(side, filterType);
				if (filter != "")
				{
					lock (GLOBAL.HOrders.Table)
					{
						return GLOBAL.HOrders.Table.Select(filter);
					}
				}
			}

			return new DataRow[0];
		}
		private DataRow[] GetOpenOrderRows(long side, int filterType, double price)
		{
			if (CurrentSecInfo != null && CurrentSymbolDetail != "")
			{
				string filter = GetOpenOrderFilter(side, filterType, price);
				if (filter != "")
				{
					lock (GLOBAL.HOrders.Table)
					{
						return GLOBAL.HOrders.Table.Select(filter);
					}
				}
			}

			return new DataRow[0];
		}

		private string GetOpenOrderFilter(long side, int filterType)
		{
			string result = "SymbolDetail = '" + CurrentSymbolDetail + "'";
			switch (side)
			{
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

			if (ShowSelectedAccountOnly && DDAccountDropDown.CurrentAcctountInfo != null)
			{
				result = result + string.Format(" And ClearingAcct = '{0}'", DDAccountDropDown.CurrentAcctountInfo.clearingAcID);
			}

			switch (filterType)
			{
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
			string result = "SymbolDetail = '" + CurrentSymbolDetail + "'";
			switch (side)
			{
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

			if (ShowSelectedAccountOnly && DDAccountDropDown.CurrentAcctountInfo != null)
			{
				result = result + string.Format(" And ClearingAcct = '{0}'", DDAccountDropDown.CurrentAcctountInfo.clearingAcID);
			}

			return result;
		}

		#endregion
	}
}
