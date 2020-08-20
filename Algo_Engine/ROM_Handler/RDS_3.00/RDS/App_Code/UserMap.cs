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

/// <summary>
/// Summary description for UserMap
/// </summary>
public static class UserMap
{
	private static DataTable _userSecMap;
	public static DataTable UserSecMap
	{
		get
		{
			try
			{
				if (_userSecMap == null)
				{
					GetUserSecMap(ref _userSecMap, ConfigurationManager.AppSettings["UserSecDef"]);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
			return _userSecMap;
		}
	}
	private static void GetUserSecMap(ref DataTable table, string path)
	{
		try
		{
			table = new DataTable("UserSecDef");

			table.Columns.Add("Symbol");
			table.Columns.Add("SecType");
			table.Columns.Add("TickSize");
			table.Columns.Add("ContractSize");
			table.Columns.Add("ExpDate");
			table.Columns.Add("DataSource");
			table.Columns.Add("MDSymbol");

			table.ReadXml(path);
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}
	public static string ReloadUserSecMap()
	{
		_userSecMap = null;

		return UserSecMap.Rows.Count.ToString();
	}
}
