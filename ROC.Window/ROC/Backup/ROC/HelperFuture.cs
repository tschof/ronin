using System;
using System.Collections.Generic;
using System.Text;
using System.Data;

namespace ROC
{
	public static class HelperFuture
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
					DataGridViewEx.GLOBAL.Future.PlusOne = _plusOne;
					RDSEx.GLOBAL.Future.PlusOne = _plusOne;
				}
				return _plusOne;
			}
		}

		private static void LoadPlusOneMothSymbols()
		{
			plusOneMonthTable = HelperFile.Load(plusOneMonthTable, Configuration.Path.Default.FutureConfigPath, _plusOneMonthFileName);
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
					plusOneMonthTable.Rows.Add(new object[] { "QM" });
					plusOneMonthTable.Rows.Add(new object[] { "XGN" });
					plusOneMonthTable.Rows.Add(new object[] { "QO" });
					plusOneMonthTable.Rows.Add(new object[] { "EN" });
					plusOneMonthTable.Rows.Add(new object[] { "CO" });
					plusOneMonthTable.Rows.Add(new object[] { "QG" });
					plusOneMonthTable.Rows.Add(new object[] { "QI" });
					plusOneMonthTable.Rows.Add(new object[] { "SB" });

					HelperFile.Save(plusOneMonthTable, Configuration.Path.Default.FutureConfigPath, _plusOneMonthFileName);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Do Not Translate -

		private static string _doNotTranslateFileName = "DoNotTranslate.xml";

		private static DataTable _doNotTranslateTable;
		private static DataTable doNotTranslateTable
		{
			get
			{
				if (_doNotTranslateTable == null)
				{
					_doNotTranslateTable = new DataTable("DoNotTranslate");
					_doNotTranslateTable.Columns.Add("Symbol", Type.GetType("System.String"));
				}
				return _doNotTranslateTable;
			}
			set
			{
				_doNotTranslateTable = value;
			}
		}

		private static List<string> _doNotTranslate;
		public static List<string> DoNotTranslate
		{
			get
			{
				if (_doNotTranslate == null)
				{
					LoadDoNotTranslateSymbols();
				}
				return _doNotTranslate;
			}
		}

		private static void LoadDoNotTranslateSymbols()
		{
			doNotTranslateTable = HelperFile.Load(doNotTranslateTable, Configuration.Path.Default.FutureConfigPath, _doNotTranslateFileName);
			LoadDoNotTranslateDefaultAndSave();

			_doNotTranslate = new List<string>();
			foreach (DataRow row in doNotTranslateTable.Rows)
			{
				if (row["Symbol"] != DBNull.Value && row["Symbol"].ToString() != "" && !_doNotTranslate.Contains(row["Symbol"].ToString()))
				{
					_doNotTranslate.Add(row["Symbol"].ToString());
				}
			} 
		}

		private static void LoadDoNotTranslateDefaultAndSave()
		{
			try
			{
				if (doNotTranslateTable.Rows.Count == 0)
				{
					doNotTranslateTable.Rows.Add(new object[] { "FF" });
					doNotTranslateTable.Rows.Add(new object[] { "MW" });
					doNotTranslateTable.Rows.Add(new object[] { "US" });
					doNotTranslateTable.Rows.Add(new object[] { "TY" });
					doNotTranslateTable.Rows.Add(new object[] { "FV" });
					doNotTranslateTable.Rows.Add(new object[] { "TU" });
					doNotTranslateTable.Rows.Add(new object[] { "ER" });
					doNotTranslateTable.Rows.Add(new object[] { "AD" });
					doNotTranslateTable.Rows.Add(new object[] { "BP" });
					doNotTranslateTable.Rows.Add(new object[] { "CD" });
					doNotTranslateTable.Rows.Add(new object[] { "EC" });
					doNotTranslateTable.Rows.Add(new object[] { "JY" });
					doNotTranslateTable.Rows.Add(new object[] { "SF" });
					doNotTranslateTable.Rows.Add(new object[] { "NK" });
					doNotTranslateTable.Rows.Add(new object[] { "JE" });
					doNotTranslateTable.Rows.Add(new object[] { "ED" });
					doNotTranslateTable.Rows.Add(new object[] { "UL" });

					HelperFile.Save(doNotTranslateTable, Configuration.Path.Default.FutureConfigPath, _doNotTranslateFileName);
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
