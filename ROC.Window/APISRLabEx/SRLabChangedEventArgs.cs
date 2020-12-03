using System;
using System.Collections.Generic;
using System.Text;
using MarketDataEx;

namespace APISRLabEx
{
	public delegate void SRLabChangedEventHandler(object sender, SRLabChangedEventArgs e);

	public enum SRLabDataChangedEventTypes
	{
		None,
		OnMsg,

		OnInitialized,
		OnEnvironmentVariableChecked,
		OnStopped,

		OnException,

		OnChange,
	}

	public class SRLabChangedEventArgs : EventArgs
	{
		private object _syncObject = new object();

		private SRLabDataChangedEventTypes _type = SRLabDataChangedEventTypes.None;
		public SRLabDataChangedEventTypes Type
		{
			get
			{
				return _type;
			}
		}

		private string _msg = "";
		public string Msg
		{
			get
			{
				return _msg;
			}
		}

		private string _stackTrace = "";
		public string StackTrace
		{
			get
			{
				return _stackTrace;
			}
		}

		private MDServerToClient _change;
		public MDServerToClient Changes
		{
			get
			{
				if (_change == null)
				{
					_change = new MDServerToClient();
				}
				return _change;
			}
		}

		public SRLabChangedEventArgs(Exception ex)
		{
			_type = SRLabDataChangedEventTypes.OnException;
			_msg = ex.Source;
			_stackTrace = ex.StackTrace;
		}
		public SRLabChangedEventArgs(MDServerToClient change)
		{
			_type = SRLabDataChangedEventTypes.OnChange;
			_change = change;
		}

		public SRLabChangedEventArgs(string msg)
		{
			_type = SRLabDataChangedEventTypes.OnMsg;
			_msg = msg;
		}
		public SRLabChangedEventArgs(SRLabDataChangedEventTypes type, string msg)
		{
			_type = type;
			_msg = msg;
		}
	}
}
