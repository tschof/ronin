using System;
using System.Diagnostics;
using System.ComponentModel;
using System.Collections.Generic;

using LBMEx;
using MarketDataEx;
using SerializationEx;
using DateTimeEx;
using CLBMEx;

namespace MDS
{
	public class HelperLBM : HelperStatusBase
	{
		public enum StatusTypes
		{
			Connected,
			Connecting,
			DisConnecting,
			NotConnected,
		}

		public sealed class SubscriptionLists
		{
			public SubscriptionLists()
			{
				SubLists = new List<MDClientToServer>();
				SubKeyLists = new List<string>();
			}

			public List<MDClientToServer> SubLists;
			// Key = Symbol + Source
			public List<string> SubKeyLists;
		}

		#region - LBM Main Thread -

		private bool _threadLBMStarted = false;
		private System.Threading.Thread _threadLBM;

		#endregion

		#region - LBM Subscribe Thread -

		private bool _threadSubscriptionProcessShouldStop = false;
		private bool _threadSubscriptionProcessStarted = false;
		private System.Threading.Thread _threadSubscriptionProcess;

		// Key souce, value subs
		private Dictionary<string, SubscriptionLists> _subLists = new Dictionary<string, SubscriptionLists>();

		#endregion

		#region - LBM Delta & Recap Thread -

		//private System.Threading.Timer _timerDeltaNotification;
		//private System.Threading.TimerCallback _timerDeltaNotificationDelagate;

		private bool _threadDeltaRecapProcessShouldStop = false;
		private bool _threadDeltaRecapProcessStarted = false;
		private System.Threading.Thread _threadDeltaRecapProcess;
		
		private Dictionary<string, MDServerToClient> _latestDeltas = new Dictionary<string, MDServerToClient>(); 

		#endregion

		#region - MDS Client Cleanup Thread -

		private bool _threadCleanupProcessShouldStop = false;
		private bool _threadCleanupProcessStarted = false;
		private System.Threading.Thread _threadCleanupProcess;

		#endregion

		#region - LBM Source Watch Time -

		private System.Threading.Timer _timerSourceWatchNotification;
		private System.Threading.TimerCallback _timerSourceWatchNotificationDelagate;

		#endregion

		#region - Local Variable -

		private bool _stopping = false;

		private bool _enableLBMLog = false;

		private DateTime _pTime = DateTime.Now;
		private TimeSpan _pTimeSpan = new TimeSpan();

		private bool _logPerformance = false;

		private DateTimeHP _dtHP = new DateTimeHP();

		private int _subscriptionTimeGap = 100;
		private int _deltaRecapTimeGap = 100;

		private LBMWrapper _lbmWrapper;

		private object _syncObj = new object();

		#endregion

		#region - Latency Check -

		// Key = Symbol + Source 
		private List<string> _monitorKeys = new List<string>();
		// Key = Symbol + Source and the first linetime to current time delta;
		private Dictionary<string, double> _monitorBaseLineDelta = new Dictionary<string, double>();

		// Key = Symbol + Source and List of Deltas (for basline calculation 
		private Dictionary<string, List<double>> _monitorBaseLineDeltaLists = new Dictionary<string, List<double>>();

		private int _latencyLimit = 1000;
		private int _driftLimit = 100;
		private bool _checkLatency = false;

		#endregion

		#region - Source Start Time -

		private Dictionary<string, DateTime> _sourceBlockTimes = new Dictionary<string, DateTime>();
		private List<string> _sourceAvailableList = new List<string>();
		private Dictionary<string, bool> _sourceIsBlockedList = new Dictionary<string, bool>();
		private bool _noMoreBlocking = false;

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

		private List<string> _lbmLogs;
		public List<string> LBMLogs
		{
			get
			{
				if (_lbmLogs == null)
				{
					_lbmLogs = new List<string>();
				}
				return _lbmLogs;
			}
			set
			{
				_lbmLogs = value;
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
				return _lbmWrapper.Subscriptions ;
			}
		}

		// Key = Symbol + Source
		public List<string> MamaSubscriptionsCreated
		{
			get
			{
				return _lbmWrapper.MamaSubscriptionsCreated;
			}
		}

		// Source Start Time for TODAY
		public Dictionary<string, DateTime> SourceBlockTimes
		{
			get
			{
				if (_sourceBlockTimes == null)
				{
					_sourceBlockTimes = new Dictionary<string, DateTime>();
				}
				return _sourceBlockTimes;
			}
		}

		// Sources Avaliable for TODAY
		public List<string> SourceAvailableList
		{
			get
			{
				if (_sourceAvailableList == null)
				{
					_sourceAvailableList = new List<string>();
				}
				return _sourceAvailableList;
			}
		}

		#endregion

		#region - Constructor -

		public HelperLBM()
		{
			_logPerformance = Configuration.User.Default.LogPerformance;

			_enableLBMLog = Configuration.LBM.Default.EnableLBMLog;

			_subscriptionTimeGap = Configuration.LBM.Default.SubscriptionTimeGap;
			_deltaRecapTimeGap = Configuration.LBM.Default.ChangeNotificationRate;

			LoadSchedules(DateTime.Now.DayOfWeek);

			LoadMonitorLatencySettings();

			if (_lbmWrapper == null && CheckSupportFile())
			{
				if (Configuration.LBM.Default.Middleware.ToUpper() == "LBM" ||
					Configuration.LBM.Default.Middleware.ToUpper() == "WMW")
				{
					_lbmWrapper = new LBMWrapper
						(
							Configuration.LBM.Default.Middleware,
							Configuration.LBM.Default.MaxRetries,
							Configuration.LBM.Default.IdelTimeBetweenRetries,
							Configuration.LBM.Default.EnableMamaLog,
							Configuration.LBM.Default.MamaLogPath,
							Configuration.LBM.Default.TransportName,
							Configuration.LBM.Default.DictionarySourceSymbolNamespace,
							Configuration.LBM.Default.ThrottleDefault,
							Configuration.LBM.Default.ThrottleInitial,
							Configuration.LBM.Default.ThrottleRecap,
							Configuration.LBM.Default.ChangeNotificationRate,
							Configuration.LBM.Default.UseChangeNotificationRate,
							Configuration.LBM.Default.SubscriptionUseWaitForSymbol,
							Configuration.LBM.Default.MamaMsgQueueLimitPerSymbol,
							Configuration.LBM.Default.SubscriptionSourceRange,
							Configuration.LBM.Default.SubscriptionSymbolRange,
							Configuration.LBM.Default.BookDepthLimit,
							Configuration.LBM.Default.StockSources,
							Configuration.LBM.Default.FutureSources,
							Configuration.LBM.Default.OptionSources,
							Configuration.LBM.Default.SetRecoverGaps,
							Configuration.User.Default.RawQueueLimit,
							Configuration.User.Default.LogDropMessageCount
						);
					_lbmWrapper.MarketDataChanged += new MarketDataChangedEventHandler(MarketDataChanged);
				}
				else
				{
					AddToLBMLog("Invalid Middleware");
				}
			}
			else
			{
				AddToLBMLog("Missing Support Folders LBM/WMW");
			}
		}

		private bool CheckSupportFile()
		{
			if (Configuration.LBM.Default.Middleware.ToUpper() == "LBM")
			{
				return CopySupportFile(Configuration.Path.Default.LbmPath.Replace(@"..", ""));
			}
			else if (Configuration.LBM.Default.Middleware.ToUpper() == "WMW")
			{
				return CopySupportFile(Configuration.Path.Default.WmwPath.Replace(@"..", ""));
			}

			return false;
		}

		private bool CopySupportFile(string path)
		{
			try
			{
				path = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath) + path;
				if (System.IO.Directory.Exists(path))
				{
					string[] files = System.IO.Directory.GetFiles(path);
					string targetPath = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);

					string fileName = "";
					string destFile = "";
					// Copy the files and overwrite destination files if they already exist.
					foreach (string file in files)
					{
						// Use static Path methods to extract only the file name from the path.
						fileName = System.IO.Path.GetFileName(file);
						destFile = System.IO.Path.Combine(targetPath, fileName);
						System.IO.File.Copy(file, destFile, true);
					}

					return true;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}

			return false;
		}

		private void LoadSchedules()
		{
			//DateTime tDT;
			//_sourceBlockTimes = new Dictionary<string, DateTime>();
			//if (!DateTime.TryParseExact(Configuration.Scheduler.Default.CTA, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
			//{
			//    Configuration.Scheduler.Default.CTA = "06:05:00.000";
			//    DateTime.TryParseExact(Configuration.Scheduler.Default.CTA, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
			//}
			//_sourceBlockTimes.Add("CTA", tDT);

			//if (!DateTime.TryParseExact(Configuration.Scheduler.Default.NASDAQ, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
			//{
			//    Configuration.Scheduler.Default.NASDAQ = "06:05:00.000";
			//    DateTime.TryParseExact(Configuration.Scheduler.Default.NASDAQ, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
			//}
			//_sourceBlockTimes.Add("NASDAQ", tDT);

			//if (!DateTime.TryParseExact(Configuration.Scheduler.Default.CME, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
			//{
			//    Configuration.Scheduler.Default.CME = "00:30:00.000";
			//    DateTime.TryParseExact(Configuration.Scheduler.Default.CME, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
			//}
			//_sourceBlockTimes.Add("CME", tDT);

			//if (!DateTime.TryParseExact(Configuration.Scheduler.Default.ICE, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
			//{
			//    Configuration.Scheduler.Default.ICE = "00:30:00.000";
			//    DateTime.TryParseExact(Configuration.Scheduler.Default.ICE, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
			//}
			//_sourceBlockTimes.Add("ICE", tDT);

			//if (!DateTime.TryParseExact(Configuration.Scheduler.Default.OPRANBBO, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
			//{
			//    Configuration.Scheduler.Default.OPRANBBO = "06:05:00.000";
			//    DateTime.TryParseExact(Configuration.Scheduler.Default.OPRANBBO, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
			//}
			//_sourceBlockTimes.Add("OPRA", tDT);
			//_sourceBlockTimes.Add("OPRANBBO", tDT);
		}

		private void LoadSchedules(DayOfWeek day)
		{
			System.Data.DataRow[] rows = HelperScheduler.ScheduleTable.Select("DayOfWeek = '" + day + "'");

			_sourceBlockTimes = new Dictionary<string, DateTime>();
			
			DateTime tDT;
			string sourceName = "";
			foreach (System.Data.DataRow row in rows)
			{
				if (row["Source"] != null && row["Source"].ToString() != "")
				{
					sourceName = row["Source"].ToString().ToUpper();
				}
				if (sourceName != "" && DateTime.TryParseExact(row["Time"].ToString(), TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
				{
					if (!_sourceBlockTimes.ContainsKey(sourceName))
					{
						// Source Block Time List
						_sourceBlockTimes.Add(sourceName, tDT);
						if (sourceName == "OPRANBBO")
						{
							if (!_sourceBlockTimes.ContainsKey("OPRA"))
							{
								_sourceBlockTimes.Add("OPRA", tDT);
							}
						}
					}
					if (!_sourceAvailableList.Contains(sourceName))
					{
						// Source Avliable List
						_sourceAvailableList.Add(sourceName);
						if (sourceName == "OPRANBBO")
						{
							if (!_sourceAvailableList.Contains("OPRA"))
							{
								_sourceAvailableList.Add("OPRA");
							}
						}
					}
				}
			}
		}

		private void LoadMonitorLatencySettings()
		{
			string[] monitorKeys = Configuration.User.Default.MonitorKeys.ToUpper().Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries);
			_monitorKeys.AddRange(monitorKeys);
			_latencyLimit = Configuration.User.Default.LatencyLimit;
			_driftLimit = Configuration.User.Default.DriftLimit;
			_checkLatency = Configuration.User.Default.CheckLatency;
		}

		#endregion

		#region - LBM Event -

		private void MarketDataChanged(object sender, LBMDataChangedEventArgs e)
		{
			switch (e.Type)
			{
				case LBMDataChangedEventTypes.OnInitializeFailed:
					_lbmWrapper.Stop();
					SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "LBM|OnInitializeFailed Wombat Connection Failed." }));
					GLOBAL.HMail.EmailLBMException("LBM|OnInitializeFailed Wombat Connection Failed.");
					break;
				case LBMDataChangedEventTypes.OnInitialized:
					SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnInitialized Wombat Connected." }));
					//GLOBAL.HMail.EmailLBMException("LBM|OnInitialized Wombat Connected.");
					break;
				case LBMDataChangedEventTypes.OnInitializeError:
					_lbmWrapper.Stop();
					SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "LBM|OnInitializeError ", e.Message }));
					GLOBAL.HMail.EmailLBMException(String.Concat(new object[] { "LBM|OnInitializeError ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnStop:
					SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "LBM|OnStop Wombat Stopped." }));
					break;
				case LBMDataChangedEventTypes.OnSubscriptionFailed:
					SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnSubscriptionFailed ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnSubscriptionGap:
					SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnSubscriptionGap ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnSubscriptionQuality:
					SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnSubscriptionQuality ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnSubscriptionRecapRequest:
					SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnSubscriptionRecapRequest ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnExecption:
					_lbmWrapper.Stop();
					SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "LBM|Exception ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnWarring:
					AddToLBMWarringLog(String.Concat(new object[] { "LBM|OnWarring ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnSubscribed:
					SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnSubscribed ", e.Message }));
					break;
				case LBMDataChangedEventTypes.OnSubscriptionMsg:
					if (e.Message != null && e.Message != "")
					{
						SetStatus(StatusTypes.Connected, String.Concat(new object[] { "LBM|OnMsg ", e.Message }));
					}
					else
					{
						MarketDataChanged(e.Change);
					}
					break;
			}
		}

		private void MarketDataChanged(MDServerToClient delta)
		{
			StoreTotalMsgCount();

			string key = delta.uSubscriptionKey;
			if (key != "")
			{
				UpdateLatestData(key, delta);
				GLOBAL.HMDS.ProcessDeltaQueue(delta);
				
				MonitorLatency(key, delta);
			}


			PerformanceLog("LBM|ProcessLBMDelta", string.Format("{0,-12}| Changes[{1,3}] - {2,10}| {3,8}ms", "DecodeTime", delta.Count, delta.IssueSymbol, delta.uDecodedTime));
		}

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

		private void MonitorLatency(string key, MDServerToClient delta)
		{
			if (_checkLatency && _monitorKeys.Contains(key))
			{
				if (delta.LineTime != "" && delta.uOnMsgTime != null)
				{
					DateTime tDT;
					if (DateTime.TryParseExact(delta.LineTime, TimeFormats.Formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
					{
						TimeSpan tTS = ((DateTime)delta.uOnMsgTime).Subtract(tDT.ToLocalTime());
						if (_monitorBaseLineDeltaLists.ContainsKey(key))
						{
							if (_monitorBaseLineDeltaLists[key].Count > _driftLimit)
							{
								double total = 0;
								foreach (double d in _monitorBaseLineDeltaLists[key])
								{
									total = total + d;
								}
								if (!_monitorBaseLineDelta.ContainsKey(key))
								{
									_monitorBaseLineDelta.Add(key, total / _monitorBaseLineDeltaLists[key].Count);
								}
							}
							else
							{
								_monitorBaseLineDeltaLists[key].Add(tTS.TotalMilliseconds);
							}
						}
						else
						{
							_monitorBaseLineDeltaLists.Add(key, new List<double>());
							_monitorBaseLineDeltaLists[key].Add(tTS.TotalMilliseconds);
						}

						if (_monitorBaseLineDelta.ContainsKey(key))
						{
							double lag = Math.Abs(_monitorBaseLineDelta[key] - tTS.TotalMilliseconds);
							if (lag > _latencyLimit || delta.uDecodedTime > _latencyLimit)
							{
								// Log as Latency
								AddToLBMWarringLog(string.Format("LINE -> OnMsg {0,10} {1,8}ms| {2}", key, lag.ToString("F3"), tDT.ToLocalTime().ToString("HH:mm:ss.fff")));
								AddToLBMWarringLog(string.Format("OnMsg -> SENT {0,10} {1,8}ms| {2}", key, ((double)delta.uDecodedTime).ToString("F3"), ((DateTime)delta.uOnMsgTime).ToString("HH:mm:ss.fff")));
							}
						}
					}
				}
			}
		}

		#endregion

		#region - LBM Main Thread -

		public void Start()
		{
			TotoalDataMsgCount = 0;

			StartLBMSubscription();
			StartDeltaRecapNotification();
			StartSourceWatchNotification();
			StartCleanupProcess();
			StartLBMThread();
		}

		private void StartLBMThread()
		{
			if (!_threadLBMStarted && !_stopping)
			{
				_threadLBM = new System.Threading.Thread(new System.Threading.ThreadStart(ProccessLBM));
				_threadLBM.Name = "ProcessLBM";
				_threadLBM.Start();

				while (!_threadLBM.IsAlive) ;
				_threadLBMStarted = true;
			}
		}
		
		private void ProccessLBM()
		{
			_lbmWrapper.Start();
			_threadLBMStarted = false;
		}

		private void StopLBMThread()
		{
			_lbmWrapper.Stop();
		}

		#endregion

		#region - LBM Subscribe Thread -

		public void Subscribe(MDClientToServer sub)
		{
			if (Status == StatusTypes.Connected)
			{
				if (_sourceAvailableList.Contains(sub.MDDataSource))
				{
					lock (_subLists)
					{
						if (!_subLists.ContainsKey(sub.MDDataSource))
						{
							SubscriptionLists list = new SubscriptionLists();
							_subLists.Add(sub.MDDataSource, new SubscriptionLists());
							_subLists[sub.MDDataSource].SubKeyLists = new List<string>();
							_subLists[sub.MDDataSource].SubLists = new List<MDClientToServer>();
						}
						if (!_subLists[sub.MDDataSource].SubKeyLists.Contains(sub.MDSymbol))
						{
							AddToStatusLogs(string.Format("{0,30} {1} {2}", "Add To Subscription Queue", sub.MDSymbol, sub.MDDataSource));
							_subLists[sub.MDDataSource].SubKeyLists.Add(sub.MDSymbol);
							_subLists[sub.MDDataSource].SubLists.Add(sub);
						}
					}
				}
			}
		}

		private void StartLBMSubscription()
		{
			if (!_threadSubscriptionProcessStarted && !_stopping)
			{
				_threadSubscriptionProcessShouldStop = false;

				_threadSubscriptionProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessLBMSubscriptionQueue));
				_threadSubscriptionProcess.Name = "MDSSubscriptionProcess";
				_threadSubscriptionProcess.Start();

				while (!_threadSubscriptionProcess.IsAlive) ;
				_threadSubscriptionProcessStarted = true;
			}
		}

		private void ProcessLBMSubscriptionQueue()
		{
			MDClientToServer[] mdss;

			while (!_threadSubscriptionProcessShouldStop)
			{
				if (!_noMoreBlocking)
				{
					GetSubscriptionQueueWithBlockCheck(out mdss);
				}
				else
				{
					GetSubscriptionQueue(out mdss);
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
								_lbmWrapper.SubscribeBook(mds.MDSymbol, mds.MDDataSource, mds.Reload);
								break;
							case "L2":
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "L2 Subscribing... To", mds.MDSymbol, mds.MDDataSource));
								_lbmWrapper.SubscribeL2(mds.MDSymbol, mds.MDDataSource, mds.Reload);
								break;
							default:
								AddToStatusLogs(string.Format("{0,30} {1} {2}", "Subscribing... To", mds.MDSymbol, mds.MDDataSource));
								_lbmWrapper.Subscribe(mds.MDSymbol, mds.MDDataSource, mds.Reload);
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

		private void GetSubscriptionQueueWithBlockCheck(out MDClientToServer[] mdss)
		{
			mdss = new MDClientToServer[0];
			Dictionary<string, bool> locSourceIsBlockedList = new Dictionary<string, bool>();

			// There is still blocking
			lock (_sourceIsBlockedList)
			{
				locSourceIsBlockedList = new Dictionary<string, bool>(_sourceIsBlockedList);
			}

			List<MDClientToServer> locSubList = new List<MDClientToServer>();
			foreach (string key in locSourceIsBlockedList.Keys)
			{
				switch (key.ToUpper())
				{
					case "OPRA":
						break;
					default:
						lock (_subLists)
						{
							if (!locSourceIsBlockedList[key] && _subLists.ContainsKey(key) && _subLists[key].SubLists.Count > 0)
							{
								// Not Blocked Anymore
								locSubList.AddRange(_subLists[key].SubLists.ToArray());
								_subLists[key].SubKeyLists.Clear();
								_subLists[key].SubLists.Clear();
							}
						}
						break;
				}
			}
			mdss = locSubList.ToArray();

			if (mdss.Length == 0)
			{
				if (_sourceIsBlockedList.ContainsKey("OPRA"))
				{
					lock (_subLists)
					{
						if (!locSourceIsBlockedList["OPRA"] && _subLists.ContainsKey("OPRA") && _subLists["OPRA"].SubLists.Count > 0)
						{
							// Not Blocked Anymore
							mdss = _subLists["OPRA"].SubLists.ToArray();
							_subLists["OPRA"].SubKeyLists.Clear();
							_subLists["OPRA"].SubLists.Clear();
						}
					}
				}
			}
		}

		private void GetSubscriptionQueue(out MDClientToServer[] mdss)
		{
			mdss = new MDClientToServer[0];

			lock (_subLists)
			{
				List<MDClientToServer> locSubList = new List<MDClientToServer>();
				foreach (string key in _subLists.Keys)
				{
					switch (key)
					{
						case "OPRA":
							break;
						default:
							if (_subLists.ContainsKey(key) && _subLists[key].SubLists.Count > 0)
							{
								// Not Blocked Anymore
								locSubList.AddRange(_subLists[key].SubLists.ToArray());
								_subLists[key].SubKeyLists.Clear();
								_subLists[key].SubLists.Clear();
							}
							break;
					}
				}
				mdss = locSubList.ToArray();

				if (mdss.Length == 0)
				{
					if (_subLists.ContainsKey("OPRA") && _subLists["OPRA"].SubLists.Count > 0)
					{
						// Not Blocked Anymore
						mdss = _subLists["OPRA"].SubLists.ToArray();
						_subLists["OPRA"].SubKeyLists.Clear();
						_subLists["OPRA"].SubLists.Clear();
					}
				}
			}
		}

		private void StopLBMSubscription()
		{
			_threadSubscriptionProcessShouldStop = true;
			if (_threadSubscriptionProcess != null)
			{
				while (_threadSubscriptionProcess.IsAlive) ;
			}
		}

		#endregion

		#region - LBM Delta & Recap Notification Thread -

		private void StartDeltaRecapNotification()
		{
			if (!_threadDeltaRecapProcessStarted && !_stopping)
			{
				_threadDeltaRecapProcessShouldStop = false;

				_threadDeltaRecapProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessLBMDeltaRecapQueue));
				_threadDeltaRecapProcess.Name = "MDSRecapProcess";
				_threadDeltaRecapProcess.Start();

				while (!_threadDeltaRecapProcess.IsAlive) ;
				_threadDeltaRecapProcessStarted = true;
			}
		}

		private void ProcessLBMDeltaRecapQueue()
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
			_threadDeltaRecapProcessShouldStop = true;
			if (_threadDeltaRecapProcess != null)
			{
				while (_threadDeltaRecapProcess.IsAlive) ;
			}
		}

		#endregion

		#region - MDS Client Cleanup Thread -

		private void StartCleanupProcess()
		{
			if (!_threadCleanupProcessStarted && !_stopping)
			{
				_threadCleanupProcessShouldStop = false;

				_threadCleanupProcess = new System.Threading.Thread(new System.Threading.ThreadStart(CleanupProcess));
				_threadCleanupProcess.Name = "MDSCleanupProcess";
				_threadCleanupProcess.Start();

				while (!_threadCleanupProcess.IsAlive) ;
				_threadCleanupProcessStarted = true;
			}
		}

		private void CleanupProcess()
		{
			while (!_threadCleanupProcessShouldStop)
			{
				// Clean Up Closed Connections.
				GLOBAL.HMDS.ProcessCleanUP();

				System.Threading.Thread.Sleep(1000);
			}
		}

		private void StopCleanupProcess()
		{
			_threadCleanupProcessShouldStop = true;
			if (_threadCleanupProcess != null)
			{
				while (_threadCleanupProcess.IsAlive) ;
			}
		}

		#endregion

		#region - LBM Source Start Time Notification Thread -

		private void StartSourceWatchNotification()
		{
			if (!_stopping)
			{
				if (_timerSourceWatchNotificationDelagate == null)
				{
					_timerSourceWatchNotificationDelagate = new System.Threading.TimerCallback(ProcessSourceWatchQueue);
				}

				if (_timerSourceWatchNotification == null)
				{
					_timerSourceWatchNotification = new System.Threading.Timer(_timerSourceWatchNotificationDelagate, null, 0, 1000);
				}
			}
		}

		private void ProcessSourceWatchQueue(object input)
		{
			Dictionary<string, bool> locSourceIsBlockedList = new Dictionary<string, bool>();
			lock (_sourceBlockTimes)
			{
				foreach (string key in _sourceBlockTimes.Keys)
				{
					locSourceIsBlockedList.Add(key, true);
					if (_sourceBlockTimes[key] < DateTime.Now)
					{
						locSourceIsBlockedList[key] = false;
					}
				}
			}

			int blockCount = 0;
			foreach (bool val in locSourceIsBlockedList.Values)
			{
				if (val)
				{
					// there is still blocking
					blockCount = blockCount + 1;
					break;
				}
			}

			lock (_sourceIsBlockedList)
			{
				_sourceIsBlockedList = new Dictionary<string, bool>(locSourceIsBlockedList);
			}

			if (blockCount == 0)
			{
				// No need to check time anymore
				StopSourceWatchNotification();
				_noMoreBlocking = true;
			}
			else
			{
				_noMoreBlocking = false;
			}
		}

		private void StopSourceWatchNotification()
		{
			if (_timerSourceWatchNotification != null)
			{
				_timerSourceWatchNotification.Dispose();
			}
			_timerSourceWatchNotification = null;
			_timerSourceWatchNotificationDelagate = null;
		}

		#endregion

		#region - Stop All Threads -

		public void Stop()
		{
			_stopping = true;

			StopLBMSubscription();
			StopDeltaRecapNotification();
			StopSourceWatchNotification();
			StopCleanupProcess();
			StopLBMThread();

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
				AddToLBMLog(string.Format("{0,30} {1,-8}ms| {2}", header, _pTimeSpan.TotalMilliseconds, details));
			}
		}

		#endregion

		#region - LBM Log -

		public void AddToLBMWarringLog(string msg)
		{
			lock (SyncObj)
			{
				LBMLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", msg }));
			}
		}

		public void AddToLBMLog(string msg)
		{
			if (_enableLBMLog)
			{
				lock (SyncObj)
				{
					LBMLogs.Add(String.Concat(new object[] { _dtHP.Now.ToString("HH:mm:ss.fffffff"), " ", msg }));
				}
			}
		}

		public void LogLBMMessages()
		{
			string[] msgs = new string[0];
			lock (SyncObj)
			{
				if (LBMLogs.Count > 0)
				{
					msgs = LBMLogs.ToArray();
					LBMLogs.Clear();
				}
			}

			foreach (string msg in msgs)
			{
				GLOBAL.LOG.LBM.Out(msg, false);
			}
		}

		#endregion

		#region - Force GC -

		public void ForceGC()
		{
			_lbmWrapper.ForceGC();
		}

		#endregion
	}
}
