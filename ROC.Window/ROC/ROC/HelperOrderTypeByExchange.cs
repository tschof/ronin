using System;
using System.Collections.Generic;
using System.Text;
using System.Data;

namespace ROC
{
	public static class HelperOrderTypeByExchange
	{
		private static string _orderTypeByExchangeFileName = "ExchConfig.xml";

		private static DataTable _orderTypeByExchangeTable;
		private static DataTable orderTypeByExchangeTable
		{
			get
			{
				if (_orderTypeByExchangeTable == null)
				{
					_orderTypeByExchangeTable = new DataTable("OrderTypeByExchange");
					_orderTypeByExchangeTable.Columns.Add("Type", Type.GetType("System.String"));
					_orderTypeByExchangeTable.Columns.Add("Exchanges", Type.GetType("System.String"));

					if (HelperFile.Load(_orderTypeByExchangeTable, Configuration.Path.Default.OrderTypeConfigPath, _orderTypeByExchangeFileName))
					{
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportExtendedMarketOrderType", "ARCA,INET,NYSE,BATS,EDG");
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportExtendedLimitOrderType", "ARCA,INET,NYSE,BATS,EDG");
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportStopOrderType", "NYSE,GLBX,GLOBEX");
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportLimitStopOrderType", "NYSE,GLBX,GLOBEX");
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportPegMktOrderType", "ARCA,INET,BATS,EDG");
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportPegPriOrderType", "ARCA,INET,BATS,EDG");
						_orderTypeByExchangeTable.Rows.Add("ExchangesSupportPegMidOrderType", "INET,BATS,EDG");
						HelperFile.Save(_orderTypeByExchangeTable, Configuration.Path.Default.OrderTypeConfigPath, _orderTypeByExchangeFileName);
					}
				}
				return _orderTypeByExchangeTable;
			}
			set
			{
				_orderTypeByExchangeTable = value;
			}
		}

		private static string _exchangesSupportExtendedMarketOrderType = "ARCA,INET,NYSE,BATS,EDG";
		public static string ExchangesSupportExtendedMarketOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportExtendedMarketOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportExtendedMarketOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportExtendedMarketOrderType;
			}
		}

		private static string _exchangesSupportExtendedLimitOrderType = "ARCA,INET,NYSE,BATS,EDG";
		public static string ExchangesSupportExtendedLimitOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportExtendedLimitOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportExtendedLimitOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportExtendedLimitOrderType;
			}
		}

		private static string _exchangesSupportStopOrderType = "NYSE,GLBX,GLOBEX";
		public static string ExchangesSupportStopOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportStopOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportStopOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportStopOrderType;
			}
		}

		private static string _exchangesSupportLimitStopOrderType = "GLBX,GLOBEX";
		public static string ExchangesSupportLimitStopOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportLimitStopOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportLimitStopOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportLimitStopOrderType;
			}
		}

		private static string _exchangesSupportPegMktOrderType = "ARCA,INET,BATS,EDG";
		public static string ExchangesSupportPegMktOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportPegMktOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportPegMktOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportPegMktOrderType;
			}
		}

		private static string _exchangesSupportPegPriOrderType = "ARCA,INET,BATS,EDG";
		public static string ExchangesSupportPegPriOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportPegPriOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportPegPriOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportPegPriOrderType;
			}
		}

		private static string _exchangesSupportPegMidOrderType = "INET,BATS,EDG";
		public static string ExchangesSupportPegMidOrderType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangesSupportPegMidOrderType'");
				if (Rows.Length > 0)
				{
					_exchangesSupportPegMidOrderType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangesSupportPegMidOrderType;
			}
		}

		private static string _exchangeSupportVWAPType = "_ALGO";
		public static string ExchangeSupportVWAPType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangeSupportVWAPType'");
				if (Rows.Length > 0)
				{
					_exchangeSupportVWAPType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangeSupportVWAPType;
			}
		}

		private static string _exchangeSupportGWAPType = "BAR_ALGO";
		public static string ExchangeSupportGWAPType
		{
			get
			{
				DataRow[] Rows = orderTypeByExchangeTable.Select("Type = 'ExchangeSupportGWAPType'");
				if (Rows.Length > 0)
				{
					_exchangeSupportGWAPType = Rows[0]["Exchanges"].ToString();
				}
				return _exchangeSupportGWAPType;
			}
		}
	}
}
