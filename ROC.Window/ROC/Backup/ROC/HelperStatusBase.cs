using System;
using System.Collections.Generic;
using DateTimeEx;
using DataGridViewEx;

namespace ROC
{
	public abstract class HelperStatusBase
	{
		#region - Property -

		private object _syncObj = new object();
		public object SyncObj
		{
			get
			{
				return _syncObj;
			}
		}

		private DateTimeHP _dtHP = new DateTimeHP();

		private string _statusDsp = "";
		public string StatusDsp
		{
			get
			{
				return _statusDsp;
			}
			set
			{
				_statusDsp = value;
			}
		}

		private List<string> _statusLogs = new List<string>();
		public List<string> StatusLogs
		{
			get
			{
				return _statusLogs;
			}
			set
			{
				_statusLogs = value;
			}
		}

		private List<string> _performanceLogs;
		public List<string> PerformaceLogs
		{
			get
			{
				if (_performanceLogs == null)
				{
					_performanceLogs = new List<string>();
				}
				return _performanceLogs;
			}
			set
			{
				_performanceLogs = value;
			}
		}
		
		private List<AlertListData> _alertList = new List<AlertListData>();
		public List<AlertListData> AlertList
		{
			get
			{
				return _alertList;
			}
			set
			{
				_alertList = value;
			}
		}

		#endregion

		#region - Status Log -

		public void AddToStatusLogs(string statusMsg)
		{
			lock (SyncObj)
			{
				StatusLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", statusMsg }));
			}
		}

		public void LogStatusMessages()
		{
			string[] msgs = new string[0];
			lock (SyncObj)
			{
				if (StatusLogs.Count > 0)
				{
					msgs = StatusLogs.ToArray();
					StatusLogs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.HLog.ROC.Out(msg, false);
			}
		}

		#endregion

		#region - Performance Log -

		public void AddToPerformanceLog(string msg)
		{
			lock (SyncObj)
			{
				PerformaceLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", msg }));
			}
		}

		public void LogPerformanceMessages()
		{
			string[] msgs = new string[0];
			lock (SyncObj)
			{
				if (PerformaceLogs.Count > 0)
				{
					msgs = PerformaceLogs.ToArray();
					PerformaceLogs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.HLog.BMK.Out(msg, false);
			}
		}

		#endregion

		#region - Alerts -

		public void AddToAlerts(string alertMsg)
		{
			lock (SyncObj)
			{
				AlertList.Add(new AlertListData(_dtHP.Now, alertMsg));
			}
		}

		public void ShowAlerts()
		{
			if (AlertList.Count > 0)
			{
				AlertListData[] alerts = new AlertListData[0];
				lock (SyncObj)
				{
					alerts = AlertList.ToArray();
					AlertList.Clear();
				}

				GLOBAL.HWindows.OpenWindow(new frmAlert()).UpdateAlert(alerts);
			}
		}

		#endregion
	}
}
