using System;

namespace ROC
{
	public abstract class HelperBaseAlerts
	{
		private DateTime _alertTime;
		public DateTime AlertTime
		{
			get
			{
				return _alertTime;
			}
		}

		private string _alertMsg;
		public string AlertMsg
		{
			get
			{
				return _alertMsg;
			}
		}

		public HelperBaseAlerts(DateTime alertTime, string alertMsg)
		{
			_alertTime = alertTime;
			_alertMsg = alertMsg;
		}
	}
}
