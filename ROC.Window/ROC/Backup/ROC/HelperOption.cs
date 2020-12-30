using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Drawing;
using MarketDataEx;

namespace ROC
{
	public static class HelperOption
	{
		#region - Plus One Month -

		private static string _plusOneMonthFileName = "PlusOneConfig.xml";

		private static DataTable _plusOneMonthTable;
		private static DataTable plusOneMonthTable
		{
			get
			{
				if (_plusOneMonthTable == null)
				{
					_plusOneMonthTable = new DataTable("PlusOneMonth");
					_plusOneMonthTable.Columns.Add("Symbol", Type.GetType("System.String"));
				}
				return _plusOneMonthTable;
			}
			set
			{
				_plusOneMonthTable = value;
			}
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
			plusOneMonthTable = HelperFile.Load(plusOneMonthTable, Configuration.Path.Default.OptionConfigPath, _plusOneMonthFileName);
			LoadPlusOneMonthDefaultAndSave();

			_plusOne = new List<string>();
			foreach (DataRow row in plusOneMonthTable.Rows)
			{
				if (row["Symbol"] != DBNull.Value && row["Symbol"].ToString() != "" && !_plusOne.Contains(row["Symbol"].ToString()))
				{
					_plusOne.Add(row["Symbol"].ToString());
				}
			}
		}

		private static void LoadPlusOneMonthDefaultAndSave()
		{
			try
			{
				if (plusOneMonthTable.Rows.Count == 0)
				{
					// Energy
					plusOneMonthTable.Rows.Add(new object[] { "CL" });
					plusOneMonthTable.Rows.Add(new object[] { "NG" });
					plusOneMonthTable.Rows.Add(new object[] { "ZE" });
					plusOneMonthTable.Rows.Add(new object[] { "BZ" });
					plusOneMonthTable.Rows.Add(new object[] { "HO" });
					plusOneMonthTable.Rows.Add(new object[] { "RB" });

					// Metals
					plusOneMonthTable.Rows.Add(new object[] { "GC" });
					plusOneMonthTable.Rows.Add(new object[] { "QC" });
					plusOneMonthTable.Rows.Add(new object[] { "UX" });
					plusOneMonthTable.Rows.Add(new object[] { "HR" });
					plusOneMonthTable.Rows.Add(new object[] { "8Q" });
					plusOneMonthTable.Rows.Add(new object[] { "QO" });
					plusOneMonthTable.Rows.Add(new object[] { "SI" });
					plusOneMonthTable.Rows.Add(new object[] { "6Q" });
					plusOneMonthTable.Rows.Add(new object[] { "QI" });
					plusOneMonthTable.Rows.Add(new object[] { "PL" });
					plusOneMonthTable.Rows.Add(new object[] { "PA" });
					plusOneMonthTable.Rows.Add(new object[] { "QS" });
					plusOneMonthTable.Rows.Add(new object[] { "QR" });
					plusOneMonthTable.Rows.Add(new object[] { "QT" });
					
					// Bonds
					plusOneMonthTable.Rows.Add(new object[] { "ZT" });
					plusOneMonthTable.Rows.Add(new object[] { "ZF" });
					plusOneMonthTable.Rows.Add(new object[] { "ZN" });
					plusOneMonthTable.Rows.Add(new object[] { "ZB" });
					
					// Comdities
					plusOneMonthTable.Rows.Add(new object[] { "ZC" });
					plusOneMonthTable.Rows.Add(new object[] { "ZW" });
					plusOneMonthTable.Rows.Add(new object[] { "ZS" });
					plusOneMonthTable.Rows.Add(new object[] { "ZM" });
					plusOneMonthTable.Rows.Add(new object[] { "ZL" });
					plusOneMonthTable.Rows.Add(new object[] { "ZR" });
					plusOneMonthTable.Rows.Add(new object[] { "ZO" });
					plusOneMonthTable.Rows.Add(new object[] { "LE" });
					plusOneMonthTable.Rows.Add(new object[] { "HE" });
					plusOneMonthTable.Rows.Add(new object[] { "GF" });
					plusOneMonthTable.Rows.Add(new object[] { "GPB" });

					HelperFile.Save(plusOneMonthTable, Configuration.Path.Default.OptionConfigPath, _plusOneMonthFileName);
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
			showAs64Table = HelperFile.Load(showAs64Table, Configuration.Path.Default.OptionConfigPath, _showAs64FileName);
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
			exchangeTable = HelperFile.Load(exchangeTable, Configuration.Path.Default.OptionConfigPath, _exchangeConfigFileName);
			LoadExchangeDefaultAndSave();

			// Display -> Code/Color
			_exchanges = new Dictionary<string, string>();
			_exchangeColors = new Dictionary<string, Color>();
			foreach (DataRow row in exchangeTable.Rows)
			{
				if (row["Display"] != DBNull.Value && row["Display"].ToString() != "")
				{
					// Code
					if (!_exchanges.ContainsKey(row["Display"].ToString()))
					{
						if (row["Code"] != DBNull.Value && row["Code"].ToString() != "")
						{
							_exchanges.Add(row["Display"].ToString(), row["Code"].ToString());
						}
					}
				}

				// Color
				if (row["Code"] != DBNull.Value && row["Code"].ToString() != "")
				{
					if (!_exchangeColors.ContainsKey(row["Code"].ToString()))
					{
						if (row["Color"] != DBNull.Value && row["Color"].ToString() != "")
						{
							_exchangeColors.Add(row["Code"].ToString(), GetColorFromHexString(row["Color"].ToString()));
						}
					}
				}
			}

			foreach (string key in exchanges.Keys)
			{
				switch (key)
				{
					case "AMEX":
						OptionExchangeCode.AMEX = exchanges[key];
						break;
					case "BATS":
						OptionExchangeCode.BATS = exchanges[key];
						break;
					case "BOX":
						OptionExchangeCode.BOX = exchanges[key];
						break;
					case "C2":
						OptionExchangeCode.C2 = exchanges[key];
						break;
					case "CBOE":
						OptionExchangeCode.CBOE = exchanges[key];
						break;
					case "ISE":
						OptionExchangeCode.ISE = exchanges[key];
						break;
					case "NSDQ":
						OptionExchangeCode.NSDQ = exchanges[key];
						break;
					case "PCX":
						OptionExchangeCode.PCX = exchanges[key];
						break;
					case "PHLX":
						OptionExchangeCode.PHLX = exchanges[key];
						break;
				}

				if (OptionExchangeCode.IDs.ContainsKey(exchanges[key]))
				{
					OptionExchangeCode.IDs[exchanges[key]] = key;
				}
				else
				{
					OptionExchangeCode.IDs.Add(exchanges[key], key);
				}

				GLOBAL.HROC.AddToStatusLogs(String.Format("Option Exchange {0} Code = {1}", key, exchanges[key]));
			}
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
