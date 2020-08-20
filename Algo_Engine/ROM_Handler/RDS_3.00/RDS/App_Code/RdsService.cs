using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Web.Services;
using System.Xml.Serialization;
using TposWrapper;
using System.Collections;
using System.Diagnostics;
using System.Runtime.Remoting.Messaging;

[WebService(Namespace = "http://ronin.com/")]
[WebServiceBinding(ConformsTo= WsiProfiles.BasicProfile1_1)]
public class RdsService : WebService
{
	private static string _version;

	public RdsService()
	{
	}

	static RdsService()
	{
		_version = "3.0.17";
		Logger.Start(_version);
	}

	[WebMethod(Description = "Get RDS web service version.", EnableSession = false)]
	public string GetVersion()
	{
		return _version;
	}

	[WebMethod(Description = "Connect To Tpos", EnableSession = false)]
	public string ConnectTPOS()
	{
		return TposClient.Connect();
	}

	[WebMethod(Description = "Disconnect From Tpos", EnableSession = false)]
	public string DisconnectTPOS()
	{
		return TposClient.DisConnect();
	}

	#region Cached User Profile

	[WebMethod(Description = "Get ROC user's profile.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TraderAcr)), XmlInclude(typeof(AcctProfile)), XmlInclude(typeof(AcctDestination))]
	public ReturnStatus GetUserProfile(string user, string pass)
	{
		return LoadUserProfile(user, pass, false);
	}

	[WebMethod(Description = "Get ROC user's profile with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TraderAcr)), XmlInclude(typeof(AcctProfile)), XmlInclude(typeof(AcctDestination))]
	public ReturnStatus LoadUserProfile(string user, string pass, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadUserProfile:", user, "/", pass }));
		
		ReturnStatus status = new ReturnStatus();
		if (!RomProxy.CheckAuthorization(user, pass, reload))
		{
			status.statusCode = RDSStatusCode.INVALID_USER;
			status.statusMessage = "Invalid user name or password";
			Logger.Logit(String.Concat(new object[] { "LoadUserProfile returns: " + status.statusCode }));
		}
		else
		{
			try
			{
				status.retValues = RomProxy.BuildUserProfile(user, reload);
			}
			catch (Exception exception)
			{
			    status.statusCode = RDSStatusCode.INTERNAL_ERROR;
				status.statusMessage = "BuildUserProfile public error: " + exception.ToString();
				Logger.Logit(String.Concat(new object[] { "LoadUserProfile: ", status.statusCode, ":", status.statusMessage }));
			}
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadUserProfile returns: ", status.statusCode }));
		return status;
	}

	[WebMethod(Description = "Get All Cached ROC user's profile.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(CachedUsers)), XmlInclude(typeof(TraderAcr)), XmlInclude(typeof(AcctProfile)), XmlInclude(typeof(AcctDestination))]
	public ReturnStatus GetUserProfileCached()
	{
		Logger.Logit(1, String.Concat(new object[] { "GetUserProfileCached:" }));

		ReturnStatus status = new ReturnStatus();

		try
		{
			status.retValues = RdsCacheProxy.ShowAllCachedUserProfile();
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "GetUserProfileCached public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "GetUserProfileCached: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "GetUserProfileCached returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Cached User Orders

	[WebMethod(Description = "Get ROM orders.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(Order))]
	public ReturnStatus GetUserOrders(string user, string pass)
	{
		return LoadUserOrders(user, pass, true);
	}

	[WebMethod(Description = "Get ROM orders with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(Order))]
	public ReturnStatus LoadUserOrders(string user, string pass, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadUserOrders:", user, "/", pass }));

		ReturnStatus status = new ReturnStatus();
		if (!RomProxy.CheckAuthorization(user, pass, reload))
		{
			status.statusCode = RDSStatusCode.INVALID_USER;
			status.statusMessage = String.Concat(new object[] { "Invalid user name or password" });
			Logger.Logit(String.Concat(new object[] { "LoadUserOrders returns: ", status.statusCode }));
		}
		else
		{
			try
			{
				status.retValues = RomProxy.BuildUserOrders(user, reload);
			}
			catch (Exception exception)
			{
				status.statusCode = RDSStatusCode.INTERNAL_ERROR;
				status.statusMessage = String.Concat(new object[] { "BuildUserOrders public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
				Logger.Logit(String.Concat(new object[] { "BuildUserOrders: ", status.statusCode, ":", status.statusMessage }));
			}
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadUserOrders returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Cached User Executions

	[WebMethod(Description = "Get ROM executions.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(Execution))]
	public ReturnStatus GetUserExecutions(string user, string pass)
	{
		return LoadUserExecutions(user, pass, true);
	}

	[WebMethod(Description = "Get ROM executions with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(Execution))]
	public ReturnStatus LoadUserExecutions(string user, string pass, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadUserExecutions:", user, "/", pass }));

		ReturnStatus status = new ReturnStatus();
		if (!RomProxy.CheckAuthorization(user, pass, reload))
		{
			status.statusCode = RDSStatusCode.INVALID_USER;
			status.statusMessage = String.Concat(new object[] { "Invalid user name or password" });
			Logger.Logit(String.Concat(new object[] { "LoadUserExecutions returns: " + status.statusCode }));
		}
		else
		{
			try
			{
				status.retValues = RomProxy.BuildUserExecutions(user, reload);
			}
			catch (Exception exception)
			{
				status.statusCode = RDSStatusCode.INTERNAL_ERROR;
				status.statusMessage = String.Concat(new object[] { "BuildUserExecutions public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
				Logger.Logit(String.Concat(new object[] { "BuildUserExecutions: ", status.statusCode, ":", status.statusMessage }));
			}
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadUserExecutions returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Security By Ticker

	[WebMethod(Description = "Get TPOS Security details by ticker.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(SecurityDesc))]
	public ReturnStatus GetSecurityByTicker(string ticker)
	{
		return LoadSecurityByTicker(ticker, false);
	}

	[WebMethod(Description = "Get TPOS Security details by ticker with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(SecurityDesc))]
	public ReturnStatus LoadSecurityByTicker(string ticker, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadSecurityByTicker:", ticker, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result = new ArrayList();

			TposClient.BuildSecurityInfo(ticker, reload, out result);

			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadSecurityByTicker public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadSecurityByTicker: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadSecurityByTicker returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Option Chain By Ticker

	[WebMethod(Description = "Get option chain by ticker symbol.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(OptionDesc))]
	public ReturnStatus GetOptionChain(string ticker)
	{
		return LoadOptionChain(ticker, false);
	}

	[WebMethod(Description = "Get option chain by ticker symbol with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(OptionDesc))]
	public ReturnStatus LoadOptionChain(string ticker, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadOptionChain:", ticker, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			ticker = RdsGlobal.GetMappedTickerFromRoc(ticker);
			TposClient.BuildOptionChain(ticker, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadOptionChain public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadOptionChain: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadOptionChain returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Future Chain By Ticker

	[WebMethod(Description = "Get Future chain by underlying symbol.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus GetFutureChain(string underlying)
	{
		return LoadFutureChain(underlying, false);
	}

	[WebMethod(Description = "Get Future chain by underlying symbol with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus LoadFutureChain(string underlying, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadFutureChain:", underlying, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.BuildFutureChain(underlying, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadFutureChain public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadFutureChain: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadFutureChain returns: ", status.statusCode }));
		return status;
	}

	[WebMethod(Description = "Get Future chain by base.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus GetFutureChainByBase(string baseSymbol)
	{
		return LoadFutureChainByBase(baseSymbol, false);
	}

	[WebMethod(Description = "Get Future chain by base with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus LoadFutureChainByBase(string baseSymbol, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadFutureChain:", baseSymbol, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.BuildFutureChainByBase(baseSymbol, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadFutureChain public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadFutureChain: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadFutureChain returns: ", status.statusCode }));
		return status;
	}

	[WebMethod(Description = "Get Single Stock Future chain by ticker.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus GetSSFutureChain(string underlying)
	{
		return LoadSSFutureChain(underlying, false);
	}

	[WebMethod(Description = "Get Single Stock Future chain by ticker with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(FutureDesc))]
	public ReturnStatus LoadSSFutureChain(string underlying, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadSSFutureChain:", underlying, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			if (!underlying.EndsWith("1C"))
			{
				underlying = underlying + "1C";
			}
			TposClient.BuildSSFutureChain(underlying, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadSSFutureChain public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadSSFutureChain: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadSSFutureChain returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Tpos Accounts

	[WebMethod(Description = "Get All TPOS Trader Accounts.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTraderAccount))]
	public ReturnStatus GetAllTposAccounts()
	{
		return LoadAllTposAccounts(false);
	}

	[WebMethod(Description = "Get All TPOS Trader Accounts with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTraderAccount))]
	public ReturnStatus LoadAllTposAccounts(bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadAllTposAccounts", " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.GetAllAccounts(reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadAllTposAccounts public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadAllTposAccounts: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadAllTposAccounts returns: ", status.statusCode }));
		return status;
	}

	[WebMethod(Description = "Get TPOS Trader Accounts for user using TradeFor.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTraderAccount))]
	public ReturnStatus GetTposAccounts(string user)
	{
		return LoadTposAccounts(user, false);
	}

	[WebMethod(Description = "Get TPOS Trader Accounts for user using TradeFor with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTraderAccount))]
	public ReturnStatus LoadTposAccounts(string user, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadTposAccounts", " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.GetAccounts(user, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadTposAccounts public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadTposAccounts: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadTposAccounts returns: ", status.statusCode }));
		return status;
	}

	[WebMethod(Description = "Get TPOS Trader Accounts for user using LocalAcct.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTraderAccount))]
	public ReturnStatus GetTposAccountsByLocalAcct(string user)
	{
		return LoadTposAccountsByLocalAcct(user, false);
	}

	[WebMethod(Description = "Get TPOS Trader Accounts for user using LocalAcct with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTraderAccount))]
	public ReturnStatus LoadTposAccountsByLocalAcct(string user, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadTposAccountsByLocalAcct", " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.GetAccountsByLocalAcct(user, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadTposAccountsByLocalAcct public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadTposAccountsByLocalAcct: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadTposAccountsByLocalAcct returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Tpos Positions

	[WebMethod(Description = "Get Positions for user using TradeFor acronym.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition))]
	public ReturnStatus GetTposPositionsByTrader(string user, string trader)
	{
		return LoadTposPositions(user, false);
	}

	[WebMethod(Description = "Get Positions for user using TradeFor acronym.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition))]
	public ReturnStatus GetTposPositions(string user, ArrayList traders)
	{
		return LoadTposPositions(user, false);
	}

	[WebMethod(Description = "Get Positions for user using TradeFor acronym with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition))]
	public ReturnStatus LoadTposPositions(string user, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadTposPositions", user, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.GetPositions(user, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadTposPositions public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadTposPositions: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadTposPositions returns: ", status.statusCode }));
		return status;
	}

	[WebMethod(Description = "Get Positions for user using LocalAcct acronym.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition))]
	public ReturnStatus GetTposPositionsBylocalAcct(string user, string localAcct)
	{
		return LoadTposPositionsByLocalAcct(user, false);
	}

	[WebMethod(Description = "Get Positions for user using LocalAcct acronym.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition))]
	public ReturnStatus GetTposPositionsBylocalAccts(string user, ArrayList localAccts)
	{
		return LoadTposPositionsByLocalAcct(user, false);
	}

	[WebMethod(Description = "Get Positions for user using LocalAcct acronym with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposPosition))]
	public ReturnStatus LoadTposPositionsByLocalAcct(string user, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadTposPositionsByLocalAcct", user, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			//TposClient.GetPositionsByLocalAcct(user, reload, out result);
			TposClient.GetPositions(user, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadTposPositionsByLocalAcct public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadTposPositionsByLocalAcct: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadTposPositionsByLocalAcct returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Tpos Trades

	[WebMethod(Description = "Get Trades for user using TradeFor and with userfilter acronym.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTrade))]
	public ReturnStatus RtrvTposTrades(string user, ArrayList traders, bool initFlag)
	{
		return LoadTposTrades(user, true, false);
	}

	[WebMethod(Description = "Get Trades for user using TradeFor and with userfilter acronym.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTrade))]
	public ReturnStatus RtrvTposTradesByTrader(string user, string trader, bool initFlag)
	{
		return LoadTposTrades(user, true, false);
	}

	[WebMethod(Description = "Get Trades for user using TradeFor acronym with reload option.", EnableSession = false), XmlInclude(typeof(ReturnStatus)), XmlInclude(typeof(TposTrade))]
	public ReturnStatus LoadTposTrades(string user, bool filterUser, bool reload)
	{
		Logger.Logit(1, String.Concat(new object[] { "LoadTposTrades", user, " ", reload.ToString() }));

		ReturnStatus status = new ReturnStatus();
		try
		{
			ArrayList result;
			TposClient.GetTrades(user, filterUser, reload, out result);
			status.retValues = result;
		}
		catch (Exception exception)
		{
			status.statusCode = RDSStatusCode.INTERNAL_ERROR;
			status.statusMessage = String.Concat(new object[] { "LoadTposTrades public error: ", exception.ToString(), "\n", exception.StackTrace.ToString() });
			Logger.Logit(String.Concat(new object[] { "LoadTposTrades: ", status.statusCode, ":", status.statusMessage }));
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadTposTrades returns: ", status.statusCode }));
		return status;
	}

	#endregion

	#region Tpos Security Maps

	[WebMethod(Description = "Get ROC To Tpos Security Map By Ticker.", EnableSession = false)]
	public string GetMappedSecurityByTicker(string ticker)
	{
		return RdsGlobal.GetMappedTickerFromRoc(ticker);
	}

	[WebMethod(Description = "Get Tpos Traded Price Mult By Ticker .", EnableSession = false)]
	public Double GetMappedSecurityMultByTicker(string ticker)
	{
		return RdsGlobal.GetMappedMultTicker(ticker);
	}

	#endregion

	#region CME Spread Security Map

	[WebMethod(Description = "Reload CME Spread Map.", EnableSession = false)]
	public string ReloadCMESpreadMap()
	{
		return CMEMap.ReloadSpreadMap();
	}

	[WebMethod(Description = "Get CME Spread details by ticker.", EnableSession = false), XmlInclude(typeof(SecurityDesc)), XmlInclude(typeof(ReturnStatus))]
	public ReturnStatus GetCMESpreadByTicker(string ticker)
	{
		ReturnStatus status = new ReturnStatus();

		ArrayList result = new ArrayList();
		result = TposClient.GetFromLocalCMESpreadMap(ticker);
		status.retValues = result;

		return status;
	}

	#endregion

	#region CME Future Security Map

	[WebMethod(Description = "Reload CME Future Map.", EnableSession = false)]
	public string ReloadCMEFutureMap()
	{
		return CMEMap.ReloadFutureMap();
	}

	[WebMethod(Description = "Get CME Future details by ticker.", EnableSession = false), XmlInclude(typeof(SecurityDesc)), XmlInclude(typeof(ReturnStatus))]
	public ReturnStatus GetCMEFutureByTicker(string ticker)
	{
		ReturnStatus status = new ReturnStatus();

		ArrayList result = new ArrayList();
		result = TposClient.GetFromLocalCMEFutureMap(ticker);
		status.retValues = result;

		return status;
	}

	#endregion

	#region ICE Spread Security Map

	[WebMethod(Description = "Reload ICE Spread Map.", EnableSession = false)]
	public string ReloadICESpreadMap()
	{
		return ICEMap.ReloadSpreadMap();
	}

	[WebMethod(Description = "Get ICE Spread details by ticker.", EnableSession = false), XmlInclude(typeof(SecurityDesc)), XmlInclude(typeof(ReturnStatus))]
	public ReturnStatus GetICESpreadByTicker(string ticker)
	{
		ReturnStatus status = new ReturnStatus();

		ArrayList result = new ArrayList();
		result = TposClient.GetFromLocalICESpreadMap(ticker);
		status.retValues = result;

		return status;
	}

	#endregion

	#region ICE Future Security Map

	[WebMethod(Description = "Reload ICE Future Map.", EnableSession = false)]
	public string ReloadICEFutureMap()
	{
		return ICEMap.ReloadFutureMap();
	}

	[WebMethod(Description = "Get ICE Future details by ticker.", EnableSession = false), XmlInclude(typeof(SecurityDesc)), XmlInclude(typeof(ReturnStatus))]
	public ReturnStatus GetICEFutureByTicker(string ticker)
	{
		ReturnStatus status = new ReturnStatus();

		ArrayList result = new ArrayList();
		result = TposClient.GetFromLocalICEFutureMap(ticker);
		status.retValues = result;

		return status;
	}

	#endregion

	#region User Defined Security Map

	[WebMethod(Description = "Reload User Defined Map.", EnableSession = false)]
	public string ReloadUserSecMap()
	{
		return UserMap.ReloadUserSecMap();
	}

	[WebMethod(Description = "Get User Defined details by ticker.", EnableSession = false), XmlInclude(typeof(SecurityDesc)), XmlInclude(typeof(ReturnStatus))]
	public ReturnStatus GetUserSecByTicker(string ticker)
	{
		ReturnStatus status = new ReturnStatus();

		ArrayList result = new ArrayList();
		result = TposClient.GetFromLocalUserSecMap(ticker);
		status.retValues = result;

		return status;
	}

	#endregion

	[WebMethod(Description = "Change ROC user's password.", EnableSession = false)]
	public ReturnStatus SetUserPassword(string user, string oldpass, string newpass)
	{
		Logger.Logit(1, String.Concat(new object[] { "SetUserPassword:", user, "/", oldpass, "/", newpass }));
		
		ReturnStatus status = new ReturnStatus();
		status.retValues = false;
		if (!RomProxy.CheckAuthorization(user, oldpass, false))
		{
			status.statusCode = RDSStatusCode.INVALID_USER;
			status.statusMessage = "Invalid user name or password";
			Logger.Logit(String.Concat(new object[] { "LoadUserProfile returns: " + status.statusCode }));
		}
		else
		{
			try
			{
				RomProxy.ChangePassword(user, oldpass, newpass);
				RomProxy.CheckAuthorization(user, newpass, true);

				status.statusCode = RDSStatusCode.OK;
				status.statusMessage = "Password Updated";
			}
			catch (Exception exception)
			{
			    status.statusCode = RDSStatusCode.INTERNAL_ERROR;
				status.statusMessage = "BuildUserProfile public error: " + exception.ToString();
				Logger.Logit(String.Concat(new object[] { "LoadUserProfile: ", status.statusCode, ":", status.statusMessage }));
			}
		}

		Logger.Logit(1, String.Concat(new object[] { "LoadUserProfile returns: ", status.statusCode }));
		return status;
	}
}
