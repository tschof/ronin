using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Diagnostics;
using System.Collections;
using System.Collections.Generic;

/// <summary>
/// Summary description for RdsTposMap
/// </summary>
public static class TposMap
{
	private static DataTable _symbolMap;
	public static DataTable SymbolMap
	{
		get
		{
			try
			{
				if (_symbolMap == null)
				{
					GetSymbolMap(ref _symbolMap, @"\\TposFutureMap.xml");
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
			return _symbolMap;
		}
	}
	private static void GetSymbolMap(ref DataTable table, string xml)
	{
		try
		{
			string path = (string)AppDomain.CurrentDomain.GetData("DataDirectory");
			path = path + xml;

			table = new DataTable("TposFutureMap");
			table.Columns.Add("ROC");
			table.Columns.Add("TPOS");
			table.ReadXml(path);
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static DataTable _symbolMultMap;
	public static DataTable SymbolMultMap
	{
		get
		{
			try
			{
				if (_symbolMultMap == null)
				{
					GetSymbolMultMap(ref _symbolMultMap, @"\\TposFutureMultipleMap.xml");
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
			return _symbolMultMap;
		}
	}
	private static void GetSymbolMultMap(ref DataTable table, string xml)
	{
		try
		{
			string path = (string)AppDomain.CurrentDomain.GetData("DataDirectory");
			path = path + xml;

			table = new DataTable("TposFutureMultipleMap");
			table.Columns.Add("ROC");
			table.Columns.Add("MULT");
			table.ReadXml(path);
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static DataTable _symbolOptionMap;
	public static DataTable SymbolOptionMap
	{
		get
		{
			try
			{
				if (_symbolOptionMap == null)
				{
					GetSymbolOptionMap(ref _symbolOptionMap, @"\\TposOptionOnFutureMap.xml");
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
			return _symbolOptionMap;
		}
	}
	private static void GetSymbolOptionMap(ref DataTable table, string xml)
	{
		try
		{
			string path = (string)AppDomain.CurrentDomain.GetData("DataDirectory");
			path = path + xml;

			table = new DataTable("TposOptionOnFutureMap");
			table.Columns.Add("ROC");
			table.Columns.Add("OPTION");
			table.ReadXml(path);
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static DataTable _groupFilterMap;
	private static DataTable groupFilterMap
	{
		get
		{
			try
			{
				if (_groupFilterMap == null)
				{
					GetGroupFilterMap(ref _groupFilterMap, @"\\TposGroupFilter.xml");
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
			return _groupFilterMap;
		}
	}
	private static void GetGroupFilterMap(ref DataTable table, string xml)
	{
		try
		{
			string path = (string)AppDomain.CurrentDomain.GetData("DataDirectory");
			path = path + xml;

			table = new DataTable("TposGroupFilterMap");
			table.Columns.Add("GroupName");
			table.ReadXml(path);
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static List<string> _groupFilter;
	public static List<string> GroupFilter
	{
		get
		{
			if (_groupFilter == null)
			{
				GetGroupFilter();
			}
			return _groupFilter;
		}
	}
	private static void GetGroupFilter()
	{
		_groupFilter = new List<string>();
		foreach (DataRow row in groupFilterMap.Rows)
		{
			if (!_groupFilter.Contains(row["GroupName"].ToString().ToUpper()))
			{
				_groupFilter.Add(row["GroupName"].ToString().ToUpper());
			}
		}
	}
}
