using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public class HelperAlerts : HelperBaseAlerts
	{
		public HelperAlerts(DateTime alertTime, string alertMsg)
			: base(alertTime, alertMsg)
		{
		}
	}
}
