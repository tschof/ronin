using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Data;
using MarketDataEx;

namespace APISRLabEx
{
	public class SRLabWrapper
	{
		#region - API Calls -

		private delegate void CallBackOnStatusMessage(string msg);

		private delegate void CallBackOnSRLabMessage(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabBookWithOrderReplace(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabBook(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabOrderReplace(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabOrder(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabQuote(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabTrade(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabImbalance(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabBestPrice(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabCustomEvent(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabSecurityStatusChange(string msg, ManagedTypedMap delta);
		private delegate void CallBackOnSRLabSubscriptionInvalid(string msg, ManagedTypedMap delta);

		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnStatusMessage(CallBackOnStatusMessage msg);

		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabMessage(CallBackOnSRLabMessage data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabBookWithOrderReplace(CallBackOnSRLabBookWithOrderReplace data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabBook(CallBackOnSRLabBook data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabOrderReplace(CallBackOnSRLabOrderReplace data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabOrder(CallBackOnSRLabOrder data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabQuote(CallBackOnSRLabQuote data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabTrade(CallBackOnSRLabTrade data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabImbalance(CallBackOnSRLabImbalance data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabBestPrice(CallBackOnSRLabBestPrice data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabCustomEvent(CallBackOnSRLabCustomEvent data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabSecurityStatusChange(CallBackOnSRLabSecurityStatusChange data);
		[DllImport("APISRLab.dll")]
		private unsafe static extern void ReturnSRLabSubscriptionInvalid(CallBackOnSRLabSubscriptionInvalid data);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Initialize(string smNames, string smType, string configlocation, string configFile, int depth);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Version();

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr StartSessionManager(string smName);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Start();

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Stop();

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Shutdown();

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Subscribe(string symbol, string source, bool reload);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr SubscribeL2(string symbol, string source, bool reload);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr SubscribeBook(string symbol, string source, bool reload);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Unsubscribe(string symbol);

		#endregion

		#region - Callbacks -

		private static CallBackOnStatusMessage srlabStatusMessageCB = new CallBackOnStatusMessage(decodeMessage);
		private static CallBackOnSRLabMessage srlabMessageCB = new CallBackOnSRLabMessage(decodeData);
		private static CallBackOnSRLabBookWithOrderReplace srlabBookWithOrderReplaceCB = new CallBackOnSRLabBookWithOrderReplace(decodeData);
		private static CallBackOnSRLabBook srlabBookCB = new CallBackOnSRLabBook(decodeData);
		private static CallBackOnSRLabOrderReplace srlabOrderReplaceCB = new CallBackOnSRLabOrderReplace(decodeData);
		private static CallBackOnSRLabOrder srlabOrderCB = new CallBackOnSRLabOrder(decodeData);
		private static CallBackOnSRLabQuote srlabQuoteCB = new CallBackOnSRLabQuote(decodeData);
		private static CallBackOnSRLabTrade srlabTradeCB = new CallBackOnSRLabTrade(decodeData);
		private static CallBackOnSRLabImbalance srlabImbalanceCB = new CallBackOnSRLabImbalance(decodeData);
		private static CallBackOnSRLabBestPrice srlabBestPriceCB = new CallBackOnSRLabBestPrice(decodeData);
		private static CallBackOnSRLabCustomEvent srlabCustomEventCB = new CallBackOnSRLabCustomEvent(decodeData);
		private static CallBackOnSRLabSecurityStatusChange srlabSecurityStatusChangeCB = new CallBackOnSRLabSecurityStatusChange(decodeData);
		private static CallBackOnSRLabSubscriptionInvalid srlabSubscriptionInvalidCB = new CallBackOnSRLabSubscriptionInvalid(decodeData);

		#endregion

		#region - Local Methods -

		private static bool _stopping = false;
		private static object _syncObject = new object();

		// <uSubscriptionKey, delta>
		private static Dictionary<string, ManagedTypedMap> _deltas;
		private static Dictionary<string, ManagedTypedMap> Deltas
		{
			get
			{
				if (_deltas == null)
				{
					_deltas = new Dictionary<string, ManagedTypedMap>();
				}
				return _deltas;
			}
			set
			{
				_deltas = value;
			}
		}

		private static void decodeData(string msg, ManagedTypedMap delta)
		{
			try
			{
				if (msg != null && msg != "")
				{
					lock (_syncObject)
					{
						Messages.Add(msg);
					}
				}

				if (delta != null && delta.uSubscriptionKey != "")
				{
					lock (_syncObject)
					{
						if (!Deltas.ContainsKey(delta.uSubscriptionKey))
						{
							Deltas.Add(delta.uSubscriptionKey, new ManagedTypedMap());
						}
						Deltas[delta.uSubscriptionKey].Update(delta);
					}
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.Print(ex.ToString());
			}
		}

		private static List<string> _messages;
		private static List<string> Messages
		{
			get
			{
				if (_messages == null)
				{
					_messages = new List<string>();
				}
				return _messages;
			}
			set
			{
				_messages = value;
			}
		}

		private static void decodeMessage(string msg)
		{
			try
			{
				if (msg != null && msg != "")
				{
					lock (_syncObject)
					{
						Messages.Add(msg);
					}
				}
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.Print(ex.ToString());
			}
		}

		private string _startupPath = "";
		private string _smNames = "CTA,NASDAQ,CME,CME-CBOT,CME-MGEX,CME-NYMEX,ICE,OPRANBBO";
		private string _smType = "MDRCClientSessionMgr";
		private string _configlocation = @"..\SRLabConfig\";
		private string _configFile = "ronin-mc-client-allfeeds.xml";
		private int _depth = 10;

		#endregion

		#region - Constructor -

		public SRLabWrapper()
		{
		}
		public SRLabWrapper(
			string startupPath,
			string smNames, 
			string smType, 
			string configlocation, 
			string configFile, 
			int depth)
		{
			_startupPath = startupPath;
			_smNames = smNames;
			_smType = smType;
			_configlocation = configlocation;
			_configFile = configFile;
			_depth = depth;
		}

		#endregion

		#region - Public Methods -

		public void Setup()
		{
			ReturnStatusMessage(srlabStatusMessageCB);

			ReturnSRLabMessage(srlabMessageCB);
			ReturnSRLabBookWithOrderReplace(srlabBookWithOrderReplaceCB);
			ReturnSRLabBook(srlabBookCB);
			ReturnSRLabOrderReplace(srlabOrderReplaceCB);
			ReturnSRLabOrder(srlabOrderCB);
			ReturnSRLabQuote(srlabQuoteCB);
			ReturnSRLabTrade(srlabTradeCB);
			ReturnSRLabImbalance(srlabImbalanceCB);
			ReturnSRLabBestPrice(srlabBestPriceCB);
			ReturnSRLabCustomEvent(srlabCustomEventCB);
			ReturnSRLabSecurityStatusChange(srlabSecurityStatusChangeCB);
			ReturnSRLabSubscriptionInvalid(srlabSubscriptionInvalidCB);

			try
			{
				Initialize(
					_smNames,
					_smType,
					_configlocation,
					_configFile,
					_depth);

				Version();

				StartSessionManager("CTA");
				StartSessionManager("NASDAQ");
				StartSessionManager("CME");
				StartSessionManager("CME-CBOT");
				StartSessionManager("CME-MGEX");
				StartSessionManager("CME-NYMEX");
				StartSessionManager("ICE");
				StartSessionManager("OPRA");

				SRLabStart();

				SRLabSubscribe("C.N", "CTA", false);
				SRLabSubscribe("GE", "CTA", false);
				SRLabSubscribe("ESM0", "CME", false);
				SRLabSubscribe("YMM0", "CME", false);
				SRLabSubscribe("NQM0", "CME", false);
				SRLabSubscribe("IBM", "CTA", false);
				SRLabSubscribe("MSFT", "NASDAQ", false);
				SRLabSubscribe("GOOG", "NASDAQ", false);
				SRLabSubscribe("RTAH0", "ICE", false);
			}
			catch (Exception ex)
			{
				System.Diagnostics.Debug.Print(ex.Message);
			}
		}

		public void SRLabStart()
		{
			StartProcessDelta();

			Start();
		}

		public void SRLabStop()
		{
			_stopping = true;

			StopProcessDelta();

			Stop();
		}

		public void SRLabShutdown()
		{
			_stopping = true;

			StopProcessDelta();

			Shutdown();
		}

		public void SRLabSubscribe(string symbol, string source)
		{
			Subscribe(symbol, source, false);
		}
		public void SRLabSubscribe(string symbol, string source, bool reload)
		{
			Subscribe(symbol, source, reload);
		}

		public void SRLabUnsubscribe(string symbol)
		{
			Unsubscribe(symbol);
		}

		#endregion

		#region - Public Events -

		public event SRLabChangedEventHandler SRLabChanged;
		private void OnSRLabChanged(SRLabChangedEventArgs e)
		{
			if (SRLabChanged != null)
			{
				SRLabChanged(this, e);
			}
		}

		#endregion

		#region - Notification Thread -

		private bool _threadProcessDeltaShouldStop = false;
		private bool _threadProcessDeltaStarted = false;
		private System.Threading.Thread _threadProcessDelta;

		private void StartProcessDelta()
		{
			if (!_threadProcessDeltaStarted && !_stopping)
			{
				_threadProcessDeltaShouldStop = false;

				_threadProcessDelta = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessDeltas));
				_threadProcessDelta.Name = "MDSProcessDeltas";
				_threadProcessDelta.Start();

				while (!_threadProcessDelta.IsAlive) ;
				_threadProcessDeltaStarted = true;
			}
		}

		private void ProcessDeltas()
		{
			while (!_threadProcessDeltaShouldStop)
			{
				Dictionary<string, ManagedTypedMap> locDeltas = null;

				lock (_syncObject)
				{
					if (Deltas.Count > 0)
					{
						// Make a Local Copy
						locDeltas = new Dictionary<string, ManagedTypedMap>(Deltas);
						Deltas.Clear();
					}
				}

				if (locDeltas != null)
				{
					MDServerToClient locDelta = new MDServerToClient();
					foreach (string key in locDeltas.Keys)
					{
						locDelta = new MDServerToClient();

						#region - Convert From ManagedTypedMap To MDServerToClient -

						if (locDeltas[key].Doubles.Count > 0)
						{
							locDelta.Update(locDeltas[key].Doubles);
						}

						if (locDeltas[key].Longs.Count > 0)
						{
							locDelta.Update(locDeltas[key].Longs);
						}

						if (locDeltas[key].Strings.Count > 0)
						{
							locDelta.Update(locDeltas[key].Strings);
						}

						if (locDeltas[key].AskBook.Count > 0)
						{
							locDelta.UpdateAsk(locDeltas[key].AskBook);
						}

						if (locDeltas[key].BidBook.Count > 0)
						{
							locDelta.UpdateBid(locDeltas[key].BidBook);
						}

						#endregion

						OnSRLabChanged(new SRLabChangedEventArgs(locDelta));
					}
				}
				else
				{
					System.Threading.Thread.Sleep(0);
				}
			}

			_threadProcessDeltaStarted = false;
		}

		private void StopProcessDelta()
		{
			_threadProcessDeltaShouldStop = true;
			if (_threadProcessDelta != null)
			{
				while (_threadProcessDelta.IsAlive) ;
			}
		}

		#endregion
	}
}
