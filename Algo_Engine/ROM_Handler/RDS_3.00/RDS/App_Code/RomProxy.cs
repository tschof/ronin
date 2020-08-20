using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Data.Odbc;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;

public static class RomProxy
{
	#region ROM Connection

	private static string _server = ConfigurationManager.AppSettings["RomDBServer"];
	private static string _user = ConfigurationManager.AppSettings["RomDBUser"];
	private static string _pass = ConfigurationManager.AppSettings["RomDBPass"];
	private static string _dbName = ConfigurationManager.AppSettings["RomDBName"];
	private static string _connectionString = "Dsn=" + _server + ";Uid=" + _user + ";Pwd=" + _pass + ";";
	
	private static OdbcConnection _connection = null;

	public static OdbcConnection Connection
	{
		get
		{
			try
			{
				Connect();
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}

			return _connection;
		}
	}

	private static void Connect()
	{
		try
		{
			if (_connection == null)
			{
				Logger.Logit(String.Concat("Connecting To DB - ", _connectionString));
				_connection = new OdbcConnection(_connectionString);
				_connection.StateChange += new StateChangeEventHandler(_connection_StateChange);
				_connection.Open();
			}
			else if (_connection.State != ConnectionState.Open)
			{
				Disconnect();
				Connect();
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static void Disconnect()
	{
		try
		{
			if (_connection != null)
			{
				_connection.StateChange -= new StateChangeEventHandler(_connection_StateChange);
				_connection.Close();
				_connection.Dispose();
				_connection = null;
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static void _connection_StateChange(object sender, StateChangeEventArgs e)
	{
		try
		{
			switch (e.CurrentState)
			{
				case ConnectionState.Open:
					Logger.Logit(String.Concat("Connected To DB - ", _server));
					break;
				case ConnectionState.Broken:
					Disconnect();
					Logger.Logit(String.Concat("Connection Broken - ", _server));
					break;
				case ConnectionState.Closed:
					Disconnect();
					Logger.Logit(String.Concat("Connection Closed - ", _server));
					break;
				case ConnectionState.Connecting:
					Logger.Logit(10, String.Concat("Connecting - ", _server));
					break;
				case ConnectionState.Executing:
					Logger.Logit(10, String.Concat("Executing - ", _server));
					break;
				case ConnectionState.Fetching:
					Logger.Logit(10, String.Concat("Fecthing - ", _server));
					break;
				default:
					Disconnect();
					break;
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	#endregion

	#region ROM Commands

	private static DataTable _romCommands;

	public static DataTable RomCommands
	{
		get
		{
			try
			{
				if (_romCommands == null)
				{
					GetCommands(ref _romCommands, @"\\SqlCommands.xml");
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
			}

			return _romCommands;
		}
	}

	private static void GetCommands(ref DataTable data, string xml)
	{
		try
		{
			string path = (string)AppDomain.CurrentDomain.GetData("DataDirectory");
			path = path + xml;

			DataSet ds = new DataSet();
			ds.ReadXml(path);

			data = ds.Tables[0];
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}
	}

	private static string GetCommand(string key)
	{
		try
		{
			DataRow[] rows = RomCommands.Select(String.Concat(new object[] { "key = '", key, "'" }));
			if (rows.Length == 1)
			{
				return rows[0]["cmd"].ToString();
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}

		return "";
	}

	#endregion

	#region ROM User Verification

	public static bool CheckAuthorization(string user, string pass, bool reload)
	{
		bool tOk = false;

		try
		{
			string filter = "username = '" + user + "' and password = '" + pass + "'";
			DataRow[] rows;
			if (!reload)
			{
				if (RdsCache.Contains(RdsCache.RomAuthorizedUsers, filter, out rows))
				{
					tOk = true;
				}
			}

			if (!tOk)
			{
				string sql = GetCommand("CheckAuthorization");
				if (sql != "")
				{
					sql = sql.Replace("@user", user);
					sql = sql.Replace("@pass", pass);

					DataTable dt = new DataTable();
					GetData(ref dt, sql);

					if (dt.Rows.Count > 0)
					{
						RdsCache.RomAuthorizedUsers = dt;
						tOk = true;
					}
				}
			}
		}
		catch (Exception ex)
		{
			Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
		}

		return tOk;
	}

	#endregion

	#region ROM User Profile

	public static ArrayList BuildUserProfile(string user, bool reload)
	{
		ArrayList accounts = null;

		if (!reload)
		{
			if (RdsCache.RomUserAccounts.ContainsKey(user))
			{
				accounts = RdsCache.RomUserAccounts[user];
			}
		}

		if (accounts == null)
		{
			string filter = "user = '" + user + "'";
			DataRow[] rows = null;

			if (rows == null || rows.Length < 1)
			{
				string sql = GetCommand("UserAccounts");
				if (sql != "")
				{

					sql = sql.Replace("@user", user);

					DataTable dt = new DataTable();
					GetData(ref dt, sql);

					if (dt.Rows.Count > 0)
					{
						if (!dt.Columns.Contains("user"))
						{
							DataColumn column;
							column = new DataColumn("user", Type.GetType("System.String"));
							column.DefaultValue = user;
							column.ReadOnly = true;
							column.AllowDBNull = false;

							dt.Columns.Add(column);
						}
					}
					rows = dt.Select();
				}
			}

			Dictionary<string, TraderAcr> traderAccts = new Dictionary<string, TraderAcr>();
			TraderAcr traderAcct;

			string tradeFor = "";
			int interfaceID = -1;
			bool newAcctProfile;
			foreach (DataRow row in rows)
			{
				interfaceID = row[0] == null ? -1 : Int32.Parse(row["OMInterfaceID"].ToString());
				if (interfaceID < 0) continue;

				tradeFor = row[3] == null ? "" : row["TraderAcr"].ToString().Trim();
				if (tradeFor == "") continue;

				if (!traderAccts.ContainsKey(tradeFor))
				{
					traderAcct = new TraderAcr();
					traderAcct.tradeFor = tradeFor;
					traderAcct.localAcAcrn = row[4] == null ? "" : row["LocalAcAcr"].ToString().Trim();
					traderAccts.Add(tradeFor, traderAcct);
				}
				else
				{
					traderAcct = traderAccts[tradeFor];
				}

				newAcctProfile = true;
				foreach (AcctProfile acctProfile in traderAcct.acctProfiles)
				{
					if (acctProfile.interfaceID == interfaceID)
					{
						newAcctProfile = false;
						break;
					}
				}

				if (newAcctProfile)
				{
					traderAcct.acctProfiles.Add(BuildProfileAcctProfile(row, interfaceID, reload));
				}

				traderAccts[tradeFor] = traderAcct;
			}

			accounts = BuildProfileArrayList(traderAccts);
		}

		if (accounts != null)
		{
			if (RdsCache.RomUserAccounts.ContainsKey(user))
			{
				RdsCache.RomUserAccounts[user] = accounts;
			}
			else
			{
				RdsCache.RomUserAccounts.Add(user, accounts);
			}
		}
		return accounts;
	}

	public static ArrayList BuildProfileDestination(int interfaceID, bool reload)
	{
		ArrayList destinations = null;
		Dictionary<int, string> destinationKeys = null;

		string filter = "interfaceID = " + interfaceID;
		DataRow[] rows = null;

		string sql = GetCommand("AccountDestinations");
		if (sql != "")
		{
			sql = sql.Replace("@interfaceID", interfaceID.ToString());

			DataTable dt = new DataTable();
			GetData(ref dt, sql);

			if (dt.Rows.Count > 0)
			{
				if (!dt.Columns.Contains("interfaceID"))
				{
					DataColumn column;
					column = new DataColumn("interfaceID", Type.GetType("System.Int64"));
					column.DefaultValue = interfaceID;
					column.ReadOnly = true;
					column.AllowDBNull = false;

					dt.Columns.Add(column);
				}
			}
			rows = dt.Select();
		}

		destinations = new ArrayList();
		destinationKeys = new Dictionary<int, string>();
		AcctDestination destination;

		int destID = -1;
		foreach (DataRow row in rows)
		{
			destID = row["DestID"] == null ? -1 : Int32.Parse(row["DestID"].ToString());
			if (destID < 0) continue;

			// Prevent duplicated destIDs
			if (!destinationKeys.ContainsKey(destID))
			{
				destination = new AcctDestination();
				destination.destID = destID;
				destination.secType = row[1] == null ? "" : row["sec_type"].ToString().Trim();

				destination.shortName = row[3] == null ? "" : row["dest_short_name"].ToString().Trim();
				destination.giveupFirmID = row[4] == null ? "" : row["GiveupFirmIdentifier"].ToString().Trim();
				destination.cmtaFirmID = row[5] == null ? "" : row["CMTAFirmIdentifier"].ToString().Trim();
				
				if (row.ItemArray.Length == 8)
				{
					destination.algoFlag = row[6] == null ? "" : row["cancel_replace_flag"].ToString().Trim();
				}

				destinationKeys.Add(destID, "");
				destinations.Add(destination);
			}
		}

		return destinations;
	}

	public static AcctProfile BuildProfileAcctProfile(DataRow row, int interfaceID, bool reload)
	{
		// new profile
		AcctProfile acctProfile = new AcctProfile();
		acctProfile.interfaceID = interfaceID;
		acctProfile.account = row[1] == null ? "" : row["Account"].ToString().Trim();
		acctProfile.type = row[2] == null ? "" : row["Type"].ToString().Trim();
		//TraderAcr 3
		acctProfile.localAcAcrn = row[4] == null ? "" : row["LocalAcAcr"].ToString().Trim();
		acctProfile.clearingAcID = row[5] == null ? "" : row["ClearingAcID"].ToString().Trim();
		acctProfile.firmAcr = row[6] == null ? "" : row["firmAcr"].ToString().Trim();
		//deflt	7
		acctProfile.omAcctType = row[8] == null ? "" : row["OMAccountType"].ToString().Trim();
		acctProfile.capacity = row[9] == null ? "" : row["capacity"].ToString().Trim();
		acctProfile.clearingFirmID = row[10] == null ? "" : row["clearingFirmID"].ToString();
		//RetailAccountID 11					
		//clearing_ac_acr 12
		acctProfile.occAcr = row[13] == null ? "" : row["OCCAcr"].ToString().Trim();
		acctProfile.homeExchange = row[14] == null ? "" : row["HomeExch"].ToString().Trim();

		// Load destinations for this interfaceID
		acctProfile.accountDestinations = BuildProfileDestination(interfaceID, reload);

		return acctProfile;
	}

	public static ArrayList BuildProfileArrayList(Dictionary<string, TraderAcr> traderAccts)
	{
		ArrayList accounts = null;
		TraderAcr traderAcct = null;

		if (traderAccts.Count > 0)
		{
			accounts = new ArrayList();
			foreach (string tkey in traderAccts.Keys)
			{
				traderAcct = traderAccts[tkey];
				accounts.Add(traderAcct);
			}
		}

		return accounts;
	}

	#endregion

	#region ROM User Orders

	public static ArrayList BuildUserOrders(string user, bool reload)
	{
		ArrayList orders = null;

		if (!reload)
		{
			if (RdsCache.RomUserOrders.ContainsKey(user))
			{
				orders = RdsCache.RomUserOrders[user];
			}
		}

		if (orders == null)
		{
			DataRow[] rows = null;

			string sql = GetCommand("UserOrders");
			if (sql != "")
			{
				sql = sql.Replace("@user", user);

				DataTable dt = new DataTable();
				GetData(ref dt, sql);

				rows = dt.Select();
			}

			orders = new ArrayList();
			Order order;
			foreach (DataRow row in rows)
			{
				order = new Order();

				try
				{
					order.callPut = row[0] == null ? "" : row[0].ToString().Trim();
					order.clearingAccount = row[1] == null ? "" : row[1].ToString().Trim();
					order.clearingID = row[2] == null ? "" : row[2].ToString().Trim();
					order.cumQty = row[3] == null ? 0 : Int32.Parse(row[3].ToString());
					order.destID = row[4] == null ? 0 : Int32.Parse(row[4].ToString());
					order.expDate = row[5] == null ? "" : row[5].ToString().Trim();
					order.firm = row[6] == null ? "" : row[6].ToString().Trim();
					order.instructions = row[7] == null ? "" : row[7].ToString().Trim();
					order.leavesQty = row[8] == null ? 0 : Int32.Parse(row[8].ToString());
					order.localAcct = row[9] == null ? "" : row[9].ToString().Trim();
					order.maxFloor = row[10] == null ? "" : row[10].ToString().Trim();
					order.multiplier = row[11] == null ? 0 : Double.Parse(row[11].ToString());
					order.omTag = row[12] == null ? "" : row[12].ToString().Trim();
					order.omTime = (row[13] == null || row[13].ToString() == "") ? DateTime.Parse("0001-01-01T00:00:00") : DateTime.Parse(row[13].ToString());
					order.openClose = row[14] == null ? "" : row[14].ToString().Trim();
					order.orderExpiresDate = (row[15] == null || row[15].ToString() == "") ? DateTime.Parse("0001-01-01T00:00:00") : DateTime.Parse(row[15].ToString());
					order.orderType = row[16] == null ? 0 : Int32.Parse(row[16].ToString());
					order.originalShares = row[17] == null ? 0 : Int32.Parse(row[17].ToString());
					order.owner = row[18] == null ? "" : row[18].ToString().Trim();
					order.price = row[19] == null ? 0 : Double.Parse(row[19].ToString());
					order.qty = row[20] == null ? 0 : Int32.Parse(row[20].ToString());
					order.secType = row[21] == null ? "" : row[21].ToString().Trim();
					order.side = row[22] == null ? 0 : Int32.Parse(row[22].ToString());
					order.status = row[23] == null ? 0 : Int32.Parse(row[23].ToString());
					order.stopPrice = row[24] == null ? 0 : Double.Parse(row[24].ToString());
					order.strikePrice = row[25] == null ? 0 : Double.Parse(row[25].ToString());
					order.symbol = row[26] == null ? "" : row[26].ToString().Trim();
					order.tag = row[27] == null ? "" : row[27].ToString().Trim();
					order.text = row[28] == null ? "" : row[28].ToString().Trim();
					order.tif = row[29] == null ? 0 : Int32.Parse(row[29].ToString());
					order.tradeFor = row[30] == null ? "" : row[30].ToString().Trim();
					order.trader = row[31] == null ? "" : row[31].ToString().Trim();
					order.underlying = row[32] == null ? "" : row[32].ToString().Trim();
					order.ex_short_name = row[33] == null ? "" : row[33].ToString().Trim();
					order.effectiveTime = (row[34] == null || row[34].ToString() == "") ? DateTime.Parse("0001-01-01T00:00:00") : DateTime.Parse(row[34].ToString());
					if (row.ItemArray.Length > 35)
					{
						order.avgPrice = row[35] == null ? 0 : Double.Parse(row[35].ToString());
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
				}
				orders.Add(order);
			}
		}

		if (orders != null)
		{
			if (RdsCache.RomUserOrders.ContainsKey(user))
			{
				RdsCache.RomUserOrders[user] = orders;
			}
			else
			{
				RdsCache.RomUserOrders.Add(user, orders);
			}
		}
		return orders;
	}

	#endregion

	#region ROM User Executions

	public static ArrayList BuildUserExecutions(string user, bool reload)
	{
		ArrayList executions = null;

		if (!reload)
		{
			if (RdsCache.RomUserExecutions.ContainsKey(user))
			{
				executions = RdsCache.RomUserExecutions[user];
			}
		}

		if (executions == null)
		{
			DataRow[] rows = null;

			string sql = GetCommand("UserExecutions");
			if (sql != "")
			{
				sql = sql.Replace("@user", user);

				DataTable dt = new DataTable();
				GetData(ref dt, sql);

				rows = dt.Select();
			}

			executions = new ArrayList();
			Execution execution;
			foreach (DataRow row in rows)
			{
				execution = new Execution();

				try
				{
					execution.execPrice = row[0] == null ? 0 : Double.Parse(row[0].ToString());
					execution.execTime = (row[1] == null || row[1].ToString() == "") ? DateTime.Parse("0001-01-01T00:00:00") : DateTime.Parse(row[1].ToString());
					execution.execQty = row[2] == null ? 0 : Double.Parse(row[2].ToString());
					execution.omExecTag = row[3] == null ? "" : row[3].ToString().Trim();
					execution.omTag = row[4] == null ? "" : row[4].ToString().Trim();
				}
				catch (Exception ex)
				{
					Debug.Assert(RdsGlobal.InIDE, ex.Message, ex.StackTrace);
				}
				executions.Add(execution);
			}
		}

		if (executions != null)
		{
			if (RdsCache.RomUserExecutions.ContainsKey(user))
			{
				RdsCache.RomUserExecutions[user] = executions;
			}
			else
			{
				RdsCache.RomUserExecutions.Add(user, executions);
			}
		}
		return executions;
	}

	#endregion

	public static void ChangePassword(string user, string oldpass, string newpass)
	{
		new OdbcCommand("update OMuser set password = '" + newpass + "' where username = '" + user + "' and password = '" + oldpass + "'", Connection).ExecuteReader();
	}

	private static void GetData(ref DataTable data, string sql)
	{
		if (sql != null && sql != "")
		{
			OdbcDataAdapter da = new OdbcDataAdapter(sql, Connection);
			data = new DataTable();
			da.Fill(data);
			da.Dispose();
		}
	}
}
