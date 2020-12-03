using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;

namespace ROC
{
	public static class HelperBatchCSV
	{
		private static Dictionary<int, string> _csvFormat;
		public static Dictionary<int, string> CSVFormat
		{
			get
			{
				if (_csvFormat == null)
				{
					_csvFormat = new Dictionary<int, string>();
				}
				return _csvFormat;
			}
		}

		private static Dictionary<string, string> _csvDefaultValue;
		public static Dictionary<string, string> CSVDefaultValue
		{
			get
			{
				if (_csvDefaultValue == null)
				{
					_csvDefaultValue = new Dictionary<string, string>();
				}
				return _csvDefaultValue;
			}
		}

		private static DataTable _csvConfigTable;
		private static DataTable csvConfigTable
		{
			get
			{
				if (_csvConfigTable == null)
				{
					_csvConfigTable = new DataTable("CSVConfig");
					_csvConfigTable.Columns.Add("Name", Type.GetType("System.String"));
					_csvConfigTable.Columns.Add("Col", Type.GetType("System.String"));
					_csvConfigTable.Columns.Add("DefaultValue", Type.GetType("System.String"));
				}
				return _csvConfigTable;
			}
			set
			{
				_csvConfigTable = value;
			}
		}

		private static int _csvStartLine = 0;
		private static int _csvEndLine = 0;

		private static string _filename = "CSVConfig.xml";

		public static void Load()
		{
			try
			{
				csvConfigTable = HelperFile.Load(csvConfigTable, Configuration.Path.Default.CSVConfigPath, _filename);

				// Load Default, and save to file;
				_csvDefaultValue = new Dictionary<string, string>();
				LoadDefaultAndSave();

				_csvFormat = new Dictionary<int, string>();
				foreach (DataRow row in csvConfigTable.Rows)
				{
					switch (row["Name"].ToString())
					{
						case "CSVStartLine":
							Int32.TryParse(row["DefaultValue"].ToString(), out _csvStartLine);
							break;
						case "CSVEndLine":
							Int32.TryParse(row["DefaultValue"].ToString(), out _csvEndLine);
							break;
						default:
							AddToCSVFormat(row["Col"].ToString(), row["Name"].ToString(), row["DefaultValue"].ToString());
							break;
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private static void AddToCSVFormat(string col, string name, string defaultValue)
		{
			int colNumber = 0;

			if (col != "")
			{
				if (Int32.TryParse(col, out colNumber))
				{
					AddToCSVFormat(colNumber, name);
				}
			}

			if (name != "")
			{
				AddToCSVDefaultValue(name, defaultValue);
			}
		}
		private static void AddToCSVFormat(int colNumber, string name)
		{
			if (name != "" && name.Substring(0, 1) != "!")
			{
				if (_csvFormat.ContainsKey(colNumber))
				{
					_csvFormat[colNumber] = name;
				}
				else
				{
					_csvFormat.Add(colNumber, name);
				}
			}
		}
		private static void AddToCSVDefaultValue(string name, string defaultValue)
		{
			if (defaultValue != "")
			{
				if (_csvDefaultValue.ContainsKey(name))
				{
					_csvDefaultValue[name] = defaultValue;
				}
				else
				{
					_csvDefaultValue.Add(name, defaultValue);
				}
			}
		}

		private static void LoadDefaultAndSave()
		{
			try
			{
				if (csvConfigTable.Rows.Count == 0)
				{
					csvConfigTable.Rows.Add(new object[] { "Symbol", "0", "" });
					csvConfigTable.Rows.Add(new object[] { "Qty", "1", "" });
					csvConfigTable.Rows.Add(new object[] { "Side", "2", "" });
					csvConfigTable.Rows.Add(new object[] { "Price", "3", "" });
					csvConfigTable.Rows.Add(new object[] { "Exchange", "4", "" });
					csvConfigTable.Rows.Add(new object[] { "LocalAccountAcrn", "5", "" });
					csvConfigTable.Rows.Add(new object[] { "TraderFor", "6", "" });
					csvConfigTable.Rows.Add(new object[] { "Display", "7", "" });
					csvConfigTable.Rows.Add(new object[] { "ShortLender", "8", "" });
					csvConfigTable.Rows.Add(new object[] { "Duration", "9", "" });
					csvConfigTable.Rows.Add(new object[] { "OrderType", "10", "" });
					csvConfigTable.Rows.Add(new object[] { "Instruction", "11", "" });
					csvConfigTable.Rows.Add(new object[] { "AlgoType", "12", "" });
					csvConfigTable.Rows.Add(new object[] { "ExecInstruction", "13", "" });
					csvConfigTable.Rows.Add(new object[] { "StopPrice", "14", "" });
					csvConfigTable.Rows.Add(new object[] { "StartTime", "15", "" });
					csvConfigTable.Rows.Add(new object[] { "EndTime", "16", "" });
					csvConfigTable.Rows.Add(new object[] { "Note", "", "" });
					csvConfigTable.Rows.Add(new object[] { "PegPrice", "", "" });
					csvConfigTable.Rows.Add(new object[] { "ProgramTrade", "", "" });
					csvConfigTable.Rows.Add(new object[] { "CSVStartLine", "", "0" });
					csvConfigTable.Rows.Add(new object[] { "CSVEndLine", "", "0" });

					HelperFile.Save(csvConfigTable, Configuration.Path.Default.CSVConfigPath, _filename);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static void Save()
		{
			HelperFile.Save(csvConfigTable, Configuration.Path.Default.CSVConfigPath, _filename);
		}

		public static string[] LoadCSV(string path, string filename)
		{
			string[] csvItems = new string[0];

			if (CSVFormat.Count == 0)
			{
				GLOBAL.HROC.AddToAlerts("Invalid or Missing CSV File Format");
			}
			else
			{
				string csvString = HelperFile.Load(path, filename, false);

				csvItems = csvString.Split(new string[] { "\r", "\r\n", "\n" }, StringSplitOptions.RemoveEmptyEntries);
				if (_csvStartLine > 0 || _csvEndLine > 0)
				{
					int count = csvItems.Length - _csvStartLine - _csvEndLine;
					if (count > 0)
					{
						csvItems = CopyFromItemArray(csvItems, _csvStartLine, count);
					}
					else
					{
						GLOBAL.HROC.AddToAlerts("CSV is empty or has lines less then specified");
						csvItems = new string[0];
					}
				}
			}

			return csvItems;
		}

		private static string[] CopyFromItemArray(string[] sourceArray, long startIndex, long count)
		{
			string[] byteArrayResult = new string[count];
			Array.Copy(sourceArray, startIndex, byteArrayResult, 0, count);
			return byteArrayResult;
		}
	}
}
