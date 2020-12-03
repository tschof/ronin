using System;
using DictionaryEx;
using CSVEx;
using SerializationEx;
using System.IO;
using System.Runtime.Serialization;
using System.Collections.Generic;

namespace MarketDataEx
{	
	[Serializable]
	public class MDClientToServer : MutiTypedDictionary
	{
		public MDClientToServer(SerializationInfo info, StreamingContext ctxt)
			: base(info, ctxt)
		{
		}

		public MDClientToServer()
		{
			Initialize("H");
		}
		public MDClientToServer(string username, string password)
		{
			Initialize("L", username, password);
		}
		public MDClientToServer(string symbol, bool reload)
		{
			Initialize("U", symbol, "", "", "", reload);
		}
		public MDClientToServer(string msgType, string symbol, string source, bool reload)
		{
			Initialize(msgType, symbol, source, "", CSVFieldIDs.SecutrityTypes.Equity, reload);
		}
		public MDClientToServer(string msgType, string symbol, string source, string secType, bool reload)
		{
			Initialize(msgType, symbol, source, "", secType, reload);
		}
		public MDClientToServer(string msgType, string symbol, string source, string subscriptionType, string secType, bool reload)
		{
			Initialize(msgType, symbol, source, subscriptionType, secType, reload);
		}

		private void Initialize(string msgType)
		{
			Update(MDInputFieldIDs.MsgType, msgType);
		}
		private void Initialize(string msgType, string username, string password)
		{
			Update(MDInputFieldIDs.MsgType, msgType);
			Update(MDInputFieldIDs.Username, username);
			Update(MDInputFieldIDs.Password, password);
		}
		private void Initialize(string msgType, string symbol, string source, string subscriptionType, string secType, bool reload)
		{
			Update(MDInputFieldIDs.MsgType, msgType);
			Update(MDInputFieldIDs.MDSymbol, symbol);
			Update(MDInputFieldIDs.MDDataSource, source);
			Update(MDInputFieldIDs.MDSubscriptionType, subscriptionType);
			Update(MDInputFieldIDs.SecType, secType);
			Update(MDInputFieldIDs.Reload, reload);
		}

		public string BaseSymbol
		{
			get
			{
				if (MDSymbol.Contains("."))
				{
					return MDSymbol.Substring(0, MDSymbol.IndexOf("."));
				}
				else
				{
					return MDSymbol;
				}
			}
		}

		public string MDSymbol
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.MDSymbol))
				{
					return Strings[MDInputFieldIDs.MDSymbol];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.MDSymbol, value);
			}
		}

		public string MDDataSource
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.MDDataSource))
				{
					return Strings[MDInputFieldIDs.MDDataSource];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.MDDataSource, value);
			}
		}

		public string MDKey
		{
			get
			{
				return string.Format("{0},{1}", new object[] { MDSymbol, MDDataSource });
			}
		}

		public string MDBaseKey
		{
			get
			{
				return string.Format("{0},{1}", new object[] { BaseSymbol, MDDataSource });
			}
		}

		public string MsgType
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.MsgType))
				{
					return Strings[MDInputFieldIDs.MsgType];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.MsgType, value);
			}
		}

		public string UserName
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.Username))
				{
					return Strings[MDInputFieldIDs.Username];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.Username, value);
			}
		}

		public string Password
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.Password))
				{
					return Strings[MDInputFieldIDs.Password];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.Password, value);
			}
		}

		public string SubscriptionType
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.MDSubscriptionType))
				{
					return Strings[MDInputFieldIDs.MDSubscriptionType];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.MDSubscriptionType, value);
			}
		}

		public string SecType
		{
			get
			{
				if (Strings.ContainsKey(MDInputFieldIDs.SecType))
				{
					return Strings[MDInputFieldIDs.SecType];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(MDInputFieldIDs.SecType, value);
			}
		}

		public bool Reload
		{
			get
			{
				if (Bools.ContainsKey(MDInputFieldIDs.Reload))
				{
					return Bools[MDInputFieldIDs.Reload];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(MDInputFieldIDs.Reload, value);
			}
		}

		public long FirstTime
		{
			get
			{
				if (Longs.ContainsKey(MDInputFieldIDs.FirstTime))
				{
					return Longs[MDInputFieldIDs.FirstTime];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(MDInputFieldIDs.FirstTime, value);
			}
		}
	}
}
