/*********************************************************************************
 *
 *  Copyright (c) 2006 by RONIN CAPITAL, LLC
 *   All Rights Reserved.
 *
 *********************************************************************************
 *
 *    Interface to TposClient
 *
 * History
 *
 * date     user       comment
 * ------- --------    -----------------------------------------------------------
 * 6/18/06  wxu        Initial version
 * 7/26/06  wxu        Add Timer
 *********************************************************************************/

using System;
using System.Collections;
using System.Configuration;
using System.Runtime.InteropServices;
using System.Threading;
using TposWrapper;

public class TposClient
{
    TposProxy           m_proxy;
    private int         m_timeout;
    private Thread      m_worker;

    Hashtable m_loginTable;       // by login user
    Hashtable m_groupTable;       // by trade group, portfolio and acct
    ArrayList m_allTraders;

    delegate int TposDelegate(string key, ArrayList result);

    struct TposParameters
    {
        public TposDelegate tposDelegate;
        public string key;
        public ArrayList result;
    }

    public TposClient()
    {
        string tposserver = ConfigurationManager.AppSettings["tposserver"];
        string tposuser = ConfigurationManager.AppSettings["tposuser"];
        string tpospass = ConfigurationManager.AppSettings["tpospass"];
        string tposport = ConfigurationManager.AppSettings["tposport"];

        try {
            m_timeout = 1000 * int.Parse(ConfigurationManager.AppSettings["timeout"]);
        } 
        catch (Exception)
        {
            m_timeout = 5;
        }
        m_proxy = new TposProxy(tposuser, tpospass, tposserver, int.Parse(tposport));
        m_worker = null;

    }

    public int Connect()
    {
        int ret = m_proxy.Connect();
        //
        m_loginTable = new Hashtable();
        m_groupTable = new Hashtable();
        m_allTraders = new ArrayList();
        
        m_proxy.GetTraderAccounts(m_allTraders);
        
        for (int i = 0; i < m_allTraders.Count; i++)
        {
            TposTraderAccount tacct = (TposTraderAccount)m_allTraders[i];
            string key = tacct.m_TradingGroup + "." + tacct.m_Portfolio + "." + tacct.m_TraderAcronym;
            if (!m_groupTable.Contains(key))
            {
                GroupItem gi = new GroupItem(tacct.m_TradingGroup, tacct.m_Portfolio, tacct.m_TraderAcronym);
                m_groupTable.Add(key, gi);
            }
        }
        
        return ret;
    }

    public void  ParameterizedTposFunction(object mp)
    {
        TposParameters param = (TposParameters)mp;
        param.tposDelegate(param.key, param.result);
        // Thread.Sleep(2000);
    }

    int TposFunction(TposDelegate tposDelegate, string key, ArrayList result)
    {
        int ret = RDSStatusCode.OK;
        lock (this)
        {
            try
            {
                result.Clear();
                if (m_timeout == 0)
                    tposDelegate(key, result);
                else
                {
                    m_worker = new Thread(new ParameterizedThreadStart(ParameterizedTposFunction));
                    TposParameters mp = new TposParameters();
                    mp.tposDelegate = tposDelegate;
                    mp.key = key;
                    mp.result = result;
                    m_worker.Start(mp);
                    m_worker.Join(m_timeout);
                    if (m_worker.ThreadState != ThreadState.Stopped)
                        ret = RDSStatusCode.TPOS_TIMEOUT;
                }
            } 
            catch(Exception ex)
            {
                Logger.instance().logError("Exception caught: " + ex.ToString());
            }
        }
        return ret;
    }

    public int BuildOptionChain(string underlying, ArrayList result)
    {
        return TposFunction(m_proxy.GetOptionChain, underlying, result);
    }

    public int BuildSSFutureChain(string underlying, ArrayList result)
    {
        if (!underlying.EndsWith("1C"))
            underlying += "1C";
        return TposFunction(m_proxy.GetSSFutureChain, underlying, result);
    }

    public int BuildFutureChainByBase(string baseSymbol, ArrayList result)
    {
        return TposFunction(m_proxy.GetFutureChainByBase, baseSymbol, result);
    }

    public int BuildFutureChain(string underlying, ArrayList result)
    {
        return TposFunction(m_proxy.GetFutureChain, underlying, result);
    }

    // public class TposPositions
    // 
    public RocLoginItem GetOrCreateLogin(string lname)
    {
        RocLoginItem ri = null;
        if (m_loginTable.Contains(lname))
            ri = (RocLoginItem)m_loginTable[lname];
        else
        {
            ri = new RocLoginItem(lname);
            m_loginTable.Add(lname, ri);
        }
        return ri;
    }

    public int SubscribeTrades(string lname, ArrayList traders)
    {
        RocLoginItem ri = GetOrCreateLogin(lname);

        // remove not in the old
        ArrayList oldtraders = ri.m_actives;
        for (int i = 0; i < oldtraders.Count; i++)
        {
            string trader = (string)oldtraders[i];
            if (traders.Contains(trader))
                continue;

            for (int j = 0; j < m_groupTable.Count; j++)
            {
                GroupItem gi = (GroupItem)m_groupTable[j];
                if (gi.m_traderAcronym == trader)
                {
                    gi.m_logins.Remove(ri);
                    if (gi.m_logins.Count == 0)
                        m_proxy.UnsubscribeTrades(gi.m_group, gi.m_portfolio, gi.m_traderAcronym);
                }
            }
        }

        // add new
        for (int i = 0; i < traders.Count; i++)
        {
            string trader = (string)traders[i];
            if (oldtraders.Contains(traders[i]))
                continue;

            for (int j = 0; j < m_groupTable.Count; j++)
            {
                GroupItem gi = (GroupItem)m_groupTable[j];
                if (gi.m_traderAcronym == trader)
                {
                    gi.m_logins.Add(ri);
                    if (gi.m_logins.Count == 1)
                    {
                        gi.m_tcb = new TradeCallback(gi, traders);
                        m_proxy.SubscribeTrades(gi.m_group, gi.m_portfolio, trader, gi.m_tcb);
                    }
                }
            }
        }
        ri.m_actives = traders;
        return traders.Count;
    }

    public int GetTrades(string lname, ArrayList traders, string dt, ArrayList results)
    {
        RocLoginItem ri = GetOrCreateLogin(lname);

        int ret = 0;
        ArrayList al = new ArrayList();
        for (int j = 0; j < m_groupTable.Count; j++)
        {
            GroupItem gi = (GroupItem)m_groupTable[j];

            al.Clear();
            m_proxy.GetTrades(gi.m_group, gi.m_portfolio, traders, dt, al);
            results.AddRange(al);
            ret += al.Count;
        }

        return ret;
    }

    public int GetPositions(string lname, ArrayList traders, string dt, ArrayList results)
    {
        RocLoginItem ri = GetOrCreateLogin(lname);

        int ret = 0;
        ArrayList al = new ArrayList();
        for (int j = 0; j < m_groupTable.Count; j++)
        {
            GroupItem gi = (GroupItem)m_groupTable[j];

            al.Clear();
            m_proxy.GetBODPositions(gi.m_group, gi.m_portfolio, traders, dt, al);
            results.AddRange(al);
            ret += al.Count;
        }

        return ret;
    }
}