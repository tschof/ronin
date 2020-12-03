using System;
using System.Collections.Generic;
using System.Text;
using DateTimeEx;

namespace SRLabMDS
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

		private List<string> _statusLogs;
		public List<string> StatusLogs
		{
			get
			{
				if (_statusLogs == null)
				{
					_statusLogs = new List<string>();
				}
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

		#endregion

		#region - Status Log -

		public void AddToExecption(Exception ex)
		{
			AddToStatusLogs(ex.Message);
			AddToStatusLogs(ex.StackTrace);
		}

		public void AddToStatusLogs(string statusMsg)
		{
			lock (SyncObj)
			{
				StatusLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("hh:mm:ss.ffffff"), " ", statusMsg }));
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
				GLOBAL.LOG.Log.Out(msg, false);
			}
		}

		#endregion

		#region - Performance Log -

		public void AddToPerformanceLog(string msg)
		{
			lock (SyncObj)
			{
				PerformaceLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("hh:mm:ss.ffffff"), " ", msg }));
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
				GLOBAL.LOG.BMK.Out(msg, false);
			}
		}

		#endregion
	}
}
