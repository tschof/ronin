using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public class HelperROC : HelperStatusBase
	{
		public void AddToException(Exception ex)
		{
			GLOBAL.HROC.AddToStatusLogs(ex.Message);
			GLOBAL.HROC.AddToStatusLogs(ex.StackTrace);
			GLOBAL.HROC.AddToAlerts(ex.Message);
		}

		public void AddToException(string msg)
		{
			GLOBAL.HROC.AddToStatusLogs(msg);
			GLOBAL.HROC.AddToAlerts(msg);
		}
	}
}
