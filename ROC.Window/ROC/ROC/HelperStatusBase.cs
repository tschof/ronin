using System.Collections.Generic;
using DateTimeEx;
using DataGridViewEx;
using Log = Common.Log;

namespace ROC
{
	public abstract class HelperStatusBase
	{
		private DateTimeHP _dtHP = new DateTimeHP();

		internal string StatusDsp = "";
		private List<string> _statusLogs = new List<string>();
		private List<string> _perfLogs = new List<string>();

		public List<AlertListData> AlertList { get; private set; } = new List<AlertListData>();

		public void AddToStatusLogs(params object[] texts)
		{
			if ((texts != null) && (texts.GetLength(0) > 0)) {
				string message = _dtHP.Now.ToString("HH:mm:ss.fffffff") + " " + string.Join(" ", texts);
				lock (this) {
					_statusLogs.Add(message);
				}
			}
		}

		public void LogStatusMessages()
		{
			List<string> copy = null;

			lock (this)
			{
				if (_statusLogs.Count > 0)
				{
					copy = _statusLogs;
					_statusLogs = new List<string>();
				}
			}

			if ((copy != null) && (copy.Count > 0)) {
				foreach (string msg in copy)
					Log.Info(Log.Target.ROC, msg);
			}
		}

		public void AddToPerformanceLog(string text)
		{
			if (!string.IsNullOrEmpty(text)) {
				lock (this) {
					_perfLogs.Add(_dtHP.Now.ToString("HH:mm:ss.fffffff") + " " + text);
				}
			}
		}

		public void LogPerformanceMessages()
		{
			List<string> copy = null;

			lock (this)
			{
				if (_perfLogs.Count > 0)
				{
					copy = _perfLogs;
					_perfLogs = new List<string>();
				}
			}

			if ((copy != null) && (copy.Count > 0)) {
				foreach (string msg in copy)
					Log.Info(msg);
			}
		}

		public void AddToAlerts(string text)
		{
			if (!string.IsNullOrEmpty(text)) {
				lock (this) {
					AlertList.Add(new AlertListData(_dtHP.Now, text));
				}
			}
		}

		public void ShowAlerts()
		{
			List<AlertListData> copy = null;

			lock (this) {
				if (AlertList.Count > 0) {
					copy = AlertList;
					AlertList = new List<AlertListData>();
				}
			}

			if ((copy != null) && (copy.Count > 0))
				GLOBAL.HWindows.OpenWindow(new frmAlert()).UpdateAlert(copy);
		}
	}
}
