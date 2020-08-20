using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

/// <summary>
/// Summary description for SymbolMap
/// </summary>
public class SymbolMap
{
	public SymbolMap()
	{
	}

	private DataSet _symbolDef;	

	public void Read()
	{
		string source = "";

		source = ConfigurationManager.AppSettings["CMEFuturesDef"];
	}
}
