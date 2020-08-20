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

public static class RdsCache
{
	private static DataTable _romAuthorizedUsers;
	private static DataTable _romAccountProfiles;
	private static DataTable _romAccountDestinations;

	private static Dictionary<string, ArrayList> _romUserAccounts = new Dictionary<string,ArrayList>();
	private static Dictionary<string, ArrayList> _romUserOrders = new Dictionary<string, ArrayList>();
	private static Dictionary<string, ArrayList> _romUserExecutions = new Dictionary<string, ArrayList>();

	private static Dictionary<string, ArrayList> _tposSecurityInfo = new Dictionary<string, ArrayList>();
	private static Dictionary<string, ArrayList> _tposOptionChain = new Dictionary<string, ArrayList>();
	private static Dictionary<string, ArrayList> _tposFutureChain = new Dictionary<string, ArrayList>();
	private static Dictionary<string, ArrayList> _tposFutureChainByBase = new Dictionary<string, ArrayList>();
	private static Dictionary<string, ArrayList> _tposSSFutureChain = new Dictionary<string, ArrayList>();

	private static Dictionary<string, ArrayList> _tposAccounts = new Dictionary<string, ArrayList>();
	private static Dictionary<string, ArrayList> _tposUserAccounts = new Dictionary<string, ArrayList>();

	private static DataTable _tposUserPositions;
	private static DataTable _tposUserTrades;

	public static bool IsEmpty(DataTable table)
	{
		bool isEmpty = false;

		try
		{
			if (table == null)
			{
				isEmpty = true;
			}
			else if (table.Columns.Count == 0)
			{
				isEmpty = true;
			}
			else
			{
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}

		return isEmpty;
	}

	public static bool Contains(DataTable table, string filter)
	{
		bool hasIt = false;

		try
		{
			if (!IsEmpty(table))
			{
				lock (table)
				{
					DataRow[] rows = table.Select(filter);
					if (rows.Length > 0)
					{
						hasIt = true;
					}
				}
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}

		return hasIt;
	}
	public static bool Contains(DataTable table, string filter, out DataRow[] rows)
	{
		bool hasIt = false;
		rows = null;

		try
		{
			if (!IsEmpty(table))
			{
				lock (table)
				{
					rows = table.Select(filter);
					if (rows.Length > 0)
					{
						hasIt = true;
					}
				}
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}

		return hasIt;
	}

	public static DataTable RomAuthorizedUsers
	{
		get
		{
			return _romAuthorizedUsers;
		}
		set
		{
			try
			{
				if (IsEmpty(_romAuthorizedUsers))
				{
					_romAuthorizedUsers = value;
				}
				else
				{
					string key = "";
					DataRow[] oldRows;
					foreach (DataRow row in value.Rows)
					{
						// Check to see if its already cached or not
						key = row["OMuserID"].ToString();
						oldRows = _romAuthorizedUsers.Select("OMuserID = " + key);

						if (oldRows.Length > 0)
						{
							// remove the old rows
							foreach (DataRow oldRow in oldRows)
							{
								_romAuthorizedUsers.Rows.Remove(oldRow);
							}
						}

						// put the new row in
						_romAuthorizedUsers.Rows.Add(row.ItemArray);
					}
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
		}
	}
	public static DataTable RomAccountProfiles
	{
		get
		{
			return _romAccountProfiles;
		}
		set
		{
			try
			{
				if (IsEmpty(_romAccountProfiles))
				{
					_romAccountProfiles = value;
				}
				else
				{
					string user = "";
					string interfaceID = "";
					DataRow[] oldRows;
					foreach (DataRow row in value.Rows)
					{
						// Check to see if its already cached or not
						user = row["user"].ToString();
						interfaceID = row["OMInterfaceID"].ToString();
						oldRows = _romAccountProfiles.Select("user = '" + user + "' and OMInterfaceID = " + interfaceID);

						if (oldRows.Length > 0)
						{
							// remove the old rows
							foreach (DataRow oldRow in oldRows)
							{
								_romAccountProfiles.Rows.Remove(oldRow);
							}
						}

						// put the new row in
						_romAccountProfiles.Rows.Add(row.ItemArray);
					}
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
		}
	}
	public static DataTable RomAccoutDestinations
	{
		get
		{
			return _romAccountDestinations;
		}
		set
		{
			try
			{
				if (IsEmpty(_romAccountDestinations))
				{
					_romAccountDestinations = value;
				}
				else
				{
					string interfaceID = "";
					DataRow[] oldRows;
					foreach (DataRow row in value.Rows)
					{
						// Check to see if its already cached or not
						interfaceID = row["interfaceID"].ToString();
						oldRows = _romAccountDestinations.Select("interfaceID = " + interfaceID);

						if (oldRows.Length > 0)
						{
							// remove the old rows
							foreach (DataRow oldRow in oldRows)
							{
								_romAccountDestinations.Rows.Remove(oldRow);
							}
						}

						_romAccountDestinations.Rows.Add(row.ItemArray);
					}
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}
		}
	}

	public static Dictionary<string, ArrayList> RomUserAccounts
	{
		get
		{
			return _romUserAccounts;
		}
		set
		{
			_romUserAccounts = value;
		}
	}
	public static Dictionary<string, ArrayList> RomUserOrders
	{
		get
		{
			return _romUserOrders;
		}
		set
		{
			_romUserOrders = value;
		}
	}
	public static Dictionary<string, ArrayList> RomUserExecutions
	{
		get
		{
			return _romUserExecutions;
		}
		set
		{
			_romUserExecutions = value;
		}
	}

	public static Dictionary<string, ArrayList> TposSecurityInfo
	{
		get
		{
			return _tposSecurityInfo;
		}
		set
		{
			_tposSecurityInfo = value;
		}
	}
	public static Dictionary<string, ArrayList> TposOptionChain
	{
		get
		{
			return _tposOptionChain;
		}
		set
		{
			_tposOptionChain = value;
		}
	}
	public static Dictionary<string, ArrayList> TposFutureChain
	{
		get
		{
			return _tposFutureChain;
		}
		set
		{
			_tposFutureChain = value;
		}
	}
	public static Dictionary<string, ArrayList> TposFutureChainByBase
	{
		get
		{
			return _tposFutureChainByBase;
		}
		set
		{
			_tposFutureChainByBase = value;
		}
	}
	public static Dictionary<string, ArrayList> TposSSFutureChain
	{
		get
		{
			return _tposSSFutureChain;
		}
		set
		{
			_tposSSFutureChain = value;
		}
	}

	// TraderAcronym
	public static Dictionary<string, ArrayList> TposAccounts
	{
		get
		{
			return _tposAccounts;
		}
		set
		{
			_tposAccounts = value;
		}
	}
	public static Dictionary<string, ArrayList> TposUserAccounts
	{
		get
		{
			return _tposUserAccounts;
		}
		set
		{
			_tposUserAccounts = value;
		}
	}

	// m_InstrumentID, m_ValidTraderKey
	public static DataTable TposUserPositions
	{
		get
		{
			if (_tposUserPositions == null)
			{
				_tposUserPositions = new DataTable();

				DataColumn col;

				col = new DataColumn("m_AsOfDate", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_AvgCost", Type.GetType("System.Double"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_ClearingAccount", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_CurQty", Type.GetType("System.Int32"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_DisplayFormat", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_ExpDate", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_InstrumentID", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_Multiplier", Type.GetType("System.Double"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_NotionalAmount", Type.GetType("System.Int32"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_OpenMark", Type.GetType("System.Double"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_OpenQty", Type.GetType("System.Int32"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_Portfolio", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_PutCall", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_RealizedPnL", Type.GetType("System.Double"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_SecType", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_Strike", Type.GetType("System.Double"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_Symbol", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_Ticker", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_TradeDate", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_TradeGroup", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_TraderAcronym", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_UndSecType", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_UndSymbol", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_ValidTraderKey", Type.GetType("System.String"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("m_Version", Type.GetType("System.Int32"));
				_tposUserPositions.Columns.Add(col);

				col = new DataColumn("position", Type.GetType("System.Object"));
				_tposUserPositions.Columns.Add(col);
			}
			return _tposUserPositions;
		}
		set
		{
			_tposUserPositions = value;
		}
	}

	//m_TradeID
	public static DataTable TposUserTrades
	{
		get
		{
			if (_tposUserTrades == null)
			{
				_tposUserTrades = new DataTable();

				DataColumn col;

				col = new DataColumn("m_ActiveState", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ClearingAccount", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ClearingStatus", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Commission", Type.GetType("System.Double"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ContraBroker", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ContraFirm", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Exchange", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ExpDate", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ExtDescription", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ExtTradeID", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_LastModDate", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_LastModReason", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_LastModReasonID", Type.GetType("System.Int32"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_LastModTime", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Note", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Portfolio", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Price", Type.GetType("System.Double"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_PutCall", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Qty", Type.GetType("System.Int32"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_SecType", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_SettleDate", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_ShortSaleFlag", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Strike", Type.GetType("System.Double"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Symbol", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Ticker", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_TradeDate", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_TradeGroup", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_TradeID", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_TraderAcronym", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_TradeSource", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_TradeTime", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_UndSecType", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_UndSymbol", Type.GetType("System.String"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("m_Version", Type.GetType("System.Int32"));
				_tposUserTrades.Columns.Add(col);

				col = new DataColumn("trade", Type.GetType("System.Object"));
				_tposUserTrades.Columns.Add(col);
			}
			return _tposUserTrades;
		}
		set
		{
			_tposUserTrades = value;
		}
	}
}
