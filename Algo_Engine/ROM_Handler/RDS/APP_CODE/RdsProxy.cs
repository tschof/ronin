/*********************************************************************************
 *
 *  Copyright (c) 2006 by RONIN CAPITAL, LLC
 *   All Rights Reserved.
 *
 *********************************************************************************
 *
 * History
 * date     user       comment
 * ------- --------    -----------------------------------------------------------
 * 6/18/06  wxu        Initial version
 * 7/26/06  wxu        Add Timer
 *********************************************************************************/

using System;
using System.Collections;
using System.Data;
using System.Data.Odbc;
using System.Diagnostics;
using System.Configuration;

public class RdsProxy 
{
	private string   dbConnectionStr;
	private Logger   logger = null;
	
	public RdsProxy()
	{
        logger = Logger.instance();
    	string dbserver = ConfigurationManager.AppSettings["dbserver"];
        string dbuser = ConfigurationManager.AppSettings["dbuser"];
        string dbpass = ConfigurationManager.AppSettings["dbpass"];
        string dbname = ConfigurationManager.AppSettings["dbname"];
		dbConnectionStr = "Dsn=" + dbserver + ";Uid=" + dbuser + ";Pwd=" + dbpass + ";";
        logger.logDebug("DB Connection: " + dbConnectionStr);
	}

	public bool CheckAuthorization(string user, string pass)
	{
		OdbcConnection dbConnection = new OdbcConnection(dbConnectionStr);
		dbConnection.Open();
		string myQuery = "select 1 from OMuser where username = '" + user + "' and password = '" + pass + "'";
		OdbcCommand myCommand = new OdbcCommand(myQuery, dbConnection);
		OdbcDataReader myReader = myCommand.ExecuteReader(CommandBehavior.CloseConnection);

		return myReader.HasRows;
	}

	public void ChangePassword(string user, string oldpass, string newpass)
	{
		OdbcConnection dbConnection = new OdbcConnection(dbConnectionStr);
		dbConnection.Open();
		string myQuery = "update OMuser set password = '" + newpass + "' where username = '" + user + "' and password = '" + oldpass + "'";
		OdbcCommand myCommand = new OdbcCommand(myQuery, dbConnection);
		OdbcDataReader myReader = myCommand.ExecuteReader(CommandBehavior.CloseConnection);
	}

	private TraderAcr FindOrCreateTraderAcr(string tradeFor, ArrayList traderAcrs)
	{
		TraderAcr ret = null;
		foreach ( Object traderObj in traderAcrs )
		{
			TraderAcr tf = (TraderAcr) traderObj;
			if ( tf.tradeFor.Equals(tradeFor) )
			{
				ret = tf;
				break;
			}
		}

		if ( ret == null)
		{
			ret = new TraderAcr();
			ret.tradeFor = tradeFor;
			traderAcrs.Add(ret);
		}

		return ret;
	}

	private void AddToUserProfile(AcctProfile acct, string tradeFor, ArrayList traderAcrs)
	{
		TraderAcr traderAcr = FindOrCreateTraderAcr(tradeFor, traderAcrs);
		traderAcr.acctProfiles.Add(acct);
	}

	public ArrayList BuildUserProfile(string user)
	{
		OdbcConnection dbConnection = new OdbcConnection(dbConnectionStr);
		dbConnection.Open();
		string myQuery = "OMGetOMUserAccounts '" + user + "'";
		OdbcCommand myCommand = new OdbcCommand(myQuery, dbConnection);
		OdbcDataReader myReader = myCommand.ExecuteReader();
		
		ArrayList ret = new ArrayList();
		if ( ! myReader.HasRows )
			return ret;

		while( myReader.Read() )
		{
			AcctProfile acct = new AcctProfile();

			acct.interfaceID = myReader.GetInt32(0);
			acct.account = myReader.GetString(1).Trim();
			acct.type = myReader.GetString(2).Trim();
			string tradeFor = myReader.GetString(3).Trim();
			acct.localAcAcrn = myReader.GetString(4).Trim();
			acct.clearingAcID = myReader.GetString(5).Trim();
			acct.firmAcr = myReader.GetString(6).Trim();
            if (!myReader.IsDBNull(8))
                acct.omAcctType = myReader.GetString(8).Trim();
			acct.capacity = myReader.GetString(9).Trim();
			acct.clearingFirmID = myReader.GetInt32(10);
            if (!myReader.IsDBNull(13))
                acct.occAcr = myReader.GetString(13).Trim();
            if (!myReader.IsDBNull(14))
                acct.homeExchange = myReader.GetString(14).Trim();
			
			// Console.WriteLine("U/Pass is: {0}, {1}", workRow[0], workRow[1]);
			AddToUserProfile(acct, tradeFor, ret);
		}
		// always call Close when done reading.
		myReader.Close();

		foreach (TraderAcr tf in ret )
		{
			foreach (AcctProfile ap in tf.acctProfiles)
			{
				myCommand.CommandText = "OMGetAccountDestinations " + ap.interfaceID;
				OdbcDataReader destReader = myCommand.ExecuteReader();

				while( destReader.Read() )
				{
					AcctDestination dest = new AcctDestination();
					dest.destID = destReader.GetInt32(0);
					dest.secType = destReader.GetString(1).Trim();
					dest.shortName = destReader.GetString(3).Trim();
                    if (!destReader.IsDBNull(4))
                        dest.giveupFirmID = destReader.GetString(4).Trim();
                    // else dest.giveupFirmID = " ";
                    if (!destReader.IsDBNull(5))
                        dest.cmtaFirmID = destReader.GetString(5).Trim();
                    // else dest.cmtaFirmID = " ";

					ap.accountDestinations.Add(dest);
				}
				destReader.Close();
			}
		}

		dbConnection.Close();
		return ret;
	}


	public ArrayList BuildUserExecutions(string user)
	{
		OdbcConnection dbConnection = new OdbcConnection(dbConnectionStr);
		dbConnection.Open();
		string myQuery = "CDS_selUsrExecutions '" + user + "'";
		OdbcCommand myCommand = new OdbcCommand(myQuery, dbConnection);
		OdbcDataReader myReader = myCommand.ExecuteReader(CommandBehavior.CloseConnection);
		
		ArrayList ret = new ArrayList();
		if ( ! myReader.HasRows )
			return ret;

		int fld = -1;
		while( myReader.Read() )
		{
			try 
			{
				Execution exec = new Execution();
				exec.execPrice = myReader.GetDouble(fld=0);
				if ( ! myReader.IsDBNull(fld=1) )
					exec.execTime = myReader.GetDateTime(fld);
				exec.execQty  = myReader.GetDouble(fld=2);
				exec.omExecTag = myReader.GetString(fld=3);
				exec.omTag = myReader.GetString(fld=4);
			
				// Console.WriteLine("U/Pass is: {0}, {1}", workRow[0], workRow[1]);
				ret.Add(exec);
			}
			catch (InvalidCastException ex)
			{
				string msg = "Failed to get Field " + fld;
				logger.logError("GetUserExecutions: " + msg);
				throw(ex);
			}
		}
		// always call Close when done reading.
		myReader.Close();
		dbConnection.Close();

		return ret;
	}

	public ArrayList BuildUserOrders(string user)
	{
		OdbcConnection dbConnection = new OdbcConnection(dbConnectionStr);
		dbConnection.Open();
		string myQuery = "RDS_selUsrOrders '" + user + "'";
		OdbcCommand myCommand = new OdbcCommand(myQuery, dbConnection);
		OdbcDataReader myReader = myCommand.ExecuteReader(CommandBehavior.CloseConnection);
		
		ArrayList ret = new ArrayList();
		if ( ! myReader.HasRows )
			return ret;

		while( myReader.Read() )
		{
			int fld = -1;
			Order order = new Order();

			try 
			{
				if ( ! myReader.IsDBNull(fld=12) )
					order.omTag = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=27) )
					order.tag = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=13) )
					order.omTime = myReader.GetDateTime(fld);
				if ( ! myReader.IsDBNull(fld=18) )
					order.owner = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=31) )
					order.trader = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=30) )
					order.tradeFor = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=4) )
					order.destID= myReader.GetInt32(fld);
				if ( ! myReader.IsDBNull(fld=9) )
					order.localAcct = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=6) )
					order.firm = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=1) )
					order.clearingAccount = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=2) )
					order.clearingID = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=22) )
					order.side = myReader.GetInt32(fld);
				if ( ! myReader.IsDBNull(fld=20) )
					order.qty = myReader.GetInt32(fld);
				if ( ! myReader.IsDBNull(fld=19) )
					order.price = myReader.GetDouble(fld);
				if ( ! myReader.IsDBNull(fld=24) )
					order.stopPrice = myReader.GetDouble(fld);
				if ( ! myReader.IsDBNull(fld=21) )
					order.secType = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=26) )
					order.symbol = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=32) )
					order.underlying = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=25) )
					order.strikePrice = myReader.GetDouble(fld);
				if ( ! myReader.IsDBNull(fld=5) )
					order.expDate = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=0) )
					order.callPut = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=14) )
					order.openClose = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=33) )
					order.ex_short_name = myReader.GetString(fld);
				if ( ! myReader.IsDBNull(fld=34) )
					order.effectiveTime = myReader.GetDateTime(fld);
				if ( ! myReader.IsDBNull(fld=7) )
					order.instructions = myReader.GetString(fld);
				order.maxFloor = myReader.GetInt32(fld=10);
				order.multiplier = myReader.GetDouble(fld=11);
				order.orderType = myReader.GetInt32(fld=16);
				order.tif = myReader.GetInt32(fld=29);
				if ( ! myReader.IsDBNull(fld=15) )
					order.orderExpiresDate = myReader.GetDateTime(fld);
				order.originalShares = myReader.GetInt32(fld=17);
				order.leavesQty = myReader.GetInt32(fld=8);
				order.cumQty = myReader.GetInt32(fld=3);
				if ( ! myReader.IsDBNull(fld=28) )
					order.text = myReader.GetString(fld);
				order.status = myReader.GetInt32(fld=23);
			} 
			catch (Exception ex)
			{
				string msg = "Failed to get Field " + fld;
				if (order.symbol.Length > 0)
					msg += ", symbol: " + order.symbol;
				logger.logError("GetUserOrders: " + msg);
				throw(ex);
			}

			// Console.WriteLine("U/Pass is: {0}, {1}", workRow[0], workRow[1]);
			ret.Add(order);
		}
		// always call Close when done reading.
		myReader.Close();
		dbConnection.Close();

		return ret;
	}
}

