using System;
using System.Collections.Generic;

using MarketDataEx;

namespace LBMEx
{
	public enum LBMDataChangedEventTypes
	{
		OnInitializeFailed,
		OnInitialized,
		OnInitializeError,
		OnStop,

		OnSubscriptionFailed,
		OnSubscriptionError,
		OnSubscriptionGap,
		OnSubscriptionMsg,
		OnSubscriptionQuality,
		OnSubscriptionRecapRequest,
		OnSubscribed,
		OnUnsubscribed,

		// Mamda Book
		OnBookClear,
		OnBookDelta,
		OnBookGap,
		OnBookRecap,

		OnExecption,
		OnWarring,
	}

	public class LBMDataChangedEventArgs : EventArgs
	{
		private LBMDataChangedEventTypes _type;
		public LBMDataChangedEventTypes Type
		{
			get
			{
				return _type;
			}
		}

		private string _msg;
		public string Message
		{
			get
			{
				return _msg;
			}
		}

		private MDServerToClient _change = new MDServerToClient();
		public MDServerToClient Change
		{
			get
			{
				return _change;
			}
		}

		public LBMDataChangedEventArgs(LBMDataChangedEventTypes type)
		{
			_type = type;
		}
		public LBMDataChangedEventArgs(LBMDataChangedEventTypes type, string msg)
		{
			_type = type;
			_msg = msg;
		}
		public LBMDataChangedEventArgs(LBMDataChangedEventTypes type, MDServerToClient change)
		{
			_type = type;
			_change = change;
		}
	}
}
