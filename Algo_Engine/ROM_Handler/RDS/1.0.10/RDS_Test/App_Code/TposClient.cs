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
using System.Runtime.InteropServices;
using TposWrapper;
using System.Threading;

/// <summary>
/// Summary description for TposClient
/// </summary>
public class TposClient
{
    // Fields
    private ArrayList m_allTraders;
    private Hashtable m_groupTable;
    private Logger m_logger;
    private Hashtable m_loginTable;
    private TposProxy m_proxy;
	private TposProxy m_proxy2;
    private int m_timeout;
    private Thread m_worker;

    // Methods
    public TposClient()
    {
		//string server = ConfigurationManager.AppSettings["tposserver"];
		//string user = ConfigurationManager.AppSettings["tposuser"];
		//string pass = ConfigurationManager.AppSettings["tpospass"];
		//string s = ConfigurationManager.AppSettings["tposport"];
		//string tposface = ConfigurationManager.AppSettings["tposinterface"];
        try
        {
            this.m_timeout = 0x3e8 * int.Parse(ConfigurationManager.AppSettings["timeout"]);
        }
        catch (Exception)
        {
            this.m_timeout = 5;
        }
        this.m_logger = Logger.instance();
        //this.m_proxy = new TposProxy(user, pass, server, int.Parse(s), tposface);
		TposProxy();
		TposProxy2();
        this.m_worker = null;
    }

	public void ResetTposProxy()
	{
		if (m_proxy != null)
		{
			m_proxy.Dispose();
			m_proxy = null;
		}

		if (m_proxy2 != null)
		{
			m_proxy2.Dispose();
			m_proxy2 = null;
		}

		TposProxy();
		TposProxy2();
	}


	private void TposProxy()
	{
		string server = ConfigurationManager.AppSettings["tposserver"];
        string user = ConfigurationManager.AppSettings["tposuser"];
        string pass = ConfigurationManager.AppSettings["tpospass"];
        string s = ConfigurationManager.AppSettings["tposport"];
        string tposface = ConfigurationManager.AppSettings["tposinterface"];
		this.m_proxy = new TposProxy(user, pass, server, int.Parse(s), tposface);
	}

	private void TposProxy2()
	{
		string server = ConfigurationManager.AppSettings["tposserver"];
		string user = ConfigurationManager.AppSettings["tposuser"];
		string pass = ConfigurationManager.AppSettings["tpospass"];
		string s = ConfigurationManager.AppSettings["tposport"];
		string tposface = ConfigurationManager.AppSettings["tposinterface"];
		this.m_proxy2 = new TposProxy(user, pass, server, int.Parse(s), "");
	}

    public int BuildFutureChain(string underlying, ArrayList result)
    {
        return this.TposFunction(new TposDelegate(this.m_proxy.GetFutureChain), underlying, result);
    }

    public int BuildFutureChainByBase(string baseSymbol, ArrayList result)
    {
        return this.TposFunction(new TposDelegate(this.m_proxy.GetFutureChainByBase), baseSymbol, result);
    }

    public int BuildOptionChain(string underlying, ArrayList result)
    {
        return this.TposFunction(new TposDelegate(this.m_proxy.GetOptionChain), underlying, result);
    }

    public int BuildSecurityInfo(string ticker, ArrayList result)
    {
        return this.TposFunction(new TposDelegate(this.m_proxy.GetSecurityByTicker), ticker, result);
    }

	// 1.0.20
	public string BuildMappedSecurityInfo(string ticker)
	{
		return this.m_proxy.GetMappedSecurity(ticker);
	}

    public int BuildSSFutureChain(string underlying, ArrayList result)
    {
        if (!underlying.EndsWith("1C"))
        {
            underlying = underlying + "1C";
        }
        return this.TposFunction(new TposDelegate(this.m_proxy.GetSSFutureChain), underlying, result);
    }

    public int Connect()
    {
        int num = this.m_proxy.Connect();
        this.m_loginTable = new Hashtable();
        this.m_groupTable = new Hashtable();
        this.m_allTraders = new ArrayList();
        this.m_proxy.GetTraderAccounts(this.m_allTraders);
        for (int i = 0; i < this.m_allTraders.Count; i++)
        {
            TposTraderAccount account = (TposTraderAccount)this.m_allTraders[i];
			if (account.m_TradingGroup.ToUpper() != "RLLUP")
			{
				// ZCHA
				string key = account.m_TradingGroup + "." + account.m_Portfolio + "." + account.m_TraderAcronym;
				//GroupItem key = new GroupItem(account.m_TradingGroup, account.m_Portfolio, account.m_TraderAcronym);
				if (!this.m_groupTable.Contains(key))
				{
					// ZCHA
					GroupItem item = new GroupItem(account.m_TradingGroup, account.m_Portfolio, account.m_TraderAcronym);
					//string item = account.m_TraderAcronym;
					this.m_groupTable.Add(key, item);
				}
			}
        }
        return num;
    }

    public RocLoginItem GetOrCreateLogin(string lname)
    {
        RocLoginItem item = null;
        if (this.m_loginTable.Contains(lname))
        {
            return (RocLoginItem)this.m_loginTable[lname];
        }
        item = new RocLoginItem(lname);
        this.m_loginTable.Add(lname, item);
        return item;
    }

    public int GetPositions(string lname, ArrayList traders, ArrayList results)
    {
        this.GetOrCreateLogin(lname);
        this.m_logger.logDebug("TPOS Traders in Portfolio/TradeGroup:");
        ArrayList list = new ArrayList(this.m_groupTable.Values);

		//TposProxy2();
		//this.m_proxy2.Connect();

		string mode = ConfigurationManager.AppSettings["TPOSConnectionMode"];
		if (mode == "0")
		{
			string server = ConfigurationManager.AppSettings["tposserver"];
			string user = ConfigurationManager.AppSettings["tposuser"];
			string pass = ConfigurationManager.AppSettings["tpospass"];
			string s = ConfigurationManager.AppSettings["tposport"];
			string tposface = ConfigurationManager.AppSettings["tposinterface"];
			TposProxy prox = new TposProxy(user, pass, server, int.Parse(s), tposface);
			prox.Connect();

			for (int i = 0; i < list.Count; i++)
			{
				GroupItem item = (GroupItem)list[i];
				if (traders.Contains(item.m_portfolio))
				{
					this.m_logger.logDebug("   " + item.m_portfolio + ", " + item.m_group + ", " + item.m_traderAcronym);
					ArrayList @out = new ArrayList();
					prox.GetBODPositions(item.m_group, item.m_portfolio, item.m_traderAcronym, @out);
					results.AddRange(@out);
				}
			}

			prox.Dispose();
			prox = null;
		}
		for (int i = 0; i < list.Count; i++)
		{
			GroupItem item = (GroupItem)list[i];
			if (traders.Contains(item.m_portfolio))
			{
				this.m_logger.logDebug("   " + item.m_portfolio + ", " + item.m_group + ", " + item.m_traderAcronym);
				ArrayList @out = new ArrayList();
				this.m_proxy2.GetBODPositions(item.m_group, item.m_portfolio, item.m_traderAcronym, @out);
				results.AddRange(@out);
			}
		}

		//this.m_proxy2.Dispose();
		//this.m_proxy2 = null;

        return results.Count;
    }

	public int GetPositionsBylocalAcct(string lname, ArrayList localAccts, ArrayList results)
	{
		this.GetOrCreateLogin(lname);
		this.m_logger.logDebug("TPOS Traders in Portfolio/TradeGroup:");
		ArrayList list = new ArrayList(this.m_groupTable.Values);

		//TposProxy2();
		//this.m_proxy2.Connect();

		string mode = ConfigurationManager.AppSettings["TPOSConnectionMode"];
		if (mode == "0")
		{
			string server = ConfigurationManager.AppSettings["tposserver"];
			string user = ConfigurationManager.AppSettings["tposuser"];
			string pass = ConfigurationManager.AppSettings["tpospass"];
			string s = ConfigurationManager.AppSettings["tposport"];
			string tposface = ConfigurationManager.AppSettings["tposinterface"];
			TposProxy prox = new TposProxy(user, pass, server, int.Parse(s), tposface);
			prox.Connect();

			for (int iAcct = 0; iAcct < localAccts.Count; iAcct++)
			{
				for (int i = 0; i < list.Count; i++)
				{
					GroupItem item = (GroupItem)list[i];
					if (item.m_portfolio == localAccts[iAcct].ToString())
					{
						this.m_logger.logDebug("   " + item.m_portfolio + ", " + item.m_group);
						ArrayList @out = new ArrayList();
						prox.GetBODPositions(item.m_group, item.m_portfolio, item.m_traderAcronym, @out);
						results.AddRange(@out);
						//break;
					}
				}
			}

			prox.Dispose();
			prox = null;
		}
		else
		{
			for (int iAcct = 0; iAcct < localAccts.Count; iAcct++)
			{
				for (int i = 0; i < list.Count; i++)
				{
					GroupItem item = (GroupItem)list[i];
					if (item.m_portfolio == localAccts[iAcct].ToString())
					{
						this.m_logger.logDebug("   " + item.m_portfolio + ", " + item.m_group);
						ArrayList @out = new ArrayList();
						this.m_proxy2.GetBODPositions(item.m_group, item.m_portfolio, item.m_traderAcronym, @out);
						results.AddRange(@out);
						//break;
					}
				}
			}
		}

		//this.m_proxy2.Dispose();
		//this.m_proxy2 = null;

			//for (int i = 0; i < list.Count; i++)
			//{
			//    GroupItem item = (GroupItem)list[i];
			//    this.m_logger.logDebug("   " + item.m_portfolio + ", " + item.m_group);
			//    if (localAccts.Contains(item.m_portfolio))
			//    {
			//        ArrayList @out = new ArrayList();
			//        this.m_proxy.GetBODPositions(item.m_group, item.m_portfolio, @out);
			//        results.AddRange(@out);
			//        break;
			//    }
			//}
		return results.Count;
	}

    public int GetTrades(string lname, ArrayList traders, ArrayList results)
    {
        this.GetOrCreateLogin(lname);
        ArrayList list = new ArrayList(this.m_groupTable.Values);
        for (int i = 0; i < list.Count; i++)
        {
            GroupItem item = (GroupItem)list[i];
            if (traders.Contains(item.m_traderAcronym))
            {
                ArrayList @out = new ArrayList();
                this.m_proxy.GetTrades(lname, item.m_group, item.m_portfolio, item.m_traderAcronym, @out);
                results.AddRange(@out);
            }
        }
        return results.Count;
    }

    public void ParameterizedTposFunction(object mp)
    {
        TposParameters parameters = (TposParameters)mp;
        parameters.tposDelegate(parameters.key, parameters.result);
    }

    public int RtrvTrades(string lname, ArrayList traders, bool initFlag, ArrayList results)
    {
        RocLoginItem orCreateLogin = this.GetOrCreateLogin(lname);
        if (initFlag)
        {
            orCreateLogin.m_actives = traders;
            orCreateLogin.m_lasttime = orCreateLogin.m_lasttime.AddHours((double)-orCreateLogin.m_lasttime.Hour);
            orCreateLogin.m_lasttime = orCreateLogin.m_lasttime.AddMinutes((double)-orCreateLogin.m_lasttime.Minute);
            orCreateLogin.m_lasttime = orCreateLogin.m_lasttime.AddSeconds((double)-orCreateLogin.m_lasttime.Second);
        }
        string bTime = orCreateLogin.m_lasttime.ToString("HH:mm:ss");
        string strB = bTime;
        string bDate = orCreateLogin.m_lasttime.ToString("yyyy-MM-dd");
        ArrayList list = new ArrayList(this.m_groupTable.Values);
        string str4 = "Rx" + lname + "-";
        for (int i = 0; i < list.Count; i++)
        {
            GroupItem item2 = (GroupItem)list[i];
            if (traders.Contains(item2.m_traderAcronym))
            {
                ArrayList @out = new ArrayList();
                string str5 = this.m_proxy.RtrvTrades(lname, item2.m_group, item2.m_portfolio, item2.m_traderAcronym, bDate, bTime, @out);
                if (str5.CompareTo(strB) > 0)
                {
                    strB = str5;
                }
                for (int j = 0; j < @out.Count; j++)
                {
                    TposTrade trade = (TposTrade)@out[j];
                    if (!trade.m_ExtTradeID.StartsWith(str4))
                    {
                        results.Add(trade);
                    }
                }
            }
        }
        if (results.Count > 0)
        {
            DateTime time = DateTime.Parse(strB);
            if (((time.Hour != orCreateLogin.m_lasttime.Hour) || (time.Minute != orCreateLogin.m_lasttime.Minute)) || (time.Second != orCreateLogin.m_lasttime.Second))
            {
                time = time.AddYears(orCreateLogin.m_lasttime.Year - time.Year);
                time = time.AddMonths(orCreateLogin.m_lasttime.Month - time.Month);
                time = time.AddDays((double)(orCreateLogin.m_lasttime.Day - time.Day));
                orCreateLogin.m_lasttime = time;
            }
            else if (results.Count == orCreateLogin.m_lastCount)
            {
                results.Clear();
                orCreateLogin.m_lasttime = orCreateLogin.m_lasttime.AddSeconds(1.0);
            }
        }
        orCreateLogin.m_lastCount = results.Count;
        return results.Count;
    }

    public int SubscribeTrades(string lname, ArrayList traders)
    {
        RocLoginItem orCreateLogin = this.GetOrCreateLogin(lname);
        ArrayList actives = orCreateLogin.m_actives;
        for (int i = 0; i < actives.Count; i++)
        {
            string item = (string)actives[i];
            if (!traders.Contains(item))
            {
                ArrayList list2 = new ArrayList(this.m_groupTable.Values);
                for (int k = 0; k < list2.Count; k++)
                {
                    GroupItem item2 = (GroupItem)list2[k];
                    if (item2.m_traderAcronym == item)
                    {
                        item2.m_logins.Remove(orCreateLogin);
                        if (item2.m_logins.Count == 0)
                        {
                            this.m_proxy.UnsubscribeTrades(item2.m_group, item2.m_portfolio, item2.m_traderAcronym);
                        }
                    }
                }
            }
        }
        for (int j = 0; j < traders.Count; j++)
        {
            string trader = (string)traders[j];
            if (!actives.Contains(traders[j]))
            {
                ArrayList list3 = new ArrayList(this.m_groupTable.Values);
                for (int m = 0; m < list3.Count; m++)
                {
                    GroupItem gi = (GroupItem)list3[m];
                    if (gi.m_traderAcronym == trader)
                    {
                        gi.m_logins.Add(orCreateLogin);
                        if (gi.m_logins.Count == 1)
                        {
                            gi.m_tcb = new TradeCallback(gi, traders);
                            this.m_proxy.SubscribeTrades(gi.m_group, gi.m_portfolio, trader, gi.m_tcb);
                        }
                    }
                }
            }
        }
        orCreateLogin.m_actives = traders;
        return traders.Count;
    }

    private int TposFunction(TposDelegate tposDelegate, string key, ArrayList result)
    {
        TposClient client;
        int oK = RDSStatusCode.OK;
        Monitor.Enter(client = this);
        try
        {
            result.Clear();
            if (this.m_timeout == 0)
            {
                tposDelegate(key, result);
                return oK;
            }
            this.m_worker = new Thread(new ParameterizedThreadStart(this.ParameterizedTposFunction));
            TposParameters parameter = new TposParameters();
            parameter.tposDelegate = tposDelegate;
            parameter.key = key;
            parameter.result = result;
            this.m_worker.Start(parameter);
            this.m_worker.Join(this.m_timeout);
            if (this.m_worker.ThreadState != ThreadState.Stopped)
            {
                oK = RDSStatusCode.TPOS_TIMEOUT;
            }
        }
        catch (Exception exception)
        {
            Logger.instance().logError("Exception caught: " + exception.ToString());
        }
        finally
        {
            Monitor.Exit(client);
        }
        return oK;
    }

    // Nested Types
    private delegate int TposDelegate(string key, ArrayList result);

    [StructLayout(LayoutKind.Sequential)]
    private struct TposParameters
    {
        public TposClient.TposDelegate tposDelegate;
        public string key;
        public ArrayList result;
    }
}
