using System;
using System.Collections.Generic;
using System.Text;
using APISRLabEx;
using DateTimeEx;
using System.Windows.Forms;

namespace APISRLabMDS
{
	public class HelperSRLab : HelperStatusBase
	{
		#region - Status Enum -

		public enum StatusTypes
		{
			Connected,
			Connecting,
			DisConnecting,
			NotConnected,

			EnvironmentVariableChecking,
			EnvironmentVariableChecked,
		}

		#endregion

		#region - Local Variable -

		private bool _stopping = false;

		private DateTime _pTime = DateTime.Now;
		private TimeSpan _pTimeSpan = new TimeSpan();

		private bool _enableSRLabLog = false;
		private bool _enableSRLabPerformanceLog = false;
		private bool _enableTCPLog = false;
		private bool _enableMDSLog = false;

		private DateTimeHP _dtHP = new DateTimeHP();

		private int _subscriptionTimeGap = 100;
		private int _deltaRecapTimeGap = 100;

		// For Rolling(Open, Close, High and Low)
		// For Calculation(Pct, Change and Tick) 
		// From Primary Exchanges (SRLab Only)
		private string _ctaPrimary = "N";
		private string _nasdaqPrimary = "Q";
		private bool _enableDataRoll = false;
		private bool _enableDataCalculation = false;

		// Converting CME to All Four CME Sources (SRLab Only)
		private List<string> _cmeSources;
		private List<string> cmeSources
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

		// Convert OPRA to OPRANBBO (SRLab Only)
		private bool _convertOPRA = true;

		// Global Sync
		private object _syncObj = new object();

		#endregion

		#region - Property -

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

		#endregion

		#region - Constructor -

		public HelperSRLab()
		{
			Initialize();
		}

		private void Initialize()
		{
			_enableSRLabLog = Configuration.SRLab.Default.EnableSRLabLog;
			_enableSRLabPerformanceLog = Configuration.SRLab.Default.EnableSRLabPerformanceLog;
			
			_enableTCPLog = Configuration.MDS.Default.EnableTCPLog;
			_enableMDSLog = Configuration.MDS.Default.EnableMDSLog;

			_subscriptionTimeGap = Configuration.SRLab.Default.SubscriptionTimeGap;
			_deltaRecapTimeGap = Configuration.SRLab.Default.ChangeNotificationRate;

			_ctaPrimary = Configuration.SRLab.Default.CTAPrimary;
			_nasdaqPrimary = Configuration.SRLab.Default.NASDAQPrimay;

			_enableDataRoll = Configuration.MDS.Default.EnableDataRoll;
			_enableDataCalculation = Configuration.MDS.Default.EnableDataCalculation;

			string[] sources = Configuration.SRLab.Default.CMEExchangeList.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);
			if (cmeSources.Count == 0)
			{
				cmeSources.Add("CME");
			}
			else
			{
				cmeSources.AddRange(sources);
			}

			_convertOPRA = Configuration.SRLab.Default.ConvertOPRA;
		}

		#endregion

		#region - SRLab Wrapper And Environment -

		// Environment
		private SRLabEnviromentValues _environment;
		private SRLabEnviromentValues environment
		{
			get
			{
				if (_environment == null)
				{
					_environment = new SRLabEnviromentValues(
						Configuration.SRLab.Default.LocalIP,
						Configuration.SRLab.Default.DelayMCJoin,
						Configuration.SRLab.Default.FireLevel1Only,
						Configuration.SRLab.Default.LastValue,
						Configuration.SRLab.Default.LogLevel,
						Configuration.SRLab.Default.MaxLogSize,
						Configuration.SRLab.Default.BusyWait,
						Configuration.SRLab.Default.DumpXML,
						Configuration.SRLab.Default.Logs,
						Configuration.SRLab.Default.IgnoreCTA,
						Configuration.SRLab.Default.IgnoreNASDAQ,
						Configuration.SRLab.Default.IgnoreCME,
						Configuration.SRLab.Default.IgnoreCME_CBOT,
						Configuration.SRLab.Default.IgnoreCME_NYMEX,
						Configuration.SRLab.Default.IgnoreCME_MGEX,
						Configuration.SRLab.Default.IgnoreICE,
						Configuration.SRLab.Default.IgnoreOPRANBBO,
						Configuration.SRLab.Default.IgnoreARCA,
						Configuration.SRLab.Default.IgnoreINET);
					_environment.SRLabEnviromentChanged += new SRLabChangedEventHandler(SRLabChanged);

				}
				return _environment;
			}
		}

		public void CheckEnvironments()
		{
			environment.Check();
		}

		// SRLab Wrapper
		private SRLabWrapper _srLabWrapper;
		private SRLabWrapper srLabWrapper
		{
			get
			{
				if (_srLabWrapper == null)
				{
					_srLabWrapper = new SRLabWrapper(
						Application.StartupPath,
						Configuration.SRLab.Default.SMNames,
						Configuration.SRLab.Default.SMType,
						Configuration.SRLab.Default.Configlocation,
						Configuration.SRLab.Default.ConfigFile,
						Configuration.SRLab.Default.Depth);
					_srLabWrapper.SRLabChanged += new SRLabChangedEventHandler(SRLabChanged);
				}
				return _srLabWrapper;
			}
		}

		private void SRLabChanged(object sender, SRLabChangedEventArgs e)
		{
			switch (e.Type)
			{
				case SRLabDataChangedEventTypes.OnEnvironmentVariableChecked:
					SetStatus(StatusTypes.EnvironmentVariableChecked, String.Concat(new object[] { "SRLAB|OnEnvironmentVariableChecked: ", e.Msg }));
					break;
				case SRLabDataChangedEventTypes.OnInitialized:
					break;
				case SRLabDataChangedEventTypes.OnStopped:
					break;
				case SRLabDataChangedEventTypes.OnException:
					SetStatus(StatusTypes.NotConnected, String.Concat(new object[] { "SRLAB|OnException: ", e.Msg }));
					break;
				case SRLabDataChangedEventTypes.OnMsg:
					SetStatus(Status, String.Concat(new object[] { "SRLAB|OnMsg: ", e.Msg }));
					break;
				case SRLabDataChangedEventTypes.OnChange:
					break;
				case SRLabDataChangedEventTypes.None:
				default:
					break;
			}
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
	}
}
