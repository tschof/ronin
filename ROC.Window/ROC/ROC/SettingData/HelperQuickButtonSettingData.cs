using System;
using System.Collections.Generic;
using System.Data;
using CSVEx;
using Path = System.IO.Path;

namespace ROC
{
	public class QuickButtonSettingData
	{
		public string symbol = "";
		public string secType = "";

		public long qty = 0;
		public string side = "";
		public string shortLocate = "";

		public string basePriceSource = "";
		public long basePriceOffset = 0;
		
		private string _displayName = "";
		public string displayName
		{
			get
			{
				if (_displayName == "")
				{
					string result = "";

					switch (secType)
					{
						case CSVFieldIDs.SecurityTypes.Equity:
							if (basePriceOffset == 0)
							{
								result = string.Format("{0} {1}", new object[] { qty, basePriceSource });
							}
							else
							{
								result = string.Format("{0} {1} {2}", new object[] { qty, basePriceSource, basePriceOffset });
							}
							break;
						case CSVFieldIDs.SecurityTypes.Option:
							result = string.Format("{0}", new object[] { qty });
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							if (basePriceOffset == 0)
							{
								result = string.Format("{0} {1}", new object[] { qty, basePriceSource });
							}
							else
							{
								result = string.Format("{0} {1} {2}", new object[] { qty, basePriceSource, basePriceOffset });
							}
							break;
					}

					if (result != "")
					{
						return result.Trim();
					}
				}
				return _displayName;
			}
			set
			{
				_displayName = value;
			}
		}

		public string hotKeys = "";
		public string special = "";
		public bool loadOnly = false;
		public bool active = false;
	}

	public class HelperQuickButtonSettingData
	{
		private DataTable _settingTable;
		public DataTable SettingTable
		{
			get
			{
				if (_settingTable == null)
				{
					_settingTable = new DataTable("QuickButtonSettings");

					_settingTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

					_settingTable.Columns.Add(new DataColumn("Quantity", Type.GetType("System.Int64")));
					_settingTable.Columns.Add(new DataColumn("Side", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("ShortLocate", Type.GetType("System.String")));

					_settingTable.Columns.Add(new DataColumn("BasePriceSource", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("BasePriceOffset", Type.GetType("System.Int64")));
					_settingTable.Columns.Add(new DataColumn("DisplayName", Type.GetType("System.String")));

					_settingTable.Columns.Add(new DataColumn("HotKey", Type.GetType("System.String")));
					_settingTable.Columns.Add(new DataColumn("Special", Type.GetType("System.String")));

					_settingTable.Columns.Add(new DataColumn("LoadOnly", Type.GetType("System.Boolean")));
					_settingTable.Columns.Add(new DataColumn("Active", Type.GetType("System.Boolean")));

					_settingTable.DefaultView.Sort = "Symbol, SecType";
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

		public List<QuickButtonSettingData> GetSymbolDefaults(string symbolDetail, string secType)
		{
			return GetSymbolDefaults(symbolDetail, secType, false);
		}

		public List<QuickButtonSettingData> GetSymbolDefaults(string symbolDetail, string secType, bool save)
		{
			List<QuickButtonSettingData> datas = new List<QuickButtonSettingData>();

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
					QuickButtonSettingData data = new QuickButtonSettingData();

					data.symbol = symbolDetail;
					data.secType = secType;

					switch (secType)
					{
						case CSVFieldIDs.SecurityTypes.Equity:
							data.qty = 100;
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							data.qty = 1;
							break;
						case CSVFieldIDs.SecurityTypes.Option:
							data.qty = 1;
							break;
					}

					datas.Add(data);

					if (save)
					{
						DataRow row = SettingTable.NewRow();
						WriteDataRow(row, data);
						SettingTable.LoadDataRow(row.ItemArray, LoadOption.OverwriteChanges);
					}
				}
			}

			if (rows.Length > 0)
			{
				return ReadDataRow(rows);
			}
			else
			{
				return datas;
			}
		}

		private List<QuickButtonSettingData> ReadDataRow(DataRowView[] rows)
		{
			List<QuickButtonSettingData> datas = new List<QuickButtonSettingData>();

			foreach (DataRowView row in rows)
			{
				QuickButtonSettingData data = new QuickButtonSettingData();

				if (row["Symbol"] != DBNull.Value)
				{
					data.symbol = (string)row["Symbol"];
				}
				if (row["SecType"] != DBNull.Value)
				{
					data.secType = (string)row["SecType"];
				}
				if (row["Quantity"] != DBNull.Value)
				{
					data.qty = (long)row["Quantity"];
				}
				if (row["Side"] != DBNull.Value)
				{
					data.side = (string)row["Side"];
				}
				if (row["ShortLocate"] != DBNull.Value)
				{
					data.shortLocate = (string)row["ShortLocate"];
				}
				if (row["BasePriceSource"] != DBNull.Value)
				{
					data.basePriceSource = (string)row["BasePriceSource"];
				}
				if (row["BasePriceOffset"] != DBNull.Value)
				{
					data.basePriceOffset = (long)row["BasePriceOffset"];
				}
				if (row["DisplayName"] != DBNull.Value)
				{
					data.displayName = (string)row["DisplayName"];
				}
				if (row["HotKey"] != DBNull.Value)
				{
					data.hotKeys = (string)row["HotKey"];
				}
				if (row["Special"] != DBNull.Value)
				{
					data.special = (string)row["Special"];
				}
				if (row["LoadOnly"] != DBNull.Value)
				{
					data.loadOnly = (bool)row["LoadOnly"];
				}
				if (row["Active"] != DBNull.Value)
				{
					data.active = (bool)row["Active"];
				}

				datas.Add(data);
			}

			return datas;
		}

		private void WriteDataRow(DataRow row, QuickButtonSettingData data)
		{
			if (data.symbol != "")
			{
				row["Symbol"] = data.symbol;
				row["SecType"] = data.secType;

				row["Quantity"] = data.qty;
				row["Side"] = data.side;
				row["Side"] = data.side;

				row["BasePriceSource"] = data.basePriceSource;
				row["BasePriceOffset"] = data.basePriceOffset;
				row["DisplayName"] = data.displayName;
	
				row["HotKey"] = data.hotKeys;
				row["Special"] = data.special;

				row["LoadOnly"] = data.loadOnly;
				row["Active"] = data.active;
			}
		}

		#region - Export & Import -

		public void Import()
		{
			if (SettingTable != null)
			{
				if (HelperFile.Load(SettingTable, Configuration.Path.Default.QuickButtonConfigPath, FileName()))
				{
					if (!Configuration.Path.Default.QuickButtonConfigPath.Contains(Configuration.Path.Default.ProfilePath))
					{
						// Check New Location Under Profild Folder
						// Put the quick button setting into the Profile Folder
						Configuration.Path.Default.QuickButtonConfigPath = Path.Combine(Configuration.Path.Default.ProfilePath, "QuickButtonConfig");
						Configuration.Path.Default.Save();
						HelperFile.Load(SettingTable, Configuration.Path.Default.QuickButtonConfigPath, FileName());
					}
					else
					{
						// Already Using New Location, but didn't find any, try old location again
						HelperFile.Load(SettingTable, "QuickButtonConfig", FileName());
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
				if (!Configuration.Path.Default.QuickButtonConfigPath.Contains(Configuration.Path.Default.ProfilePath))
				{
					// Put the quick button setting into the Profile Folder
					Configuration.Path.Default.QuickButtonConfigPath = Path.Combine(Configuration.Path.Default.ProfilePath, "QuickButtonConfig");
					Configuration.Path.Default.Save();
				}
				HelperFile.Save(SettingTable, Configuration.Path.Default.QuickButtonConfigPath, FileName());
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
