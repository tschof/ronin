using System;
using System.Data;
using System.Collections;
using System.Configuration;
using System.Web;
using System.Web.Security;
using TposWrapper;

public class RocLoginItem
{
    string             m_user;
    public Queue       m_msgQueue;
    public ArrayList   m_groups;      // An item of GroupItem
    public ArrayList   m_actives;     // trader acronym names

    public RocLoginItem(string name)
    {
        m_user = name;
        m_msgQueue = new Queue();
        m_groups = new ArrayList();
        m_actives = new ArrayList();
    }

    public void AddTrade(TposTrade trade)
    {
        m_msgQueue.Enqueue(trade);
    }
}


public class GroupItem
{
    public string       m_group;
    public string       m_portfolio;
    public string       m_traderAcronym;
    public ArrayList    m_logins;       // list of LoginItem
    public TradeCallback m_tcb;

    public GroupItem(string g, string p, string trader)
    {
        m_group = g;
        m_portfolio = p;
        m_traderAcronym = trader;
        m_logins = new ArrayList();
        m_tcb = null;
    }

    public void AddRocUser(RocLoginItem login)
    {
        m_logins.Add(login);
    }
}

/// <summary>
/// Summary description for TposPositions
/// </summary>
/// 
public class TradeCallback : TposTradeCallback
{
    GroupItem m_group;
    ArrayList m_traderAcronyms;

    public TradeCallback(GroupItem gi, ArrayList traders)
    {
        m_group = gi;
        m_traderAcronyms = traders;
    }

    public override void OnTrade(TposTrade trade)
    {
        for (int i = 0; i < m_group.m_logins.Count; i++)
        {
            RocLoginItem ri = (RocLoginItem)m_group.m_logins[i];
            string trader = trade.m_TraderAcronym;
            if (ri != null && m_traderAcronyms.Contains(trader))
                ri.AddTrade(trade);
        }
    }
}


