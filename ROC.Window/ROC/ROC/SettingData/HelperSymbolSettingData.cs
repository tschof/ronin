using System;
using System.Collections.Generic;
using System.Data;
using CSVEx;

namespace ROC
{
	public class SymbolSettingData
	{
		public string symbol = "";

		public decimal qty = 1000;
		public decimal qtyIncrement = 100;

		public decimal limitPrice = 0;
		public decimal limitPriceIncrement = 0.01M;
		public decimal stopPrice = 0;
		public decimal stopPriceIncrement = 0.01M;
		public decimal pegPrice = 0;
		public decimal pegPriceIncrement = 0.01M;
		public decimal stopLimitOffset = 0;

		public string orderType = "";
		public string duration = "";

		public decimal show = 100;
		public decimal showIncrement = 100;
		public bool useShow = false;

		public decimal minQty = 100;
		public decimal minQtyIncrement = 100;
		public bool useMinQty = false;

		public string shortLender = "";

		public string instruction = "";
		public bool useInstruction = false;

		public string tradeFor = "";
		public string account = "";
		public string exchange = "";

		public bool level2 = false;

		public string expiration = "";

		public string callput = "";
		public double strike = 0;

		public string secType = "";
	}

	// Saved to user profile
	public class HelperSymbolSettingData
	{
		private DataTable _settingTable;
		public DataTable SettingTable
		{
			get
			{
				if (_settingTable == null)
				{
					_settingTable = new DataTable("SymbolSettings");

					_settingTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_settingTable.Columns["Symbol"]);

					_settingTable.Columns.Add(new DataColumn("Quantity", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("QuantityIncrement", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("LimitPrice", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("LimitPriceIncrement", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("StopPrice", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("StopPriceIncrement", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("PegPrice", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("PegPriceIncrement", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("StopLimitOffset", Type.GetType("System.Decimal")));

					_settingTable.Columns.Add(new DataColumn("OrderType", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("Duration", Type.GetType("System.String")));

					_settingTable.Columns.Add(new DataColumn("Show", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("ShowIncrement", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("UseShow", Type.GetType("System.Boolean")));

					_settingTable.Columns.Add(new DataColumn("MinQty", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("MinQtyIncrement", Type.GetType("System.Decimal")));
					_settingTable.Columns.Add(new DataColumn("UseMinQty", Type.GetType("System.Boolean")));

					_settingTable.Columns.Add(new DataColumn("ShortLender", Type.GetType("System.String")));

					_settingTable.Columns.Add(new DataColumn("Instructions", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("UseInstructions", Type.GetType("System.Boolean")));

					_settingTable.Columns.Add(new DataColumn("Trader", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("Account", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("Exchange", Type.GetType("System.String")));

					// Stock
					_settingTable.Columns.Add(new DataColumn("UseLevel2", Type.GetType("System.Boolean")));

					// Future and Option
					_settingTable.Columns.Add(new DataColumn("Expiration", Type.GetType("System.String")));

					// Option
					_settingTable.Columns.Add(new DataColumn("CallPut", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("Strike", Type.GetType("System.Double")));

					_settingTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
					keyColumns.Add(_settingTable.Columns["SecType"]);

					_settingTable.PrimaryKey = keyColumns.ToArray();

					_settingTable.DefaultView.Sort = "Symbol, SecType";

					#region - Default Values -

					_settingTable.Columns["LimitPrice"].DefaultValue = 0.0M;
					_settingTable.Columns["LimitPriceIncrement"].DefaultValue = 0.01M;
					_settingTable.Columns["StopPrice"].DefaultValue = 0.0M;
					_settingTable.Columns["StopPriceIncrement"].DefaultValue = 0.01M;
					_settingTable.Columns["PegPrice"].DefaultValue = 0.0M;
					_settingTable.Columns["PegPriceIncrement"].DefaultValue = 0.01M;
					_settingTable.Columns["StopLimitOffset"].DefaultValue = 0;

					_settingTable.Columns["OrderType"].DefaultValue = "LIMIT";
					_settingTable.Columns["Duration"].DefaultValue = "DAY";

					_settingTable.Columns["Show"].DefaultValue = 100;
					_settingTable.Columns["ShowIncrement"].DefaultValue = 100;
					_settingTable.Columns["UseShow"].DefaultValue = false;

					_settingTable.Columns["MinQty"].DefaultValue = 100;
					_settingTable.Columns["MinQtyIncrement"].DefaultValue = 100;
					_settingTable.Columns["UseMinQty"].DefaultValue = false;

					_settingTable.Columns["ShortLender"].DefaultValue = "";

					_settingTable.Columns["Instructions"].DefaultValue = "";
					_settingTable.Columns["UseInstructions"].DefaultValue = false;

					_settingTable.Columns["Trader"].DefaultValue = "";
					_settingTable.Columns["Account"].DefaultValue = "";
					_settingTable.Columns["Exchange"].DefaultValue = "";

					_settingTable.Columns["UseLevel2"].DefaultValue = false;
					_settingTable.Columns["Expiration"].DefaultValue = "";
					_settingTable.Columns["CallPut"].DefaultValue = "";
					_settingTable.Columns["Strike"].DefaultValue = 0.0;

					#endregion
				}

				return _settingTable;
			}
			set
			{
				_settingTable = value;
			}
		}

		public void CheckDefaults()
		{
			GetSymbolDefaults(TicketDefaults.Stock, CSVFieldIDs.SecurityTypes.Equity, true);

			GetSymbolDefaults(TicketDefaults.Future, CSVFieldIDs.SecurityTypes.Future, true);

			GetSymbolDefaults(TicketDefaults.Option, CSVFieldIDs.SecurityTypes.Option, true);
		}

		public SymbolSettingData GetSymbolDefaults(string secType)
		{
			return GetSymbolDefaults("", secType, false);
		}
		public SymbolSettingData GetSymbolDefaults(string symbolDetail, string secType)
		{
			return GetSymbolDefaults(symbolDetail, secType, false);
		}
		public SymbolSettingData GetSymbolDefaults(string symbolDetail, string secType, bool save)
		{
			SymbolSettingData data = new SymbolSettingData();

			DataRowView[] rows = new DataRowView[0];

			rows = SettingTable.DefaultView.FindRows(new object[] { symbolDetail, secType });

			if (rows.Length == 0 && symbolDetail != "")
			{
				if (SettingData.Utility.TryGetBaseSymbol(symbolDetail, out string baseSymbol))
				{
					rows = SettingTable.DefaultView.FindRows(new object[] { baseSymbol, secType });
				}
			}

			if (rows.Length == 0)
			{
				switch (secType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
						rows = SettingTable.DefaultView.FindRows(new object[] { TicketDefaults.Option, secType });
						break;
					case CSVFieldIDs.SecurityTypes.Future:
						rows = SettingTable.DefaultView.FindRows(new object[] { TicketDefaults.Future, secType });
						break;
					case CSVFieldIDs.SecurityTypes.Equity:
					default:
						rows = SettingTable.DefaultView.FindRows(new object[] { TicketDefaults.Stock, secType });
						break;
				}

				if (rows.Length == 0)
				{
					data = new SymbolSettingData();

					data.symbol = symbolDetail;
					data.secType = secType;

					switch (secType)
					{
						case CSVFieldIDs.SecurityTypes.Equity:
							data.qty = 100;
							data.qtyIncrement = 100;
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							data.qty = 1;
							data.qtyIncrement = 1;
							break;
						case CSVFieldIDs.SecurityTypes.Option:
							data.qty = 1;
							data.qtyIncrement = 1;
							break;
					}

					data.limitPrice = 0.0M;
					data.limitPriceIncrement = 0.01M;
					data.stopPrice = 0.0M;
					data.stopPriceIncrement = 0.01M;
					data.pegPrice = 0.0M;
					data.pegPriceIncrement = 0.01M;
					data.stopLimitOffset = 0;

					data.orderType = "LIMIT";
					data.duration = "DAY";

					data.show = 100;
					data.showIncrement = 100;
					data.useShow = false;

					data.minQty = 100;
					data.minQtyIncrement = 100;
					data.useMinQty = false;

					data.tradeFor = "";
					data.account = "";
					data.exchange = "";
					data.shortLender = "";

					data.instruction = "";
					data.useInstruction = false;

					data.strike = 0.0;
					data.callput = "";
					data.expiration = "";

					data.level2 = false;
				}
				else if (rows.Length == 1 && !symbolDetail.Contains("_DEF"))
				{
					data = ReadDataRow(rows[0].Row);
					data.symbol = symbolDetail;
				}
				else
				{
					GLOBAL.HROC.AddToAlerts("Duplicate Defaults in Symbol Setting Table");
				}

				if (save)
				{
					DataRow row = SettingTable.NewRow();
					WriteDataRow(row, data);
					SettingTable.LoadDataRow(row.ItemArray, LoadOption.OverwriteChanges);
				}
			}
			else
			{
				// Read Only
				foreach (DataRowView row in rows)
				{
					return ReadDataRow(row.Row);
				}
			}

			return data;
		}

		public void SetSymbolDefaults(SymbolSettingData data)
		{
			DataRowView[] rows = new DataRowView[0];

			rows = SettingTable.DefaultView.FindRows(new object[] { data.symbol, data.secType });
			if (rows.Length == 0)
			{
				GetSymbolDefaults(data.symbol, data.secType, true);
				rows = SettingTable.DefaultView.FindRows(new object[] { data.symbol, data.secType });
			}

			// Update Only
			foreach (DataRowView row in rows)
			{
				row["Symbol"] = data.symbol;
				row["Quantity"] = data.qty;
				row["QuantityIncrement"] = data.qtyIncrement;
				row["LimitPrice"] = data.limitPrice;
				row["LimitPriceIncrement"] = data.limitPriceIncrement;		// Times Tick Size
				row["StopPrice"] = data.stopPrice;
				row["StopPriceIncrement"] = data.stopPriceIncrement;		// Times Tick Size
				row["PegPrice"] = data.stopPrice;
				row["PegPriceIncrement"] = data.stopPriceIncrement;			// Times Tick Size
				row["StopLimitOffset"] = data.stopLimitOffset;

				row["OrderType"] = data.orderType;
				row["Duration"] = data.duration;

				row["Show"] = data.show;
				row["ShowIncrement"] = data.showIncrement;
				row["UseShow"] = data.useShow;

				row["MinQty"] = data.minQty;
				row["MinQtyIncrement"] = data.minQtyIncrement;
				row["UseMinQty"] = data.useMinQty;

				row["ShortLender"] = data.shortLender;
				row["Instructions"] = data.instruction;
				row["UseInstructions"] = data.useInstruction;

				row["Expiration"] = data.expiration;
				row["CallPut"] = data.callput;
				row["Strike"] = data.strike;

				row["Trader"] = data.tradeFor;
				row["Account"] = data.account;
				row["Exchange"] = data.exchange;

				row["UseLevel2"] = data.level2;
				row["SecType"] = data.secType;
				break;
			}
		}

		public void RemoveSymbolDefault(string symbolDeatil, string secType)
		{
			DataRowView[] rows = new DataRowView[0];

			lock (SettingTable)
			{
				rows = SettingTable.DefaultView.FindRows(new object[] { symbolDeatil, secType });
				if (rows.Length > 0)
				{
					foreach (DataRowView row in rows)
					{
						SettingTable.Rows.Remove(row.Row);
					}
				}
			}
		}

		private SymbolSettingData ReadDataRow(DataRow row)
		{
			SymbolSettingData data = new SymbolSettingData();

			if (row["Symbol"] != DBNull.Value)
			{
				data.symbol = (string)row["Symbol"];
			}
			if (row["Quantity"] != DBNull.Value)
			{
				data.qty = (decimal)row["Quantity"];
			}
			if (row["QuantityIncrement"] != DBNull.Value)
			{
				data.qtyIncrement = (decimal)row["QuantityIncrement"];
			}
			if (row["LimitPrice"] != DBNull.Value)
			{
				data.limitPrice = (decimal)row["LimitPrice"];
			}
			if (row["LimitPriceIncrement"] != DBNull.Value)
			{
				data.limitPriceIncrement = (decimal)row["LimitPriceIncrement"];		// Times Tick Size
			}
			if (row["StopPrice"] != DBNull.Value)
			{
				data.stopPrice = (decimal)row["StopPrice"];
			}
			if (row["StopPriceIncrement"] != DBNull.Value)
			{
				data.stopPriceIncrement = (decimal)row["StopPriceIncrement"];		// Times Tick Size
			}
			if (row["StopLimitOffset"] != DBNull.Value)
			{
				data.stopLimitOffset = (decimal)row["StopLimitOffset"];
			}

			if (row["OrderType"] != DBNull.Value)
			{
				data.orderType = (string)row["OrderType"];
			}
			if (row["Duration"] != DBNull.Value)
			{
				data.duration = (string)row["Duration"];
			}

			if (row["Show"] != DBNull.Value)
			{
				data.show = (decimal)row["Show"];
			}
			if (row["ShowIncrement"] != DBNull.Value)
			{
				data.showIncrement = (decimal)row["ShowIncrement"];
			}
			if (row["UseShow"] != DBNull.Value)
			{
				data.useShow = (bool)row["UseShow"];
			}

			if (row["MinQty"] != DBNull.Value)
			{
				data.minQty = (decimal)row["MinQty"];
			}
			if (row["MinQtyIncrement"] != DBNull.Value)
			{
				data.minQtyIncrement = (decimal)row["MinQtyIncrement"];
			}
			if (row["UseMinQty"] != DBNull.Value)
			{
				data.useMinQty = (bool)row["UseMinQty"];
			}

			if (row["ShortLender"] != DBNull.Value)
			{
				data.shortLender = (string)row["ShortLender"];
			}
			if (row["Instructions"] != DBNull.Value)
			{
				data.instruction = (string)row["Instructions"];
			}
			if (row["UseInstructions"] != DBNull.Value)
			{
				data.useInstruction = (bool)row["UseInstructions"];
			}

			if (row["Expiration"] != DBNull.Value)
			{
				data.expiration = (string)row["Expiration"];
			}
			if (row["CallPut"] != DBNull.Value)
			{
				data.callput = (string)row["CallPut"];
			}
			if (row["Strike"] != DBNull.Value)
			{
				data.strike = (double)row["Strike"];
			}

			if (row["Trader"] != DBNull.Value)
			{
				data.tradeFor = (string)row["Trader"];
			}
			if (row["Account"] != DBNull.Value)
			{
				data.account = (string)row["Account"];
			}
			if (row["Exchange"] != DBNull.Value)
			{
				data.exchange = (string)row["Exchange"];
			}

			if (row["UseLevel2"] != DBNull.Value)
			{
				data.level2 = (bool)row["UseLevel2"];
			}
			if (row["SecType"] != DBNull.Value)
			{
				data.secType = (string)row["SecType"];
			}

			return data;
		}

		private void WriteDataRow(DataRow row, SymbolSettingData data)
		{
			if (data.symbol != "")
			{
				row["Symbol"] = data.symbol;
				row["Quantity"] = data.qty;
				row["QuantityIncrement"] = data.qtyIncrement;
				row["LimitPrice"] = data.limitPrice;
				row["LimitPriceIncrement"] = data.limitPriceIncrement;		// Times Tick Size
				row["StopPrice"] = data.stopPrice;
				row["StopPriceIncrement"] = data.stopPriceIncrement;		// Times Tick Size
				row["PegPrice"] = data.stopPrice;
				row["PegPriceIncrement"] = data.stopPriceIncrement;		// Times Tick Size

				row["OrderType"] = data.orderType;
				row["Duration"] = data.duration;

				row["Show"] = data.show;
				row["ShowIncrement"] = data.showIncrement;
				row["UseShow"] = data.useShow;

				row["MinQty"] = data.minQty;
				row["MinQtyIncrement"] = data.minQtyIncrement;
				row["UseMinQty"] = data.useMinQty;

				row["ShortLender"] = data.shortLender;
				row["Instructions"] = data.instruction;
				row["UseInstructions"] = data.useInstruction;

				row["Expiration"] = data.expiration;
				row["CallPut"] = data.callput;
				row["Strike"] = data.strike;

				row["Trader"] = data.tradeFor;
				row["Account"] = data.account;
				row["Exchange"] = data.exchange;

				row["UseLevel2"] = data.level2;
				row["SecType"] = data.secType;
			}
		}

		#region - Export & Import -

		public void Import()
		{
			if (SettingTable != null)
			{
				if (HelperFile.Load(SettingTable, Configuration.Path.Default.SymbolSettingPath, FileName()))
				{
					if (!Configuration.Path.Default.SymbolSettingPath.Contains(Configuration.Path.Default.ProfilePath))
					{
						// Check New Location Under Profild Folder
						// Put the symbols setting into the Profile Folder
						Configuration.Path.Default.SymbolSettingPath = System.IO.Path.Combine(Configuration.Path.Default.ProfilePath, "Symbols");
						Configuration.Path.Default.Save();
						HelperFile.Load(SettingTable, Configuration.Path.Default.SymbolSettingPath, FileName());
					}
					else
					{
						// Already Using New Location, but didn't find any, try old location again
						HelperFile.Load(SettingTable, "Symbols", FileName());
					}
				}

				CheckDefaults();
			}
		}

		public void Export()
		{
			if (SettingTable != null)
			{
				CheckDefaults();
				if (!Configuration.Path.Default.SymbolSettingPath.Contains(Configuration.Path.Default.ProfilePath))
				{
					// Put the symbols setting into the Profile Folder
					Configuration.Path.Default.SymbolSettingPath = System.IO.Path.Combine(Configuration.Path.Default.ProfilePath, "Symbols");
					Configuration.Path.Default.Save();
				}
				HelperFile.Save(SettingTable, Configuration.Path.Default.SymbolSettingPath, FileName());
			}
		}

		private string FileName()
		{
			if (GLOBAL.OrderManagers.UserName == "")
			{
				return "Default.xml";
			}
			else
			{
				return string.Concat(new object[] { GLOBAL.OrderManagers.UserName + ".xml" });
			}
		}

		#endregion
	}
}
