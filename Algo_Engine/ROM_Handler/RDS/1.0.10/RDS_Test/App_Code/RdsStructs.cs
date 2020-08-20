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

public class ReturnStatus
{
    public int statusCode;
    public string statusMessage;
    public object retValues;

    public ReturnStatus()
    {
        statusCode = RDSStatusCode.OK;
        statusMessage = "";
    }
}

public class AcctDestination
{
    public int destID;
    public string secType;
    public string shortName;
    public string giveupFirmID; // v 1.0.5
    public string cmtaFirmID;   // v 1.0.5
}

public class AcctProfile
{
    public int interfaceID;
    public string account;
    public string type;
    public string clearingAcID;
    public string localAcAcrn;
    public string firmAcr;
    public string omAcctType;             // v 1.0.5
    public string capacity;
    public string clearingFirmID;
    public string occAcr;                 // v 1.0.5
    public string homeExchange;           // v 1.0.5
    public ArrayList accountDestinations;	// Array of AcctDestination

    public AcctProfile()
    {
        accountDestinations = new ArrayList();
    }
}

public class TraderAcr
{
    public string tradeFor;
    public ArrayList acctProfiles;  // array of AcctProfile

    public TraderAcr()
    {
        acctProfiles = new ArrayList();
    }

    public virtual bool Equals(TraderAcr tf)
    {
        if (tf == null)
            return false;
        else
            return tradeFor.Equals(tf.tradeFor);
    }
}

public class Execution
{
    public string omTag;
    public string omExecTag;
    public DateTime execTime;
    public double execPrice;
    public double execQty;
}

public class Order
{
    public string omTag;
    public string tag;
    public DateTime omTime;
    public string owner;
    public string trader;
    public string tradeFor;
    public int destID;
    public string localAcct;
    public string firm;
    public string clearingAccount;
    public string clearingID;

    public int side;
    public int qty;
    public double price;
	public double avgPrice;
    public double stopPrice;
    public string secType;
    public string symbol;
    public string underlying;
    public double strikePrice;
    public string expDate;
    public string callPut;
    public string openClose;

    public string ex_short_name;
    public DateTime effectiveTime;

    public string instructions;
    public int maxFloor;
    public double multiplier;

    public int orderType;
    public int tif;
    public DateTime orderExpiresDate;
    public int originalShares;
    public int cumQty;
    public int leavesQty;

    public string text;
    public int status;
}

public class SymbolInfo
{
	public string symbol;
	public string secType;
	public double tickSize;
	public double displayFactor;
	public double contractSize;
	public string expirationDate;
}

