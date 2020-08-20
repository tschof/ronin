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

/// <summary>
/// Summary description for TposPositions
/// </summary>
public class RocLoginItem
{
    // Fields
    public ArrayList m_actives;
    public int m_lastCount;
    public DateTime m_lasttime;
    public Queue m_msgQueue;
    private string m_user;

    // Methods
    public RocLoginItem(string name)
    {
        this.m_user = name;
        this.m_msgQueue = new Queue();
        this.m_actives = new ArrayList();
        this.m_lasttime = DateTime.Today;
        this.m_lasttime.AddHours((double)-this.m_lasttime.Hour);
        this.m_lasttime.AddMinutes((double)-this.m_lasttime.Minute);
        this.m_lasttime.AddSeconds((double)-this.m_lasttime.Second);
        this.m_lastCount = 0;
    }

    public void AddTrade(TposTrade trade)
    {
        this.m_msgQueue.Enqueue(trade);
    }
}

public class GroupItem
{
    public string m_group;
    public string m_portfolio;
    public string m_traderAcronym;
    public ArrayList m_logins;       // list of LoginItem
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
