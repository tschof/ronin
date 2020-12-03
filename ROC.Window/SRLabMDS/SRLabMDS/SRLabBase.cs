using System;
using System.Collections.Generic;
using System.Text;
using CSMD;
using System.Reflection;
using System.Diagnostics;
using System.IO;
using System.Collections;

namespace SRLabTest
{
	public delegate void ChangedEventHandler(object sender, MDChangedEventArgs e);

	public class SRLabBase : MDListener
	{
		#region - Event - 

		public event ChangedEventHandler MDChanged;
		private void OnMDChanged(MDChangedEventArgs e)
		{
			if (MDChanged != null)
			{
				MDChanged(this, e);
			}
		}

		#endregion

		private static char[] _splitChars = new char[] { ',' };

		private static SRLabBase _gHandle;
		private static bool _isShutdown;

		private MDApplication _application;
		
		private HelperSRLabEnvironments _environment;
		public HelperSRLabEnvironments Environment
		{
			get
			{
				if (_environment == null)
				{
					_environment = new HelperSRLabEnvironments();
				}
				return _environment;
			}
		}

		private string _configlocation = "";
		private string _smType = "";
		private List<string> _smNames = new List<string>();
		private int _depth = 5;

		private Dictionary<string, MDSessionMgr> _sessions;

		private Dictionary<string, SRLabInstruments> _instrumentsBySource;

		public void CheckEnvironment()
		{
			Environment.Check();
		}

		public SRLabBase()
		{
			Initialize();
		}

		public void Initialize()
		{
			_gHandle = this;

			_smType = Configuration.SRLab.Default.SMType;
			string[] smNames = Configuration.SRLab.Default.SMNames.Split(_splitChars, StringSplitOptions.RemoveEmptyEntries);
			_smNames.AddRange(smNames);
			_depth = Configuration.SRLab.Default.Depth;

			_configlocation = Configuration.SRLab.Default.Configlocation.Replace("..", System.Windows.Forms.Application.StartupPath);

			_sessions = new Dictionary<string, MDSessionMgr>();
			_instrumentsBySource = new Dictionary<string, SRLabInstruments>();
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
					_application.load(_configlocation, Configuration.SRLab.Default.ConfigFile);
				}
				catch (MDException ex)
				{
					OnMDChanged(new MDChangedEventArgs(ex.StackTrace));
				}
			}

			// Create all the Sessions By Source Names
			foreach (string source in _smNames)
			{
				CheckSession(source);
			}
		}

		public void Subscribe(string symbol, string source)
		{
			Subscribe(symbol, source, false);
		}
		public void Subscribe(string symbol, string source, bool reload)
		{
			CheckSession(source);

			if (CheckInstrument(symbol, source) || reload)
			{
				try
				{
					OnMDChanged(new MDChangedEventArgs("Subscribe to symbol " + symbol + " source " + source));
					_sessions[source].subscribe(_instrumentsBySource[source].Instruments[symbol], this);
				}
				catch (Exception ex)
				{
					OnMDChanged(new MDChangedEventArgs(ex.StackTrace));
				}
			}
		}

		private void CheckSession(string source)
		{
			if (!_sessions.ContainsKey(source))
			{
				OnMDChanged(new MDChangedEventArgs("CheckSession " + source));
				
				_sessions.Add(source, new MDSessionMgr(_application, _smType, source, _depth));
				_sessions[source].appNotify();
			}
		}

		private bool CheckInstrument(string symbol, string source)
		{
			bool isNew = true;

			try
			{
				if (!_instrumentsBySource.ContainsKey(source))
				{
					_instrumentsBySource.Add(source, new SRLabInstruments(source));
				}

				if (_instrumentsBySource[source].Instruments.ContainsKey(symbol))
				{
					isNew = false;
				}
				else
				{
					_instrumentsBySource[source].Set(symbol);
				}
			}
			catch (MDException ex)
			{
				OnMDChanged(new MDChangedEventArgs(ex.StackTrace));
			}

			return isNew;
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

				OnMDChanged(new MDChangedEventArgs("Shutdown Start"));

				_isShutdown = true;

				foreach (string source in _instrumentsBySource.Keys)
				{
					foreach (InstrumentDef inst in _instrumentsBySource[source].Instruments.Values)
					{
						if (_sessions.ContainsKey(source))
						{
							_sessions[source].unsubscribe(inst);
						}
					}
				}
				_instrumentsBySource.Clear();
				//foreach (MDSessionMgr sm in _sessions.Values)
				//{
				//    sm.stop();
				//}
				//_sessions.Clear();
				//_application = null;

				OnMDChanged(new MDChangedEventArgs("Shutdown Done"));
			}
			catch (MDException ex)
			{
				OnMDChanged(new MDChangedEventArgs(ex.StackTrace));
			}
		}

		#region - MDListener Members -

		public void onBestPrice(MDSubscription sub, MDQuote q)
		{
			OnMDChanged(new MDChangedEventArgs("onBestPrice"));
		}

		public void onBook(MDSubscription sub, MDOrderBook book, MDOrder o, MDTrade t)
		{
			OnMDChanged(new MDChangedEventArgs("onBook"));
		}

		public void onBookWithOrderReplace(MDSubscription sub, MDOrderBook book, MDOrder oldOrder, MDOrder newOrder)
		{
			OnMDChanged(new MDChangedEventArgs("onBookWithOrderReplace"));
		}

		public void onCustomEvent(MDSubscription sub, MDCustomEvent c)
		{
			OnMDChanged(new MDChangedEventArgs("onCustomEvent"));
		}

		public void onImbalance(MDSubscription sub, MDImbalance i)
		{
			OnMDChanged(new MDChangedEventArgs("onImbalance"));
		}

		public void onOrder(MDSubscription sub, MDOrder o, MDTrade t)
		{
			OnMDChanged(new MDChangedEventArgs("onOrder"));
		}

		public void onOrderReplace(MDSubscription sub, MDOrder oldOrder, MDOrder newOrder)
		{
			OnMDChanged(new MDChangedEventArgs("onOrderReplace"));
		}

		public void onQuote(MDSubscription sub, MDQuote q)
		{
			OnMDChanged(new MDChangedEventArgs("onQuote"));
		}

		public void onSecurityStatusChange(MDSubscription sub, InstrumentDef instrument, InstrumentDef.InstrumentMktState oldState, InstrumentDef.InstrumentMktState newState)
		{
			OnMDChanged(new MDChangedEventArgs("onSecurityStatusChange"));
		}

		public void onSubscriptionInvalid(MDSubscription sub, string reason)
		{
			OnMDChanged(new MDChangedEventArgs("onSubscriptionInvalid"));
		}

		public void onTrade(MDSubscription sub, MDTrade t)
		{
			OnMDChanged(new MDChangedEventArgs("onTrade"));
		}

		public void onTradeDisseminationTimeAlert(MDSubscription mDSubscription, MDMktTradeDisseminationTimeAlert t)
		{
			OnMDChanged(new MDChangedEventArgs("onTradeDisseminationTimeAlert"));
		}

		public void onTradingIndicationAlert(MDSubscription mDSubscription, MDMktTradingIndicationAlert t)
		{
			OnMDChanged(new MDChangedEventArgs("onTradingIndicationAlert"));
		}

		#endregion
	}

	public class MDChangedEventArgs : EventArgs
	{
		private string _msg;
		private string _stackTrace;

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

		public MDChangedEventArgs(MDException ex)
		{
			_msg = ex.Source;
			_stackTrace = ex.StackTrace;
		}
		public MDChangedEventArgs(string msg)
		{
			_msg = msg;
		}
	}
}
