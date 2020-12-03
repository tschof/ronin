using System;
using System.Collections.Generic;
using System.Text;

namespace SRLabEx
{
	public delegate void SRLabEnviromentChangedEventHandler(object sender, SRLabChangedEventArgs e);

	public sealed class SRLabEnviromentValues
	{
		private sealed class SRLabEnviromentKeys
		{
			public const string SRLABS_RCV_INTFC = "SRLABS_RCV_INTFC";
			public const string SRLABS_ARCA_RCV_INTFC = "SRLABS_ARCA_RCV_INTFC";
			public const string SRLABS_CME_RCV_INTFC = "SRLABS_CME_RCV_INTFC";
			public const string SRLABS_CTA_RCV_INTFC = "SRLABS_CTA_RCV_INTFC";
			public const string SRLABS_ICE_RCV_INTFC = "SRLABS_ICE_RCV_INTFC";
			public const string SRLABS_INET_RCV_INTFC = "SRLABS_INET_RCV_INTFC";
			public const string SRLABS_NASDAQ_RCV_INTFC = "SRLABS_NASDAQ_RCV_INTFC";
			public const string SRLABS_OPRA_RCV_INTFC = "SRLABS_OPRA_RCV_INTFC";

			public const string SRLABS_DELAY_MCJOIN = "SRLABS_DELAY_MCJOIN";
			public const string SRLABS_BUSY_WAIT = "SRLABS_BUSY_WAIT";

			public const string SRLABS_FIRE_LEVEL1_ONLY = "SRLABS_FIRE_LEVEL1_ONLY";
			public const string SRLABS_LAST_VALUE = "SRLABS_LAST_VALUE";
			public const string SRLABS_LOG_LEVEL = "SRLABS_LOG_LEVEL";
			public const string SRLABS_MAX_LOG_SIZE = "SRLABS_MAX_LOG_SIZE";
		}

		#region - Event -

		public event SRLabEnviromentChangedEventHandler SRLabEnviromentChanged;
		private void OnSRLabEnviromentChanged(SRLabChangedEventArgs e)
		{
			if (SRLabEnviromentChanged != null)
			{
				SRLabEnviromentChanged(this, e);
			}
		}

		#endregion

		#region - Properties -

		private string _startupPath = "";
		public string StartupPath
		{
			get
			{
				return _startupPath;
			}
		}

		// CTA,NASDAQ,CME,CME-CBOT,CME-MGEX,CME-NYMEX,ICE,OPRA
		private string _smNames = "";
		public string SMNames
		{
			get
			{
				return _smNames;
			}
		}

		private string _configlocation = @"..\SRLabConfig";
		public string Configlocation
		{
			get
			{
				return _configlocation;
			}
		}

		private string _configFile = "ronin-mc-client-allfeeds.xml";
		public string ConfigFile
		{
			get
			{
				return _configFile;
			}
		}

		private int _depth = 5;
		public int Depth
		{
			get
			{
				return _depth;
			}
		}

		private string _localIP = "10.103.102.29";
		public string LocalIP
		{
			get
			{
				return _localIP;
			}
		}

		private string _smType = "MDRCClientSessionMgr";
		public string SMType
		{
			get
			{
				return _smType;
			}
		}

		private bool _delayMCJoin = true;
		public bool DelayMCJoin
		{
			get
			{
				return _delayMCJoin;
			}
		}

		private bool _fireLevel1Only = false;
		public bool FireLevel1Only
		{
			get
			{
				return _fireLevel1Only;
			}
		}

		private bool _lastValue = true;
		public bool LastValue
		{
			get
			{
				return _lastValue;
			}
		}

		private bool _busyWait = false;
		public bool BusyWait
		{
			get
			{
				return _busyWait;
			}
		}

		private string _logLevel = "WARN";
		public string LogLevel
		{
			get
			{
				return _logLevel;
			}
		}

		private string _maxLogSize = "1000";
		public string MaxLogSize
		{
			get
			{
				return _maxLogSize;
			}
		}

		#endregion

		#region - Local Variable -

		private bool _isProcessing = false;

		#endregion

		#region - Constructor -

		public SRLabEnviromentValues(
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
			bool busyWait)
		{
			_startupPath = startupPath;
			_smNames = smNames;
			_smType = smType;
			_configlocation = configlocation;
			_configFile = configFile;
			_depth = depth;
			_localIP = localIP;
			_delayMCJoin = delayMCJoin;
			_fireLevel1Only = fireLevel1Only;
			_lastValue = lastValue;
			_logLevel = logLevel;
			_maxLogSize = maxLogSize;
			_busyWait = busyWait;
		}

		#endregion

		#region - Public Method -

		public void Check()
		{
			if (!_isProcessing)
			{
				_isProcessing = true;
				System.Threading.ThreadPool.QueueUserWorkItem(Check);
			}
		}

		#endregion

		#region - Local Method -

		private void Check(object input)
		{
			try
			{
				if (SRLABS_RCV_INTFC != LocalIP)
				{
					SRLABS_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_RCV_INTFC = " + LocalIP));

				if (SRLABS_ARCA_RCV_INTFC != LocalIP)
				{
					SRLABS_ARCA_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_ARCA_RCV_INTFC = " + LocalIP));

				if (SRLABS_CME_RCV_INTFC != LocalIP)
				{
					SRLABS_CME_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CME_RCV_INTFC = " + LocalIP));

				if (SRLABS_CTA_RCV_INTFC != LocalIP)
				{
					SRLABS_CTA_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CTA_RCV_INTFC = " + LocalIP));
				}
				if (SRLABS_ICE_RCV_INTFC != LocalIP)
				{
					SRLABS_ICE_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_ICE_RCV_INTFC = " + LocalIP));

				if (SRLABS_NASDAQ_RCV_INTFC != LocalIP)
				{
					SRLABS_NASDAQ_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_NASDAQ_RCV_INTFC = " + LocalIP));

				if (SRLABS_INET_RCV_INTFC != LocalIP)
				{
					SRLABS_INET_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_INET_RCV_INTFC = " + LocalIP));

				if (SRLABS_OPRA_RCV_INTFC != LocalIP)
				{
					SRLABS_OPRA_RCV_INTFC = LocalIP;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_OPRA_RCV_INTFC = " + LocalIP));

				SRLABS_DELAY_MCJOIN = DelayMCJoin;
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_DELAY_MCJOIN = " + DelayMCJoin.ToString()));

				SRLABS_FIRE_LEVEL1_ONLY = FireLevel1Only;
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_FIRE_LEVEL1_ONLY = " + FireLevel1Only.ToString()));

				SRLABS_LAST_VALUE = LastValue;
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_LAST_VALUE = " + LastValue.ToString()));

				SRLABS_BUSY_WAIT = BusyWait;
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_BUSY_WAIT = " + BusyWait.ToString()));

				if (SRLABS_LOG_LEVEL != LogLevel)
				{
					SRLABS_LOG_LEVEL = LogLevel;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_LOG_LEVEL = " + LogLevel));

				if (SRLABS_MAX_LOG_SIZE != MaxLogSize)
				{
					SRLABS_MAX_LOG_SIZE = MaxLogSize;
				}
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_MAX_LOG_SIZE = " + MaxLogSize));

			}
			catch (Exception ex)
			{
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs(ex));
			}

			OnSRLabEnviromentChanged(new SRLabChangedEventArgs( SRLabDataChangedEventTypes.OnEnvironmentVariableChecked, "Enviroment Value Check Done."));
			_isProcessing = false;
		}

		#endregion

		#region - Environment Properties -

		public string SRLABS_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_ARCA_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ARCA_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ARCA_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_CME_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_CTA_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CTA_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CTA_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_ICE_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ICE_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ICE_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_INET_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_INET_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_INET_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_NASDAQ_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_NASDAQ_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_NASDAQ_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_OPRA_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRA_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRA_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}


		public bool SRLABS_DELAY_MCJOIN
		{
			get
			{
				bool result = false;
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DELAY_MCJOIN, EnvironmentVariableTarget.Machine), out result);
				return result;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DELAY_MCJOIN, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public bool SRLABS_FIRE_LEVEL1_ONLY
		{
			get
			{
				bool result = false;
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_FIRE_LEVEL1_ONLY, EnvironmentVariableTarget.Machine), out result);
				return result;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_FIRE_LEVEL1_ONLY, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public bool SRLABS_LAST_VALUE
		{
			get
			{
				bool result = false;
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LAST_VALUE, EnvironmentVariableTarget.Machine), out result);
				return result;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LAST_VALUE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_BUSY_WAIT
		{
			get
			{
				bool result = false;
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_BUSY_WAIT, EnvironmentVariableTarget.Machine), out result);
				return (bool)result;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_BUSY_WAIT, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}


		// DEBUG, INFO, WARN
		public string SRLABS_LOG_LEVEL
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LOG_LEVEL, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LOG_LEVEL, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_MAX_LOG_SIZE
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_MAX_LOG_SIZE, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_MAX_LOG_SIZE, value, EnvironmentVariableTarget.Machine);
			}
		}

		#endregion
	}
}
