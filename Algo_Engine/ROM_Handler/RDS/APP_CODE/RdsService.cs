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
using System.Threading;
using System.Collections;
using System.ComponentModel;
using System.Data;

using System.Web;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.Configuration;
using System.Diagnostics;
using System.IO;
using TposWrapper;

[WebService(Namespace = "http://roncap.com/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
public class RdsService : System.Web.Services.WebService
{

    private static string       version;
    private static TposClient   tpos = null;
    private static RdsProxy     proxy = null;
    private static Logger       logger = null;
    private static Timer        timer4tpos = null;

    public RdsService () 
    {
        //Uncomment the following line if using designed components 
        //InitializeComponent(); 
    }

    static RdsService()
    {
        version = "1.0.7";
        logger = Logger.instance();
        logger.logInfo("RDS " + version + " service starts");

        proxy = new RdsProxy();

        logger.logDebug(">>>> Connect to TPOS");
        tpos = new TposClient();
        tpos.Connect();
        logger.logDebug("TPOS is connected");

        // Reconnect to TPOS everyday in the morning
        //
        DateTime now = DateTime.Now;
        TimeSpan ts;
        try
        {
            string param = ConfigurationManager.AppSettings["tposReconnect"];
            ts = TimeSpan.Parse(param);
        }
        catch (Exception ex)
        {
            logger.logError("Can't read 'tposReconnect' " + ex.ToString());
            ts = new TimeSpan(6, 20, 0);     // 6:20:00 am
        }
        Int64 interval = 24 * 3600;
        ts = new DateTime(now.Year, now.Month, now.Day, ts.Hours, ts.Minutes, ts.Seconds).Subtract(now);
        Int32 dueTime = (int)(ts.TotalSeconds + 24 * 3600) % (24 * 3600);
        timer4tpos = new System.Threading.Timer(new TimerCallback(tposReconnectTimerCallback), null, dueTime * 1000, interval * 1000);
        logger.logDebug("Reconnect every " + interval + " seconds " + dueTime + " seconds later from now");
    }

    private static void tposReconnectTimerCallback(object state)
    {
        logger.logDebug(">>>> Reconnect to TPOS");
        tpos = new TposClient();
        tpos.Connect();
        logger.logDebug("TPOS is connected");
    }

    [WebMethod(Description = "Get RDS web service version.", EnableSession = false)]
    public string GetVersion()
    {
        return version;
    }

    #region DatabaseAccess

    [WebMethod(Description = "Get ROC user's profile.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(TraderAcr))]
    [XmlInclude(typeof(AcctProfile))]
    [XmlInclude(typeof(AcctDestination))]
    public ReturnStatus GetUserProfile(string user, string pass)
    {
        logger.logDebug("GetUserProfile:" + user + "/" + pass);
        ReturnStatus ret = new ReturnStatus();
        if (!proxy.CheckAuthorization(user, pass))
        {
            ret.statusCode = RDSStatusCode.INVALID_USER;
            ret.statusMessage = "Invalid user name or password";
            logger.logDebug("GetUserProfile returns: " + ret.statusCode);
            return ret;
        }

        try
        {
            ret.retValues = proxy.BuildUserProfile(user);
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
            logger.logError("GetUserProfile: " + ret.statusCode + ":" + ret.statusMessage);
        }
        logger.logDebug("GetUserProfile returns: " + ret.statusCode);

        return ret;
    }

    [WebMethod(Description = "Get ROC executions.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(Execution))]
    public ReturnStatus GetUserExecutions(string user, string pass)
    {
        logger.logDebug("GetUserExecutions:" + user + "/" + pass);
        ReturnStatus ret = new ReturnStatus();
        if (!proxy.CheckAuthorization(user, pass))
        {
            ret.statusCode = RDSStatusCode.INVALID_USER;
            ret.statusMessage = "Invalid user name or password";
            logger.logDebug("GetUserExecutions returns: " + ret.statusCode);
            return ret;
        }

        try
        {
            ret.retValues = proxy.BuildUserExecutions(user);
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
            logger.logError("GetUserExecutions: " + ret.statusCode + ":" + ret.statusMessage);
        }

        logger.logDebug("GetUserExecutions returns: " + ret.statusCode);
        return ret;
    }

    [WebMethod(Description = "Get ROC orders.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(Order))]
    public ReturnStatus GetUserOrders(string user, string pass)
    {
        logger.logDebug("GetUserOrders:" + user + "/" + pass);
        ReturnStatus ret = new ReturnStatus();
        if (!proxy.CheckAuthorization(user, pass))
        {
            ret.statusCode = RDSStatusCode.INVALID_USER;
            ret.statusMessage = "Invalid user name or password";
            logger.logDebug("GetUserOrders returns: " + ret.statusCode);
            return ret;
        }

        try
        {
            ret.retValues = proxy.BuildUserOrders(user);
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
            logger.logError("GetUserOrders error: " + ret.statusCode + ":" + ret.statusMessage);
        }

        logger.logDebug("GetUserOrders returns: " + ret.statusCode);
        return ret;
    }


    [WebMethod(Description = "Change ROC user's password.", EnableSession = false)]
    public ReturnStatus SetUserPassword(string user, string oldpass, string newpass)
    {
        logger.logDebug("SetUserPassword:" + user + "/" + oldpass + "/" + newpass);

        ReturnStatus ret = new ReturnStatus();
        ret.retValues = false;

        if (!proxy.CheckAuthorization(user, oldpass))
        {
            ret.statusCode = RDSStatusCode.INVALID_USER;
            ret.statusMessage = "Invalid user name or password";
            logger.logDebug("SetUserPassword returns: " + ret.statusCode);
            return ret;
        }

        try
        {
            proxy.ChangePassword(user, oldpass, newpass);
            ret.retValues = true;
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
            logger.logError("SetUserPassword error: " + ret.statusCode + ":" + ret.statusMessage);
        }

        logger.logDebug("SetUserPassword returns: " + ret.statusCode);

        return ret;
    }
    #endregion

    #region TposConnection
    delegate int TposDelegate(string underlying, ArrayList result);

    ReturnStatus TposFunction(string name, string symbol, TposDelegate delegater)
    {
        logger.logDebug(name + ": " + symbol);
        ReturnStatus ret = new ReturnStatus();

        try
        {
            ret.retValues = new ArrayList();
            ret.statusCode = delegater(symbol, (ArrayList)ret.retValues);
            if (ret.statusCode == RDSStatusCode.TPOS_TIMEOUT)
                ret.statusMessage = "TPOS timeout";
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
        }

        if (ret.statusCode != 0)
            logger.logError(name + " error: " + ret.statusCode + ":" + ret.statusMessage);

        logger.logDebug(name + " returns: " + ret.statusCode);
        return ret;
    }

    [WebMethod(Description = "Get option chain by underlying symbol.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(OptionDesc))]
    public ReturnStatus GetOptionChain(string underlying)
    {
        return TposFunction("GetOptionChain", underlying, tpos.BuildOptionChain);
    }

    [WebMethod(Description = "Get Stock Future chain by underlying symbol.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(FutureDesc))]
    public ReturnStatus GetSSFutureChain(string underlying)
    {
        return TposFunction("GetSSFutureChain", underlying, tpos.BuildSSFutureChain);
    }

    [WebMethod(Description = "Get Future chain by base.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(FutureDesc))]
    public ReturnStatus GetFutureChainByBase(string baseSymbol)
    {
        return TposFunction("GetFutureChainByBase", baseSymbol, tpos.BuildFutureChainByBase);
    }

    [WebMethod(Description = "Get Future chain by underlying.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(FutureDesc))]
    public ReturnStatus GetFutureChain(string underlying)
    {
        return TposFunction("GetFutureChain", underlying, tpos.BuildFutureChain);
    }

    [WebMethod(Description = "Get Positions for user, clearingAccts.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(TposPosition))]
    public ReturnStatus GetPositions(string user, ArrayList traders, string dt)
    {
        logger.logDebug("GetPositions: " + user + ", traders: " + traders.ToString());
        ReturnStatus ret = new ReturnStatus();

        try
        {
            ret.retValues = new ArrayList();
            ret.statusCode = tpos.GetPositions(user, traders, dt, (ArrayList)ret.retValues); 

            // if (ret.statusCode == RDSStatusCode.TPOS_TIMEOUT)
            //    ret.statusMessage = "TPOS timeout";
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
        }

        if (ret.statusCode != 0)
            logger.logError("GetPositions error: " + ret.statusCode + ":" + ret.statusMessage);

        logger.logDebug("GetPositions returns: " + ret.statusCode);
        return ret;
    }

    [WebMethod(Description = "Get Positions for user and clearingAcct.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(TposPosition))]
    public ReturnStatus GetAcctPositions(string user, string trader, string dt)
    {
        logger.logDebug("GetAcctPositions: " + user + ", trader: " + trader);
        ReturnStatus ret = new ReturnStatus();
        ArrayList traders = new ArrayList();

        try
        {
            ret.retValues = new ArrayList();
            traders.Add(trader);
            ret.statusCode = tpos.GetPositions(user, traders, dt, (ArrayList)ret.retValues);

            // if (ret.statusCode == RDSStatusCode.TPOS_TIMEOUT)
            //    ret.statusMessage = "TPOS timeout";
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
        }

        if (ret.statusCode != 0)
            logger.logError("GetAcctPositions error: " + ret.statusCode + ":" + ret.statusMessage);

        logger.logDebug("GetAcctPositions returns: " + ret.statusCode);
        return ret;
    }

    [WebMethod(Description = "Get trades for user and traderAcronym.", EnableSession = false)]
    [XmlInclude(typeof(ReturnStatus))]
    [XmlInclude(typeof(TposTrade))]
    public ReturnStatus SubscribeTrades(string user, string trader)
    {
        logger.logDebug("SubscribeTrades: " + user + ", trader: " + trader);
        ReturnStatus ret = new ReturnStatus();

        try
        {
            ArrayList results = new ArrayList();
            RocLoginItem ri = tpos.GetOrCreateLogin(user);
            Queue q = ri.m_msgQueue;
            while (q.Count > 0)
            {
                TposTrade trd = (TposTrade)q.Dequeue();
                if ( trd.m_TraderAcronym.Equals(trader) && ! trd.m_ExtTradeID.StartsWith(user) )
                    results.Add(trd);
            }
            ret.retValues = results;
        }
        catch (Exception ex)
        {
            ret.statusCode = RDSStatusCode.INTERNAL_ERROR;
            ret.statusMessage = "Internal error: " + ex.ToString();
        }

        if (ret.statusCode != 0)
            logger.logError("SubscribeTrades error: " + ret.statusCode + ":" + ret.statusMessage);

        logger.logDebug("SubscribeTrades returns: " + ret.statusCode);
        return ret;
    }
    #endregion
}
