using System;
using System.Collections.Generic;
using System.Text;
using CSMD;
using MarketDataEx;
using DateTimeEx;

namespace SRLabEx
{
	public delegate void SRLabChangedEventHandler(object sender, SRLabChangedEventArgs e);

	#region - Subscription Fields -

	// Used for varification
	public class SubscriptionFields
	{
		private string _symbol = "";
		private string _source = "";
		private string _type = "";

		public SubscriptionFields(string symbol, string source, string type)
		{
			_symbol = symbol;
			_source = source;
			_type = type;
		}

		public string Symbol
		{
			get
			{
				return _symbol;
			}
		}

		public string Source
		{
			get
			{
				return _source;
			}
		}

		public string Type
		{
			get
			{
				return _type;
			}
		}
	}

	#endregion

	public class SRLabBase : MDListener, IDisposable
	{
		#region - Event -

		public event SRLabChangedEventHandler SRLabChanged;
		private void OnSRLabChanged(SRLabChangedEventArgs e)
		{
			if (SRLabChanged != null)
			{
				SRLabChanged(this, e);
			}
		}

		#endregion

		private sealed class SecTypes
		{
			public const string E = "E";
			public const string F = "F";
			public const string O = "O";
		}

		private enum DataRollTypes
		{
			Book,
			Primary,
		}

		#region - Local Static Variables -

		private static char[] _splitChars = new char[] { ',' };

		private static SRLabBase _gHandle;
		private static bool _isShutdown;

		private static DateTimeHP _dtHP = new DateTimeHP();

		private const string _ctaRollSource = "CTA";
		private const string _nasdaqRollSource = "NASDAQ";

		#endregion

		#region - Local Variables -

		private SRLabEnviromentValues _environment;
		private SRLabEnviromentValues environment
		{
			get
			{
				return _environment;
			}
		}

		private string _configlocation = "";
		private string _smType = "";
		private int _depth = 5;

		#region - Source and Symbol Subscription Filter -

		private List<string> _soruceList = new List<string>();
		private List<string> _symbolList = new List<string>();

		#endregion

		// Sources by Type
		private List<string> _stockSources = new List<string>();
		private List<string> _futureSources = new List<string>();
		private List<string> _optionSources = new List<string>();

		private int _changeNotificationRate = 100;
		private int _lineTimeHourOffset = -1;
		private int _lineTimeMinOffset = 0;
		private int _lineTimeSecOffset = 0;

		private bool _subscriptionUseWaitForSymbol = false;
		private bool _subscribeWithSource = true;

		private bool _forceGC = false;

		private MDApplication _application;

		private List<string> _smNames;

		private Dictionary<string, MDSessionMgr> _sessions;
		private List<string> _startedSessions;

		private string _ctaPrimary = "N";
		private string _nasdaqPrimary = "Q";

		#endregion

		#region - Queue & Decode Logic -

		private static object _syncObj = new object();

		private bool _useChangeNotificationRate = false;

		// Key = Symbol + Source
		private Dictionary<string, OnDeltaObject> _changeQueues = new Dictionary<string, OnDeltaObject>();

		// This should olny be used for Inital Snap Shots
		private List<OnDeltaObject> _criticalQueues = new List<OnDeltaObject>();

		private class OnDeltaObject
		{
			public DateTime RecivedTime
			{
				get
				{
					if (Deltas.uOnMsgTime != null)
					{
						return (DateTime)Deltas.uOnMsgTime;
					}

					return DateTime.Now;
				}
			}

			private string _source = "";
			public string Source
			{
				get
				{
					return _source;
				}
			}

			private string _symbol = "";
			public string Symbol
			{
				get
				{
					return _symbol;
				}
			}

			private string _type;
			public string Type
			{
				get
				{
					return _type;
				}
			}

			public string SecType
			{
				get
				{
					switch (Source)
					{
						case "CTA":
						case "NASDAQ":
							return SecTypes.E;
						case "OPRA":
						case "OPRANBBO":
							return SecTypes.O;
						case "ICE":
						default:
							return SecTypes.F;
					}
				}
			}

			public string Key
			{
				get
				{
					return string.Format("{0},{1}", new object[] { Symbol, Source });
				}
			}

			private MDServerToClient _deltas;
			public MDServerToClient Deltas
			{
				get
				{
					if (_deltas == null)
					{
						_deltas = new MDServerToClient();
					}
					return _deltas;
				}
				set
				{
					_deltas = value;
				}
			}

			public OnDeltaObject(string symbol, string source, string type)
			{
				Initialize(symbol, source, type);
			}

			private void Initialize(string symbol, string source, string type)
			{
				if (Deltas.uOnMsgTime == null)
				{
					Deltas.Update(WombatFieldIDs.uOnMsgTime, DateTime.Now.ToOADate());
				}
				Deltas.Update(WombatFieldIDs.SubscriptionSymbol, symbol);

				//CME,CME-CBOT,CME-MGEX,CME-NYMEX
				switch (source)
				{
					case "CME-CBOT":
					case "CME-MGEX":
					case "CME-NYMEX":
						source = "CME";
						break;
					case "OPRA":
						if (!symbol.Contains("."))
						{
							source = "OPRANBBO";
						}
						break;
					case "OPRANBBO":
						if (symbol.Contains("."))
						{
							source = "OPRA";
						}
						break;
				}
				Deltas.Update(WombatFieldIDs.SubscriptionSoruce, source);

				_source = source;
				_symbol = symbol;
				_type = type;
			}

			// TODO Remove this after Wombat Data Formate is removed from ROC
			public OnDeltaObject(OnDeltaObject src, DataRollTypes type)
			{
				switch (type)
				{
					case DataRollTypes.Primary:
						_symbol = src.Symbol;
						_source = src.Source;
						_type = src.SecType;

						Deltas.Update(src.Deltas);
						Deltas.Update(WombatFieldIDs.SubscriptionSymbol, _symbol);
						Deltas.Update(WombatFieldIDs.IssueSymbol, _symbol);
						break;
					case DataRollTypes.Book:
						_symbol = "b" + src.Symbol;
						_source = src.Source;
						_type = src.SecType;

						Deltas.Update(src.Deltas);
						Deltas.Update(WombatFieldIDs.SubscriptionSymbol, _symbol);
						Deltas.Update(WombatFieldIDs.IssueSymbol, _symbol);
						break;
				}

			}
		}

		#endregion

		#region - Public Properties -

		// For Play back and resubscribe only 
		// Key = Symbol + Source
		private Dictionary<string, SubscriptionFields> _subscriptions = new Dictionary<string, SubscriptionFields>();
		public Dictionary<string, SubscriptionFields> Subscriptions
		{
			get
			{
				return _subscriptions;
			}
		}

		// Key = Symbol + Source
		private Dictionary<string, SRLabInstruments> _instrumentsBySource;
		public Dictionary<string, SRLabInstruments> InstrumentsBySource
		{
			get
			{
				if (_instrumentsBySource == null)
				{
					_instrumentsBySource = new Dictionary<string, SRLabInstruments>();
				}
				return _instrumentsBySource;
			}
		}

		private List<string> _subscriptionsCreated;
		public List<string> SubscriptionsCreated
		{
			get
			{
				if (_subscriptionsCreated == null)
				{
					_subscriptionsCreated = new List<string>();
				}
				return _subscriptionsCreated;
			}
		}

		#endregion

		#region - Constructor -

		public SRLabBase(
			string startupPath,
			string smNames,
			string smType,
			string configlocation,
			string configFile,
			int depth,
			string localIP,
			bool delayMCJoin,
			bool fireLevel1Only,
			bool lastValue,
			string logLevel,
			string maxLogSize,
			string sourceRange,
			string symbolRange,
			string stockSources,
			string futureSources,
			string optionSources,
			bool busyWait,
			int changeNotificationRate,
			int lineTimeHourOffset,
			int lineTimeMinOffset,
			int lineTimeSecOffset,
			bool useChangeNotificationRate,
			bool subscriptionUseWaitForSymbol,
			bool subscribeWithSource)
		{
			if (_environment == null)
			{
				_environment = new SRLabEnviromentValues(
					startupPath,
					smNames,
					smType,
					configlocation,
					configFile,
					depth,
					localIP,
					delayMCJoin,
					fireLevel1Only,
					lastValue,
					logLevel,
					maxLogSize,
					busyWait);
				_environment.SRLabEnviromentChanged+= new SRLabEnviromentChangedEventHandler(SRLabEnviromentChanged);
			}

			#region - Source & Symbol Filter -

			// accepted sources
			if (sourceRange != "" && sourceRange.ToUpper() != "ALL")
			{
				string[] sources = sourceRange.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_soruceList.AddRange(sources);
				}
			}
			// accepted symbols
			if (symbolRange != "" && symbolRange.ToUpper() != "ALL")
			{
				string[] symbols = symbolRange.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (symbols.Length > 0)
				{
					_symbolList.AddRange(symbols);
				}
			}
			// stock sources
			if (stockSources != "" && stockSources.ToUpper() != "ALL")
			{
				string[] sources = stockSources.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_stockSources.AddRange(sources);
				}
			}
			else
			{
				_stockSources.Add("CTA");
				_stockSources.Add("NASDAQ");
			}
			// future sources
			if (futureSources != "" && futureSources.ToUpper() != "ALL")
			{
				string[] sources = futureSources.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_futureSources.AddRange(sources);
				}
			}
			else
			{
				_futureSources.Add("CME");
				_futureSources.Add("ICE");
			}
			// option sources
			if (optionSources != "" && optionSources.ToUpper() != "ALL")
			{
				string[] sources = optionSources.ToUpper().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
				if (sources.Length > 0)
				{
					_optionSources.AddRange(sources);
				}
			}
			else
			{
				_optionSources.Add("OPRA");
				_optionSources.Add("OPRANBBO");
			}

			#endregion

			_changeNotificationRate = changeNotificationRate;
			
			_lineTimeHourOffset = lineTimeHourOffset;
			_lineTimeMinOffset = lineTimeMinOffset;
			_lineTimeSecOffset = lineTimeSecOffset;

			_useChangeNotificationRate = useChangeNotificationRate;
			_subscriptionUseWaitForSymbol = subscriptionUseWaitForSymbol;

			Initialize();
		}

		private void Initialize()
		{
			_gHandle = this;

			// Type
			_smType = environment.SMType;
			
			// Sources
			string[] smNames = environment.SMNames.Split(_splitChars, StringSplitOptions.RemoveEmptyEntries);
			_smNames = new List<string>();
			_smNames.AddRange(smNames);

			_depth = environment.Depth;

			if (environment.Configlocation.Contains(".."))
			{
				_configlocation = environment.Configlocation.Replace("..", environment.StartupPath);
			}
			else
			{
				_configlocation = environment.Configlocation;
			}

			_sessions = new Dictionary<string, MDSessionMgr>();
			_startedSessions = new List<string>();

			_instrumentsBySource = new Dictionary<string, SRLabInstruments>();
			_subscriptionsCreated = new List<string>();
		}

		#endregion

		#region - Public Method - 

		public void CheckEnvironments()
		{
			environment.Check();
		}

		public void Start()
		{
			_isShutdown = false;

			// Create application space
			if (_application == null || !_application.loaded())
			{
				_application = MDApplication.Instance;
				try
				{
					_application.load(_configlocation, environment.ConfigFile);
				}
				catch (MDException ex)
				{
					OnSRLabChanged(new SRLabChangedEventArgs(ex));
				}
			}

			// Create all the Sessions By Source Names
			foreach (string source in _smNames)
			{
				CheckSession(source);

				OnSRLabChanged(new SRLabChangedEventArgs("Create Seesion For: " + source));
			}

			_running = true;

			StartDeltaNotifyThread();

			OnSRLabChanged(new SRLabChangedEventArgs(SRLabDataChangedEventTypes.OnInitialized, "Initialized."));
		}

		public void Subscribe(string symbol, string source)
		{
			Subscribe(symbol, source, false);
		}
		public void Subscribe(string symbol, string source, bool reload)
		{
			SubscribeNormal(symbol, source, reload);
		}

		private void SubscribeNormal(string symbol, string source, bool reload)
		{
			if (PreSubscribeCheck(symbol, source))
			{
				CheckSession(source);

				string key = "";
				bool isNew = CheckInstrument(symbol, source, ref key);
				if (isNew || reload)
				{
					try
					{
						if (!isNew && reload)
						{
							// Unsubscribe if already subscribed
							if (_instrumentsBySource[source].Instruments.ContainsKey(key))
							{
								_sessions[source].unsubscribe(_instrumentsBySource[source].Instruments[key]);
							}
						}

						OnSRLabChanged(new SRLabChangedEventArgs("Subscribe to symbol " + symbol + " source " + source));
						_sessions[source].subscribe(_instrumentsBySource[source].Instruments[key], this);

						if (!_subscriptionsCreated.Contains(source))
						{
							System.Threading.Thread.Sleep(1000);
							_subscriptionsCreated.Add(source);
							_sessions[source].appNotify();
						}
					}
					catch (Exception ex)
					{
						OnSRLabChanged(new SRLabChangedEventArgs(ex));
					}
				}
			}

			if (_forceGC)
			{
				MemClean();
			}
		}

		public void SubscribeBook(string symbol, string source, bool reload)
		{
			Subscribe(symbol, source, reload);
		}

		public void SubscribeL2(string symbol, string source, bool reload)
		{
			Subscribe(symbol, source, reload);
		}

		public void UnSubscribe(string source, InstrumentDef inst)
		{
			if (_sessions.ContainsKey(source))
			{
				_sessions[source].unsubscribe(inst);
			}
		}

		protected static void ShutDown(object sender, ConsoleCancelEventArgs args)
		{
			_gHandle.Shutdown();
		}

		public virtual void Shutdown()
		{
			try
			{
				if (_isShutdown) return;

				OnSRLabChanged(new SRLabChangedEventArgs("Shutdown Start"));

				_isShutdown = true;

				foreach (string source in _instrumentsBySource.Keys)
				{
					foreach (InstrumentDef inst in _instrumentsBySource[source].Instruments.Values)
					{
						UnSubscribe(source, inst);
					}
				}
				_instrumentsBySource.Clear();
				//foreach (MDSessionMgr sm in _sessions.Values)
				//{
				//    sm.stop();
				//}
				//_sessions.Clear();
				//_application = null;

				if (_running)
				{
					_running = false;

					StopDeltaNotifyThread();

					_subscriptions.Clear();
					_subscriptionsCreated.Clear();
				}

				OnSRLabChanged(new SRLabChangedEventArgs(SRLabDataChangedEventTypes.OnStopped, "Shutdown Done!"));
			}
			catch (MDException ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void ForceGC()
		{
			_forceGC = true;
		}

		#endregion

		#region - Local Method -

		private bool PreSubscribeCheck(string symbol, string source)
		{
			bool sourceOk = false;
			bool symbolOk = false;

			if (_soruceList.Count > 0)
			{
				if (_soruceList.Contains(source))
				{
					sourceOk = true;
				}
			}
			else
			{
				sourceOk = true;
			}

			if (_symbolList.Count > 0)
			{
				if (_symbolList.Contains(symbol.Substring(0, 1)))
				{
					symbolOk = true;
				}
				else if (symbol.Length > 2 && _symbolList.Contains(symbol.Substring(0, 2)))
				{
					symbolOk = true;
				}
			}
			else
			{
				symbolOk = true;
			}

			if (sourceOk && symbolOk)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		private void CheckSession(string source)
		{
			if (!_sessions.ContainsKey(source))
			{
				OnSRLabChanged(new SRLabChangedEventArgs("CheckSession " + source));

				_sessions.Add(source, new MDSessionMgr(_application, _smType, source, _depth));
				// High CPU Useage
				//_sessions[source].appNotify();
			}
		}

		private bool CheckInstrument(string symbol, string source, ref string key)
		{
			bool isNew = true;
			key = "";

			try
			{
				if (!_instrumentsBySource.ContainsKey(source))
				{
					_instrumentsBySource.Add(source, new SRLabInstruments(source));
				}

				key = string.Format("{0},{1}", new object[] { symbol, source });
				if (_instrumentsBySource[source].Instruments.ContainsKey(key))
				{
					isNew = false;
				}
				else
				{
					if (_subscribeWithSource)
					{
						_instrumentsBySource[source].Set(symbol, source, key);
					}
					else
					{
						_instrumentsBySource[source].Set(symbol, key);
					}
				}
			}
			catch (MDException ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex.StackTrace));
			}

			return isNew;
		}

		private void UnSubscribe(string symbol, string source)
		{
			if (_instrumentsBySource[source].Instruments.ContainsKey(symbol))
			{
				if (_sessions.ContainsKey(source))
				{
					_sessions[source].unsubscribe(_instrumentsBySource[source].Instruments[symbol]);
				}
			}
		}

		private void UnSubscribe(InstrumentDef inst, string source)
		{
			if (_sessions.ContainsKey(source))
			{
				_sessions[source].unsubscribe(inst);
			}
		}

		#endregion

		#region - Environment Memebers -

		private void SRLabEnviromentChanged(object sender, SRLabChangedEventArgs e)
		{
			switch (e.Type)
			{
				case SRLabDataChangedEventTypes.OnMsg:
					OnSRLabChanged(new SRLabChangedEventArgs(e.Msg));
					break;
				case SRLabDataChangedEventTypes.OnException:
					OnSRLabChanged(new SRLabChangedEventArgs(e.Ex));
					break;
				case SRLabDataChangedEventTypes.OnMDException:
					OnSRLabChanged(new SRLabChangedEventArgs(e.ExMD));
					break;
				case SRLabDataChangedEventTypes.OnEnvironmentVariableChecked:
				case SRLabDataChangedEventTypes.OnInitialized:
				case SRLabDataChangedEventTypes.OnStopped:
				case SRLabDataChangedEventTypes.OnChange:
					OnSRLabChanged(e);
					break;
				default:
					break;
			}
		}

		#endregion

		#region - MDListener Members -

		public void onBook(MDSubscription sub, MDOrderBook book, MDOrder o, MDTrade t)
		{
			try
			{
				//OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onBook", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
					PickDataBook(sub, book, DateTime.Now.ToOADate());
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onQuote(MDSubscription sub, MDQuote q)
		{
			try
			{
				//OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onQuote", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
					PickQuote(sub, q, DateTime.Now.ToOADate());
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onTrade(MDSubscription sub, MDTrade t)
		{
			try
			{
				//OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onTrade", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
					PickTrade(sub, t, DateTime.Now.ToOADate());
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onCustomEvent(MDSubscription sub, MDCustomEvent c)
		{
			try
			{
				//OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onCustomEvent", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
					PickCustom(sub, c, DateTime.Now.ToOADate());
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onSubscriptionInvalid(MDSubscription sub, string reason)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onSubscriptionInvalid", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				string source = sub.instrument().getSource();
				string symbol = sub.instrument().getSymbol();
				if (_instrumentsBySource[source].Instruments.ContainsKey(symbol))
				{
					UnSubscribe(source, _instrumentsBySource[source].Instruments[symbol]);
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onBestPrice(MDSubscription sub, MDQuote q)
		{
			try
			{
				//OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onBestPrice", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
					PickQuote(sub, q, DateTime.Now.ToOADate());
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onBookWithOrderReplace(MDSubscription sub, MDOrderBook book, MDOrder oldOrder, MDOrder newOrder)
		{
			try
			{
				//OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onBookWithOrderReplace", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
					PickDataBook(sub, book, DateTime.Now.ToOADate());
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onImbalance(MDSubscription sub, MDImbalance i)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onImbalance", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onOrder(MDSubscription sub, MDOrder o, MDTrade t)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onOrder", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		} 

		public void onOrderReplace(MDSubscription sub, MDOrder oldOrder, MDOrder newOrder)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onOrderReplace", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onSecurityStatusChange(MDSubscription sub, InstrumentDef instrument, InstrumentDef.InstrumentMktState oldState, InstrumentDef.InstrumentMktState newState)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onSecurityStatusChange", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onTradeDisseminationTimeAlert(MDSubscription sub, MDMktTradeDisseminationTimeAlert t)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onTradeDisseminationTimeAlert", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		public void onTradingIndicationAlert(MDSubscription sub, MDMktTradingIndicationAlert t)
		{
			try
			{
				OnSRLabChanged(new SRLabChangedEventArgs(string.Format("{0},{1},{2}", new object[] { "onTradingIndicationAlert", sub.instrument().getSymbol(), sub.instrument().getSource() })));
				if (IsCritical(sub.instrument().getSymbol(), sub.instrument().getSource(), sub.instrument().getInstrumentType().ToString()))
				{
					PickData(sub, DateTime.Now.ToOADate());
				}
				else
				{
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		#endregion

		#region - Decode Data By Type -

		private void PickData(MDSubscription sub, double onMsgTime)
		{
			// Critical First Message
			OnDeltaObject changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
			OnDeltaObject changesBook = null;

			DecodeSymbol(changes.Symbol, ref changes);

			if(!SubscriptionsCreated.Contains(changes.Key))
			{
				SubscriptionsCreated.Add(changes.Key);
			}

			//PickData(sub.imbalance(), ref changes);
			//PickData(sub.interest(), ref changes);
			//PickData(sub.pinfo(), ref changes);
			//PickData(sub.order(), ref changes);

			PickData(onMsgTime, sub.customevent(), ref changes);
			if (changes.SecType == SecTypes.F)
			{
				changesBook = new OnDeltaObject("b" + sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
				PickData(onMsgTime, sub.book(), ref changes);
				changesBook.Deltas.AskBooks = new Dictionary<double, long>(changes.Deltas.AskBooks);
				changesBook.Deltas.BidBooks = new Dictionary<double, long>(changes.Deltas.BidBooks);
			}
			PickData(onMsgTime, sub.trade(), ref changes);
			PickData(onMsgTime, sub.quote(), ref changes);

			lock (_subscriptions)
			{
				if (!_subscriptions.ContainsKey(changes.Key))
				{
					_subscriptions.Add(changes.Key, new SubscriptionFields(changes.Symbol, changes.Source, changes.SecType));
				}

				if (changesBook != null)
				{
					if (!_subscriptions.ContainsKey(changesBook.Key))
					{
						_subscriptions.Add(changesBook.Key, new SubscriptionFields(changesBook.Symbol, changesBook.Source, changesBook.SecType));
					}
				}
			}

			if (_useChangeNotificationRate)
			{
				lock (_criticalQueues)
				{
					_criticalQueues.Add(changes);
					if (changesBook != null)
					{
						_criticalQueues.Add(changesBook);
					}
				}
			}
			else
			{
				MDServerToClient delta = new MDServerToClient(changes.Deltas);
				OnSRLabChanged(new SRLabChangedEventArgs(delta));

				if (changesBook != null)
				{
					delta = new MDServerToClient(changesBook.Deltas);
					OnSRLabChanged(new SRLabChangedEventArgs(delta));
				}
			}
		}

		private void PickDataBook(MDSubscription sub, MDOrderBook book, double onMsgTime)
		{
			DateTime start = _dtHP.Now;

			OnDeltaObject changes = null;

			if (_useChangeNotificationRate)
			{
				try
				{
					lock (_changeQueues)
					{
						string key = string.Format("{0},{1}", new object[] { sub.instrument().getSymbol(), sub.instrument().getSource() });

						if (_changeQueues.ContainsKey(key))
						{
							changes = _changeQueues[key];
						}
						else
						{
							changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
							DecodeSymbol(changes.Symbol, ref changes);
							_changeQueues.Add(key, changes);
						}

						PickData(onMsgTime, book, ref changes);

						TimeSpan ts = _dtHP.Now.Subtract(start);
						changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

						_changeQueues[key] = changes;

						// TODO Remove this after Wombat Data Formate is removed from ROC
						OnDeltaObject bookChanges = new OnDeltaObject(changes, DataRollTypes.Book);
						ts = _dtHP.Now.Subtract(start);
						changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

						key = bookChanges.Key;
						if (_changeQueues.ContainsKey(key))
						{
							_changeQueues[key] = bookChanges;
						}
						else
						{
							_changeQueues.Add(key, bookChanges);
						}
					}
				}
				catch (Exception ex)
				{
					OnSRLabChanged(new SRLabChangedEventArgs(ex));
				}
			}
			else
			{
				try
				{
					changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
					DecodeSymbol(changes.Symbol, ref changes);

					PickData(onMsgTime, book, ref changes);

					TimeSpan ts = _dtHP.Now.Subtract(start);
					changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

					MDServerToClient delta = new MDServerToClient(changes.Deltas);
					OnSRLabChanged(new SRLabChangedEventArgs(delta));

					// TODO Remove this after Wombat Data Formate is removed from ROC
					OnDeltaObject bookChanges = new OnDeltaObject(changes, DataRollTypes.Book);
					ts = _dtHP.Now.Subtract(start);
					bookChanges.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

					delta = new MDServerToClient(bookChanges.Deltas);
					OnSRLabChanged(new SRLabChangedEventArgs(delta));
				}
				catch (Exception ex)
				{
					OnSRLabChanged(new SRLabChangedEventArgs(ex));
				}
			}
		}

		private void PickQuote(MDSubscription sub, MDQuote q, double onMsgTime)
		{
			DateTime start = _dtHP.Now;

			OnDeltaObject changes = null;
			string key = string.Format("{0},{1}", new object[] { sub.instrument().getSymbol(), sub.instrument().getSource() });

			if (_useChangeNotificationRate)
			{
				lock (_changeQueues)
				{
					if (_changeQueues.ContainsKey(key))
					{
						changes = _changeQueues[key];
					}
					else
					{
						changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
						DecodeSymbol(changes.Symbol, ref changes);
						_changeQueues.Add(key, changes);
					}

					PickData(onMsgTime, q, ref changes);

					TimeSpan ts = _dtHP.Now.Subtract(start);
					changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

					_changeQueues[key] = changes;
				}
			}
			else
			{
				changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
				DecodeSymbol(changes.Symbol, ref changes);

				PickData(onMsgTime, q, ref changes);

				TimeSpan ts = _dtHP.Now.Subtract(start);
				changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

				MDServerToClient delta = new MDServerToClient(changes.Deltas);
				OnSRLabChanged(new SRLabChangedEventArgs(delta));
			}
		}

		private void PickTrade(MDSubscription sub, MDTrade t, double onMsgTime)
		{
			DateTime start = _dtHP.Now;

			OnDeltaObject changes = null;
			string key = string.Format("{0},{1}", new object[] { sub.instrument().getSymbol(), sub.instrument().getSource() });

			if (_useChangeNotificationRate)
			{
				lock (_changeQueues)
				{
					if (_changeQueues.ContainsKey(key))
					{
						changes = _changeQueues[key];
					}
					else
					{
						changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
						DecodeSymbol(changes.Symbol, ref changes);
						_changeQueues.Add(key, changes);
					}

					PickData(onMsgTime, t, ref changes);

					TimeSpan ts = _dtHP.Now.Subtract(start);
					changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

					_changeQueues[key] = changes;
				}
			}
			else
			{
				changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
				DecodeSymbol(changes.Symbol, ref changes);

				PickData(onMsgTime, t, ref changes);

				TimeSpan ts = _dtHP.Now.Subtract(start);
				changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

				MDServerToClient delta = new MDServerToClient(changes.Deltas);
				OnSRLabChanged(new SRLabChangedEventArgs(delta));
			}
		}

		private void PickCustom(MDSubscription sub, MDCustomEvent c, double onMsgTime)
		{
			DateTime start = _dtHP.Now;

			OnDeltaObject changes = null;
			string key = string.Format("{0},{1}", new object[] { sub.instrument().getSymbol(), sub.instrument().getSource() });

			if (_useChangeNotificationRate)
			{
				lock (_changeQueues)
				{
					if (_changeQueues.ContainsKey(key))
					{
						changes = _changeQueues[key];
					}
					else
					{
						changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
						DecodeSymbol(changes.Symbol, ref changes);
						_changeQueues.Add(key, changes);
					}

					PickData(onMsgTime, c, ref changes);

					TimeSpan ts = _dtHP.Now.Subtract(start);
					changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

					_changeQueues[key] = changes;

					switch (changes.Source)
					{
						case _ctaRollSource:
							RollLatestData(new MDServerToClient(changes.Deltas), _ctaPrimary);
							break;
						case _nasdaqRollSource:
							RollLatestData(new MDServerToClient(changes.Deltas), _nasdaqPrimary);
							break;
					}
				}
			}
			else
			{
				changes = new OnDeltaObject(sub.instrument().getSymbol(), sub.instrument().getSource(), DecodeInstrumentType(sub.instrument().getInstrumentType()));
				DecodeSymbol(changes.Symbol, ref changes);

				PickData(onMsgTime, c, ref changes);

				TimeSpan ts = _dtHP.Now.Subtract(start);
				changes.Deltas.Update(WombatFieldIDs.uDecodedTime, Math.Abs(ts.TotalMilliseconds));

				MDServerToClient delta = new MDServerToClient(changes.Deltas);
				OnSRLabChanged(new SRLabChangedEventArgs(delta));

				switch (changes.Source)
				{
					case _ctaRollSource:
						RollLatestData(delta, _ctaPrimary);
						break;
					case _nasdaqRollSource:
						RollLatestData(delta, _nasdaqPrimary);
						break;
				}
			}
		}

		private void RollLatestData(MDServerToClient delta, string primary)
		{
			if (delta.uParticipentSymbol == primary)
			{
				
			}
		}

		#endregion

		#region - Decode Data -

		// Sub Data Functions
		private void PickData(double onMsgTime, MDQuote data, ref OnDeltaObject changes)
		{
			try
			{
				if (data == null) return;

				lock (data._qLock)
				{
					if (data._exchangeTime != null)
					{
						changes.Deltas.Update(WombatFieldIDs.LineTime, GetStringTime(data._exchangeTime.tv_sec(), data._exchangeTime.tv_usec(), changes.Source));
					}

					changes.Deltas.Update(WombatFieldIDs.uOnMsgTime, onMsgTime);

					if (data._bid != 0 && data._bidsize != 0)
					{
						changes.Deltas.Update(WombatFieldIDs.BidPrice, data._bid);
						changes.Deltas.Update(WombatFieldIDs.BidSize, data._bidsize);
						changes.Deltas.Update(WombatFieldIDs.BidPartId, data._bexchange.ToString());
					}

					if (data._ask != 0 && data._asksize != 0)
					{
						changes.Deltas.Update(WombatFieldIDs.AskPrice, data._ask);
						changes.Deltas.Update(WombatFieldIDs.AskSize, data._asksize);
						changes.Deltas.Update(WombatFieldIDs.AskPartId, data._oexchange.ToString());
					}
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		private void PickData(double onMsgTime, MDTrade data, ref OnDeltaObject changes)
		{
			try
			{
				if (data == null) return;

				lock (data._tLock)
				{
					if (data._exchangeTime != null)
					{
						changes.Deltas.Update(WombatFieldIDs.LineTime, GetStringTime(data._exchangeTime.tv_sec(), data._exchangeTime.tv_usec(), changes.Source));
					}

					changes.Deltas.Update(WombatFieldIDs.uOnMsgTime, DateTime.Now.ToOADate());

					if (data._last != 0 && data._lastsize != 0)
					{
						changes.Deltas.Update(WombatFieldIDs.TradePrice, data._last);
						changes.Deltas.Update(WombatFieldIDs.TradeVolume, data._lastsize);
						changes.Deltas.Update(WombatFieldIDs.TradePartId, data._cpid.ToString());
						
						changes.Deltas.Update(WombatFieldIDs.TotalVolume, data._cvol);
					}
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		private void PickData(double onMsgTime, MDOrderBook data, ref OnDeltaObject changes)
		{
			try
			{
				if (data == null) return;

				lock (data._bLock)
				{
					int bSize = data.buysSize();
					int sSize = data.sellsSize();

					if (bSize > 0 || sSize > 0)
					{
						if (data.exchangeTime() != null)
						{
							changes.Deltas.Update(WombatFieldIDs.LineTime, GetStringTime(data.exchangeTime().tv_sec(), data.exchangeTime().tv_usec(), changes.Source));
						}

						changes.Deltas.Update(WombatFieldIDs.uOnMsgTime, onMsgTime);
					}

					if (bSize > 0)
					{
						MDOrderBookInternal[] bList = new MDOrderBookInternal[0];
						bList = data.buys();

						// Update Level 1
						MDOrderBookInternal b = bList[0];
						changes.Deltas.Update(WombatFieldIDs.BidPrice, b.px());
						changes.Deltas.Update(WombatFieldIDs.BidSize, b.size());

						for (int bLevel = 0; bLevel < bSize; bLevel++)
						{
							b = bList[bLevel];
							//if (b.changed())
							//{
								if (changes.Deltas.BidBooks.ContainsKey(b.px()))
								{
									changes.Deltas.BidBooks[b.px()] = b.size();
								}
								else
								{
									changes.Deltas.BidBooks.Add(b.px(), b.size());
								}
							//}
						}
					}

					if (sSize > 0)
					{
						MDOrderBookInternal[] sList = new MDOrderBookInternal[0];
						sList = data.sells();

						// Update Level 1
						MDOrderBookInternal s = sList[0];
						changes.Deltas.Update(WombatFieldIDs.AskPrice, s.px());
						changes.Deltas.Update(WombatFieldIDs.AskSize, s.size());

						for (int sLevel = 0; sLevel < sSize; sLevel++)
						{
							s = sList[sLevel];
							//if (s.changed())
							//{
								if (changes.Deltas.AskBooks.ContainsKey(s.px()))
								{
									changes.Deltas.AskBooks[s.px()] = s.size();
								}
								else
								{
									changes.Deltas.AskBooks.Add(s.px(), s.size());
								}
							//}
						}
					}
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		private void PickData(double onMsgTime, MDCustomEvent data, ref OnDeltaObject changes)
		{
			try
			{
				if (data.getFields() == null) return;

				lock (data._cLock)
				{
					changes.Deltas.Update(WombatFieldIDs.uOnMsgTime, onMsgTime);

					if (data.getFields().Count > 0)
					{
						Dictionary<MDCustomEvent.EventFields, object> item = data.getFields();

						if (item.ContainsKey(MDCustomEvent.EventFields.CF_CLOSE_PRICE))
						{
							changes.Deltas.Update(WombatFieldIDs.ClosePrice, Convert.ToDouble(item[MDCustomEvent.EventFields.CF_CLOSE_PRICE]));
						}

						if (item.ContainsKey(MDCustomEvent.EventFields.CF_PREVCLOSE_PRICE))
						{
							changes.Deltas.Update(WombatFieldIDs.PrevClosePrice, Convert.ToDouble(item[MDCustomEvent.EventFields.CF_PREVCLOSE_PRICE]));
						}

						if (item.ContainsKey(MDCustomEvent.EventFields.CF_OPEN_PRICE))
						{
							changes.Deltas.Update(WombatFieldIDs.OpenPrice, Convert.ToDouble(item[MDCustomEvent.EventFields.CF_OPEN_PRICE]));
						}

						if (item.ContainsKey(MDCustomEvent.EventFields.CF_LOW_PRICE))
						{
							changes.Deltas.Update(WombatFieldIDs.LowPrice, Convert.ToDouble(item[MDCustomEvent.EventFields.CF_LOW_PRICE]));
						}

						if (item.ContainsKey(MDCustomEvent.EventFields.CF_HIGH_PRICE))
						{
							changes.Deltas.Update(WombatFieldIDs.HighPrice, Convert.ToDouble(item[MDCustomEvent.EventFields.CF_HIGH_PRICE]));
						}

						if (item.ContainsKey(MDCustomEvent.EventFields.CF_SETTLE_PRICE))
						{
							changes.Deltas.Update(WombatFieldIDs.SettlePrice, Convert.ToDouble(item[MDCustomEvent.EventFields.CF_SETTLE_PRICE]));
						}

						//foreach (MDCustomEvent.EventFields key in data.getFields().Keys)
						//{
						//    switch (key)
						//    {
						//        case MDCustomEvent.EventFields.CF_CLOSE_PRICE:
						//            changes.Deltas.Update(WombatFieldIDs.ClosePrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_CLOSE_PRICE]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_PREVCLOSE_PRICE:
						//            changes.Deltas.Update(WombatFieldIDs.PrevClosePrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_PREVCLOSE_PRICE]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_ADJ_PREV_CLOSE:
						//            changes.Deltas.Update(WombatFieldIDs.AdjPrevClosePrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_ADJ_PREV_CLOSE]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_BEST_ASK:
						//            changes.Deltas.Update(WombatFieldIDs.BestAsk, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_BEST_ASK]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_BEST_BID:
						//            changes.Deltas.Update(WombatFieldIDs.BestBid, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_BEST_BID]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_SETTLE_PRICE:
						//            changes.Deltas.Update(WombatFieldIDs.SettlePrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_SETTLE_PRICE]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_HIGH_PRICE:
						//            changes.Deltas.Update(WombatFieldIDs.HighPrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_HIGH_PRICE]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_LOW_PRICE:
						//            changes.Deltas.Update(WombatFieldIDs.LowPrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_LOW_PRICE]));
						//            break;
						//        case MDCustomEvent.EventFields.CF_OPEN_PRICE:
						//            changes.Deltas.Update(WombatFieldIDs.OpenPrice, Convert.ToDouble(data.getFields()[MDCustomEvent.EventFields.CF_OPEN_PRICE]));
						//            break;
						//    }
						//}
					}
				}
			}
			catch (Exception ex)
			{
				OnSRLabChanged(new SRLabChangedEventArgs(ex));
			}
		}

		private void PickData(double onMsgTime, MDImbalance data, ref OnDeltaObject changes)
		{
		}

		private void PickData(double onMsgTime, MDOrder data, ref OnDeltaObject changes)
		{
		}

		private string GetStringTime(long sec, long usec, string source)
		{
			switch (source)
			{
				case "CTA":
				case "NASDAQ":
					sec = sec - 3600;
					break;
			}

			long _mins = sec / 60;
			sec = sec % 60;

			long _hours = _mins / 60;
			_mins = _mins % 60;

			_hours = _hours % 24 + _lineTimeHourOffset;

			usec = usec / 1000;

			return String.Format("{0:00}:{1:00}:{2:00}.{3:000}", _hours, _mins, sec, usec);
		}

		private bool IsCritical(string symbol, string source, string type)
		{
			string key = GetSubscribeKey(symbol, source);

			lock (_subscriptions)
			{
				if (_subscriptions.ContainsKey(key))
				{
					return false;
				}
				else
				{
					_subscriptions.Add(key, new SubscriptionFields(symbol, source, type));
					return true;
				}
			}
		}

		private string GetSubscribeKey(string symbol, string source)
		{
			return string.Format("{0},{1}", new object[] { symbol, source });
		}

		private void DecodeSymbol(string symbol, ref OnDeltaObject changes)
		{
			// valid data
			int div = symbol.IndexOf(".");

			changes.Deltas.Update(WombatFieldIDs.IssueSymbol, symbol);
			if (div > 0)
			{
				changes.Deltas.Update(WombatFieldIDs.uParticipentSymbol, symbol.Substring(div + 1));
				changes.Deltas.Update(WombatFieldIDs.uSymbol, symbol.Substring(0, div));
			}
			else
			{
				changes.Deltas.Update(WombatFieldIDs.uSymbol, symbol);
			}
		}

		private string DecodeInstrumentType(CSMD.InstrumentDef.InstrumentType code)
		{
			switch (code)
			{
				case InstrumentDef.InstrumentType.EXCH_AM_OPTION_CALL:
					return "EXCH_AM_OPTION_CALL";
				case InstrumentDef.InstrumentType.EXCH_AM_OPTION_PUT:
					return "EXCH_AM_OPTION_PUT";
				case InstrumentDef.InstrumentType.EXCH_CONVERTIBLE_BOND:
					return "EXCH_CONVERTIBLE_BOND";
				case InstrumentDef.InstrumentType.EXCH_EU_OPTION_CALL:
					return "EXCH_EU_OPTION_CALL";
				case InstrumentDef.InstrumentType.EXCH_EU_OPTION_PUT:
					return "EXCH_EU_OPTION_PUT";
				case InstrumentDef.InstrumentType.EXCH_FUTURE:
					return "EXCH_FUTURE";
				case InstrumentDef.InstrumentType.EXCH_GOVERNMENT_BOND:
					return "EXCH_GOVERNMENT_BOND";
				case InstrumentDef.InstrumentType.EXCH_INDEX_FUND:
					return "EXCH_INDEX_FUND";
				case InstrumentDef.InstrumentType.EXCH_OPTION:
					return "EXCH_OPTION";
				case InstrumentDef.InstrumentType.EXCH_OTHER_BOND:
					return "EXCH_OTHER_BOND";
				case InstrumentDef.InstrumentType.EXCH_SPREAD:
					return "EXCH_SPREAD";
				case InstrumentDef.InstrumentType.EXCH_STOCK:
					return "EXCH_STOCK";
				case InstrumentDef.InstrumentType.EXCH_STOCK_INDEX:
					return "EXCH_STOCK_INDEX";
				case InstrumentDef.InstrumentType.EXCH_UNIT_TRUST_CERT:
					return "EXCH_UNIT_TRUST_CERT";
				case InstrumentDef.InstrumentType.EXCH_WARRANT:
					return "EXCH_WARRANT";
				case InstrumentDef.InstrumentType.EXCH_UNKNOWN:
				default:
					return "EXCH_UNKNOWN";
			}
		}

		#endregion

		#region - Delta & Book Notify Thread -

		private System.Threading.Timer _timerDeltaNotify;
		private System.Threading.TimerCallback _timerDeltaNotifyDelagate;
		
		private bool _running;

		private void StartDeltaNotifyThread()
		{
			if (_running)
			{
				if (_timerDeltaNotifyDelagate == null)
				{
					_timerDeltaNotifyDelagate = new System.Threading.TimerCallback(DeltaNotify);
				}

				if (_timerDeltaNotify == null)
				{
					if (_useChangeNotificationRate)
					{
						_timerDeltaNotify = new System.Threading.Timer(_timerDeltaNotifyDelagate, null, 0, _changeNotificationRate);
					}
					else
					{
						_timerDeltaNotify = new System.Threading.Timer(_timerDeltaNotifyDelagate, null, 0, 100);
					}
				}
			}
		}

		private void DeltaNotify(object input)
		{
			OnDeltaObject[] criticalMsgs = new OnDeltaObject[0];
			Dictionary<string, OnDeltaObject> msgs = new Dictionary<string, OnDeltaObject>();

			lock (_criticalQueues)
			{
				if (_criticalQueues.Count > 0)
				{
					criticalMsgs = _criticalQueues.ToArray();
					_criticalQueues.Clear();
				}
			}

			lock (_changeQueues)
			{
				if (_changeQueues.Count > 0)
				{
					msgs = new Dictionary<string, OnDeltaObject>(_changeQueues);
					_changeQueues.Clear();
				}
			}

			foreach (OnDeltaObject criticalMsg in criticalMsgs)
			{
				// Full interation of the initial snap shoot
				PickMsg(criticalMsg.Deltas);
			}

			foreach (string key in msgs.Keys)
			{
				if (!_running) break;
				if (msgs[key].Deltas != null)
				{
					PickMsg(msgs[key].Deltas);
				}
			}

			#region - GC -

			if (_forceGC)
			{
				MemClean();

				_forceGC = false;
			}

			#endregion
		}

		private void PickMsg(MDServerToClient delta)
		{
			if (delta.IssueSymbol != null && delta.IssueSymbol != "")
			{
				OnSRLabChanged(new SRLabChangedEventArgs(delta));
			}
		}

		private void StopDeltaNotifyThread()
		{
			if (_timerDeltaNotify != null)
			{
				_timerDeltaNotify.Dispose();
			}
			_timerDeltaNotify = null;
			_timerDeltaNotifyDelagate = null;
		}

		#endregion

		#region - IDisposable Members -

		public void Dispose()
		{
			Shutdown();
		}

		#endregion

		#region - Mem Clean -

		private void MemClean()
		{
			GC.Collect();
			GC.WaitForPendingFinalizers();
		}

		#endregion
	}
}
