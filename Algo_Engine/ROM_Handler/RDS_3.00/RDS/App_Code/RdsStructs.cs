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
using System.Collections.Generic;
using System.Diagnostics;

public class RDSStatusCode
{
	public const int OK = 0;
	public const int INVALID_USER = 1;
	public const int INTERNAL_ERROR = 2;
	public const int TPOS_TIMEOUT = 3;
}

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
	public string maxFloor;
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

public class UntypedOrder
{
	public ArrayList values;

	public UntypedOrder()
	{
		values = new ArrayList();
	}
}

public class AcctDestination
{
	public int destID;
	public string secType;
	public string shortName;
	public string giveupFirmID;
	public string cmtaFirmID;
	public string algoFlag;  //Cancel_Replace_Flag
}

public class AcctProfile
{
	public int interfaceID;
	public string account;
	public string type;
	public string clearingAcID;
	public string localAcAcrn;
	public string firmAcr;
	public string omAcctType;
	public string capacity;
	public string clearingFirmID;
	public string occAcr;
	public string homeExchange;

	public ArrayList accountDestinations;

	public AcctProfile()
	{
		accountDestinations = new ArrayList();
	}
}

public class TraderAcr
{
	public string tradeFor;
	public string localAcAcrn;
	public ArrayList acctProfiles;

	public TraderAcr()
	{
		acctProfiles = new ArrayList();
	}
}

public class CachedUsers
{
	public string username;
	public ArrayList userAccts;

	public CachedUsers()
	{
		userAccts = new ArrayList();
	}
}