using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using DateTimeEx;
using SRLabEx;
using MarketDataEx;

namespace SRLabMDS
{
	public class HelperSRLab : HelperStatusBase
	{
		public enum StatusTypes
		{
			Connected,
			Connecting,
			DisConnecting,
			NotConnected,

			EnvironmentVariableChecking,
			EnvironmentVariableChecked,
		}

		#region - SRLab Main Thread -

		private bool _threadSRLabStarted = false;
		private System.Threading.Thread _threadSRLab;

		#endregion

		#region - LBM Subscribe Thread -

		//private System.Threading.Timer _timerSubscription;
		//private System.Threading.TimerCallback _timerSubscriptionDelagate;

		private bool _threadSubscriptionProcessShouldStop = false;
		private bool _threadSubscriptionProcessStarted = false;
		private System.Threading.Thread _threadSubscriptionProcess;

		private Dictionary<string, MDClientToServer> _subList = new Dictionary<string, MDClientToServer>();
		private Dictionary<string, MDClientToServer> _subOPRAList = new Dictionary<string, MDClientToServer>();

		#endregion

		#region - LBM Delta & Recap Thread -

		//private System.Threading.Timer _timerDeltaNotification;
		//private System.Threading.TimerCallback _timerDeltaNotificationDelagate;

		private bool _threadDeltaRecapProcessShouldStop = false;
		private bool _threadDeltaRecapProcessStarted = false;
		private System.Threading.Thread _threadDeltaRecapProcess;

		private Dictionary<string, MDServerToClient> _latestDeltas = new Dictionary<string, MDServerToClient>();

		#endregion

		#region - Local Variable -

		private bool _stopping = false;

		private DateTime _pTime = DateTime.Now;
		private TimeSpan _pTimeSpan = new TimeSpan();

		private bool _logPerformance = false;

		private DateTimeHP _dtHP = new DateTimeHP();

		private int _subscriptionTimeGap = 100;
		private int _deltaRecapTimeGap = 100;

		private SRLabWrapper _srLabWrapper;

		private object _syncObj = new object();

		private string _ctaPrimary = "N";
		private string _nasdaqPrimary = "Q";
		private bool _convertOPRA = true;

		private bool _enableDataCalculation = false;
		private bool _enableDataRoll = false;

		#endregion

		#region - Property -

		private DateTime _firstMsgTime;
		public DateTime FirstMsgTime
		{
			get
			{
				return _firstMsgTime;
			}
		}

		private bool _statusChanged = true;
		public bool StatusChanged
		{
			get
			{
				return _statusChanged;
			}
			set
			{
				_statusChanged = value;
			}
		}

		private StatusTypes _status = StatusTypes.NotConnected;
		public StatusTypes Status
		{
			get
			{
				return _status;
			}
			set
			{
				if (_status != value)
				{
					_status = value;
					StatusChanged = true;
				}
			}
		}

		private bool _totalDataMsgCountChanged = true;
		public bool TotalDataMsgCountChanged
		{
			get
			{
				return _totalDataMsgCountChanged;
			}
			set
			{
				_totalDataMsgCountChanged = value;
			}
		}

		private long _totalDataMsgCount;
		public long TotoalDataMsgCount
		{
			get
			{
				return _totalDataMsgCount;
			}
			set
			{
				if (_totalDataMsgCount != value || 0 == value)
				{
					_totalDataMsgCount = value;
					TotalDataMsgCountChanged = true;
				}
			}
		}

		private List<string> _srLabLogs;
		public List<string> SRLabLogs
		{
			get
			{
				if (_srLabLogs == null)
				{
					_srLabLogs = new List<string>();
				}
				return _srLabLogs;
			}
			set
			{
				_srLabLogs = value;
			}
		}

		// Key = Symbol + Source
		private Dictionary<string, MDServerToClient> _latestData;
		public Dictionary<string, MDServerToClient> LatestData
		{
			get
			{
				if (_latestData == null)
				{
					_latestData = new Dictionary<string, MDServerToClient>();
				}
				return _latestData;
			}
			set
			{
				_latestData = value;
			}
		}

		// Key = Symbol + Source
		public Dictionary<string, SubscriptionFields> Subscriptions
		{
			get
			{
				return _srLabWrapper.Subscriptions;
			}
		}

		// Key = Symbol + Source
		public List<string> MamaSubscriptionsCreated
		{
			get
			{
				return _srLabWrapper.SubscriptionsCreated;
			}
		}

		private List<string> _cmeSources;
		public List<string> CMESources
		{
			get
			{
				if (_cmeSources == null)
				{
					_cmeSources = new List<string>();
				}
				return _cmeSources;
			}
			set
			{
				_cmeSources = value;
			}
		}

		#endregion

		#region - Constructor -

		public HelperSRLab()
		{
			Initialize();
		}

		private void Initialize()
		{
			_logPerformance = Configuration.User.Default.LogPerformance;

			_subscriptionTimeGap = Configuration.SRLab.Default.SubscriptionTimeGap;
			_deltaRecapTimeGap = Configuration.SRLab.Default.ChangeNotificationRate;

			_ctaPrimary = Configuration.SRLab.Default.CTAPrimary;
			_nasdaqPrimary = Configuration.SRLab.Default.NASDAQPrimay;
			_convertOPRA = Configuration.SRLab.Default.ConvertOPRA;

			_enableDataCalculation = Configuration.MDS.Default.EnableDataCalculation;
			_enableDataRoll = Configuration.MDS.Default.EnableDataRoll;

			string[] cmeSources = Configuration.SRLab.Default.CMEExchangeList.Split(new string[] { "," },  StringSplitOptions.RemoveEmptyEntries);
			if (cmeSources.Length == 0)
			{
				CMESources.Add("CME");
			}
			else
			{
				CMESources.AddRange(cmeSources);
			}

			if (_srLabWrapper == null)
			{
				_srLabWrapper = new SRLabWrapper(
					Application.StartupPath,
					Configuration.SRLab.Default.SMNames,
					Configuration.SRLab.Default.SMType,
					Configuration.SRLab.Default.Configlocation,
					Configuration.SRLab.Default.ConfigFile,
					Configuration.SRLab.Default.Depth,
					Configuration.SRLab.Default.LocalIP,
					Configuration.SRLab.Default.DelayMCJoin,
					Configuration.SRLab.Default.FireLevel1Only,
					Configuration.SRLab.Default.LastValue,
					Configuration.SRLab.Default.LogLevel,
					Configuration.SRLab.Default.MaxLogSize,
					Configuration.SRLab.Default.SubscriptionSourceRange,
					Configuration.SRLab.Default.SubscriptionSymbolRange,
					Configuration.SRLab.Default.StockSources,
					Configuration.SRLab.Default.FutureSources,
					Configuration.SRLab.Default.OptionSources,
					Configuration.SRLab.Default.BusyWait,
					Configuration.SRLab.Default.ChangeNotificationRate,
					Configuration.SRLab.Default.LineTimeHourOffset,
					Configuration.SRLab.Default.LineTimeMinOffset,
					Configuration.SRLab.Default.LineTimeSecOffset,
					Configuration.SRLab.Default.UseChangeNotificationRate,
					Configuration.SRLab.Default.SubscriptionUseWaitForSymbol,
					Configuration.SRLab.Default.SubscribeWithSource);
				_srLabWrapper.MarketDataChanged += new MarketDataChangedEventHandler(MarketDataChanged);
			}
		}

		#endregion

		#region - SRLab Event -

		private void MarketDataChanged(object sender, SRLabChangedEventArgs e)
		{
			try
			{
				switch (e.Type)
				{
					case SRLabDataChangedEventTypes.OnException:
						_srLabWrapper.Stop();
						SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "SRLAB|OnException: ", e.Msg }));
						break;
					case SRLabDataChangedEventTypes.OnMDException:
						_srLabWrapper.Stop();
						SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "SRLAB|OnMDException: ", e.Msg }));
						break;
					case SRLabDataChangedEventTypes.OnMsg:
						SetStatus(Status, String.Concat(new object[] { "SRLAB|OnMsg: ", e.Msg }));
						break;
					case SRLabDataChangedEventTypes.OnInitialized:
						SetStatus(StatusTypes.Connected, String.Concat(new object[] { "SRLAB|OnInitialized: " }));
						break;
					case SRLabDataChangedEventTypes.OnStopped:
						SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "SRLAB|OnStopped: ", e.Msg }));
						break;
					case SRLabDataChangedEventTypes.OnEnvironmentVariableChecked:
						SetStatus(StatusTypes.EnvironmentVariableChecked, String.Concat(new object[] { "SRLAB|OnEnvironmentVariableChecked: ", e.Msg }));
						break;
					case SRLabDataChangedEventTypes.OnChange:
						MarketDataChanged(e.Change);
						break;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}

		private void MarketDataChanged(MDServerToClient delta)
		{
			StoreTotalMsgCount();

			if (_enableDataRoll)
			{
				RollLatestData(delta);
			}

			string key = delta.uSubscriptionKey;
			if (key != "")
			{
				UpdateLatestData(key, delta);
				GLOBAL.HMDS.ProcessDeltaQueue(delta);
			}

			PerformanceLog("SRLAB|ProcessSRLABDelta", string.Format("{0,-12}| Changes[{1,3}] - {2,10}| {3,8}ms", "DecodeTime", delta.Count, delta.IssueSymbol, delta.uDecodedTime));
		}

		#region - Roll Data -

		private void RollLatestData(MDServerToClient delta)
		{
			switch (delta.uSubscriptionSoruce)
			{
				case "CTA":
					RollLatestData(delta, _ctaPrimary);
					break;
				case "NASDAQ":
					RollLatestData(delta, _nasdaqPrimary);
					break;
			}
		}

		private void RollLatestData(MDServerToClient delta, string primary)
		{
			if (delta.uParticipentSymbol == primary)
			{
				bool hasChanges = false;

				MDServerToClient localDelta = new MDServerToClient();

				if (delta.PrevClosePrice != null)
				{
					localDelta.Update(WombatFieldIDs.PrevClosePrice, (double)delta.PrevClosePrice);
					hasChanges = true;
				}
				if (delta.SettlePrice != null)
				{
					localDelta.Update(WombatFieldIDs.SettlePrice, (double)delta.SettlePrice);
					hasChanges = true;
				}
				if (delta.HighPrice != null)
				{
					localDelta.Update(WombatFieldIDs.HighPrice, (double)delta.HighPrice);
					hasChanges = true;
				}
				if (delta.LowPrice != null)
				{
					localDelta.Update(WombatFieldIDs.LowPrice, (double)delta.LowPrice);
					hasChanges = true;
				}
				if (delta.OpenPrice != null)
				{
					localDelta.Update(WombatFieldIDs.OpenPrice, (double)delta.OpenPrice);
					hasChanges = true;
				}
				if (delta.TotalVolume != null)
				{
					localDelta.Update(WombatFieldIDs.TotalVolume, (long)delta.TotalVolume);
					hasChanges = true;
				}

				if (hasChanges)
				{
					localDelta.Update(WombatFieldIDs.SubscriptionSoruce, delta.uSubscriptionSoruce);
					localDelta.Update(WombatFieldIDs.SubscriptionSymbol, delta.uSymbol);
					localDelta.Update(WombatFieldIDs.IssueSymbol, delta.uSymbol);


					string key = localDelta.uSubscriptionKey;
					if (key != "")
					{
						if (_enableDataCalculation)
						{
							CalculateLatestData(key, ref localDelta);
						}
						UpdateLatestData(key, localDelta);
						GLOBAL.HMDS.ProcessDeltaQueue(localDelta);
					}
				}
			}
		}

		private void CalculateLatestData(string key, ref MDServerToClient delta)
		{
			try
			{
				Nullable<double> tradePrice = null;

				if (delta.TradePrice != null)
				{
					tradePrice = delta.TradePrice;
				}
				else if (tradePrice == null && LatestData[key].TradePrice != null)
				{
					tradePrice = LatestData[key].TradePrice;
				}

				if (tradePrice != null)
				{
					lock (LatestData)
					{
						if (LatestData[key].TradePrice != null)
						{
							if ((double)tradePrice >= (double)LatestData[key].TradePrice)
							{
								delta.Update(WombatFieldIDs.TradeTick, "+");
							}
							else
							{
								delta.Update(WombatFieldIDs.TradeTick, "-");
							}
						}

						if (delta.OpenPrice != null)
						{
							delta.Update(WombatFieldIDs.NetChange, (double)delta.OpenPrice - (double)tradePrice);
							delta.Update(WombatFieldIDs.PctChange, (double)delta.NetChange / (double)tradePrice);
						}
						else if (LatestData[key].OpenPrice != null)
						{
							delta.Update(WombatFieldIDs.NetChange, (double)LatestData[key].OpenPrice - (double)tradePrice);
							delta.Update(WombatFieldIDs.PctChange, (double)delta.NetChange / (double)tradePrice);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}

		#endregion

		private void UpdateLatestData(string key, MDServerToClient delta)
		{
			lock (LatestData)
			{
				if (LatestData.ContainsKey(key))
				{
					LatestData[key].Update(delta);
					if (LatestData[key].TotalVolume <= 0)
					{
						LatestData[key].AskBooks.Clear();
						LatestData[key].BidBooks.Clear();
						LatestData[key].TradedVolumes.Clear();
					}
				}
				else
				{
					LatestData.Add(key, delta);
				}
			}
		}

		private void StoreTotalMsgCount()
		{
			if (TotoalDataMsgCount == 0)
			{
				_firstMsgTime = DateTime.Now;
			}
			TotoalDataMsgCount += 1;
		}

		#endregion

		#region - Check Environments -

		public void CheckEnvironments()
		{
			_srLabWrapper.CheckEnvironments();
		}

		#endregion

		#region - SRLab Main Thread -

		public void Start()
		{
			TotoalDataMsgCount = 0;

			StartSRLabSubscription();
			StartDeltaRecapNotification();
			StartSRLabThread();
		}

		private void StartSRLabThread()
		{
			if (!_threadSRLabStarted && !_stopping)
			{
				_threadSRLab = new System.Threading.Thread(new System.Threading.ThreadStart(ProccessSRLab));
				_threadSRLab.Name = "ProcessSRLab";
				_threadSRLab.Start();

				while (!_threadSRLab.IsAlive) ;
				_threadSRLabStarted = true;
			}
		}

		private void ProccessSRLab()
		{
			_srLabWrapper.Start();
			_threadSRLabStarted = false;
		}

		private void StopSRLabThread()
		{
			_srLabWrapper.Stop();
		}

		#endregion

		#region - SRLab Subscribe Thread -

		public void Subscribe(MDClientToServer sub)
		{
			if (Status == StatusTypes.Connected)
			{
				//switch (sub.SecType)
				//{
				//    case SubscriptionTypes.SSFuture:
				//    case SubscriptionTypes.Option:
				//    case SubscriptionTypes.OptionOnFuture:
				//    case SubscriptionTypes.Future:
				//    case SubscriptionTypes.Stock:
				//    default:
				//        AddToStatusLogs(string.Format("{0,30} {1} {2}", "Subscribing... To", sub.MDSymbol, sub.MDDataSource));
				//        _wombat.Subscribe(sub.MDSymbol, sub.MDDataSource);

				//        // Need to wait for before next subscription?
				//        System.Threading.Thread.Sleep(_subscriptionTimeGap);
				//        break;
				//}

				switch (sub.MDDataSource)
				{
					case "OPRA":
						lock (_subOPRAList)
						{
							if (_convertOPRA)
							{
								MDClientToServer opraSub = new MDClientToServer(sub.MsgType, sub.MDSymbol, "OPRANBBO", sub.SubscriptionType, sub.SecType, sub.Reload);
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", opraSub.MDSymbol, opraSub.MDDataSource));
								if (!_subOPRAList.ContainsKey(opraSub.MDKey))
								{
									_subOPRAList.Add(opraSub.MDKey, opraSub);
								}
							}
							else
							{
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", sub.MDSymbol, sub.MDDataSource));
								if (!_subOPRAList.ContainsKey(sub.MDKey))
								{
									_subOPRAList.Add(sub.MDKey, sub);
								}
							}
						}
						break;
					case "OPRANBBO":
						lock (_subList)
						{
							if (!_convertOPRA)
							{
								MDClientToServer opranbboSub = new MDClientToServer(sub.MsgType, sub.MDSymbol, "OPRA", sub.SubscriptionType, sub.SecType, sub.Reload);
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", opranbboSub.MDSymbol, opranbboSub.MDDataSource));
								if (!_subList.ContainsKey(opranbboSub.MDKey))
								{
									_subList.Add(opranbboSub.MDKey, opranbboSub);
								}
							}
							else
							{
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", sub.MDSymbol, sub.MDDataSource));
								if (!_subOPRAList.ContainsKey(sub.MDKey))
								{
									_subOPRAList.Add(sub.MDKey, sub);
								}
							}
						}
						break;
					case "CME":
						lock (_subList)
						{
							foreach (string cmeSource in CMESources)
							{
								MDClientToServer cmeSub = new MDClientToServer(sub.MsgType, sub.MDSymbol, cmeSource, sub.SubscriptionType, sub.SecType, sub.Reload);
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", cmeSub.MDSymbol, cmeSub.MDDataSource));
								if (!_subList.ContainsKey(cmeSub.MDKey))
								{
									_subList.Add(cmeSub.MDKey, cmeSub);
								}
							}
						}
						break;
					case "CTA":
						lock (_subList)
						{
							AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", sub.MDSymbol, sub.MDDataSource));
							if (!_subList.ContainsKey(sub.MDKey))
							{
								_subList.Add(sub.MDKey, sub);
							}

							// Sub to the primary market too N is CTA.
							MDClientToServer ctaSub = new MDClientToServer(sub.MsgType, sub.MDSymbol + "." + _ctaPrimary, "CTA", sub.SubscriptionType, sub.SecType, sub.Reload);
							AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", ctaSub.MDSymbol, ctaSub.MDDataSource));
							if (!_subList.ContainsKey(ctaSub.MDKey))
							{
								_subList.Add(ctaSub.MDKey, ctaSub);
							}
						}
						break;
					case "NASDAQ":
						lock (_subList)
						{
							AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", sub.MDSymbol, sub.MDDataSource));
							if (!_subList.ContainsKey(sub.MDKey))
							{
								_subList.Add(sub.MDKey, sub);
							}

							// Sub to the primary market too N is CTA.
							MDClientToServer nasdaqSub = new MDClientToServer(sub.MsgType, sub.MDSymbol + "." + _nasdaqPrimary, "NASDAQ", sub.SubscriptionType, sub.SecType, sub.Reload);
							AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", nasdaqSub.MDSymbol, nasdaqSub.MDDataSource));
							if (!_subList.ContainsKey(nasdaqSub.MDKey))
							{
								_subList.Add(nasdaqSub.MDKey, nasdaqSub);
							}
						}
						break;
					default:
						lock (_subList)
						{
							AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", sub.MDSymbol, sub.MDDataSource));
							if (!_subList.ContainsKey(sub.MDKey))
							{
								_subList.Add(sub.MDKey, sub);
							}
						}
						break;
				}
			}
		}

		private void StartSRLabSubscription()
		{
			//if (!_stopping)
			//{
			//    if (_timerSubscriptionDelagate == null)
			//    {
			//        _timerSubscriptionDelagate = new System.Threading.TimerCallback(ProcessLBMSubscriptionQueue);
			//    }
			//    if (_timerSubscription == null)
			//    {
			//        _timerSubscription = new System.Threading.Timer(_timerSubscriptionDelagate, null, 0, 100);
			//    }
			//}

			if (!_threadSubscriptionProcessStarted && !_stopping)
			{
				_threadSubscriptionProcessShouldStop = false;

				_threadSubscriptionProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessSRLabSubscriptionQueue));
				_threadSubscriptionProcess.Name = "MDSSubscriptionProcess";
				_threadSubscriptionProcess.Start();

				while (!_threadSubscriptionProcess.IsAlive) ;
				_threadSubscriptionProcessStarted = true;
			}
		}

		private void ProcessSRLabSubscriptionQueue()
		{
			MDClientToServer[] mdss = new MDClientToServer[0];

			while (!_threadSubscriptionProcessShouldStop)
			{
				mdss = new MDClientToServer[0];

				// Get All Normal Subscriptions
				lock (_subList)
				{
					if (_subList.Count > 0)
					{
						mdss = new MDClientToServer[_subList.Count];
						_subList.Values.CopyTo(mdss, 0);
						_subList.Clear();
					}
				}

				if (mdss.Length == 0)
				{
					// Get All OPRA Subscriptions IFF Normal Subscriptions Are Done
					lock (_subOPRAList)
					{
						if (_subOPRAList.Count > 0)
						{
							mdss = new MDClientToServer[_subOPRAList.Count];
							_subOPRAList.Values.CopyTo(mdss, 0);
							_subOPRAList.Clear();
						}
					}
				}

				if (mdss.Length > 0)
				{
					foreach (MDClientToServer mds in mdss)
					{
						if (_threadSubscriptionProcessShouldStop) break;

						switch (mds.SubscriptionType)
						{
							case "BOOK":
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "BOOK Subscribing... To", mds.MDSymbol, mds.MDDataSource));
								_srLabWrapper.SubscribeBook(mds.MDSymbol.TrimStart(new char[] { 'b' }), mds.MDDataSource, mds.Reload);
								break;
							case "L2":
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "L2 Subscribing... To", mds.MDSymbol, mds.MDDataSource));
								_srLabWrapper.SubscribeL2(mds.MDSymbol, mds.MDDataSource, mds.Reload);
								break;
							default:
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Subscribing... To", mds.MDSymbol, mds.MDDataSource));
								_srLabWrapper.Subscribe(mds.MDSymbol, mds.MDDataSource, mds.Reload);
								break;
						}

						// Need to wait for before next subscription?
						System.Threading.Thread.Sleep(_subscriptionTimeGap);
					}
				}
				else
				{
					System.Threading.Thread.Sleep(1);
				}
			}

			_threadSubscriptionProcessStarted = false;
		}

		private void StopSRLabSubscription()
		{
			//if (_timerSubscription != null)
			//{
			//    _timerSubscription.Dispose();
			//}
			//_timerSubscription = null;
			//_timerSubscriptionDelagate = null;

			_threadSubscriptionProcessShouldStop = true;
			if (_threadSubscriptionProcess != null)
			{
				while (_threadSubscriptionProcess.IsAlive) ;
			}
		}

		#endregion

		#region - SRLab Delta & Recap Notification Thread -

		private void StartDeltaRecapNotification()
		{
			//if (!_stopping)
			//{
			//    if (_timerDeltaNotificationDelagate == null)
			//    {
			//        _timerDeltaNotificationDelagate = new System.Threading.TimerCallback(ProcessLBMDeltaQueue);
			//    }

			//    if (_timerDeltaNotification == null)
			//    {
			//        if (Configuration.LBM.Default.UseChangeNotificationRate)
			//        {
			//            _timerDeltaNotification = new System.Threading.Timer(_timerDeltaNotificationDelagate, null, 0, Configuration.LBM.Default.ChangeNotificationRate);
			//        }
			//        else
			//        {
			//            _timerDeltaNotification = new System.Threading.Timer(_timerDeltaNotificationDelagate, null, 0, 100);
			//        }
			//    }
			//}

			if (!_threadDeltaRecapProcessStarted && !_stopping)
			{
				_threadDeltaRecapProcessShouldStop = false;

				_threadDeltaRecapProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessSRLabDeltaRecapQueue));
				_threadDeltaRecapProcess.Name = "MDSRecapProcess";
				_threadDeltaRecapProcess.Start();

				while (!_threadDeltaRecapProcess.IsAlive) ;
				_threadDeltaRecapProcessStarted = true;
			}
		}

		private void ProcessSRLabDeltaRecapQueue()
		{
			while (!_threadDeltaRecapProcessShouldStop)
			{
				StartPerformanceLog();

				//Recap is always checked even when there is no delta
				GLOBAL.HMDS.ProcessRecapQueue();

				System.Threading.Thread.Sleep(_deltaRecapTimeGap);
			}
		}

		private void StopDeltaRecapNotification()
		{
			//if (_timerDeltaNotification != null)
			//{
			//    _timerDeltaNotification.Dispose();
			//}
			//_timerDeltaNotification = null;
			//_timerDeltaNotificationDelagate = null;

			_threadDeltaRecapProcessShouldStop = true;
			if (_threadDeltaRecapProcess != null)
			{
				while (_threadDeltaRecapProcess.IsAlive) ;
			}
		}

		#endregion

		#region - Stop All Threads -

		public void Stop()
		{
			_stopping = true;

			StopSRLabSubscription();
			StopDeltaRecapNotification();
			StopSRLabThread();

			_stopping = false;
		}

		#endregion

		#region - Set Status Display, Log and Alert -

		private void SetStatus(StatusTypes type, string statusDsp)
		{
			SetStatus(type, statusDsp, true);
		}
		private void SetStatus(StatusTypes type, string statusDsp, bool addTolog)
		{
			Status = type;
			StatusDsp = statusDsp;

			if (addTolog)
			{
				AddToStatusLogs(statusDsp);
			}
		}

		#endregion

		#region - Performance Log -

		private void StartPerformanceLog()
		{
			if (_logPerformance)
			{
				_pTime = _dtHP.Now;
				_pTimeSpan = new TimeSpan();
			}
		}

		private void PerformanceLog(string header, string details)
		{
			if (_logPerformance)
			{
				_pTimeSpan = _dtHP.Now.Subtract(_pTime);
				AddToSRLabLog(string.Format("{0,30} {1,-8}ms| {2}", header, _pTimeSpan.TotalMilliseconds, details));
			}
		}

		#endregion

		#region - SRLab Log -

		public void AddToSRLabLog(string msg)
		{
			if (Configuration.SRLab.Default.EnableSRLabLog)
			{
				lock (SyncObj)
				{
					SRLabLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("hh:mm:ss.ffffff"), " ", msg }));
				}
			}
		}

		public void LogSRLabMessages()
		{
			string[] msgs = new string[0];
			lock (SyncObj)
			{
				if (SRLabLogs.Count > 0)
				{
					msgs = SRLabLogs.ToArray();
					SRLabLogs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.LOG.SRLab.Out(msg, false);
			}
		}

		#endregion

		#region - Force GC -

		public void ForceGC()
		{
			_srLabWrapper.ForceGC();
		}

		#endregion
	}
}
