using System;
using System.Collections.Generic;
using System.Text;
using CSMD;
using MarketDataEx;

namespace SRLabEx
{
	public enum SRLabDataChangedEventTypes
	{
		OnMsg,

		OnInitialized,
		OnEnvironmentVariableChecked,
		OnStopped,

		OnException,
		OnMDException,

		OnChange,
	}

	public class SRLabChangedEventArgs : EventArgs
	{
		private SRLabDataChangedEventTypes _type;
		private string _msg;
		private string _stackTrace;
		private MDException _exMD;
		private Exception _ex;
		private MDServerToClient _change;
		
		public SRLabDataChangedEventTypes Type
		{
			get
			{
				return _type;
			}
		}
		public string Msg
		{
			get
			{
				return _msg;
			}
		}
		public string StackTrace
		{
			get
			{
				return _stackTrace;
			}
		}
		public MDException ExMD
		{
			get
			{
				return _exMD;
			}
		}
		public Exception Ex
		{
			get
			{
				return _ex;
			}
		}
		public MDServerToClient Change
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
			_ex = ex;
		}
		public SRLabChangedEventArgs(MDException ex)
		{
			_type = SRLabDataChangedEventTypes.OnMDException;
			_msg = ex.Source;
			_stackTrace = ex.StackTrace;
			_exMD = ex;
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
		public SRLabChangedEventArgs(MDServerToClient change)
		{
			_type = SRLabDataChangedEventTypes.OnChange;
			_change = change;
		}
	}
}
