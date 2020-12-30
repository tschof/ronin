using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using MarketData;
using Log = Common.Log;

namespace ROC
{
	public static class HelperOption
	{
		#region - Plus One Month -

		private const string PLUS_ONE_MONTH_FILE = "PlusOneConfig.xml";

		private static DataTable _plusOneMonthTable;

		private static DataTable getPlusOneMonthTable()
		{
				if (_plusOneMonthTable == null)
				{
					_plusOneMonthTable = new DataTable("PlusOneMonth");
					_plusOneMonthTable.Columns.Add("Symbol", Type.GetType("System.String"));
				}
				return _plusOneMonthTable;
		}

		private static List<string> _plusOne;
		public static List<string> PlusOne
		{
			get
			{
				if (_plusOne == null)
				{
					LoadPlusOneMothSymbols();

					// TODO Make the DataGridView Object Aware of the product
					DataGridViewEx.GLOBAL.OptionOnFuture.PlusOne = _plusOne;
					RDSEx.GLOBAL.OptionOnFuture.PlusOne = _plusOne;
				}
				return _plusOne;
			}
		}

		private static void LoadPlusOneMothSymbols()
		{
			var table = getPlusOneMonthTable();
			if (HelperFile.Load(table, Configuration.Path.Default.OptionConfigPath, PLUS_ONE_MONTH_FILE)) {
				LoadPlusOneMonthDefaultAndSave();

				_plusOne = new List<string>();
				foreach (DataRow row in table.Rows) {
					if (row["Symbol"] != DBNull.Value && row["Symbol"].ToString() != "" && !_plusOne.Contains(row["Symbol"].ToString())) {
						_plusOne.Add(row["Symbol"].ToString());
					}
				}
			} else {
				Log.Error(Log.Target.ROC, $"Can't find PlusOneMonthSymbols file {PLUS_ONE_MONTH_FILE}.");
			}
		}

		private static void LoadPlusOneMonthDefaultAndSave()
		{
			string[][] symbols = new string[][] {
				// Energy
				new string[] {"CL","NG","ZE","BZ","HO","RB"},

				// Metals
				new string[] {"GC","QC","UX","HR","8Q","QO","SI","6Q","QI","PL","PA","QS","QR","QT"},

				// Bonds (missing 3Y, 7Y, and Ultra)
				new string[] {"ZT","ZF","ZN","ZB"},

				// Comdities
				new string[] {"ZC","ZW","ZS","ZM","ZL","ZR","ZO","LE","HE","GF","GPB"}
			};

			try
			{
				var table = getPlusOneMonthTable();
				if (table.Rows.Count == 0)
				{
					foreach (var sector in symbols) {
						foreach (string symbol in sector) {
							table.Rows.Add(new object[] { symbol });
						}
					}
					HelperFile.Save(table, Configuration.Path.Default.OptionConfigPath, PLUS_ONE_MONTH_FILE);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Show As 1/64 -

		private static string _showAs64FileName = "ShowAs64TH.xml";

		private static DataTable _showAs64Table;
		private static DataTable showAs64Table
		{
			get
			{
				if (_showAs64Table == null)
				{
					_showAs64Table = new DataTable("ShowAs64");
					_showAs64Table.Columns.Add("Symbol", Type.GetType("System.String"));
				}
				return _showAs64Table;
			}
			set
			{
				_showAs64Table = value;
			}
		}

		private static List<string> _showAs64th;
		public static List<string> ShowAs64TH
		{
			get
			{
				if (_showAs64th == null)
				{
					Load64ThSymbols();

					// TODO Make the DataGridView Object Aware of the product
					DataGridViewEx.GLOBAL.OptionOnFuture.ShowAs64TH = _showAs64th;
					RDSEx.GLOBAL.OptionOnFuture.ShowAs64TH = _showAs64th;
				}
				return _showAs64th;
			}
		}

		private static void Load64ThSymbols()
		{
			HelperFile.Load(showAs64Table, Configuration.Path.Default.OptionConfigPath, _showAs64FileName);
			Load64ThDefaultAndSave();

			_showAs64th = new List<string>();
			foreach (DataRow row in showAs64Table.Rows)
			{
				if (row["Symbol"] != DBNull.Value && row["Symbol"].ToString() != "" && !_showAs64th.Contains(row["Symbol"].ToString()))
				{
					_showAs64th.Add(row["Symbol"].ToString());
				}
			}
		}

		private static void Load64ThDefaultAndSave()
		{
			try
			{
				if (showAs64Table.Rows.Count == 0)
				{
					// Energy
					showAs64Table.Rows.Add(new object[] { "ZB" });
					showAs64Table.Rows.Add(new object[] { "ZN" });
					showAs64Table.Rows.Add(new object[] { "ZF" });
					showAs64Table.Rows.Add(new object[] { "ZT" });

					HelperFile.Save(showAs64Table, Configuration.Path.Default.OptionConfigPath, _showAs64FileName);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Exchange Config -

		private static string _exchangeConfigFileName = "ExchangeConfig.xml";

		private static DataTable _exchangeTable;
		private static DataTable exchangeTable
		{
			get
			{
				if (_exchangeTable == null)
				{
					_exchangeTable = new DataTable("ExchConfig");
					_exchangeTable.Columns.Add("Name", Type.GetType("System.String"));
					_exchangeTable.Columns.Add("Display", Type.GetType("System.String"));
					_exchangeTable.Columns.Add("Code", Type.GetType("System.String"));
					_exchangeTable.Columns.Add("Color", Type.GetType("System.String"));
				}
				return _exchangeTable;
			}
			set
			{
				_exchangeTable = value;
			}
		}

		private static Dictionary<string, string> _exchanges;
		public static Dictionary<string, string> exchanges
		{
			get
			{
				if (_exchanges == null)
				{
					LoadExchangeTable();
				}
				return _exchanges;
			}
		}

		private static Dictionary<string, Color> _exchangeColors;
		public static Dictionary<string, Color> exchangeColors
		{
			get
			{
				if (_exchangeColors == null)
				{
					LoadExchangeTable();
				}
				return _exchangeColors;
			}
		}

		private static void LoadExchangeTable()
		{
			HelperFile.Load(exchangeTable, Configuration.Path.Default.OptionConfigPath, _exchangeConfigFileName);
			LoadExchangeDefaultAndSave();

			// Display -> Code/Color
			_exchanges = new Dictionary<string, string>();
			_exchangeColors = new Dictionary<string, Color>();
			foreach (DataRow row in exchangeTable.Rows)
			{
				string displayValue = (row["Display"] == DBNull.Value) ? null : row["Display"].ToString();
				string codeValue = (row["Code"] == DBNull.Value) ? null : row["Code"].ToString();

				if (!string.IsNullOrEmpty(displayValue) && !_exchanges.ContainsKey(displayValue)) {
					if (!string.IsNullOrEmpty(codeValue))
						_exchanges.Add(displayValue, codeValue);
				}

				if (!string.IsNullOrEmpty(codeValue) && !_exchangeColors.ContainsKey(codeValue))
				{
					string colorValue = row["Color"] == DBNull.Value ? null : row["Color"].ToString();
					if (!string.IsNullOrEmpty(colorValue))
					{
						_exchangeColors.Add(codeValue, GetColorFromHexString(colorValue));
					}
				}
			}

			OptionExchangeCode.AddOrReplace(exchanges);
			OptionExchangeCode.ForEach((code, name) => GLOBAL.HROC.AddToStatusLogs(string.Format("Option Exchange {0} Code = {1}", name, code)));
		}

		private static Color GetColorFromHexString(string hexString)
		{
			try
			{
				if ((hexString.StartsWith("#")) && (hexString.Length == 7))
				{

					int r = HexToInt(hexString.Substring(1, 2));
					int g = HexToInt(hexString.Substring(3, 2));
					int b = HexToInt(hexString.Substring(5, 2));
					return Color.FromArgb(r, g, b);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return Color.White;
		}

		private static int HexToInt(string hexValue)
		{
			return Convert.ToInt32(hexValue, 16); 
		}

		private static void LoadExchangeDefaultAndSave()
		{
			try
			{
				if (exchangeTable.Rows.Count == 0)
				{

					exchangeTable.Rows.Add(new object[] { "NYSE AMEX", "AMEX", "A", "#FFFF00" });
					exchangeTable.Rows.Add(new object[] { "Boston Stock Exchange", "BOX", "B", "#FF4500" });
					exchangeTable.Rows.Add(new object[] { "Chicago Board Options Exchange", "CBOE", "C", "#ADFF2F" });
					exchangeTable.Rows.Add(new object[] { "International Securities Exchange", "ISE", "I", "#00BFFF" });
					exchangeTable.Rows.Add(new object[] { "NYSE ARCA", "PCX", "N", "#FFA500" });
					exchangeTable.Rows.Add(new object[] { "NASDAQ Stock Market", "NSDQ", "Q", "#FFFFFF" });
					exchangeTable.Rows.Add(new object[] { "C2", "C2", "W", "#BDB76B" });
					exchangeTable.Rows.Add(new object[] { "NASDAQ OMX PHLX", "AMEX", "X", "#9370D8" });
					exchangeTable.Rows.Add(new object[] { "BATS", "BATS", "Z", "#EE82EE" });

					HelperFile.Save(exchangeTable, Configuration.Path.Default.OptionConfigPath, _exchangeConfigFileName);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion
	}
}
