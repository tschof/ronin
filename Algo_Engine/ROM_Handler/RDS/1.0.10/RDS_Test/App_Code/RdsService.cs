using System;
using System.Web;
using System.Web.Services;
using System.Web.Services.Protocols;

using System.Configuration;
using TposWrapper;
using System.Threading;
using System.Xml.Serialization;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Data;
using System.Diagnostics;

[WebService(Namespace = "http://ronin.com/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
public class RdsService : System.Web.Services.WebService
{
    private static Logger logger;
    private static RdsProxy proxy;
    private static Timer timer4tpos;
    private static TposClient tpos;
    private static string version;
	private static Hashtable mIMMap;

	private static DataTable mCMEMap;
	private static DataTable mCMESpreadMap;

	private static DataTable mUserMap;
	//private static Dictionary<string, SymbolInfo> mSpreadMap;

    public RdsService () {

        //Uncomment the following line if using designed components 
        //InitializeComponent(); 
    }

    static RdsService()
    {
        TimeSpan span;
        version = "1.0.28";
        //logger = new Logger(); 
        logger = Logger.instance();
        logger.logInfo("RDS " + version + " service starts");
        proxy = new RdsProxy();
		logger.logDebug(">>>> Connect to TPOS");
		tpos = new TposClient();
		tpos.Connect();
		logger.logDebug("TPOS is connected");
        DateTime now = DateTime.Now;
        try
        {
            string s = ConfigurationManager.AppSettings["tposReconnect"];
            span = TimeSpan.Parse(s);
        }
        catch (Exception exception)
        {
            logger.logError("Can't read 'tposReconnect' " + exception.ToString());
            span = new TimeSpan(6, 20, 0);
        }
        long num = 0x15180L;
        DateTime time2 = new DateTime(now.Year, now.Month, now.Day, span.Hours, span.Minutes, span.Seconds);
        int num2 = ((int)(time2.Subtract(now).TotalSeconds + 86400.0)) % 0x15180;
        timer4tpos = new Timer(new TimerCallback(RdsService.tposReconnectTimerCallback), null, (long)(num2 * 0x3e8), num * 0x3e8L);
        logger.logDebug(string.Concat(new object[] { "Reconnect every ", num, " seconds ", num2, " seconds later from now" }));

		//Load_CMEFutureMap();
		//Load_CMESpreadMap();
	}

	#region 1.0.20
	private static void Load_IMMap()
	{
		try
		{
			mIMMap = new Hashtable();

			string str = ConfigurationManager.AppSettings["IMMap"];

			string[] ar = str.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);

			string[] im;
			for (int i = 0; i < ar.Length; i++)
			{
				im = ar[i].Split(new string[] { "=" }, StringSplitOptions.RemoveEmptyEntries);
				if (!mIMMap.ContainsKey(im[0]))
				{
					mIMMap.Add(im[0], im[1]);
				}
			}
		}
		catch (Exception ex)
		{
			logger.logDebug(ex.ToString());
		}
	}
	private static string Get_IMSymbol(string ticker)
	{
		if (mIMMap.ContainsKey(ticker))
		{
			return (string)mIMMap[ticker];
		}
		else
		{
			return ticker;
		}
	}
	#endregion

	[XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get Future chain by underlying.", EnableSession = false), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus GetFutureChain(string underlying)
    {
        return this.TposFunction("GetFutureChain", underlying, new TposDelegate(tpos.BuildFutureChain));
    }

    [XmlInclude(typeof(FutureDesc)), XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get Future chain by base.", EnableSession = false)]
    public ReturnStatus GetFutureChainByBase(string baseSymbol)
    {
        return this.TposFunction("GetFutureChainByBase", baseSymbol, new TposDelegate(tpos.BuildFutureChainByBase));
    }

    [XmlInclude(typeof(OptionDesc)), XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get option chain by underlying symbol.", EnableSession = false)]
    public ReturnStatus GetOptionChain(string underlying)
    {
        return this.TposFunction("GetOptionChain", underlying, new TposDelegate(tpos.BuildOptionChain));
    }

    [XmlInclude(typeof(SecurityDesc)), WebMethod(Description = "Get Security details by ticker.", EnableSession = false), XmlInclude(typeof(ReturnStatus))]
    public ReturnStatus GetSecurityByTicker(string ticker)
    {
		ReturnStatus rs = GetUserMappedSymbolByTicker(ticker);

		// It not user defined and is a spread symbol then look in the spread map
		if (rs == null && ticker.Contains("-"))
		{
			rs = GetSpreadByTicker(ticker);
		}
		
		if (rs == null)
		{
			// Its not user defined then look in the local map
			rs = GetMappedSymbolByTicker(ticker);
		}

		if (rs == null)
		{
			return this.TposFunction("GetSecurityByTicker", ticker, new TposDelegate(tpos.BuildSecurityInfo));
		}
		else
		{
			return rs;
		}

		//if (ticker.Contains("-"))
		//{
		//    return GetSpreadByTicker(ticker);
		//}
		//else
		//{
		//    ReturnStatus rs = GetMappedSymbolByTicker(ticker);
		//    if (rs != null)
		//    {
		//        return rs;
		//    }
		//    else
		//    {
		//        return this.TposFunction("GetSecurityByTicker", ticker, new TposDelegate(tpos.BuildSecurityInfo));
		//    }
		//}
	}

	#region 1.0.20
	[WebMethod(Description = "Get Mapped Security name by ticker.", EnableSession = false)]
	public string GetMappedSecurityByTicker(string ticker)
	{
		string tUnder = tpos.BuildMappedSecurityInfo(ticker);

		if (tUnder == "")
		{
			if (mIMMap == null)
			{
				Load_IMMap();
			}
			tUnder = Get_IMSymbol(ticker);
		}
		if (tUnder == ticker)
		{
			tUnder = tUnder.Replace("F:", "");
		}
		return tUnder;
	}
	#endregion

	#region 1.0.27

	#region - Future -

	[XmlInclude(typeof(SecurityDesc)), WebMethod(Description = "Get Future details by ticker.", EnableSession = false), XmlInclude(typeof(ReturnStatus))]
	private ReturnStatus GetMappedSymbolByTicker(string ticker)
	{
		SecurityDesc sd = new SecurityDesc();
		ReturnStatus rs = new ReturnStatus();

		ArrayList result = new ArrayList();
		result.Add(sd);
		rs.retValues = result;

		if (mCMEMap == null || mCMEMap.Rows.Count == 0)
		{
			Load_CMEFutureMap();
		}

		DataRow[] rows = mCMEMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			sd.tickSize = 0.01;
			Double.TryParse(rows[0]["TickSize"].ToString(), out sd.tickSize);
			sd.securityType = rows[0]["SecType"].ToString();
			sd.genericMDSymbol = rows[0]["Symbol"].ToString();
			sd.baseSymbol = rows[0]["Symbol"].ToString();
			sd.fullSymbol = rows[0]["Symbol"].ToString();
			sd.exchange = "CME";
			sd.dataSourceInfo = "CME";
			sd.expirationDate = rows[0]["ExpDate"].ToString();
			sd.contractSize = 1;
			Double.TryParse(rows[0]["ContractSize"].ToString(), out sd.contractSize);

			result.Clear();
			result.Add(sd);
			rs.retValues = result;

			return rs;
		}

		return null;
	}

	[WebMethod(Description = "Reload CME Future Map.", EnableSession = false)]
	public int ReloadCMEFutureMap()
	{
		mCMEMap = null;

		Load_CMEFutureMap();

		return mCMEMap.Rows.Count;
	}

	private static void Load_CMEFutureMap()
	{
		string source = ConfigurationManager.AppSettings["CMEFuturesDef"];
		if (File.Exists(source))
		{
			try
			{
				if (mCMEMap == null)
				{
					mCMEMap = new DataTable("CMESecDef");

					mCMEMap.Columns.Add("Symbol");
					mCMEMap.Columns.Add("SecType");
					mCMEMap.Columns.Add("TickSize");
					mCMEMap.Columns.Add("ContractSize");
					mCMEMap.Columns.Add("ExpDate");
					mCMEMap.ReadXml(source);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(!Debugger.IsAttached, ex.Message);
			}
		}
	}

	#endregion

	#region - Spread -

	[XmlInclude(typeof(SecurityDesc)), WebMethod(Description = "Get Spread details by ticker.", EnableSession = false), XmlInclude(typeof(ReturnStatus))]
	public ReturnStatus GetSpreadByTicker(string ticker)
	{
		SecurityDesc sd = new SecurityDesc();
		ReturnStatus rs = new ReturnStatus();

		ArrayList result = new ArrayList();
		result.Add(sd);
		rs.retValues = result;

		if (mCMESpreadMap == null || mCMESpreadMap.Rows.Count == 0)
		{
			Load_CMESpreadMap();
		}

		DataRow[] rows = mCMESpreadMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			sd.tickSize = 0.01;
			Double.TryParse(rows[0]["TickSize"].ToString(), out sd.tickSize);
			sd.securityType = rows[0]["SecType"].ToString();
			sd.genericMDSymbol = rows[0]["Symbol"].ToString();
			sd.baseSymbol = rows[0]["Symbol"].ToString();
			sd.fullSymbol = rows[0]["Symbol"].ToString();
			sd.exchange = "CME";
			sd.dataSourceInfo = "CME";
			sd.expirationDate = rows[0]["ExpDate"].ToString();
			sd.contractSize = 1;
			Double.TryParse(rows[0]["ContractSize"].ToString(), out sd.contractSize);

			result.Clear();
			result.Add(sd);
			rs.retValues = result;

			return rs;
		}

		return null;
	}

	[WebMethod(Description = "Reload Spread Map.", EnableSession = false)]
	public int ReloadSpreadMap()
	{
		mCMESpreadMap = null;

		Load_CMESpreadMap();

		return mCMESpreadMap.Rows.Count;
	}

	private static void Load_CMESpreadMap()
	{
		string source = ConfigurationManager.AppSettings["CMESpreadsDef"];
		if (File.Exists(source))
		{
			try
			{
				if (mCMESpreadMap == null)
				{
					mCMESpreadMap = new DataTable("CMESecDef");

					mCMESpreadMap.Columns.Add("Symbol");
					mCMESpreadMap.Columns.Add("SecType");
					mCMESpreadMap.Columns.Add("TickSize");
					mCMESpreadMap.Columns.Add("ContractSize");
					mCMESpreadMap.Columns.Add("ExpDate");
					mCMESpreadMap.ReadXml(source);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(!Debugger.IsAttached, ex.Message);
			}
		}
	}

	#endregion

	#region - User Map -

	[XmlInclude(typeof(SecurityDesc)), WebMethod(Description = "Get Future details by ticker.", EnableSession = false), XmlInclude(typeof(ReturnStatus))]
	private ReturnStatus GetUserMappedSymbolByTicker(string ticker)
	{
		SecurityDesc sd = new SecurityDesc();
		ReturnStatus rs = new ReturnStatus();

		ArrayList result = new ArrayList();
		result.Add(sd);
		rs.retValues = result;

		if (mUserMap == null || mUserMap.Rows.Count == 0)
		{
			Load_UserMap();
		}

		DataRow[] rows = mUserMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			sd.tickSize = 0.01;
			Double.TryParse(rows[0]["TickSize"].ToString(), out sd.tickSize);
			sd.securityType = rows[0]["SecType"].ToString();
			sd.genericMDSymbol = rows[0]["MDSymbol"].ToString();
			sd.baseSymbol = rows[0]["Symbol"].ToString();
			sd.fullSymbol = rows[0]["MDSymbol"].ToString();
			sd.exchange = rows[0]["DataSource"].ToString();
			sd.dataSourceInfo = rows[0]["DataSource"].ToString();
			sd.baseSymbol = rows[0]["MDSymbol"].ToString();
			sd.expirationDate = rows[0]["ExpDate"].ToString();
			sd.contractSize = 1;
			Double.TryParse(rows[0]["ContractSize"].ToString(), out sd.contractSize);

			result.Clear();
			result.Add(sd);
			rs.retValues = result;

			return rs;
		}

		return null;
	}

	[WebMethod(Description = "Reload User Symbol Map.", EnableSession = false)]
	public int ReloadUserMap()
	{
		mUserMap = null;

		Load_UserMap();

		return mUserMap.Rows.Count;
	}

	private static void Load_UserMap()
	{
		string source = ConfigurationManager.AppSettings["UserSecDef"];
		if (File.Exists(source))
		{
			try
			{
				if (mUserMap == null)
				{
					mUserMap = new DataTable("UserSecDef");

					mUserMap.Columns.Add("Symbol");
					mUserMap.Columns.Add("SecType");
					mUserMap.Columns.Add("TickSize");
					mUserMap.Columns.Add("ContractSize");
					mUserMap.Columns.Add("ExpDate");
					mUserMap.Columns.Add("DataSource");
					mUserMap.Columns.Add("MDSymbol");
					mUserMap.ReadXml(source);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(!Debugger.IsAttached, ex.Message);
			}
		}
	}

	#endregion

	#endregion

	[XmlInclude(typeof(FutureDesc)), XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get Stock Future chain by underlying symbol.", EnableSession = false)]
    public ReturnStatus GetSSFutureChain(string underlying)
    {
        return this.TposFunction("GetSSFutureChain", underlying, new TposDelegate(tpos.BuildSSFutureChain));
    }

	[XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get Positions for user and local account acronym / Protfolio List.", EnableSession = false), XmlInclude(typeof(TposPosition))]
	public ReturnStatus GetTposPositionsBylocalAccts(string user, ArrayList localAccts)
	{
		logger.logDebug("GetPositions: " + user + ", localAccts: " + localAccts.ToString());
		ReturnStatus status = new ReturnStatus();
		try
		{
			status.retValues = new ArrayList();
			status.statusCode = tpos.GetPositionsBylocalAcct(user, localAccts, (ArrayList)status.retValues);
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = "Internal error: " + exception.ToString();
		}
		if (status.statusCode != 0)
		{
			logger.logError(string.Concat(new object[] { "GetPositionsBylocalAccts error: ", status.statusCode, ":", status.statusMessage }));
		}
		logger.logDebug("GetPositionsBylocalAccts returns: " + status.statusCode);
		return status;
	}

	[XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get Positions for user and local account acronym / Protfolio.", EnableSession = false), XmlInclude(typeof(TposPosition))]
	public ReturnStatus GetTposPositionsBylocalAcct(string user, string localAcct)
	{
		logger.logDebug("GetPositions By Local Account - Protfolio: " + user + ", trader: " + localAcct);
		ReturnStatus status = new ReturnStatus();
		ArrayList localAccts = new ArrayList();
		try
		{
			status.retValues = new ArrayList();
			string[] accts = localAcct.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
			for (int i = 0; i < accts.Length; i++)
			{
				localAccts.Add(accts[i].ToString());
			}
			//localAccts.Add(localAcct);
			status.statusCode = tpos.GetPositionsBylocalAcct(user, localAccts, (ArrayList)status.retValues);
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = "Internal error: " + exception.ToString();
		}
		if (status.statusCode != 0)
		{
			logger.logError(string.Concat(new object[] { "GetPositionsBylocalAcct error: ", status.statusCode, ":", status.statusMessage }));
		}
		logger.logDebug("GetPositionsBylocalAcct returns: " + status.statusCode);
		return status;
	}

	[XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition)), WebMethod(Description = "Get Positions for user and trader acronym list.", EnableSession = false)]
    public ReturnStatus GetTposPositions(string user, ArrayList traders)
    {
        logger.logDebug("GetPositions: " + user + ", traders: " + traders.ToString());
        ReturnStatus status = new ReturnStatus();
        try
        {
            status.retValues = new ArrayList();
            status.statusCode = tpos.GetPositions(user, traders, (ArrayList)status.retValues);
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { "GetPositions error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("GetPositions returns: " + status.statusCode);
        return status;
    }

    [XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get Positions for user and trader acronym.", EnableSession = false), XmlInclude(typeof(TposPosition))]
    public ReturnStatus GetTposPositionsByTrader(string user, string trader)
    {
        logger.logDebug("GetAcctPositions: " + user + ", trader: " + trader);
        ReturnStatus status = new ReturnStatus();
        ArrayList traders = new ArrayList();
        try
        {
            status.retValues = new ArrayList();
            traders.Add(trader);
            status.statusCode = tpos.GetPositions(user, traders, (ArrayList)status.retValues);
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { "GetAcctPositions error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("GetAcctPositions returns: " + status.statusCode);
        return status;
    }

    [XmlInclude(typeof(Execution)), WebMethod(Description = "Get ROC executions.", EnableSession = false), XmlInclude(typeof(ReturnStatus))]
    public ReturnStatus GetUserExecutions(string user, string pass)
    {
        logger.logDebug("GetUserExecutions:" + user + "/" + pass);
        ReturnStatus status = new ReturnStatus();
        if (!proxy.CheckAuthorization(user, pass))
        {
            status.statusCode = RDSStatusCode.INVALID_USER;
            status.statusMessage = "Invalid user name or password";
            logger.logDebug("GetUserExecutions returns: " + status.statusCode);
            return status;
        }
        try
        {
            status.retValues = proxy.BuildUserExecutions(user);
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
            logger.logError(string.Concat(new object[] { "GetUserExecutions: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("GetUserExecutions returns: " + status.statusCode);
        return status;
    }

    [XmlInclude(typeof(Order)), XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get ROC orders.", EnableSession = false)]
    public ReturnStatus GetUserOrders(string user, string pass)
    {
        logger.logDebug("GetUserOrders:" + user + "/" + pass);
        ReturnStatus status = new ReturnStatus();
        if (!proxy.CheckAuthorization(user, pass))
        {
            status.statusCode = RDSStatusCode.INVALID_USER;
            status.statusMessage = "Invalid user name or password";
            logger.logDebug("GetUserOrders returns: " + status.statusCode);
            return status;
        }
        try
        {
            status.retValues = proxy.BuildUserOrders(user);
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
            logger.logError(string.Concat(new object[] { "GetUserOrders error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("GetUserOrders returns: " + status.statusCode);
        return status;
    }

    [XmlInclude(typeof(AcctProfile)), WebMethod(Description = "Get ROC user's profile.", EnableSession = false), XmlInclude(typeof(AcctDestination)), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TraderAcr))]
    public ReturnStatus GetUserProfile(string user, string pass)
    {
        logger.logDebug("GetUserProfile:" + user + "/" + pass);
        ReturnStatus status = new ReturnStatus();
        if (!proxy.CheckAuthorization(user, pass))
        {
            status.statusCode = RDSStatusCode.INVALID_USER;
            status.statusMessage = "Invalid user name or password";
            logger.logDebug("GetUserProfile returns: " + status.statusCode);
            return status;
        }
        try
        {
            status.retValues = proxy.BuildUserProfile(user);
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
            logger.logError(string.Concat(new object[] { "GetUserProfile: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("GetUserProfile returns: " + status.statusCode);
        return status;
    }

    [WebMethod(Description = "Get RDS web service version.", EnableSession = false)]
    public string GetVersion()
    {
        return version;
    }

    [XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTrade)), WebMethod(Description = "Poll trades by a list of trader acronyms.", EnableSession = false)]
    public ReturnStatus RtrvTposTrades(string user, ArrayList traders, bool initFlag)
    {
        logger.logDebug("RtrvTposTrades: " + user + ", traders: " + traders.ToString());
        ReturnStatus status = new ReturnStatus();
        try
        {
            ArrayList results = new ArrayList();
            tpos.GetOrCreateLogin(user);
            tpos.RtrvTrades(user, traders, initFlag, results);
            status.retValues = results;
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { "RtrvTposTrades error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("RtrvTposTrades returns: " + status.statusCode);
        return status;
    }

    [WebMethod(Description = "Poll trades by trader acronym.", EnableSession = false), XmlInclude(typeof(TposTrade)), XmlInclude(typeof(ReturnStatus))]
    public ReturnStatus RtrvTposTradesByTrader(string user, string trader, bool initFlag)
    {
        logger.logDebug("RtrvTposTradesByTrader: " + user + ", trader: " + trader);
        ReturnStatus status = new ReturnStatus();
        try
        {
            ArrayList results = new ArrayList();
            tpos.GetOrCreateLogin(user);
            ArrayList traders = new ArrayList();
            traders.Add(trader);
            tpos.RtrvTrades(user, traders, initFlag, results);
            status.retValues = results;
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { "RtrvTposTradesByTrader error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("RtrvTposTradesByTrader returns: " + status.statusCode);
        return status;
    }

    [WebMethod(Description = "Change ROC user's password.", EnableSession = false)]
    public ReturnStatus SetUserPassword(string user, string oldpass, string newpass)
    {
        logger.logDebug("SetUserPassword:" + user + "/" + oldpass + "/" + newpass);
        ReturnStatus status = new ReturnStatus();
        status.retValues = false;
        if (!proxy.CheckAuthorization(user, oldpass))
        {
            status.statusCode = RDSStatusCode.INVALID_USER;
            status.statusMessage = "Invalid user name or password";
            logger.logDebug("SetUserPassword returns: " + status.statusCode);
            return status;
        }
        try
        {
            proxy.ChangePassword(user, oldpass, newpass);
            status.retValues = true;
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
            logger.logError(string.Concat(new object[] { "SetUserPassword error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("SetUserPassword returns: " + status.statusCode);
        return status;
    }

    [XmlInclude(typeof(TposTrade)), XmlInclude(typeof(ReturnStatus)), WebMethod(Description = "Get trades for user and trader acronym list.", EnableSession = false)]
    public ReturnStatus SubscribeTposTrades(string user, ArrayList traders, bool initFlag)
    {
        logger.logDebug("SubscribeTrades: " + user + ", trader: " + traders.ToString());
        ReturnStatus status = new ReturnStatus();
        try
        {
            ArrayList results = new ArrayList();
            RocLoginItem orCreateLogin = tpos.GetOrCreateLogin(user);
            if (initFlag)
            {
                tpos.SubscribeTrades(user, traders);
                tpos.GetTrades(user, traders, results);
            }
            Queue msgQueue = orCreateLogin.m_msgQueue;
            string str = "Rx" + user.Trim() + "-";
            while (msgQueue.Count > 0)
            {
                TposTrade trade = (TposTrade)msgQueue.Dequeue();
                for (int i = 0; i < traders.Count; i++)
                {
                    string str2 = (string)traders[i];
                    logger.logError("        : " + trade.m_ExtTradeID);
                    if (trade.m_TraderAcronym.Equals(str2) && !trade.m_ExtTradeID.StartsWith(str))
                    {
                        results.Add(trade);
                    }
                }
            }
            status.retValues = results;
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { "SubscribeTrades error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("SubscribeTrades returns: " + status.statusCode);
        return status;
    }

    [WebMethod(Description = "Get trades for user and trader acronym.", EnableSession = false), XmlInclude(typeof(TposTrade)), XmlInclude(typeof(ReturnStatus))]
    public ReturnStatus SubscribeTposTradesByTrader(string user, string trader, bool initFlag)
    {
        logger.logDebug("SubscribeTrades: " + user + ", trader: " + trader);
        ReturnStatus status = new ReturnStatus();
        try
        {
            ArrayList results = new ArrayList();
            RocLoginItem orCreateLogin = tpos.GetOrCreateLogin(user);
            if (initFlag)
            {
                Thread.GetDomain();
                ArrayList traders = new ArrayList();
                traders.Add(trader);
                tpos.SubscribeTrades(user, traders);
                tpos.GetTrades(user, traders, results);
            }
            Queue msgQueue = orCreateLogin.m_msgQueue;
            string str = "Rx" + user.Trim() + "-";
            while (msgQueue.Count > 0)
            {
                TposTrade trade = (TposTrade)msgQueue.Dequeue();
                logger.logError("        : " + trade.m_ExtTradeID);
                if (trade.m_TraderAcronym.Equals(trader) && !trade.m_ExtTradeID.StartsWith(str))
                {
                    results.Add(trade);
                }
            }
            status.retValues = results;
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { "SubscribeTrades error: ", status.statusCode, ":", status.statusMessage }));
        }
        logger.logDebug("SubscribeTrades returns: " + status.statusCode);
        return status;
    }

    [WebMethod(Description = "Test Tpos Connection By TposClient", EnableSession = false)]
    public int ConnectTPOS()
    {
		if (tpos != null)
		{
			tpos.ResetTposProxy();
			return 2;
		}
		else
		{
			tpos = new TposClient();
			return tpos.Connect();
		}
    }

    [WebMethod(Description = "Test Tpos Connection By RdsService", EnableSession = false)]
    public int ConnectRDS()
    {
        String l_ip = ConfigurationManager.AppSettings["tposserver"];
        String l_user = ConfigurationManager.AppSettings["tposuser"];
        String l_pass = ConfigurationManager.AppSettings["tpospass"];
        Int32 l_port = Convert.ToInt32(ConfigurationManager.AppSettings["tposport"]);
        String l_interface = ConfigurationManager.AppSettings["tposinterface"];

        //TposProxy 
        TposProxy l_conn = new TposProxy(l_user, l_pass, l_ip, l_port, "ROC");
        int tR = l_conn.Connect();

		// 1.0.20
		Load_IMMap();

        return tR;
    }

    private ReturnStatus TposFunction(string name, string symbol, TposDelegate delegater)
    {
        logger.logDebug(name + ": " + symbol);
        ReturnStatus status = new ReturnStatus();
        try
        {
            status.retValues = new ArrayList();
            status.statusCode = delegater(symbol, (ArrayList)status.retValues);
            if (status.statusCode == RDSStatusCode.TPOS_TIMEOUT)
            {
                status.statusMessage = "TPOS timeout";
            }
        }
        catch (Exception exception)
        {
            status.statusCode = RDSStatusCode.INTERNAL_ERROR;
            status.statusMessage = "Internal error: " + exception.ToString();
        }
        if (status.statusCode != 0)
        {
            logger.logError(string.Concat(new object[] { name, " error: ", status.statusCode, ":", status.statusMessage }));

			// Reset after timeout
			if (tpos != null)
			{
				tpos.ResetTposProxy();
			}
			else
			{
				tpos = new TposClient();
				tpos.Connect();
			}
        }
        logger.logDebug(name + " returns: " + status.statusCode);
        return status;
    }

    private static void tposReconnectTimerCallback(object state)
    {
        logger.logDebug(">>>> Reconnect to TPOS");
		if (tpos != null)
		{
			tpos.ResetTposProxy();
		}
		else
		{
			tpos = new TposClient();
			tpos.Connect();
		}
        logger.logDebug("TPOS is connected");
    }

    private delegate int TposDelegate(string underlying, ArrayList result);
}
