using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

using System.Collections;
using System.Data.Odbc;

/// <summary>
/// Summary description for RdsProxy
/// </summary>
public class RdsProxy
{
    // Fields
    private string dbConnectionStr;
    private Logger logger = Logger.instance();

    // Methods
    public RdsProxy()
    {
        string str = ConfigurationManager.AppSettings["dbserver"];
        string str2 = ConfigurationManager.AppSettings["dbuser"];
        string str3 = ConfigurationManager.AppSettings["dbpass"];
        string text1 = ConfigurationManager.AppSettings["dbname"];
        this.dbConnectionStr = "Dsn=" + str + ";Uid=" + str2 + ";Pwd=" + str3 + ";";
        this.logger.logDebug("DB Connection: " + this.dbConnectionStr);
    }

    private void AddToUserProfile(AcctProfile acct, string tradeFor, ArrayList traderAcrs)
    {
        this.FindOrCreateTraderAcr(tradeFor, traderAcrs).acctProfiles.Add(acct);
    }

    public ArrayList BuildUserExecutions(string user)
    {
        //OdbcConnection connection = new OdbcConnection(this.dbConnectionStr);
		//Connection.Open();
		OdbcDataReader reader = new OdbcCommand("CDS_selUsrExecutions '" + user + "'", Connection).ExecuteReader(CommandBehavior.CloseConnection);
        ArrayList list = new ArrayList();
        if (reader.HasRows)
        {
            int ordinal = -1;
            while (reader.Read())
            {
                try
                {
                    Execution execution = new Execution();
                    execution.execPrice = reader.GetDouble(ordinal = 0);
                    if (!reader.IsDBNull(ordinal = 1))
                    {
                        execution.execTime = reader.GetDateTime(ordinal);
                    }
                    execution.execQty = reader.GetDouble(ordinal = 2);
                    execution.omExecTag = reader.GetString(ordinal = 3);
                    execution.omTag = reader.GetString(ordinal = 4);
                    list.Add(execution);
                    continue;
                }
                catch (InvalidCastException exception)
                {
                    this.logger.logError("GetUserExecutions: " + ("Failed to get Field " + ordinal));
                    throw exception;
                }
            }
            reader.Close();
			//Connection.Close();
        }
        return list;
    }

    public ArrayList BuildUserOrders(string user)
    {
        //OdbcConnection connection = new OdbcConnection(this.dbConnectionStr);
		//Connection.Close();
		//Connection.Open();
		OdbcDataReader reader = new OdbcCommand("RDS_selUsrOrders '" + user + "'", Connection).ExecuteReader(CommandBehavior.CloseConnection);
        ArrayList list = new ArrayList();
        if (reader.HasRows)
        {
            while (reader.Read())
            {
                int ordinal = -1;
                Order order = new Order();
                try
                {
                    if (!reader.IsDBNull(ordinal = 12))
                    {
                        order.omTag = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x1b))
                    {
                        order.tag = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 13))
                    {
                        order.omTime = reader.GetDateTime(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x12))
                    {
                        order.owner = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x1f))
                    {
                        order.trader = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 30))
                    {
                        order.tradeFor = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 4))
                    {
                        order.destID = reader.GetInt32(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 9))
                    {
                        order.localAcct = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 6))
                    {
                        order.firm = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 1))
                    {
                        order.clearingAccount = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 2))
                    {
                        order.clearingID = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x16))
                    {
                        order.side = reader.GetInt32(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 20))
                    {
                        order.qty = reader.GetInt32(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x13))
                    {
                        order.price = reader.GetDouble(ordinal);
                    }
					if (reader.FieldCount > 35)
					{
						if (!reader.IsDBNull(ordinal = 35))
						{
							order.avgPrice = reader.GetDouble(ordinal);
						}
					}
                    if (!reader.IsDBNull(ordinal = 0x18))
                    {
                        order.stopPrice = reader.GetDouble(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x15))
                    {
                        order.secType = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x1a))
                    {
                        order.symbol = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x20))
                    {
                        order.underlying = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x19))
                    {
                        order.strikePrice = reader.GetDouble(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 5))
                    {
                        order.expDate = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0))
                    {
                        order.callPut = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 14))
                    {
                        order.openClose = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x21))
                    {
                        order.ex_short_name = reader.GetString(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 0x22))
                    {
                        order.effectiveTime = reader.GetDateTime(ordinal);
                    }
                    if (!reader.IsDBNull(ordinal = 7))
                    {
                        order.instructions = reader.GetString(ordinal);
                    }
                    order.maxFloor = reader.GetInt32(ordinal = 10);
                    order.multiplier = reader.GetDouble(ordinal = 11);
                    order.orderType = reader.GetInt32(ordinal = 0x10);
                    order.tif = reader.GetInt32(ordinal = 0x1d);
                    if (!reader.IsDBNull(ordinal = 15))
                    {
                        order.orderExpiresDate = reader.GetDateTime(ordinal);
                    }
                    order.originalShares = reader.GetInt32(ordinal = 0x11);
                    order.leavesQty = reader.GetInt32(ordinal = 8);
                    order.cumQty = reader.GetInt32(ordinal = 3);
                    if (!reader.IsDBNull(ordinal = 0x1c))
                    {
                        order.text = reader.GetString(ordinal);
                    }
                    order.status = reader.GetInt32(ordinal = 0x17);
                }
                catch (Exception exception)
                {
                    string str2 = "Failed to get Field " + ordinal;
                    if (order.symbol.Length > 0)
                    {
                        str2 = str2 + ", symbol: " + order.symbol;
                    }
                    this.logger.logError("GetUserOrders: " + str2);
                    throw exception;
                }
                list.Add(order);
            }
            reader.Close();
            return list;
        }
		//Connection.Close();
		//connection.Dispose();
		//connection = null;
		return list;
    }

    public ArrayList BuildUserProfile(string user)
    {
        //OdbcConnection connection = new OdbcConnection(this.dbConnectionStr);
		//Connection.Close();
		//Connection.Open();
		OdbcCommand command = new OdbcCommand("OMGetOMUserAccounts '" + user + "'", Connection);
        OdbcDataReader reader = command.ExecuteReader();
        ArrayList traderAcrs = new ArrayList();
        if (reader.HasRows)
        {
            while (reader.Read())
            {
                AcctProfile acct = new AcctProfile();
                acct.interfaceID = reader.GetInt32(0);
                acct.account = reader.GetString(1).Trim();
                acct.type = reader.GetString(2).Trim();
                string tradeFor = reader.GetString(3).Trim();
                acct.localAcAcrn = reader.GetString(4).Trim();
                acct.clearingAcID = reader.GetString(5).Trim();
                acct.firmAcr = reader.GetString(6).Trim();
                if (!reader.IsDBNull(8))
                {
                    acct.omAcctType = reader.GetString(8).Trim();
                }
                acct.capacity = reader.GetString(9).Trim();
                //acct.clearingFirmID = reader.GetInt32(10);
				acct.clearingFirmID = reader.GetString(10);
                if (!reader.IsDBNull(13))
                {
                    acct.occAcr = reader.GetString(13).Trim();
                }
                if (!reader.IsDBNull(14))
                {
                    acct.homeExchange = reader.GetString(14).Trim();
                }
                this.AddToUserProfile(acct, tradeFor, traderAcrs);
            }
            reader.Close();
            foreach (TraderAcr acr in traderAcrs)
            {
                foreach (AcctProfile profile2 in acr.acctProfiles)
                {
                    command.CommandText = "OMGetAccountDestinations " + profile2.interfaceID;
                    OdbcDataReader reader2 = command.ExecuteReader();
                    while (reader2.Read())
                    {
                        AcctDestination destination = new AcctDestination();
                        destination.destID = reader2.GetInt32(0);
                        destination.secType = reader2.GetString(1).Trim();
                        destination.shortName = reader2.GetString(3).Trim();
                        if (!reader2.IsDBNull(4))
                        {
                            destination.giveupFirmID = reader2.GetString(4).Trim();
                        }
                        if (!reader2.IsDBNull(5))
                        {
                            destination.cmtaFirmID = reader2.GetString(5).Trim();
                        }
                        profile2.accountDestinations.Add(destination);
                    }
                    reader2.Close();
                }
            }
			//Connection.Close();
            return traderAcrs;
        }
        return traderAcrs;
    }

    public void ChangePassword(string user, string oldpass, string newpass)
    {
        //OdbcConnection connection = new OdbcConnection(this.dbConnectionStr);
		//Connection.Close();
		//Connection.Open();
		new OdbcCommand("update OMuser set password = '" + newpass + "' where username = '" + user + "' and password = '" + oldpass + "'", Connection).ExecuteReader(CommandBehavior.CloseConnection);
		//Connection.Close();
    }

	private OdbcConnection mConnection;
	private OdbcConnection Connection
	{
		get
		{
			if (mConnection == null)
			{
				mConnection = new OdbcConnection(this.dbConnectionStr);
				mConnection.Open();
			}
			if (mConnection.State != ConnectionState.Open)
			{
				mConnection.Close();
				mConnection.Open();
			}
			return mConnection;
		}
		set
		{
			mConnection = value;
		}
	}

    public bool CheckAuthorization(string user, string pass)
    {
		bool tOk;

        //OdbcConnection connection = new OdbcConnection(this.dbConnectionStr);
		//Connection.Close();
		//Connection.Open();
		OdbcCommand command = new OdbcCommand("select 1 from OMuser where username = '" + user + "' and password = '" + pass + "'", Connection);
        tOk = command.ExecuteReader(CommandBehavior.CloseConnection).HasRows;
		//Connection.Close();
		return tOk;
    }

    private TraderAcr FindOrCreateTraderAcr(string tradeFor, ArrayList traderAcrs)
    {
        TraderAcr acr = null;
        foreach (object obj2 in traderAcrs)
        {
            TraderAcr acr2 = (TraderAcr)obj2;
            if (acr2.tradeFor.Equals(tradeFor))
            {
                acr = acr2;
                break;
            }
        }
        if (acr == null)
        {
            acr = new TraderAcr();
            acr.tradeFor = tradeFor;
            traderAcrs.Add(acr);
        }
        return acr;
    }
}
