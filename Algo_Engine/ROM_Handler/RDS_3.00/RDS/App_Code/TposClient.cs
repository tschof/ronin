using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using TposWrapper;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;

public static class TposClient
{
	private sealed class TPOSKeys
	{
		public const string IM = "IM";
		public const string POS = "POS";
		public const string TRD = "TRD";
	}

	private delegate int IMDelegate(string key, ArrayList result);
	private delegate int PosDelegate(string tradingGroup, string portfolio, string traderAcronym, ArrayList positions);
	private delegate string TradeDelegate(string user, string tradingGroup, string portfolio, string traderAcronym, string bDate, string bTime, bool filterUser, ArrayList trades);

	private static string _server = ConfigurationManager.AppSettings["TposServer"];
	private static string _user = ConfigurationManager.AppSettings["TposUser"];
	private static string _pass = ConfigurationManager.AppSettings["TposPass"];
	private static string _port = ConfigurationManager.AppSettings["TposPort"];
	private static string _interfaces = ConfigurationManager.AppSettings["TposInterfaces"];
	
	private static string _imInterface = "";
	private static string imInterface
	{
		get
		{
			try
			{
				if (_imInterface == "")
				{
					string[] items = _interfaces.Split(new char[] { ',' }, StringSplitOptions.None);
					foreach (string item in items)
					{
						if (item.ToUpper().Contains("|IM"))
						{
							_imInterface = item.Replace("|IM", "");
						}
					}
				}

				if (_imInterface != "")
				{
					return _imInterface;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("imInterface Error ", ex);
			}

			return "RDS";
		}
	}

	private static string _tpInterface = "";
	private static string tpInterface
	{
		get
		{
			try
			{
				if (_tpInterface == "")
				{
					string[] items = _interfaces.Split(new char[] { ',' }, StringSplitOptions.None);
					foreach (string item in items)
					{
						if (item.ToUpper().Contains("|POS"))
						{
							_tpInterface = item.Replace("|POS", "");
						}
					}
				}

				if (_tpInterface != "")
				{
					return _tpInterface;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("imInterface Error ", ex);
			}

			return "WOW";
		}
	}

	private static int _timeOut = Int32.Parse(ConfigurationManager.AppSettings["TposTimeout"]);
	private static Thread _imWorker = null;
	private static Thread _posWorker = null;
	private static Thread _tradeWorker = null;

	private static DateTime _today = DateTime.Today;

	private static Dictionary<string, TposProxy> _tposProxys = new Dictionary<string,TposProxy>();
	public static Dictionary<string, TposProxy> TposProxys
	{
		get
		{
			if (_tposProxys == null || _tposProxys.Count == 0)
			{
				Connect();
			}
			return _tposProxys;
		}
	}

	private static Dictionary<string, TposTraderAccount> _tradeSubscribed = new Dictionary<string, TposTraderAccount>();
	private static Dictionary<string, TposTraderAccount> _positionSubscribed = new Dictionary<string, TposTraderAccount>();

	private static Dictionary<string, TradeCallback> _tradeCallBacks = new Dictionary<string, TradeCallback>();
	private static Dictionary<string, PositionCallback> _positionCallBacks = new Dictionary<string, PositionCallback>();

	#region Tpos Proxys

	//public static string Connect()
	//{
	//    string status = "";

	//    string[] items = _interfaces.Split(new char[] { ',' }, StringSplitOptions.None);

		
	//    _tposProxys.Clear();

	//    TposProxy tpos;
	//    if (items.Length < 1)
	//    {
	//        Logger.Logit(String.Concat(new object[] { 
	//                    "TPOS.Connect... ", 
	//                    " user: ", _user,
	//                    " pass: ", _pass,
	//                    " server: ", _server,
	//                    " port: ", _port,
	//                    " name: ", "RDS" }));
	//        tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), "RDS");
	//        try
	//        {
	//            tpos.Connect();
	//        }
	//        catch (Exception ex)
	//        {
	//            Logger.Logit("TPOS.Connect Error ", ex);
	//        }
	//        AddTposProxys("IM", tpos);

	//        Logger.Logit(String.Concat(new object[] { 
	//                    "TPOS.Connect... ", 
	//                    " user: ", _user,
	//                    " pass: ", _pass,
	//                    " server: ", _server,
	//                    " port: ", _port,
	//                    " name: ", "WOW" }));
	//        tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), "WOW");
	//        try
	//        {
	//            tpos.Connect();
	//        }
	//        catch (Exception ex)
	//        {
	//            Logger.Logit("TPOS.Connect Error ", ex);
	//        }
	//        AddTposProxys("POS", tpos);

	//        Logger.Logit(String.Concat(new object[] { 
	//                    "TPOS.Connect... ", 
	//                    " user: ", _user,
	//                    " pass: ", _pass,
	//                    " server: ", _server,
	//                    " port: ", _port,
	//                    " name: ", "WOW" }));
	//        tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), "WOW");
	//        try
	//        {
	//            tpos.Connect();
	//        }
	//        catch (Exception ex)
	//        {
	//            Logger.Logit("TPOS.Connect Error ", ex);
	//        }
	//        AddTposProxys("TRD", tpos);

	//        status = "Tpos default interface connected"; 
	//    }
	//    else
	//    {
	//        foreach (string item in items)
	//        {
	//            if (item.ToUpper().Contains("|IM"))
	//            {
	//                Logger.Logit(String.Concat(new object[] { 
	//                    "TPOS.Connect... ", 
	//                    " user: ", _user,
	//                    " pass: ", _pass,
	//                    " server: ", _server,
	//                    " port: ", _port,
	//                    " name: ", "RDS" }));
	//                tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), item.Replace("|IM", ""));
	//                try
	//                {
	//                    tpos.Connect();
	//                }
	//                catch (Exception ex)
	//                {
	//                    Logger.Logit("TPOS.Connect Error ", ex);
	//                }
	//                AddTposProxys("IM", tpos);
	//            }
	//            else if (item.ToUpper().Contains("|POS"))
	//            {
	//                Logger.Logit(String.Concat(new object[] { 
	//                    "TPOS.Connect... ", 
	//                    " user: ", _user,
	//                    " pass: ", _pass,
	//                    " server: ", _server,
	//                    " port: ", _port,
	//                    " name: ", "WOW" }));
	//                tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), item.Replace("|POS", ""));
	//                try
	//                {
	//                    tpos.Connect();
	//                }
	//                catch (Exception ex)
	//                {
	//                    Logger.Logit("TPOS.Connect Error ", ex);
	//                }
	//                AddTposProxys("POS", tpos);

	//                Logger.Logit(String.Concat(new object[] { 
	//                    "TPOS.Connect... ", 
	//                    " user: ", _user,
	//                    " pass: ", _pass,
	//                    " server: ", _server,
	//                    " port: ", _port,
	//                    " name: ", "WOW" }));
	//                tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), item.Replace("|POS", ""));
	//                try
	//                {
	//                    tpos.Connect();
	//                }
	//                catch (Exception ex)
	//                {
	//                    Logger.Logit("TPOS.Connect Error ", ex);
	//                }
	//                AddTposProxys("TRD", tpos);
	//            }
	//            else
	//            {
	//            }
	//        }

	//        status = "Tpos interface connected"; 
	//    }

	//    return status;
	//}

	public static string Connect()
	{
		string status = "";

		status = Connect(TPOSKeys.IM);
		status = status + " - " + Connect(TPOSKeys.POS);
		status = status + " - " + Connect(TPOSKeys.TRD);

		return status;
	}
	public static string Connect(string name)
	{
		string status = name + " Tpos interface connected";

		if (_tposProxys.ContainsKey(name))
		{
			DisConnect(name);
			status = name + " Tpos interface reconnected";
		}

		string locInterface = "";
		switch (name)
		{
			case TPOSKeys.IM:
				// Special Faster Symbol IM Server for RDS Only
				locInterface = imInterface;
				break;
			case TPOSKeys.POS:
			case TPOSKeys.TRD:
			default:
				locInterface = tpInterface;
				break;
		}

		if (locInterface != "")
		{
			Logger.Logit(String.Concat(new object[] { 
				"TPOS.Connect... ", 
				" user: ", _user,
				" pass: ", _pass,
				" server: ", _server,
				" port: ", _port,
				" name: ", locInterface }));

			try
			{
				TposProxy tpos = new TposProxy(_user, _pass, _server, int.Parse(_port), locInterface);
				tpos.Connect();
				AddTposProxys(name, tpos);
			}
			catch (Exception ex)
			{
				Logger.Logit("TPOS.Connect Error ", ex);
				status = name + " Tpos interface connection errored out";
			}
		}

		return status;
	}

	public static string DisConnect()
	{
		string status = "";

		status = DisConnect(TPOSKeys.IM);
		status = status + " - " + DisConnect(TPOSKeys.POS);
		status = status + " - " + DisConnect(TPOSKeys.TRD);

		return status;
	}
	private static string DisConnect(string name)
	{
		string status = name + " Tpos interface disconnected";

		lock (_tposProxys)
		{
			Logger.Logit("TPOS.DisConnect " + name);
			TposProxy tpos = _tposProxys[name];
			tpos.Dispose();
			tpos = null;

			_tposProxys.Remove(name);
		}

		return status;
	}

	private static void AddTposProxys(string key, TposProxy tpos)
	{
		lock (_tposProxys)
		{
			if (_tposProxys.ContainsKey(key))
			{
				if (_tposProxys[key] != null)
				{
					_tposProxys[key].Dispose();
					_tposProxys[key] = null;
				}
				_tposProxys[key] = tpos;
			}
			else
			{
				_tposProxys.Add(key, tpos);
			}
		}
	}

	private static void Reconnect(string name)
	{
		switch (name)
		{
			case TPOSKeys.IM:
				Connect(name);
				break;
			case TPOSKeys.POS:
			case TPOSKeys.TRD:
				//Connect(name);
				break;
		}
	}

	#region - Not Used -

	//public static string DisConnect()
	//{
	//    return DisConnect("");
	//}
	//public static string DisConnect(string name)
	//{
	//    string status = "";

	//    #region Don't DisConnect From POS Connection
	//    //string[] keys;

	//    //TposTraderAccount tposTraderAcct;

	//    //keys = new string[_tradeSubscribed.Count];
	//    //_tradeSubscribed.Keys.CopyTo(keys, 0);
	//    //foreach (string key in keys)
	//    //{
	//    //    tposTraderAcct = _tradeSubscribed[key];
	//    //    TposProxys["POS"].UnsubscribePositions(tposTraderAcct.TradingGroup, tposTraderAcct.Portfolio, tposTraderAcct.TraderAcronym);
	//    //    _tradeSubscribed.Remove(key);
	//    //}

	//    //keys = new string[_positionSubscribed.Count];
	//    //_positionSubscribed.Keys.CopyTo(keys, 0);
	//    //foreach (string key in keys)
	//    //{
	//    //    tposTraderAcct = _positionSubscribed[key];
	//    //    TposProxys["POS"].UnsubscribePositions(tposTraderAcct.TradingGroup, tposTraderAcct.Portfolio, tposTraderAcct.TraderAcronym);
	//    //    _positionSubscribed.Remove(key);
	//    //}
	//    #endregion

	//    if (_tposProxys != null && _tposProxys.Count > 0)
	//    {
	//        TposProxy tpos;
	//        string[] keys = new string[_tposProxys.Count];
	//        _tposProxys.Keys.CopyTo(keys, 0);
	//        foreach (string key in keys)
	//        {
	//            if (name == "" || key == name)
	//            {
	//                Logger.Logit("TPOS.DisConnect " + key);
	//                tpos = _tposProxys[key];
	//                tpos.Dispose();
	//                tpos = null;

	//                _tposProxys.Remove(key);
	//            }
	//        }

	//        status = "Tpos interface disconnected";
	//    }
	//    else
	//    {
	//        status = "No open Tpos connection";
	//    }

	//    return status;
	//}

	//public static void Reconnect()
	//{
	//    DisConnect();
	//    System.Threading.Thread.Sleep(5000);
	//    Connect();
	//}

	#endregion

	#endregion

	#region Local Map Info

	public static ArrayList GetFromLocalUserSecMap(string ticker)
	{
		ArrayList result = new ArrayList();
		bool localOk = false;
		DataRow[] rows = new DataRow[0];

		#region - User Map Test -

		// Is a User Defined Symbol
		rows = UserMap.UserSecMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			SecurityDesc desc = new SecurityDesc();

			if (rows[0]["ContractSize"] != null &&
				rows[0]["ContractSize"].ToString() != "" &&
				rows[0]["TickSize"] != null &&
				rows[0]["TickSize"].ToString() != ""
				)
			{
				localOk = Double.TryParse(rows[0]["ContractSize"].ToString(), out desc.contractSize);
				localOk = Double.TryParse(rows[0]["TickSize"].ToString(), out desc.tickSize);

				desc.securityType = rows[0]["SecType"].ToString();
				if (desc.securityType == "D")
				{
					desc.securityType = "F";
				}
				desc.genericMDSymbol = rows[0]["MDSymbol"].ToString();
				desc.baseSymbol = rows[0]["Symbol"].ToString();
				desc.fullSymbol = rows[0]["MDSymbol"].ToString();
				desc.exchange = rows[0]["DataSource"].ToString();
				desc.dataSourceInfo = rows[0]["DataSource"].ToString();
				desc.baseSymbol = rows[0]["MDSymbol"].ToString();
				desc.expirationDate = rows[0]["ExpDate"].ToString();
			}
			if (localOk)
			{
				desc.InstrumentId = "LocalUserMap";

				result = new ArrayList();
				result.Add(desc);
			}
		}

		#endregion

		return result;
	}

	public static ArrayList GetFromLocalCMESpreadMap(string ticker)
	{
		ArrayList result = new ArrayList();
		bool localOk = false;
		DataRow[] rows = new DataRow[0];

		#region - Spread Test -

		rows = CMEMap.SpreadMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			SecurityDesc desc = new SecurityDesc();

			if (rows[0]["ContractSize"] != null &&
				rows[0]["ContractSize"].ToString() != "" &&
				rows[0]["TickSize"] != null &&
				rows[0]["TickSize"].ToString() != ""
				)
			{
				localOk = Double.TryParse(rows[0]["ContractSize"].ToString(), out desc.contractSize);
				localOk = Double.TryParse(rows[0]["TickSize"].ToString(), out desc.tickSize);

				if (rows[0]["ExpDate"] != null && rows[0]["ExpDate"].ToString() != "")
				{
					desc.expirationDate = rows[0]["ExpDate"].ToString();
				}
			}
			if (localOk)
			{
				string[] baseSymbols = ticker.Split(new char[] { '-' }, StringSplitOptions.RemoveEmptyEntries);

				if (baseSymbols.Length > 0 && RdsGlobal.IsFutureBySymbol(baseSymbols[0]))
				{
					desc.baseSymbol = ticker.Substring(0, baseSymbols[0].Length - 2);
				}
				else
				{
					desc.baseSymbol = ticker;
				}
				desc.fullSymbol = ticker;
				desc.genericMDSymbol = ticker;
				desc.dataSourceInfo = "CME";
				desc.securityType = "F";
				desc.InstrumentId = "LocalCMESpreadMap";

				result = new ArrayList();
				result.Add(desc);

				return result;
			}
		}

		#endregion

		return result;
	}

	public static ArrayList GetFromLocalCMEFutureMap(string ticker)
	{
		ArrayList result = new ArrayList();
		bool localOk = false;
		DataRow[] rows = new DataRow[0];

		#region - Future Test -

		// Is a future symbol
		rows = CMEMap.FutureMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			SecurityDesc desc = new SecurityDesc();

			if (rows[0]["ContractSize"] != null &&
				rows[0]["ContractSize"].ToString() != "" &&
				rows[0]["TickSize"] != null &&
				rows[0]["TickSize"].ToString() != ""
				)
			{
				localOk = Double.TryParse(rows[0]["ContractSize"].ToString(), out desc.contractSize);
				localOk = Double.TryParse(rows[0]["TickSize"].ToString(), out desc.tickSize);

				if (rows[0]["ExpDate"] != null && rows[0]["ExpDate"].ToString() != "")
				{
					desc.expirationDate = rows[0]["ExpDate"].ToString();
				}
			}
			if (localOk)
			{
				desc.baseSymbol = ticker.Substring(0, ticker.Length - 2);
				desc.fullSymbol = ticker;
				desc.genericMDSymbol = ticker;
				desc.dataSourceInfo = "CME";
				desc.securityType = "F";
				desc.InstrumentId = "LocalCMEFutureMap";

				result = new ArrayList();
				result.Add(desc);

				return result;
			}
		}

		#endregion

		return result;
	}

	public static ArrayList GetFromLocalICESpreadMap(string ticker)
	{
		ArrayList result = new ArrayList();
		bool localOk = false;
		DataRow[] rows = new DataRow[0];

		#region - Spread Test -

		rows = ICEMap.SpreadMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			SecurityDesc desc = new SecurityDesc();

			if (rows[0]["ContractSize"] != null &&
			rows[0]["ContractSize"].ToString() != "" &&
			rows[0]["TickSize"] != null &&
			rows[0]["TickSize"].ToString() != ""
			)
			{
				localOk = Double.TryParse(rows[0]["ContractSize"].ToString(), out desc.contractSize);
				localOk = Double.TryParse(rows[0]["TickSize"].ToString(), out desc.tickSize);

				desc.securityType = rows[0]["SecType"].ToString();
				if (desc.securityType == "D")
				{
					desc.securityType = "F";
				}
				desc.genericMDSymbol = rows[0]["MDSymbol"].ToString();
				desc.baseSymbol = rows[0]["Symbol"].ToString();
				desc.fullSymbol = rows[0]["MDSymbol"].ToString();
				desc.exchange = rows[0]["DataSource"].ToString();
				desc.dataSourceInfo = rows[0]["DataSource"].ToString();
				desc.baseSymbol = rows[0]["MDSymbol"].ToString();
				desc.expirationDate = rows[0]["ExpDate"].ToString();
			}
			if (localOk)
			{
				desc.InstrumentId = "LocalICESpreadMap";

				result = new ArrayList();
				result.Add(desc);
			}
		}

		#endregion

		return result;
	}

	public static ArrayList GetFromLocalICEFutureMap(string ticker)
	{
		ArrayList result = new ArrayList();
		bool localOk = false;
		DataRow[] rows = new DataRow[0];

		#region - Future Test -

		// Is a future symbol
		rows = ICEMap.FutureMap.Select("Symbol = '" + ticker + "'");

		if (rows.Length > 0)
		{
			SecurityDesc desc = new SecurityDesc();

			if (rows[0]["ContractSize"] != null &&
				rows[0]["ContractSize"].ToString() != "" &&
				rows[0]["TickSize"] != null &&
				rows[0]["TickSize"].ToString() != ""
				)
			{
				localOk = Double.TryParse(rows[0]["ContractSize"].ToString(), out desc.contractSize);
				localOk = Double.TryParse(rows[0]["TickSize"].ToString(), out desc.tickSize);

				desc.securityType = rows[0]["SecType"].ToString();
				desc.genericMDSymbol = rows[0]["MDSymbol"].ToString();
				desc.baseSymbol = rows[0]["Symbol"].ToString();
				desc.fullSymbol = rows[0]["MDSymbol"].ToString();
				desc.exchange = rows[0]["DataSource"].ToString();
				desc.dataSourceInfo = rows[0]["DataSource"].ToString();
				desc.baseSymbol = rows[0]["MDSymbol"].ToString();
				desc.expirationDate = rows[0]["ExpDate"].ToString();
			}
			if (localOk)
			{
				desc.InstrumentId = "LocalICEMap";

				result = new ArrayList();
				result.Add(desc);
			}
		}

		#endregion

		return result;
	}

	private static ArrayList GetFromLocalMap(string ticker)
	{
		ArrayList result = new ArrayList();

		try
		{
			result = GetFromLocalCMESpreadMap(ticker);

			if (result == null || result.Count == 0)
			{
				result = GetFromLocalCMEFutureMap(ticker);
			}
			
			if (result == null || result.Count == 0)
			{
				result = GetFromLocalICESpreadMap(ticker);
			}

			if (result == null || result.Count == 0)
			{
				result = GetFromLocalICEFutureMap(ticker);
			}
		}
		catch (Exception ex)
		{
			Logger.Logit(String.Concat(new object[] { "GetFromLocalMap: ", ex.Message, ":", ex.StackTrace }));
		}

		return result;
	}

	#endregion

	#region Tpos Ticker Info

	public static int BuildSecurityInfo(string ticker, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			try
			{
				if (RdsCache.TposSecurityInfo.ContainsKey(ticker))
				{
					result = RdsCache.TposSecurityInfo[ticker];
				}
				else
				{
					reload = true;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildSecurityInfo.Reload Error ", ex);
			}
		}

		if (reload)
		{
			try
			{
				result = GetFromLocalUserSecMap(ticker);
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildSecurityInfo.GetFromLocalUserSecMap Error ", ex);
			}

			try
			{
				if ((result == null || result.Count == 0) && RdsGlobal.IsFutureBySymbol(ticker))
				{
					result = GetFromLocalMap(ticker);
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildSecurityInfo.GetFromLocalMap Error ", ex);
			}

			if (result == null || result.Count == 0)
			{
				ticker = RdsGlobal.GetMappedTickerFromRoc(ticker);
				try
				{
					status = RDSStatusCode.OK;
					if (TposProxys.ContainsKey(TPOSKeys.IM))
					{
						//status = TposProxys["IM"].GetSecurityByTicker(ticker, result);
						status = IMFunction(new IMDelegate(TposProxys["IM"].GetSecurityByTicker), ticker, result);
					}
				}
				catch (Exception ex)
				{
					Logger.Logit(String.Concat(new object[] { 
						"BuildSecurityInfo.GetSecurityByTicker Error ", 
						" ticker: ", ticker }));
					Logger.Logit("BuildSecurityInfo.GetSecurityByTicker Error ", ex);
					status = RDSStatusCode.INTERNAL_ERROR;
				}

				if (status != RDSStatusCode.OK)
				{
					Logger.Logit(String.Concat(new object[] { 
						"BuildSecurityInfo.GetSecurityByTicker INTERNAL_ERROR ", 
						" ticker: ", ticker }));

					Logger.Logit("BuildSecurityInfo.Reconnect After Error.");
					Reconnect(TPOSKeys.IM);
					return status;
				}
				else
				{
					if (result != null && result.Count > 0)
					{
						foreach (SecurityDesc secDesc in result)
						{
							secDesc.expirationDate = ReformatExpDate(secDesc.expirationDate);
						}
					}
				}
			}

			if (result != null || result.Count > 0)
			{
				if (RdsCache.TposSecurityInfo.ContainsKey(ticker))
				{
					RdsCache.TposSecurityInfo[ticker] = result;
				}
				else
				{
					RdsCache.TposSecurityInfo.Add(ticker, result);
				}
			}
		}

		return status;
	}

	public static int BuildOptionChain(string ticker, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			try
			{
				if (RdsCache.TposOptionChain.ContainsKey(ticker))
				{
					result = RdsCache.TposOptionChain[ticker];
				}
				else
				{
					reload = true;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildOptionChain.Reload Error ", ex);
			}
		}

		if (reload)
		{
			ticker = RdsGlobal.GetMappedTickerFromRoc(ticker);

			try
			{
				status = RDSStatusCode.OK;
				if (TposProxys.ContainsKey(TPOSKeys.IM))
				{
					//status = TposProxys["IM"].GetOptionChain(ticker, result);
					status = IMFunction(new IMDelegate(TposProxys[TPOSKeys.IM].GetOptionChain), ticker, result);
				}
			}
			catch (Exception ex)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildOptionChain.GetOptionChain Error ", 
						" ticker: ", ticker }));
				Logger.Logit("BuildOptionChain.GetOptionChain Error ", ex);
				status = RDSStatusCode.INTERNAL_ERROR;
			}

			if (status != RDSStatusCode.OK)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildOptionChain.GetOptionChain INTERNAL_ERROR ", 
						" ticker: ", ticker }));

				Logger.Logit("BuildOptionChain.Reconnect After Error.");
				Reconnect(TPOSKeys.IM);
				return status;
			}
			else
			{
				if (result != null && result.Count > 0)
				{
					foreach (OptionDesc secDesc in result)
					{
						secDesc.expirationDate = ReformatExpDate(secDesc.expirationDate, true);
					}

					if (RdsCache.TposOptionChain.ContainsKey(ticker))
					{
						RdsCache.TposOptionChain[ticker] = result;
					}
					else
					{
						RdsCache.TposOptionChain.Add(ticker, result);
					}
				}
			}
		}

		return status;
	}

	public static int BuildFutureChain(string ticker, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			try
			{
				if (RdsCache.TposFutureChain.ContainsKey(ticker))
				{
					result = RdsCache.TposFutureChain[ticker];
				}
				else
				{
					reload = true;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildFutureChain.Reload Error ", ex);
			}
		}

		if (reload)
		{
			ticker = RdsGlobal.GetMappedTickerFromRoc(ticker);
			try
			{
				status = RDSStatusCode.OK;
				if (TposProxys.ContainsKey(TPOSKeys.IM))
				{
					//status = TposProxys["IM"].GetFutureChain(ticker, result);
					status = IMFunction(new IMDelegate(TposProxys[TPOSKeys.IM].GetFutureChain), ticker, result);
				}
			}
			catch (Exception ex)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildFutureChain.GetFutureChain Error ", 
						" ticker: ", ticker }));
				Logger.Logit("BuildFutureChain.GetFutureChain Error ", ex);
				status = RDSStatusCode.INTERNAL_ERROR;
			}

			if (status != RDSStatusCode.OK)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildFutureChain.GetFutureChain INTERNAL_ERROR ", 
						" ticker: ", ticker }));

				Logger.Logit("BuildFutureChain.GetFutureChain After Error.");
				Reconnect(TPOSKeys.IM);
				return status;
			}
			else
			{
				if (result != null && result.Count > 0)
				{
					foreach (FutureDesc secDesc in result)
					{
						secDesc.expirationDate = ReformatExpDate(secDesc.expirationDate);
					}

					if (RdsCache.TposFutureChain.ContainsKey(ticker))
					{
						RdsCache.TposFutureChain[ticker] = result;
					}
					else
					{
						RdsCache.TposFutureChain.Add(ticker, result);
					}
				}
			}
		}

		return status;
	}

	public static int BuildFutureChainByBase(string baseTicker, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			try
			{
				if (RdsCache.TposFutureChainByBase.ContainsKey(baseTicker))
				{
					result = RdsCache.TposFutureChainByBase[baseTicker];
				}
				else
				{
					reload = true;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildFutureChainByBase.Reload Error ", ex);
			}
		}

		if (reload)
		{
			try
			{
				status = RDSStatusCode.OK;
				if (TposProxys.ContainsKey(TPOSKeys.IM))
				{
					//status = TposProxys["IM"].GetFutureChainByBase(baseTicker, result);
					status = IMFunction(new IMDelegate(TposProxys[TPOSKeys.IM].GetFutureChainByBase), baseTicker, result);
				}
			}
			catch (Exception ex)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildFutureChainByBase.GetFutureChainByBase Error ", 
						" baseticker: ", baseTicker }));
				Logger.Logit("BuildFutureChainByBase.GetFutureChainByBase Error ", ex);
				status = RDSStatusCode.INTERNAL_ERROR;
			}

			if (status != RDSStatusCode.OK)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildFutureChainByBase.GetFutureChainByBase INTERNAL_ERROR ", 
						" baseticker: ", baseTicker }));

				Logger.Logit("BuildFutureChainByBase.GetFutureChainByBase After Error.");
				Reconnect(TPOSKeys.IM);
				return status;
			}
			else
			{
				if (result != null && result.Count > 0)
				{
					foreach (FutureDesc secDesc in result)
					{
						secDesc.expirationDate = ReformatExpDate(secDesc.expirationDate);
					}

					if (RdsCache.TposFutureChainByBase.ContainsKey(baseTicker))
					{
						RdsCache.TposFutureChainByBase[baseTicker] = result;
					}
					else
					{
						RdsCache.TposFutureChainByBase.Add(baseTicker, result);
					}
				}
			}
		}

		return status;
	}

	public static int BuildSSFutureChain(string ticker, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			try
			{
				if (RdsCache.TposSSFutureChain.ContainsKey(ticker))
				{
					result = RdsCache.TposSSFutureChain[ticker];
				}
				else
				{
					reload = true;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("BuildSSFutureChain.Reload Error ", ex);
			}
		}

		if (reload)
		{
			try
			{
				status = RDSStatusCode.OK;
				if (TposProxys.ContainsKey(TPOSKeys.IM))
				{
					//status = TposProxys["IM"].GetSSFutureChain(ticker, result);
					status = IMFunction(new IMDelegate(TposProxys[TPOSKeys.IM].GetSSFutureChain), ticker, result);
				}
			}
			catch (Exception ex)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildSSFutureChain.GetSSFutureChain Error ", 
						" ticker: ", ticker }));
				Logger.Logit("BuildSSFutureChain.GetSSFutureChain Error ", ex);
				status = RDSStatusCode.INTERNAL_ERROR;
			}

			if (status != RDSStatusCode.OK)
			{
				Logger.Logit(String.Concat(new object[] { 
						"BuildSSFutureChain.GetSSFutureChain INTERNAL_ERROR ", 
						" ticker: ", ticker }));

				Logger.Logit("BuildSSFutureChain.GetSSFutureChain After Error.");
				Reconnect(TPOSKeys.IM);
				return status;
			}
			else
			{
				if (result != null && result.Count > 0)
				{
					foreach (FutureDesc secDesc in result)
					{
						secDesc.expirationDate = ReformatExpDate(secDesc.expirationDate);
					}

					if (RdsCache.TposSSFutureChain.ContainsKey(ticker))
					{
						RdsCache.TposSSFutureChain[ticker] = result;
					}
					else
					{
						RdsCache.TposSSFutureChain.Add(ticker, result);
					}
				}
			}
		}

		return status;
	}

	private static string ReformatExpDate(string expDate)
	{
		return ReformatExpDate(expDate, false);
	}

	private static string ReformatExpDate(string expDate, bool withDate)
	{
		DateTime exp = new DateTime();
		if (DateTime.TryParse(expDate, out exp))
		{
			if (withDate)
			{
				return exp.ToString("yyyyMMdd");
			}
			else
			{
				return exp.ToString("yyyyMM");
			}
		}

		return expDate;
	}

	#endregion

	#region Tpos Accounts

	public static int GetAllAccounts(bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			try
			{
				if (RdsCache.TposAccounts.Count > 0)
				{
					ArrayList subAccounts;
					foreach (string portfolio in RdsCache.TposAccounts.Keys)
					{
						subAccounts = RdsCache.TposAccounts[portfolio];
						result.AddRange(subAccounts);
					}
				}
				else
				{
					reload = true;
				}
			}
			catch (Exception ex)
			{
				Logger.Logit("GetAllAccounts.Reload Error ", ex);
				RdsCache.TposAccounts.Clear();
			}
		}

		if (reload)
		{
			if (TposProxys.ContainsKey(TPOSKeys.POS))
			{
				status = TposProxys[TPOSKeys.POS].GetTraderAccounts(result);
			}

			if (result != null && result.Count > 0)
			{
				TposTraderAccount account;
				ArrayList subAccounts;
				for (int i = 0; i < result.Count; i++)
				{
					account = (TposTraderAccount)result[i];
					if (!TposMap.GroupFilter.Contains(account.TradingGroup.ToUpper()))
					{
						if (RdsCache.TposAccounts.ContainsKey(account.Portfolio))
						{
							subAccounts = RdsCache.TposAccounts[account.Portfolio];
							subAccounts.Add(account);
							RdsCache.TposAccounts[account.Portfolio] = subAccounts;
						}
						else
						{
							subAccounts = new ArrayList();
							subAccounts.Add(account);
							RdsCache.TposAccounts.Add(account.Portfolio, subAccounts);
						}
					}
				}
			}
		}

		return status;
	}

	public static int GetAccounts(string user, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			if (RdsCache.TposUserAccounts.ContainsKey(user))
			{
				result = RdsCache.TposUserAccounts[user];
			}
			else
			{
				reload = true;
			}
		}

		if (reload)
		{
			// Get all the tader accounts assigned to this user in ROM
			ArrayList userAccts = new ArrayList();
			if (RdsCache.RomUserAccounts.ContainsKey(user))
			{
				// already in memory
				userAccts = RdsCache.RomUserAccounts[user];
			}
			else
			{
				// new user;
				userAccts = RomProxy.BuildUserProfile(user, true);
			}

			// Get all the trader accounts in TPOS
			ArrayList tposAccounts = new ArrayList();
			if (RdsCache.TposAccounts.Count > 0)
			{
				// Already have all TPOS accounts
			}
			else
			{
				// first TPOS account read
				GetAllAccounts(true, out tposAccounts);
			}

			TraderAcr userAcct;
			ArrayList tposTraderAccts;
			TposTraderAccount tposTraderAcct;

			// Get all the TPOS Acct for this user
			for (int userProfileCount = 0; userProfileCount < userAccts.Count; userProfileCount++)
			{
				userAcct = (TraderAcr)userAccts[userProfileCount];
				if (RdsCache.TposAccounts.ContainsKey(userAcct.tradeFor))
				{
					tposTraderAccts = RdsCache.TposAccounts[userAcct.tradeFor];
					for (int tposTraderAcctCount = 0; tposTraderAcctCount < tposTraderAccts.Count; tposTraderAcctCount++)
					{
						tposTraderAcct = (TposTraderAccount)tposTraderAccts[tposTraderAcctCount];
						result.Add(tposTraderAcct);
					}
				}
			}

			if (result != null && result.Count > 0)
			{
				if (RdsCache.TposUserAccounts.ContainsKey(user))
				{
					RdsCache.TposUserAccounts[user] = result;
				}
				else
				{
					RdsCache.TposUserAccounts.Add(user, result);
				}
			}
		}

		return status;
	}

	public static int GetAccountsByLocalAcct(string user, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (!reload)
		{
			if (RdsCache.TposUserAccounts.ContainsKey(user))
			{
				result = RdsCache.TposUserAccounts[user];
			}
			else
			{
				reload = true;
			}
		}

		if (reload)
		{
			// Get all the tader accounts assigned to this user in ROM
			ArrayList userAccts = new ArrayList();
			if (RdsCache.RomUserAccounts.ContainsKey(user))
			{
				// already in memory
				userAccts = RdsCache.RomUserAccounts[user];
			}
			else
			{
				// new user;
				userAccts = RomProxy.BuildUserProfile(user, true);
			}

			// Get all the trader accounts in TPOS
			ArrayList tposAccounts = new ArrayList();
			if (RdsCache.TposAccounts.Count > 0)
			{
				// Already have all TPOS accounts
			}
			else
			{
				// first TPOS account read
				GetAllAccounts(true, out tposAccounts);
			}

			TraderAcr userAcct;
			ArrayList tposTraderAccts;
			TposTraderAccount tposTraderAcct;

			// Get all the TPOS Acct for this user
			for (int userProfileCount = 0; userProfileCount < userAccts.Count; userProfileCount++)
			{
				userAcct = (TraderAcr)userAccts[userProfileCount];
				if (RdsCache.TposAccounts.ContainsKey(userAcct.localAcAcrn))
				{
					tposTraderAccts = RdsCache.TposAccounts[userAcct.localAcAcrn];
					for (int tposTraderAcctCount = 0; tposTraderAcctCount < tposTraderAccts.Count; tposTraderAcctCount++)
					{
						tposTraderAcct = (TposTraderAccount)tposTraderAccts[tposTraderAcctCount];
						result.Add(tposTraderAcct);
					}
				}
			}

			if (result != null && result.Count > 0)
			{
				if (RdsCache.TposAccounts.ContainsKey(user))
				{
					RdsCache.TposAccounts[user] = result;
				}
				else
				{
					RdsCache.TposAccounts.Add(user, result);
				}
			}
		}

		return status;
	}

	#endregion

	#region Tpos Positions

	private static bool _getPositionBusy = false;
	public static int GetPositions(string user, bool reload, out ArrayList result)
	{
		int status = 0;
		result = new ArrayList();

		if (_getPositionBusy)
		{
			return status;
		}

		_getPositionBusy = true;

		// Get all the tader accounts assigned to this user in ROM
		ArrayList userAccts = new ArrayList();
		try
		{
			if (RdsCache.RomUserAccounts.ContainsKey(user))
			{
				// already in memory
				userAccts = RdsCache.RomUserAccounts[user];
			}
			else
			{
				// new user;
				userAccts = RomProxy.BuildUserProfile(user, true);
			}
		}
		catch (Exception ex)
		{
			Logger.Logit("GetPositions.userAccts Error ", ex);
		}

		// Get all the trader accounts in TPOS
		ArrayList tposAccounts = new ArrayList();
		try
		{
			if (RdsCache.TposAccounts.Count > 0)
			{
				// Already have all TPOS accounts
			}
			else
			{
				// first TPOS account read
				GetAllAccounts(true, out tposAccounts);
			}
		}
		catch (Exception ex)
		{
			Logger.Logit("GetPositions.userAccts Error ", ex);
		}

		if (RdsCache.TposAccounts.Count == 0)
		{
			Logger.Logit("GetPositions.userAccts No TPOS Account Info!");

			_getPositionBusy = false;
			return status;
		}

		TraderAcr userAcct;
		AcctProfile userAcctProfile;
		ArrayList tposTraderAccts;
		TposTraderAccount tposTraderAcct;

		ArrayList positions;
		// Get all the TPOS position for this user
		try
		{
			for (int userProfileCount = 0; userProfileCount < userAccts.Count; userProfileCount++)
			{
				userAcct = (TraderAcr)userAccts[userProfileCount];
				ArrayList userAcctProfiles = userAcct.acctProfiles;

				if (userAcctProfiles != null)
				{
					for (int userAcctProfilesCount = 0; userAcctProfilesCount < userAcctProfiles.Count; userAcctProfilesCount++)
					{
						userAcctProfile = (AcctProfile)userAcctProfiles[userAcctProfilesCount];

						if (RdsCache.TposAccounts.ContainsKey(userAcctProfile.localAcAcrn))
						{
							tposTraderAccts = RdsCache.TposAccounts[userAcctProfile.localAcAcrn];
							for (int tposTraderAcctCount = 0; tposTraderAcctCount < tposTraderAccts.Count; tposTraderAcctCount++)
							{
								tposTraderAcct = (TposTraderAccount)tposTraderAccts[tposTraderAcctCount];
								positions = new ArrayList();

								try
								{
									status = RDSStatusCode.OK;
									if (TposProxys.ContainsKey(TPOSKeys.POS))
									{
										status = PosFunction(new PosDelegate(TposProxys[TPOSKeys.POS].GetBODPositions), tposTraderAcct.TradingGroup, tposTraderAcct.Portfolio, tposTraderAcct.TraderAcronym, positions);
									}
								}
								catch (Exception ex)
								{
									Logger.Logit(String.Concat(new object[] { 
										"GetPositions.GetBODPositions Error ", 
										" TradingGroup: ", tposTraderAcct.TradingGroup, 
										" Portfolio: ", tposTraderAcct.Portfolio, 
										" TraderAcronym: ", tposTraderAcct.TraderAcronym }));
									Logger.Logit("GetPositions.RtrvTrades Error ", ex);

									status = RDSStatusCode.INTERNAL_ERROR;
								}

								if (status == RDSStatusCode.OK)
								{
									if (positions != null && positions.Count > 0)
									{
										ArrayList modPositions = new ArrayList();
										lock (positions)
										{
											modPositions = new ArrayList(positions);
										}
										modPositions = RdsGlobal.ConvertToRocPositions(modPositions);
										result.AddRange(modPositions);
									}
								}
								else
								{
									Logger.Logit(String.Concat(new object[] { 
										"GetPositions.GetBODPositions INTERNAL_ERROR ", 
										" TradingGroup: ", tposTraderAcct.TradingGroup, 
										" Portfolio: ", tposTraderAcct.Portfolio, 
										" TraderAcronym: ", tposTraderAcct.TraderAcronym }));

									Logger.Logit("GetPositions.Reconnect After Error.");
									try
									{
										Reconnect(TPOSKeys.POS);
									}
									catch (Exception ex)
									{
										Logger.Logit("GetPositions.Reconnect Error ", ex);
									}

									_getPositionBusy = false;
									return status;
								}
							}
						}
					}
				}
			}
		}
		catch (Exception ex)
		{
			Logger.Logit("GetPositions Error ", ex);
		}

		_getPositionBusy = false;
		return status;
	}

	private class PositionCallback : TposPositionCallback
	{
		public override void OnPosition(TposPosition pos)
		{
			if (pos.m_CurQty != 0)
			{
				lock (RdsCache.TposUserPositions)
				{
					DataRow[] rows = RdsCache.TposUserPositions.Select("m_InstrumentID = '" + pos.m_InstrumentID + "' and m_ValidTraderKey = '" + pos.m_ValidTraderKey + "'");
					DataRow row;
					if (rows.Length > 0)
					{
						row = rows[0];

						row["m_AsOfDate"] = pos.m_AsOfDate;
						row["m_AvgCost"] = pos.m_AvgCost;
						row["m_ClearingAccount"] = pos.m_ClearingAccount;
						row["m_CurQty"] = pos.m_CurQty;
						row["m_DisplayFormat"] = pos.m_DisplayFormat;
						row["m_ExpDate"] = pos.m_ExpDate;
						row["m_InstrumentID"] = pos.m_InstrumentID;
						row["m_Multiplier"] = pos.m_Multiplier;
						row["m_NotionalAmount"] = pos.m_NotionalAmount;
						row["m_OpenMark"] = pos.m_OpenMark;
						row["m_OpenQty"] = pos.m_OpenQty;
						row["m_Portfolio"] = pos.m_Portfolio;
						row["m_PutCall"] = pos.m_PutCall;
						row["m_RealizedPnL"] = pos.m_RealizedPnL;
						row["m_SecType"] = pos.m_SecType;
						row["m_Strike"] = pos.m_Strike;
						row["m_Symbol"] = pos.m_Symbol;
						row["m_Ticker"] = pos.m_Ticker;
						row["m_TradeDate"] = pos.m_TradeDate;
						row["m_TradeGroup"] = pos.m_TradeGroup;
						row["m_TraderAcronym"] = pos.m_TraderAcronym;
						row["m_UndSecType"] = pos.m_UndSecType;
						row["m_UndSymbol"] = pos.m_UndSymbol;
						row["m_ValidTraderKey"] = pos.m_ValidTraderKey;
						row["m_Version"] = pos.m_Version;
						row["position"] = pos;

						row.AcceptChanges();
					}
					else
					{
						row = RdsCache.TposUserPositions.NewRow();

						row["m_AsOfDate"] = pos.m_AsOfDate;
						row["m_AvgCost"] = pos.m_AvgCost;
						row["m_ClearingAccount"] = pos.m_ClearingAccount;
						row["m_CurQty"] = pos.m_CurQty;
						row["m_DisplayFormat"] = pos.m_DisplayFormat;
						row["m_ExpDate"] = pos.m_ExpDate;
						row["m_InstrumentID"] = pos.m_InstrumentID;
						row["m_Multiplier"] = pos.m_Multiplier;
						row["m_NotionalAmount"] = pos.m_NotionalAmount;
						row["m_OpenMark"] = pos.m_OpenMark;
						row["m_OpenQty"] = pos.m_OpenQty;
						row["m_Portfolio"] = pos.m_Portfolio;
						row["m_PutCall"] = pos.m_PutCall;
						row["m_RealizedPnL"] = pos.m_RealizedPnL;
						row["m_SecType"] = pos.m_SecType;
						row["m_Strike"] = pos.m_Strike;
						row["m_Symbol"] = pos.m_Symbol;
						row["m_Ticker"] = pos.m_Ticker;
						row["m_TradeDate"] = pos.m_TradeDate;
						row["m_TradeGroup"] = pos.m_TradeGroup;
						row["m_TraderAcronym"] = pos.m_TraderAcronym;
						row["m_UndSecType"] = pos.m_UndSecType;
						row["m_UndSymbol"] = pos.m_UndSymbol;
						row["m_ValidTraderKey"] = pos.m_ValidTraderKey;
						row["m_Version"] = pos.m_Version;
						row["position"] = pos;

						RdsCache.TposUserPositions.Rows.Add(row);
					}
				}
			}
		}
	}

	#endregion

	#region Tpos Trades

	private static bool _getTradesBusy = false;
	public static int GetTrades(string user, bool filterUser, bool reload, out ArrayList result)
	{
		int status = 0;
		//string lastGetTime = "";
		result = new ArrayList();

		if (_getTradesBusy)
		{
			return status;
		}

		_getTradesBusy = true;

		// Get all the tader accounts assigned to this user in ROM
		ArrayList userAccts = new ArrayList();
		try
		{
			if (RdsCache.RomUserAccounts.ContainsKey(user))
			{
				// already in memory
				userAccts = RdsCache.RomUserAccounts[user];
			}
			else
			{
				// new user;
				userAccts = RomProxy.BuildUserProfile(user, true);
			}
		}
		catch (Exception ex)
		{
			Logger.Logit("GetTrades.userAccts Error ", ex);
		}

		// Get all the trader accounts in TPOS
		ArrayList tposAccounts = new ArrayList();
		try
		{
			if (RdsCache.TposAccounts.Count > 0)
			{
				// Already have all TPOS accounts
			}
			else
			{
				// first TPOS account read
				GetAllAccounts(true, out tposAccounts);
			}
		}
		catch (Exception ex)
		{
			Logger.Logit("GetTrades.tposAccounts Error ", ex);
		}

		if (RdsCache.TposAccounts.Count == 0)
		{
			Logger.Logit("GetTrades.tposAccounts No TPOS Account Info!");

			_getTradesBusy = false;
			return status;
		}

		TraderAcr userAcct;
		AcctProfile userAcctProfile;
		ArrayList tposTraderAccts;
		TposTraderAccount tposTraderAcct;

		ArrayList trades;
		// Get all the TPOS position for this user
		try
		{
			for (int userProfileCount = 0; userProfileCount < userAccts.Count; userProfileCount++)
			{
				userAcct = (TraderAcr)userAccts[userProfileCount];
				ArrayList userAcctProfiles = userAcct.acctProfiles;

				if (userAcctProfiles != null)
				{
					for (int userAcctProfilesCount = 0; userAcctProfilesCount < userAcctProfiles.Count; userAcctProfilesCount++)
					{
						userAcctProfile = (AcctProfile)userAcctProfiles[userAcctProfilesCount];

						if (RdsCache.TposAccounts.ContainsKey(userAcctProfile.localAcAcrn))
						{
							tposTraderAccts = RdsCache.TposAccounts[userAcctProfile.localAcAcrn];
							for (int tposTraderAcctCount = 0; tposTraderAcctCount < tposTraderAccts.Count; tposTraderAcctCount++)
							{
								tposTraderAcct = (TposTraderAccount)tposTraderAccts[tposTraderAcctCount];
								trades = new ArrayList();

								try
								{
									status = RDSStatusCode.OK;
									if (TposProxys.ContainsKey(TPOSKeys.TRD))
									{
										status = TradeFunction(new TradeDelegate(TposProxys[TPOSKeys.TRD].RtrvTrades), user, tposTraderAcct.TradingGroup, tposTraderAcct.Portfolio, tposTraderAcct.TraderAcronym, _today.ToString("yyyy-MM-dd"), _today.ToString("HH:mm:ss"), filterUser, trades);
									}
								}
								catch (Exception ex)
								{
									Logger.Logit(String.Concat(new object[] { 
										"GetTrades.RtrvTrades Error ", 
										" user: ", user,
										" TradingGroup: ", tposTraderAcct.TradingGroup, 
										" Portfolio: ", tposTraderAcct.Portfolio, 
										" TraderAcronym: ", tposTraderAcct.TraderAcronym,
										" filterUser: ", filterUser}));
									Logger.Logit("GetTrades.RtrvTrades Error ", ex);
									status = RDSStatusCode.INTERNAL_ERROR;
								}

								if (status == RDSStatusCode.OK)
								{
									if (trades != null && trades.Count > 0)
									{
										trades = RdsGlobal.ConvertToRocTrades(trades);
										result.AddRange(trades);
									}
								}
								else
								{
									Logger.Logit(String.Concat(new object[] { 
										"GetTrades.RtrvTrades INTERNAL_ERROR ", 
										" user: ", user,
										" TradingGroup: ", tposTraderAcct.TradingGroup, 
										" Portfolio: ", tposTraderAcct.Portfolio, 
										" TraderAcronym: ", tposTraderAcct.TraderAcronym,
										" filterUser: ", filterUser}));

									Logger.Logit("GetTrades.Reconnect After Error.");
									try
									{
										Reconnect(TPOSKeys.TRD);
									}
									catch (Exception ex)
									{
										Logger.Logit("GetTrades.Reconnect Error ", ex);
									}

									_getTradesBusy = false;
									return status;
								}
							}
						}
					}
				}
			}
		}
		catch (Exception ex)
		{
			Logger.Logit("GetTrades Error ", ex);
		}

		_getTradesBusy = false;
		return status;
	}

	private class TradeCallback : TposTradeCallback
	{
		public override void OnTrade(TposTrade trade)
		{
			populateTrade(trade);
		}

		private void populateTrade(TposTrade trade)
		{
			DataRow[] rows = RdsCache.TposUserTrades.Select("m_TradeID = '" + trade.m_TradeID + "'");
			DataRow row;
			if (rows.Length > 0)
			{
				row = rows[0];

				populateTradeRow(ref row, trade);

				row.AcceptChanges();
			}
			else
			{
				row = RdsCache.TposUserTrades.NewRow();

				populateTradeRow(ref row, trade);

				RdsCache.TposUserTrades.Rows.Add(row);
			}
		}

		private void populateTradeRow(ref DataRow row, TposTrade trade)
		{
			row["m_ActiveState"] = trade.m_ActiveState;
			row["m_ClearingAccount"] = trade.m_ClearingAccount;
			row["m_ClearingStatus"] = trade.m_ClearingStatus;
			row["m_Commission"] = trade.m_Commission;
			row["m_ContraBroker"] = trade.m_ContraBroker;
			row["m_ContraFirm"] = trade.m_ContraFirm;
			row["m_Exchange"] = trade.m_Exchange;
			row["m_ExpDate"] = trade.m_ExpDate;
			row["m_ExtDescription"] = trade.m_ExtDescription;
			row["m_ExtTradeID"] = trade.m_ExtTradeID;
			row["m_LastModDate"] = trade.m_LastModDate;
			row["m_LastModReason"] = trade.m_LastModReason;
			row["m_LastModReasonID"] = trade.m_LastModReasonID;
			row["m_LastModTime"] = trade.m_LastModTime;
			row["m_Note"] = trade.m_Note;
			row["m_Portfolio"] = trade.m_Portfolio;
			row["m_Price"] = trade.m_Price;
			row["m_PutCall"] = trade.m_PutCall;
			row["m_Qty"] = trade.m_Qty;
			row["m_SecType"] = trade.m_SecType;
			row["m_SettleDate"] = trade.m_SettleDate;
			row["m_ShortSaleFlag"] = trade.m_ShortSaleFlag;
			row["m_Strike"] = trade.m_Strike;
			row["m_Symbol"] = trade.m_Symbol;
			row["m_Ticker"] = trade.m_Ticker;
			row["m_TradeDate"] = trade.m_TradeDate;
			row["m_TradeGroup"] = trade.m_TradeGroup;
			row["m_TradeID"] = trade.m_TradeID;
			row["m_TraderAcronym"] = trade.m_TraderAcronym;
			row["m_TradeSource"] = trade.m_TradeSource;
			row["m_TradeTime"] = trade.m_TradeTime;
			row["m_UndSecType"] = trade.m_UndSecType;
			row["m_UndSymbol"] = trade.m_UndSymbol;
			row["m_Version"] = trade.m_Version;
			row["trade"] = trade;
		}
	}

	private static void populateTrade(TposTrade trade)
	{
		lock (RdsCache.TposUserTrades)
		{
			DataRow[] rows = RdsCache.TposUserTrades.Select("m_TradeID = '" + trade.m_TradeID + "'");
			DataRow row;
			if (rows.Length > 0)
			{
				row = rows[0];

				populateTradeRow(ref row, trade);

				row.AcceptChanges();
			}
			else
			{
				row = RdsCache.TposUserTrades.NewRow();

				populateTradeRow(ref row, trade);

				RdsCache.TposUserTrades.Rows.Add(row);
			}
		}
	}

	private static void populateTradeRow(ref DataRow row, TposTrade trade)
	{
		row["m_ActiveState"] = trade.m_ActiveState;
		row["m_ClearingAccount"] = trade.m_ClearingAccount;
		row["m_ClearingStatus"] = trade.m_ClearingStatus;
		row["m_Commission"] = trade.m_Commission;
		row["m_ContraBroker"] = trade.m_ContraBroker;
		row["m_ContraFirm"] = trade.m_ContraFirm;
		row["m_Exchange"] = trade.m_Exchange;
		row["m_ExpDate"] = trade.m_ExpDate;
		row["m_ExtDescription"] = trade.m_ExtDescription;
		row["m_ExtTradeID"] = trade.m_ExtTradeID;
		row["m_LastModDate"] = trade.m_LastModDate;
		row["m_LastModReason"] = trade.m_LastModReason;
		row["m_LastModReasonID"] = trade.m_LastModReasonID;
		row["m_LastModTime"] = trade.m_LastModTime;
		row["m_Note"] = trade.m_Note;
		row["m_Portfolio"] = trade.m_Portfolio;
		row["m_Price"] = trade.m_Price;
		row["m_PutCall"] = trade.m_PutCall;
		row["m_Qty"] = trade.m_Qty;
		row["m_SecType"] = trade.m_SecType;
		row["m_SettleDate"] = trade.m_SettleDate;
		row["m_ShortSaleFlag"] = trade.m_ShortSaleFlag;
		row["m_Strike"] = trade.m_Strike;
		row["m_Symbol"] = trade.m_Symbol;
		row["m_Ticker"] = trade.m_Ticker;
		row["m_TradeDate"] = trade.m_TradeDate;
		row["m_TradeGroup"] = trade.m_TradeGroup;
		row["m_TradeID"] = trade.m_TradeID;
		row["m_TraderAcronym"] = trade.m_TraderAcronym;
		row["m_TradeSource"] = trade.m_TradeSource;
		row["m_TradeTime"] = trade.m_TradeTime;
		row["m_UndSecType"] = trade.m_UndSecType;
		row["m_UndSymbol"] = trade.m_UndSymbol;
		row["m_Version"] = trade.m_Version;
		row["trade"] = trade;
	}
	
	#endregion

	#region Threaded Tpos Call With Timeout

	private static void ParameterizedIMFunction(object mp)
	{
		IMParameters parameters = (IMParameters)mp;
		parameters.tposDelegate(parameters.key, parameters.result);
	}

	private static void ParameterizedPosFunction(object mp)
	{
		PosParameters parameters = (PosParameters)mp;
		parameters.tposDelegate(parameters.tradingGroup, parameters.portfolio, parameters.traderAcronym, parameters.positions);
	}

	private static void ParameterizedTradeFunction(object mp)
	{
		TradeParameters parameters = (TradeParameters)mp;
		parameters.tposDelegate(parameters.user, parameters.tradingGroup, parameters.portfolio, parameters.traderAcronym, parameters.bDate, parameters.bTime, parameters.filterUser, parameters.trades);
	}

	private static int IMFunction(IMDelegate tposDelegate, string key, ArrayList result)
	{
		int ok = RDSStatusCode.OK;
		try
		{
			result.Clear();
			if (_timeOut == 0)
			{
				tposDelegate(key, result);
				return ok;
			}
			_imWorker = new Thread(new ParameterizedThreadStart(ParameterizedIMFunction));
			IMParameters parameter = new IMParameters();
			parameter.tposDelegate = tposDelegate;
			parameter.key = key;
			parameter.result = result;
			_imWorker.Start(parameter);
			_imWorker.Join(_timeOut);
			if (_imWorker.ThreadState != ThreadState.Stopped)
			{
				ok = RDSStatusCode.TPOS_TIMEOUT;
			}
		}
		catch (Exception exception)
		{
			Logger.Logit("Exception caught: " + exception.ToString());
			ok = RDSStatusCode.INTERNAL_ERROR;
		}

		return ok;
	}

	private static int PosFunction(PosDelegate tposDelegate, string tradingGroup, string portfolio, string traderAcronym, ArrayList positions)
	{
		int ok = RDSStatusCode.OK;
		try
		{
			positions.Clear();
			if (_timeOut == 0)
			{
				tposDelegate(tradingGroup, portfolio, traderAcronym, positions);
				return ok;
			}
			_posWorker = new Thread(new ParameterizedThreadStart(ParameterizedPosFunction));
			PosParameters parameter = new PosParameters();
			parameter.tposDelegate = tposDelegate;
			parameter.tradingGroup = tradingGroup;
			parameter.portfolio = portfolio;
			parameter.traderAcronym = traderAcronym;
			parameter.positions = positions;
			_posWorker.Start(parameter);
			_posWorker.Join(_timeOut);
			if (_posWorker.ThreadState != ThreadState.Stopped)
			{
				ok = RDSStatusCode.TPOS_TIMEOUT;
			}
		}
		catch (Exception exception)
		{
			Logger.Logit("Exception caught: " + exception.ToString());
			ok = RDSStatusCode.INTERNAL_ERROR;
		}

		return ok;
	}

	private static int TradeFunction(TradeDelegate tposDelegate, string user, string tradingGroup, string portfolio, string traderAcronym, string bDate, string bTime, bool filterUser, ArrayList trades)
	{
		int ok = RDSStatusCode.OK;
		try
		{
			trades.Clear();
			if (_timeOut == 0)
			{
				tposDelegate(user, tradingGroup, portfolio, traderAcronym, bDate, bTime, filterUser, trades);
				return ok;
			}
			_tradeWorker = new Thread(new ParameterizedThreadStart(ParameterizedTradeFunction));
			TradeParameters parameter = new TradeParameters();
			parameter.user = user;
			parameter.tposDelegate = tposDelegate;
			parameter.tradingGroup = tradingGroup;
			parameter.portfolio = portfolio;
			parameter.traderAcronym = traderAcronym;
			parameter.bDate = bDate;
			parameter.bTime = bTime;
			parameter.filterUser = filterUser;			
			parameter.trades = trades;
			_tradeWorker.Start(parameter);
			_tradeWorker.Join(_timeOut);
			if (_tradeWorker.ThreadState != ThreadState.Stopped)
			{
				ok = RDSStatusCode.TPOS_TIMEOUT;
			}
		}
		catch (Exception exception)
		{
			Logger.Logit("Exception caught: " + exception.ToString());
			ok = RDSStatusCode.INTERNAL_ERROR;
		}

		return ok;
	}

	[StructLayout(LayoutKind.Sequential)]
	private struct IMParameters
	{
		public TposClient.IMDelegate tposDelegate;
		public string key;
		public ArrayList result;
	}

	[StructLayout(LayoutKind.Sequential)]
	private struct PosParameters
	{
		public TposClient.PosDelegate tposDelegate;
		public string tradingGroup;
		public string portfolio;
		public string traderAcronym;
		public ArrayList positions;
	}

	[StructLayout(LayoutKind.Sequential)]
	private struct TradeParameters
	{
		public TposClient.TradeDelegate tposDelegate;
		public string user;
		public string tradingGroup;
		public string portfolio;
		public string traderAcronym;
		public string bDate;
		public string bTime;
		public bool filterUser;
		public ArrayList trades;
	}

	#endregion
}