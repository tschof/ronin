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

public static class RdsCacheProxy
{
	public static ArrayList ShowAllCachedUserProfile()
	{
		ArrayList users = null;
		
		if (RdsCache.RomUserAccounts.Count > 0)
		{
			CachedUsers cachedUser;
			users = new ArrayList();
			foreach (string key in RdsCache.RomUserAccounts.Keys)
			{
				cachedUser = new CachedUsers();
				cachedUser.username = key;
				cachedUser.userAccts = RdsCache.RomUserAccounts[key];
				users.Add(cachedUser);
			}
		}

		return users;
	}
}
