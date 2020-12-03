using System;
using System.Diagnostics;
using System.Collections.Generic;

using Wombat;

using MarketDataEx;
using DateTimeEx;
using System.Collections.ObjectModel;

namespace LBMEx
{
	internal delegate void LBMChangedEventHandler(object sender, LBMDataChangedEventArgs e);

	internal class LBM : MamaDictionaryCallback, MamaSubscriptionCallback, MamdaOrderBookHandler, IDisposable
	{
		internal event LBMChangedEventHandler LBMChanged;
		internal virtual void OnLBMChanged(LBMDataChangedEventArgs e)
		{
			if (LBMChanged != null)
			{
				LBMChanged(this, e);
			}
		}

		#region - Local Variable -

		private DateTimeHP _dtHP;

		private string _middleware;
		private int _maxRetries;
		private uint _idelTimeBetweenRetries;
		private bool _enableMamaLog;
		private string _mamaLogFilePath;
		private string _transportName;
		private string _dictionarySourceSymbolNamespace;
		private int _throttleDefault;
		private int _throttleInitial;
		private int _throttleRecap;

		private double _dictionaryTimeOut = 10;
		private int _dictionaryRetries = 3;

		private double _subscriptionTimeOut = 15;
		private int _subscriptionRetries = 1;

		private MamaBridge _mamaBridge;
		private MamaTransport _mamaTransport;
		private MamaQueue _mamaDefaultQueue;

		private bool _gotDictionaries;
		private MamaSource _mamaDictionarySource;
		private MamaDictionary _mamaDictionary;

		private MamaMsgIterator _mainMsgIterator = null;

		private bool _running;

		// Key to different sources by string
		private Dictionary<string, MamaSource> _mamaSources = new Dictionary<string, MamaSource>();
		// Key to different symbols by string
		private Dictionary<string, MamaSubscription> _mamaSubscriptions = new Dictionary<string, MamaSubscription>();

		private object _syncObj = new object();

		private bool _useThreadedDecode;
		private bool _useChangeNotificationRate;
		private uint _changeNotificationRate;

		private bool _forceGC = false;

		#endregion

		#region - Thread Notify -

		private bool _threadNotifyShouldStop = false;
		private bool _threadNotifyStarted = false;
		private System.Threading.Thread _threadNotify;

		// Used by threaded decode
		private List<MamaMsg> _mamaMsgQueue = new List<MamaMsg>();

		MamaFieldDescriptor _issueSymbolDescriptor = new MamaFieldDescriptor(WombatFieldIDs.IssueSymbol, mamaFieldType.MAMA_FIELD_TYPE_STRING, "");
		
		#endregion

		#region - Constructor -

		public LBM(string middleware, int maxRetries, uint idelTimeBetweenRetries, bool enableMamaLog, string mamaLogFilePath, string transportName, string dictionarySourceSymbolNamespace, int throttleDefault, int throttleInitial, int throttleRecap, bool useThreadedDecode, int changeNotificationRate, bool useChangeNotificationRate)
		{
			_middleware = middleware;
			_maxRetries = maxRetries;
			_idelTimeBetweenRetries = idelTimeBetweenRetries;
			_enableMamaLog = enableMamaLog;
			_mamaLogFilePath = mamaLogFilePath;
			_transportName = transportName;
			_dictionarySourceSymbolNamespace = dictionarySourceSymbolNamespace;
			_throttleDefault = throttleDefault;
			_throttleInitial = throttleInitial;
			_throttleRecap = throttleRecap;

			_useThreadedDecode = useThreadedDecode;
			_changeNotificationRate = (uint)changeNotificationRate;
			_useChangeNotificationRate = useChangeNotificationRate;

			Initialize();
		}
		
		private void Initialize()
		{
			_dtHP = new DateTimeHP();

			_mamaBridge = null;
			_transportName = null;
			_mamaDefaultQueue = null;

			_gotDictionaries = false;
			_mamaDictionarySource = null;
			_mamaDictionary = null;

			_running = false;
		}

		#endregion

		#region - Public Method -

		public void Start(object info)
		{
			bool redo = false;
			int retries = 0;

			#region - Mama Bridge -

			do
			{
				try
				{
					_mamaBridge = new MamaBridge(_middleware);
					Mama.open();
					redo = false;
				}
				catch (Exception ex)
				{
					if (retries < _maxRetries)
					{
						API.Sleep(_idelTimeBetweenRetries);
						redo = true;
						++retries;
					}
					else
					{
						Debug.Assert(!Debugger.IsAttached, ex.Message, ex.StackTrace);
					}
				}
			}
			while (redo && retries < _maxRetries);

			#endregion

			#region - Mama Log -

			if (_enableMamaLog)
			{
				Mama.logToFile(_mamaLogFilePath + String.Format("ROC_{0:G}{1:G3}.log", DateTime.Today.Year, DateTime.Today.DayOfYear), MamaLogLevel.MAMA_LOG_LEVEL_FINEST);
			}

			#endregion

			#region - Mama Transport -

			_mamaTransport = new MamaTransport();
			_mamaTransport.create(_transportName, _mamaBridge);

			#endregion

			#region - Mama Queue -

			_mamaDefaultQueue = Mama.getDefaultEventQueue(_mamaBridge);

			#endregion

			#region - Mama Dictionary -

			_gotDictionaries = false;
			_mamaDictionarySource = new MamaSource();

			_mamaDictionarySource.symbolNamespace = _dictionarySourceSymbolNamespace;
			_mamaDictionarySource.transport = _mamaTransport;
			_mamaDictionary = new MamaDictionary();

			_mamaDictionary.create(_mamaDefaultQueue, this, _mamaDictionarySource, _dictionaryRetries, _dictionaryTimeOut);

			#endregion

			try
			{
				Mama.start(_mamaBridge);

				MamdaOrderBookFields.setDictionary(_mamaDictionary, null);
				if (!_gotDictionaries)
				{
					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeFailed));
				}
				else
				{
					// Set Iterator
					_mainMsgIterator = new MamaMsgIterator(_mamaDictionary);

					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitialized));

					_mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_DEFAULT, _throttleDefault);
					_mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_INITIAL, _throttleInitial);
					_mamaTransport.setOutboundThrottle(MamaTransport.MamaThrottleInstance.MAMA_THROTTLE_RECAP, _throttleRecap);

					_running = true;

					Mama.start(_mamaBridge);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(!Debugger.IsAttached, ex.Message, ex.StackTrace);
			}
		}

		public void SubscribeBook(string symbol, string source)
		{
		}

		public void SubscribeL2(string symbol, string source)
		{
		}

		public void Subscribe(string symbol, string source)
		{
			Subscribe(symbol, source, mamaSubscriptionType.MAMA_SUBSC_TYPE_NORMAL);
		}

		private void Subscribe(string symbol, string source, mamaSubscriptionType type)
		{
			if (_running)
			{
				MamaSource src;

				if (_mamaSources.ContainsKey(source))
				{
					src = _mamaSources[source];
				}
				else
				{
					src = new MamaSource();
					src.symbolNamespace = source;
					src.transport = _mamaTransport;

					// remember the source
					_mamaSources.Add(source, src);
				}

				MamaSubscription sub;
				if (_mamaSubscriptions.ContainsKey(symbol))
				{
					// Already subscribed
				}
				else
				{
					sub = new MamaSubscription();
					sub.setServiceLevel(mamaServiceLevel.MAMA_SERVICE_LEVEL_REAL_TIME);
					sub.setSubscriptionType(type);
					sub.setTimeout(_subscriptionTimeOut);
					sub.setRetries(_subscriptionRetries);
					sub.setRequiresInitial(true);

					sub.create(_mamaDefaultQueue, this, src, symbol);

					// remember the symbol
					_mamaSubscriptions.Add(symbol, sub);
				}

			}
			else
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionFailed, "Wombat Not Running " + symbol + ": " + source));
			}
		}

		public void Unsubscribe(string symbol)
		{
			if (_mamaSubscriptions.ContainsKey(symbol))
			{
				_mamaSubscriptions[symbol].destroy();
				_mamaSubscriptions.Remove(symbol);
			}
		}

		public void ForceGC()
		{
			_forceGC = true;
		}

		#endregion

		#region - Local Method -

		private void Stop()
		{
			if (_running)
			{				
				foreach (MamaSubscription sub in _mamaSubscriptions.Values)
				{
					sub.destroy();
				}
				_mamaSubscriptions = new Dictionary<string, MamaSubscription>();

				foreach (MamaSource scr in _mamaSources.Values)
				{
					scr.destroy();
				}
				_mamaSources = new Dictionary<string, MamaSource>();

				Mama.stop(_mamaBridge);

				StopNotify();
				
				Initialize();

				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnStop));

				MemClean();
			}
		}

		#endregion

		#region - MamaDictionaryCallback Members -

		public void onComplete(MamaDictionary dictionary)
		{
			_gotDictionaries = true;
			Mama.stop(_mamaBridge);
		}

		public void onError(MamaDictionary dictionary, string message)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeError, "MamaDictionary:Error " + message));
			Mama.stop(_mamaBridge);
		}

		public void onTimeout(MamaDictionary dictionary)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeError, "MamaDictionary:Timeout " + dictionary.ToString()));
			Mama.stop(_mamaBridge);
		}

		#endregion

		#region - MamaSubscriptionCallback Members -

		public void onCreate(MamaSubscription subscription)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscribed, String.Concat(new object[] { "MamaSubscription|onCreate ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionError, String.Concat(new object[] { "MamaSubscription|onError ", subscription.subscSymbol, " : ", subscription.subscSource, " Status: " + status.ToString(), " Subject: ", subject })));
		}

		public void onGap(MamaSubscription subscription)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionGap, String.Concat(new object[] { "MamaSubscription|onGap ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		public void onMsg(MamaSubscription subscription, MamaMsg msg)
		{
			MamaMsg locMsg = new MamaMsg(msg);
			locMsg.addString("SubscriptionSoruce", (ushort)WombatFieldIDs.SubscriptionSoruce, subscription.subscSymbol);
			locMsg.addString("SubscriptionSymbol", (ushort)WombatFieldIDs.SubscriptionSymbol, subscription.subscSource);

			if (_useThreadedDecode)
			{
				lock (_mamaMsgQueue)
				{
					_mamaMsgQueue.Add(locMsg);
				}

				StartNotify();
			}
			else
			{
				Dictionary<string, MDServerToClient> locChanges = new Dictionary<string, MDServerToClient>();
				InterateMsg(locMsg, ref locChanges);
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionMsg, ConvertToList(locChanges)));
			}

			msg.destroy();
			msg.Dispose();

			#region - GC -

			if (_forceGC)
			{
				MemClean();

				_forceGC = false;
			}

			#endregion
		}

		public void onQuality(MamaSubscription subscription, mamaQuality quality, string symbol)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionQuality, String.Concat(new object[] { "MamaSubscription|onQuality ", symbol, " : ", subscription.subscSource, " Quality: " + quality.ToString() })));
		}

		public void onRecapRequest(MamaSubscription subscription)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionRecapRequest, String.Concat(new object[] { "MamaSubscription|onRecapRequest ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		#endregion

		#region - Threaded Notify -

		private void StartNotify()
		{
			if (!_threadNotifyStarted)
			{
				_threadNotifyShouldStop = false;
				_threadNotify = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessMsgQueue));
				_threadNotify.Priority = System.Threading.ThreadPriority.BelowNormal;

				_threadNotify.Name = "LBM_ProcessMsgQueue";
				_threadNotify.Start();

				while (!_threadNotify.IsAlive) ;
				_threadNotifyStarted = true;
			}
		}

		private void ProcessMsgQueue()
		{
			while (!_threadNotifyShouldStop)
			{
				if (_mamaMsgQueue.Count > 0)
				{
					MamaMsg[] msgs = new MamaMsg[0];
					lock (_mamaMsgQueue)
					{
						msgs = _mamaMsgQueue.ToArray();
						_mamaMsgQueue.Clear();
					}

					Dictionary<string, MDServerToClient> locChanges = new Dictionary<string, MDServerToClient>();
					foreach (MamaMsg msg in msgs)
					{
						if (_threadNotifyShouldStop) break;
						InterateMsg(msg, ref locChanges);
					}

					OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionMsg, ConvertToList(locChanges)));
				}

				if (_useChangeNotificationRate)
				{
					API.Sleep(_changeNotificationRate);
				}
				else
				{
					API.Sleep(1);
				}
			}

			_threadNotifyStarted = false;
		}

		private void InterateMsg(MamaMsg msg, ref Dictionary<string, MDServerToClient> changes)
		{
			MamaMsgField field;
			int fid = 0;

			string symbol = "";
			if (msg.tryString(_issueSymbolDescriptor, ref symbol))
			{
				if (!changes.ContainsKey(symbol))
				{
					string participentSymbol = "";

					changes.Add(symbol, new MDServerToClient());

					DecodeIssueSymbol(ref symbol, ref participentSymbol);

					changes[symbol].Update(WombatFieldIDs.IssueSymbol, symbol);
					changes[symbol].Update(WombatFieldIDs.uSymbol, symbol);
					changes[symbol].Update(WombatFieldIDs.uParticipentSymbol, participentSymbol);
				}

				msg.begin(ref _mainMsgIterator);
				while ((field = _mainMsgIterator.getField()) != null)
				{
					fid = field.getFid();
					if (MDOutputFieldIDs.FNames.ContainsKey(fid))
					{
						switch (field.getType())
						{
							case mamaFieldType.MAMA_FIELD_TYPE_STRING:
								changes[symbol].Update(fid, field.getString());
								break;
							case mamaFieldType.MAMA_FIELD_TYPE_F64:
								changes[symbol].Update(fid, Convert.ToDouble(field.getF64()));
								break;
							case mamaFieldType.MAMA_FIELD_TYPE_I64:
								changes[symbol].Update(fid, Convert.ToInt64(field.getI64()));
								break;
							case mamaFieldType.MAMA_FIELD_TYPE_I32:
								changes[symbol].Update(fid, Convert.ToInt64(field.getI32()));
								break;
						}
					}

					_mainMsgIterator++;
				}
			}
			else
			{
				// Not a real message
			}
		}

		private void StopNotify()
		{
			lock (_syncObj)
			{
				_threadNotifyShouldStop = true;
			}

			if (_threadNotify != null)
			{
				while (_threadNotify.IsAlive) ;
			}
		}

		private List<MDServerToClient> ConvertToList(Dictionary<string, MDServerToClient> cKeyedList)
		{
			List<MDServerToClient> outList = new List<MDServerToClient>();
			if (cKeyedList.Count > 0)
			{
				outList.AddRange(cKeyedList.Values);
			}
			return outList;
		}

		private void DecodeIssueSymbol(ref string symbol, ref string participent)
		{
			// valid data
			int div = symbol.IndexOf(".");

			if (div > 0)
			{
				participent = symbol.Substring(div + 1);
				symbol = symbol.Substring(0, div);
			}
		}

		#endregion

		#region - MamdaOrderBookHandler Members -

		public void onBookClear(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookDelta delta, MamdaOrderBookRecap fullBook)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		public void onBookDelta(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookDelta delta, MamdaOrderBookRecap fullBook)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		public void onBookGap(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookGap gapEvent, MamdaOrderBookRecap fullBook)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		public void onBookRecap(MamdaSubscription subscription, MamdaOrderBookListener listener, MamaMsg msg, MamdaOrderBookDelta delta, MamdaOrderBookRecap fullBook)
		{
			throw new Exception("The method or operation is not implemented.");
		}

		#endregion

		#region - IDisposable Members -

		public void Dispose()
		{
			Stop();
		}

		#endregion

		private void MemClean()
		{
			GC.Collect();
			GC.WaitForPendingFinalizers();
		}
	}
}
