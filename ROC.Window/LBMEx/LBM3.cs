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

	internal class LBM : MamaDictionaryCallback, MamaSubscriptionCallback, MamaMsgFieldIterator, MamdaOrderBookHandler, IDisposable
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

		private bool _running;

		// Key to different sources by string
		private Dictionary<string, MamaSource> _mamaSources = new Dictionary<string, MamaSource>();
		// Key to different symbols by string
		private Dictionary<string, MamaSubscription> _mamaSubscriptions = new Dictionary<string, MamaSubscription>();

		private object _syncObj = new object();

		private bool _useThreadedDecode;
		private bool _useChangeNotificationRate;
		private int _changeNotificationRate;

		private bool _forceGC = false;

		#endregion

		#region - Thread Decode and Notify -

		private bool _threadMDChangesShouldStop = false;
		private bool _threadMDChangesStarted = false;
		private System.Threading.Thread _threadMDChanges;
		private List<MDServerToClient> _mamaMDChangesQueue = new List<MDServerToClient>();

		#endregion

		#region - Thread Notify-

		private Dictionary<object, MDServerToClient> _mamaOnFieldChanges = new Dictionary<object, MDServerToClient>();

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
			_changeNotificationRate = changeNotificationRate;
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

		public void SubscribeOption(string symbol, string source)
		{
			//Subscribe(symbol + ".GRP", source, mamaSubscriptionType.MAMA_SUBSC_TYPE_GROUP);
		}

		private void Subscribe(string symbol, string source, mamaSubscriptionType type)
		{
			lock (_syncObj)
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

				lock (_syncObj)
				{
					_threadMDChangesShouldStop = true;
				}

				if (_threadMDChanges != null)
				{
					while (_threadMDChanges.IsAlive) ;
				}
				
				Initialize();

				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnStop));

				MemClean();
			}
		}

		#endregion

		#region - MamaDictionaryCallback Members -

		public void onComplete(MamaDictionary dictionary)
		{
			lock (_syncObj)
			{
				_gotDictionaries = true;
				Mama.stop(_mamaBridge);
			}
		}

		public void onError(MamaDictionary dictionary, string message)
		{
			lock (_syncObj)
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeError, "MamaDictionary:Error " + message));
				Mama.stop(_mamaBridge);
			}
		}

		public void onTimeout(MamaDictionary dictionary)
		{
			lock (_syncObj)
			{
				OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnInitializeError, "MamaDictionary:Timeout " + dictionary.ToString()));
				Mama.stop(_mamaBridge);
			}
		}

		#endregion

		#region - MamaSubscriptionCallback Members -

		public void onCreate(MamaSubscription subscription)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscribed, String.Concat(new object[] { "MamaSubscription|onCreate ", subscription.subscSymbol, " : ", subscription.subscSource } )));
		}

		public void onError(MamaSubscription subscription, MamaStatus.mamaStatus status, string subject)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionError, String.Concat(new object[] { "MamaSubscription|onError ", subscription.subscSymbol, " : ", subscription.subscSource, " Status: " + status.ToString(), " Subject: ",  subject })));
		}

		public void onGap(MamaSubscription subscription)
		{
			OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionGap, String.Concat(new object[] { "MamaSubscription|onGap ", subscription.subscSymbol, " : ", subscription.subscSource })));
		}

		public void onMsg(MamaSubscription subscription, MamaMsg msg)
		{
			if (!_useThreadedDecode)
			{
				//#region - Msg Queue -

				//lock (_syncObj)
				//{
				//    msg.detach();
				//    _mamaMsgQueue.Add(new MamaMsg(msg));
				//}

				//#endregion

				//#region - Threaded Decode and Notify -

				//if (!_threadDecodeStarted)
				//{
				//    _threadDecodeShouldStop = false;
				//    _threadDecode = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessMsgQueueAndNotify));
				//    _threadDecode.Priority = System.Threading.ThreadPriority.AboveNormal;

				//    _threadDecode.Name = "LBM_ProcessMsg";
				//    _threadDecode.Start();

				//    while (!_threadDecode.IsAlive) ;
				//    _threadDecodeStarted = true;
				//}

				//#endregion
			}
			else
			{
				#region - Thread Decode -

				string issueSymbol = "";	
				MamaMsg locMsg = new MamaMsg(msg);
				//locMsg.tryString(new MamaFieldDescriptor(WombatFieldIDs.IssueSymbol, mamaFieldType.MAMA_FIELD_TYPE_STRING, ""), ref issueSymbol);
				//if (issueSymbol != "")
				//{
					//locMsg.iterateFields(this, _mamaDictionary, new string[] { issueSymbol, subscription.subscSymbol, subscription.subscSource });
					locMsg.iterateFields(this, _mamaDictionary, null);
				//}

				#endregion

				#region - Thread Notify -

				if (!_threadMDChangesStarted)
				{
					_threadMDChangesShouldStop = false;
					_threadMDChanges = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessMDChangeQueueAndNotify));
					_threadMDChanges.Priority = System.Threading.ThreadPriority.AboveNormal;

					_threadMDChanges.Name = "LBM_ProcessMDChange";
					_threadMDChanges.Start();

					while (!_threadMDChanges.IsAlive) ;
					_threadMDChangesStarted = true;
				}

				#endregion
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

		#region - MamaMsgFieldIterator Members -

		public void onField(MamaMsg msg, MamaMsgField field, object closure)
		{
			//MDServerToClient c = new MDServerToClient();

			//string[] strings = (string[])closure;

			// 0 issueSymbol;
			// 1 symbol;
			// 2 source;

			lock (_syncObj)
			{
				string issueSymbol = "";
				msg.tryString(new MamaFieldDescriptor(WombatFieldIDs.IssueSymbol, mamaFieldType.MAMA_FIELD_TYPE_STRING, ""), ref issueSymbol);
				if (_mamaOnFieldChanges.ContainsKey(issueSymbol))
				{
					c = _mamaOnFieldChanges[issueSymbol];
				}
				else
				{
					//string symbol = strings[0];
					string participentSymbol = "";

					DecodeIssueSymbol(ref issueSymbol, ref participentSymbol);

					c.Add(WombatFieldIDs.IssueSymbol, issueSymbol);
					c.Add(WombatFieldIDs.uSymbol, symbol);
					c.Add(WombatFieldIDs.uParticipentSymbol, participentSymbol);

					//c.Add(WombatFieldIDs.uSubscriptionSymbol, strings[1]);
					//c.Add(WombatFieldIDs.uSubscriptionSoruce, strings[2]);

					_mamaOnFieldChanges.Add(strings[0], c);
				}
			}

			int fid = field.getFid();
			if (MDOutputFieldIDs.FNames.ContainsKey(fid))
			{
				switch (field.getType())
				{
					case mamaFieldType.MAMA_FIELD_TYPE_PRICE:
						c.Update(fid, field.getPrice().getValue());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_F64:
						c.Update(fid, field.getF64());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_I8:
					case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
						c.Update(fid, field.getChar().ToString());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_STRING:
						c.Update(fid, field.getString());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_BOOL:
						c.Update(fid, Convert.ToInt32(field.getBool()));
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_U8:
						c.Update(fid, field.getU8());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_I16:
						c.Update(fid, field.getI16());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_U16:
						c.Update(fid, field.getU16());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_I32:
						c.Update(fid, field.getI32());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_U32:
						c.Update(fid, field.getU32());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_I64:
						c.Update(fid, field.getI64());
						break;
					case mamaFieldType.MAMA_FIELD_TYPE_U64:
						c.Update(fid, field.getU64());
						break;
					default:
						Debug.Assert(!Debugger.IsAttached, field.getTypeName());
						break;
				}
			}

			//lock (_syncObj)
			//{
			//    _mamaOnFieldChanges[closure] = c;
			//}
		}

		#endregion

		#region - Threaded Decode and Notify -

		//private MDChanges IterateMsg(MamaMsg msg)
		//{
		//    int fid;
		//    MDChanges c = new MDChanges();

		//    c.Add(WombatFieldIDs.uOnMsgTime, _dtHP.UtcNow);

		//    MamaMsgIterator i = new MamaMsgIterator(_mamaDictionary);
		//    MamaMsgField f;

		//    try
		//    {
		//        msg.begin(ref i);
		//        while ((f = i.getField()) != null)
		//        {
		//            fid = f.getFid();
		//            if (MDFieldDefinitions.FNames.ContainsKey(fid))
		//            {
		//                switch (f.getType())
		//                {
		//                    case mamaFieldType.MAMA_FIELD_TYPE_PRICE:
		//                        c.Add(fid, f.getPrice().getValue());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_F64:
		//                        c.Add(fid, f.getF64());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_I8:
		//                    case mamaFieldType.MAMA_FIELD_TYPE_CHAR:
		//                        c.Add(fid, f.getChar().ToString());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_STRING:
		//                        c.Add(fid, f.getString());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_BOOL:
		//                        c.Add(fid, Convert.ToInt32(f.getBool()));
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_U8:
		//                        c.Add(fid, f.getU8());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_I16:
		//                        c.Add(fid, f.getI16());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_U16:
		//                        c.Add(fid, f.getU16());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_I32:
		//                        c.Add(fid, f.getI32());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_U32:
		//                        c.Add(fid, f.getU32());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_I64:
		//                        c.Add(fid, f.getI64());
		//                        break;
		//                    case mamaFieldType.MAMA_FIELD_TYPE_U64:
		//                        c.Add(fid, f.getU64());
		//                        break;
		//                    default:
		//                        Debug.Assert(!Debugger.IsAttached, f.getTypeName());
		//                        break;
		//                }

		//                #region - Not Decoded Fields -
		//                //else
		//                //{
		//                //    // Fields not decoded
		//                //    //Debug.Print("Not Decoded : {0,-10} {1}", f.getFid().ToString(), f.getName());
		//                //}
		//                #endregion
		//            }

		//            f.Dispose();
		//            i++;
		//        }
		//    }
		//    catch (Exception ex)
		//    {
		//        Debug.Assert(!Debugger.IsAttached, ex.Message, ex.StackTrace);
		//    }
		//    i.Dispose();

		//    #region - uSymbol and uParticipentSymbol -

		//    if (c.Strings.ContainsKey(WombatFieldIDs.IssueSymbol))
		//    {
		//        string issueSymbol = c.Strings[WombatFieldIDs.IssueSymbol];
		//        string symbol = issueSymbol;
		//        string participent = "";

		//        DecodeIssueSymbol(ref symbol, ref participent);

		//        c.Add(WombatFieldIDs.uSymbol, symbol);
		//        c.Add(WombatFieldIDs.uParticipentSymbol, participent);

		//        c.Add(WombatFieldIDs.uDecodedTime, _dtHP.UtcNow);
		//    }
		//    else
		//    {
		//        // not a valid message
		//        c = new MDChanges();
		//    }

		//    #endregion

		//    return c;
		//}

		//private List<MDChanges> ConsolidateMamaMsgList(MamaMsg[] msgList)
		//{
		//    Dictionary<string, MDChanges> cKeyedList = new Dictionary<string, MDChanges>();
		//    MDChanges c;

		//    foreach (MamaMsg msg in msgList)
		//    {
		//        c = IterateMsg(msg);
		//        ConsolidateList(ref cKeyedList, c);
		//    }

		//    return ConvertToList(cKeyedList);
		//}

		//private void ConsolidateList(ref Dictionary<string, MDChanges> cKeyedList, MDChanges c)
		//{
		//    string issueSymbol = "";
		//    if (c.Strings.ContainsKey(WombatFieldIDs.IssueSymbol))
		//    {
		//        issueSymbol = c.Strings[WombatFieldIDs.IssueSymbol];
		//        if (cKeyedList.ContainsKey(issueSymbol))
		//        {
		//            cKeyedList[issueSymbol].Update(c);
		//        }
		//        else
		//        {
		//            cKeyedList.Add(issueSymbol, c);
		//        }
		//    }
		//}

		//private List<MDChanges> ConvertToList(Dictionary<string, MDChanges> cKeyedList)
		//{
		//    List<MDChanges> outList = new List<MDChanges>();
		//    if (cKeyedList.Count > 0)
		//    {
		//        outList.AddRange(cKeyedList.Values);
		//    }
		//    return outList;			
		//}

		//private void ProcessMsgQueueAndNotify()
		//{
		//    while (!_threadDecodeShouldStop)
		//    {
		//        MamaMsg[] locMsgs;
		//        lock (_syncObj)
		//        {
		//            if (_mamaMsgQueue.Count > 0)
		//            {
		//                locMsgs = _mamaMsgQueue.ToArray();
		//                _mamaMsgQueue.Clear();
		//            }
		//            else
		//            {
		//                locMsgs = null;
		//                System.Threading.Thread.Sleep(1);
		//            }
		//        }

		//        if (locMsgs != null)
		//        {
		//            OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionMsg, ConsolidateMamaMsgList(locMsgs)));

		//            if (_useChangeNotificationRate)
		//            {
		//                System.Threading.Thread.Sleep(_changeNotificationRate);
		//            }
		//            else
		//            {
		//                System.Threading.Thread.Sleep(1);
		//            }
		//        }
		//    }

		//    lock (_syncObj)
		//    {
		//        _threadDecodeStarted = false;
		//    }
		//}

		#endregion

		#region - Threaded Notify -

		private List<MDServerToClient> ConvertToList(Dictionary<object, MDServerToClient> cKeyedList)
		{
			List<MDServerToClient> outList = new List<MDServerToClient>();
			if (cKeyedList.Count > 0)
			{
				outList.AddRange(cKeyedList.Values);
			}
			return outList;
		}

		private void ProcessMDChangeQueueAndNotify()
		{
			while (!_threadMDChangesShouldStop)
			{
				lock (_syncObj)
				{
					if (_mamaOnFieldChanges.Count > 0)
					{
						OnLBMChanged(new LBMDataChangedEventArgs(LBMDataChangedEventTypes.OnSubscriptionMsg, ConvertToList(_mamaOnFieldChanges)));
						_mamaOnFieldChanges.Clear();
					}
				}

				if (_useChangeNotificationRate)
				{
					System.Threading.Thread.Sleep(_changeNotificationRate);
				}
				else
				{
					System.Threading.Thread.Sleep(1);
				}
			}

			lock (_syncObj)
			{
				_threadMDChangesStarted = false;
			}
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
