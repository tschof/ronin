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
					RDSEx.ROCSecurity.SetPlusOnesOnFutures(_plusOne);
				}
				return _plusOne;
			}
		}

		private static void LoadPlusOneMothSymbols()
		{
			HelperFile.Load(plusOneMonthTable, Configuration.Path.Default.FutureConfigPath, _plusOneMonthFileName);
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
					plusOneMonthTable.Rows.Add("QM");
					plusOneMonthTable.Rows.Add("XGN");
					plusOneMonthTable.Rows.Add("QO");
					plusOneMonthTable.Rows.Add("EN");
					plusOneMonthTable.Rows.Add("CO");
					plusOneMonthTable.Rows.Add("QG");
					plusOneMonthTable.Rows.Add("QI");
					plusOneMonthTable.Rows.Add("SB");

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
			HelperFile.Load(doNotTranslateTable, Configuration.Path.Default.FutureConfigPath, _doNotTranslateFileName);
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
					doNotTranslateTable.Rows.Add("FF");
					doNotTranslateTable.Rows.Add("MW");
					doNotTranslateTable.Rows.Add("US");
					doNotTranslateTable.Rows.Add("TY");
					doNotTranslateTable.Rows.Add("FV");
					doNotTranslateTable.Rows.Add("TU");
					doNotTranslateTable.Rows.Add("ER");
					doNotTranslateTable.Rows.Add("AD");
					doNotTranslateTable.Rows.Add("BP");
					doNotTranslateTable.Rows.Add("CD");
					doNotTranslateTable.Rows.Add("EC");
					doNotTranslateTable.Rows.Add("JY");
					doNotTranslateTable.Rows.Add("SF");
					doNotTranslateTable.Rows.Add("NK");
					doNotTranslateTable.Rows.Add("JE");
					doNotTranslateTable.Rows.Add("ED");
					doNotTranslateTable.Rows.Add("UL");

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
