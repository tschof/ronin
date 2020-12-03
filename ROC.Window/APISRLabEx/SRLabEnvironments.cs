using System;
using System.Collections.Generic;
using System.Text;

namespace APISRLabEx
{
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
			public const string SRLABS_OPRANBBO_RCV_INTFC = "SRLABS_OPRANBBO_RCV_INTFC";

			public const string SRLABS_DELAY_MCJOIN = "SRLABS_DELAY_MCJOIN";
			public const string SRLABS_BUSY_WAIT = "SRLABS_BUSY_WAIT";

			public const string SRLABS_FIRE_LEVEL1_ONLY = "SRLABS_FIRE_LEVEL1_ONLY";
			public const string SRLABS_LAST_VALUE = "SRLABS_LAST_VALUE";
			public const string SRLABS_LOG_LEVEL = "SRLABS_LOG_LEVEL";
			public const string SRLABS_MAX_LOG_SIZE = "SRLABS_MAX_LOG_SIZE";

			public const string SRLABS_DUMP_XML = "SRLABS_DUMP_XML";
			public const string SRLABS_LOGS = "SRLABS_LOGS";

			public const string SRLABS_CTA_IGNORE = "SRLABS_CTA_IGNORE";
			public const string SRLABS_NASDAQ_IGNORE = "SRLABS_NASDAQ_IGNORE";
			public const string SRLABS_ICE_IGNORE = "SRLABS_ICE_IGNORE";
			public const string SRLABS_CME_IGNORE = "SRLABS_CME_IGNORE";
			public const string SRLABS_CME_CBOT_IGNORE = "SRLABS_CME_CBOT_IGNORE";
			public const string SRLABS_CME_NYMEX_IGNORE = "SRLABS_CME_NYMEX_IGNORE";
			public const string SRLABS_CME_MGEX_IGNORE = "SRLABS_CME_MGEX_IGNORE";
			public const string SRLABS_OPRANBBO_IGNORE = "SRLABS_OPRANBBO_IGNORE";
			public const string SRLABS_ARCA_IGNORE = "SRLABS_ARCA_IGNORE";
			public const string SRLABS_INET_IGNORE = "SRLABS_INET_IGNORE";
		}

		#region - Event -

		public event SRLabChangedEventHandler SRLabEnviromentChanged;
		private void OnSRLabEnviromentChanged(SRLabChangedEventArgs e)
		{
			if (SRLabEnviromentChanged != null)
			{
				SRLabEnviromentChanged(this, e);
			}
		}

		#endregion

		#region - Properties -

		private string _localIP = "10.103.102.29";
		public string LocalIP
		{
			get
			{
				return _localIP;
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

		private string _dumpXML = @"c:\temp";
		public string DumpXML
		{
			get
			{
				return _dumpXML;
			}
		}

		private string _logs = @"c:\temp";
		public string Logs
		{
			get
			{
				return _logs;
			}
		}

		private bool _ignoreCTA = false;
		public bool IgnoreCTA
		{
			get
			{
				return _ignoreCTA;
			}
		}

		private bool _ignoreNASDAQ = false;
		public bool IgnoreNASDAQ
		{
			get
			{
				return _ignoreNASDAQ;
			}
		}

		private bool _ignoreCME = false;
		public bool IgnoreCME
		{
			get
			{
				return _ignoreCME;
			}
		}

		private bool _ignoreCME_CBOT = false;
		public bool IgnoreCME_CBOT
		{
			get
			{
				return _ignoreCME_CBOT;
			}
		}

		private bool _ignoreCME_NYMEX = false;
		public bool IgnoreCME_NYMEX
		{
			get
			{
				return _ignoreCME_NYMEX;
			}
		}

		private bool _ignoreCME_MGEX = false;
		public bool IgnoreCME_MGEX
		{
			get
			{
				return _ignoreCME_MGEX;
			}
		}

		private bool _ignoreICE = false;
		public bool IgnoreICE
		{
			get
			{
				return _ignoreICE;
			}
		}

		private bool _ignoreOPRANBBO = false;
		public bool IgnoreOPRANBBO
		{
			get
			{
				return _ignoreOPRANBBO;
			}
		}

		private bool _ignoreARCA = true;
		public bool IgnoreARCA
		{
			get
			{
				return _ignoreARCA;
			}
		}

		private bool _ignoreINET = true;
		public bool IgnoreINET
		{
			get
			{
				return _ignoreINET;
			}
		}

		#endregion

		#region - Local Variable -

		private bool _isProcessing = false;

		#endregion

		#region - Constructor -

		public SRLabEnviromentValues(
			string localIP,
			bool delayMCJoin,
			bool fireLevel1Only,
			bool lastValue,
			string logLevel,
			string maxLogSize,
			bool busyWait,
			string dumpXML,
			string logs,
			bool ignoreCTA,
			bool ignoreNASDAQ,
			bool ignoreCME,
			bool ignoreCME_CBOT,
			bool ignoreCME_NYMEX,
			bool ignoreCME_MGEX,
			bool ignoreICE,
			bool ignoreOPRANBBO,
			bool ignoreARCA,
			bool ignoreINET)
		{
			_localIP = localIP;
			_delayMCJoin = delayMCJoin;
			_fireLevel1Only = fireLevel1Only;
			_lastValue = lastValue;
			_logLevel = logLevel;
			_maxLogSize = maxLogSize;
			_busyWait = busyWait;
			_dumpXML = dumpXML;
			_logs = logs;
			_ignoreCTA = ignoreCTA;
			_ignoreNASDAQ = ignoreNASDAQ;
			_ignoreCME = ignoreCME;
			_ignoreCME_CBOT = ignoreCME_CBOT;
			_ignoreCME_NYMEX = ignoreCME_NYMEX;
			_ignoreCME_MGEX = ignoreCME_MGEX;
			_ignoreICE = ignoreICE;
			_ignoreOPRANBBO = ignoreOPRANBBO;
			_ignoreARCA = ignoreARCA;
			_ignoreINET = ignoreINET;
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
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_ARCA_RCV_INTFC != LocalIP)
				{
					SRLABS_ARCA_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_ARCA_RCV_INTFC = " + LocalIP));

				}

				if (SRLABS_CME_RCV_INTFC != LocalIP)
				{
					SRLABS_CME_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CME_RCV_INTFC = " + LocalIP));

				}

				if (SRLABS_CTA_RCV_INTFC != LocalIP)
				{
					SRLABS_CTA_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CTA_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_ICE_RCV_INTFC != LocalIP)
				{
					SRLABS_ICE_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_ICE_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_NASDAQ_RCV_INTFC != LocalIP)
				{
					SRLABS_NASDAQ_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_NASDAQ_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_INET_RCV_INTFC != LocalIP)
				{
					SRLABS_INET_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_INET_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_OPRA_RCV_INTFC != LocalIP)
				{
					SRLABS_OPRA_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_OPRA_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_OPRANBBO_RCV_INTFC != LocalIP)
				{
					SRLABS_OPRANBBO_RCV_INTFC = LocalIP;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_OPRANBBO_RCV_INTFC = " + LocalIP));
				}

				if (SRLABS_DELAY_MCJOIN == null || (bool)SRLABS_DELAY_MCJOIN != DelayMCJoin)
				{
					SRLABS_DELAY_MCJOIN = DelayMCJoin;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_DELAY_MCJOIN = " + DelayMCJoin.ToString()));
				}

				if (SRLABS_FIRE_LEVEL1_ONLY == null || (bool)SRLABS_FIRE_LEVEL1_ONLY != FireLevel1Only)
				{
					SRLABS_FIRE_LEVEL1_ONLY = FireLevel1Only;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_FIRE_LEVEL1_ONLY = " + FireLevel1Only.ToString()));
				}

				if (SRLABS_LAST_VALUE == null || (bool)SRLABS_LAST_VALUE != LastValue)
				{
					SRLABS_LAST_VALUE = LastValue;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_LAST_VALUE = " + LastValue.ToString()));
				}

				if (SRLABS_BUSY_WAIT == null || (bool)SRLABS_BUSY_WAIT != BusyWait)
				{
					SRLABS_BUSY_WAIT = BusyWait;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_BUSY_WAIT = " + BusyWait.ToString()));
				}

				if (SRLABS_LOG_LEVEL != LogLevel)
				{
					SRLABS_LOG_LEVEL = LogLevel;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_LOG_LEVEL = " + LogLevel));
				}

				if (SRLABS_MAX_LOG_SIZE != MaxLogSize)
				{
					SRLABS_MAX_LOG_SIZE = MaxLogSize;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_MAX_LOG_SIZE = " + MaxLogSize));
				}

				if (SRLABS_DUMP_XML != DumpXML)
				{
					SRLABS_DUMP_XML = DumpXML;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_DUMP_XML = " + DumpXML));
				}

				if (SRLABS_LOGS != Logs)
				{
					SRLABS_LOGS = Logs;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_LOGS = " + Logs));
				}

				if (SRLABS_CTA_IGNORE == null || (bool)SRLABS_CTA_IGNORE != IgnoreCTA)
				{
					SRLABS_CTA_IGNORE = IgnoreCTA;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CTA_IGNORE = " + IgnoreCTA.ToString()));
				}

				if (SRLABS_NASDAQ_IGNORE == null || (bool)SRLABS_NASDAQ_IGNORE != IgnoreNASDAQ)
				{
					SRLABS_NASDAQ_IGNORE = IgnoreNASDAQ;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_NASDAQ_IGNORE = " + IgnoreNASDAQ.ToString()));
				}

				if (SRLABS_CME_IGNORE == null || (bool)SRLABS_CME_IGNORE != IgnoreCME)
				{
					SRLABS_CME_IGNORE = IgnoreCME;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CME_IGNORE = " + IgnoreCME.ToString()));
				}

				if (SRLABS_CME_CBOT_IGNORE == null || (bool)SRLABS_CME_CBOT_IGNORE != IgnoreCME_CBOT)
				{
					SRLABS_CME_CBOT_IGNORE = IgnoreCME_CBOT;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CME_CBOT_IGNORE = " + IgnoreCME_CBOT.ToString()));
				}

				if (SRLABS_CME_NYMEX_IGNORE == null || (bool)SRLABS_CME_NYMEX_IGNORE != IgnoreCME_NYMEX)
				{
					SRLABS_CME_NYMEX_IGNORE = IgnoreCME_NYMEX;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CME_NYMEX_IGNORE = " + IgnoreCME_NYMEX.ToString()));
				}

				if (SRLABS_CME_MGEX_IGNORE == null || (bool)SRLABS_CME_MGEX_IGNORE != IgnoreCME_MGEX)
				{
					SRLABS_CME_MGEX_IGNORE = IgnoreCME_MGEX;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_CME_MGEX_IGNORE = " + IgnoreCME_MGEX.ToString()));
				}

				if (SRLABS_ICE_IGNORE == null || (bool)SRLABS_ICE_IGNORE != IgnoreICE)
				{
					SRLABS_ICE_IGNORE = IgnoreICE;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_ICE_IGNORE = " + IgnoreICE.ToString()));
				}

				if (SRLABS_OPRANBBO_IGNORE == null || (bool)SRLABS_OPRANBBO_IGNORE != IgnoreOPRANBBO)
				{
					SRLABS_OPRANBBO_IGNORE = IgnoreOPRANBBO;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_OPRANBBO_IGNORE = " + IgnoreOPRANBBO.ToString()));
				}

				if (SRLABS_ARCA_IGNORE == null || (bool)SRLABS_ARCA_IGNORE != IgnoreARCA)
				{
					SRLABS_ARCA_IGNORE = IgnoreARCA;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_ARCA_IGNORE = " + IgnoreARCA.ToString()));
				}

				if (SRLABS_INET_IGNORE == null || (bool)SRLABS_INET_IGNORE != IgnoreINET)
				{
					SRLABS_INET_IGNORE = IgnoreINET;
					OnSRLabEnviromentChanged(new SRLabChangedEventArgs("SRLABS_INET_IGNORE = " + IgnoreINET.ToString()));
				}
			}
			catch (Exception ex)
			{
				OnSRLabEnviromentChanged(new SRLabChangedEventArgs(ex));
			}

			OnSRLabEnviromentChanged(new SRLabChangedEventArgs(SRLabDataChangedEventTypes.OnEnvironmentVariableChecked, "Enviroment Value Check Done."));
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

		public string SRLABS_OPRANBBO_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRANBBO_RCV_INTFC, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRANBBO_RCV_INTFC, value, EnvironmentVariableTarget.Machine);
			}
		}


		public Nullable<bool> SRLABS_DELAY_MCJOIN
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DELAY_MCJOIN, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DELAY_MCJOIN, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_FIRE_LEVEL1_ONLY
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_FIRE_LEVEL1_ONLY, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_FIRE_LEVEL1_ONLY, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_LAST_VALUE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LAST_VALUE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
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
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_BUSY_WAIT, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
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

		// Logs, XML
		public string SRLABS_DUMP_XML
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DUMP_XML, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DUMP_XML, value, EnvironmentVariableTarget.Machine);
			}
		}

		public string SRLABS_LOGS
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LOGS, EnvironmentVariableTarget.Machine);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LOGS, value, EnvironmentVariableTarget.Machine);
			}
		}

		// IGNORE
		public Nullable<bool> SRLABS_CTA_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CTA_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CTA_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_NASDAQ_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_NASDAQ_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_NASDAQ_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_CME_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_CME_CBOT_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_CBOT_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_CBOT_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_CME_NYMEX_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_NYMEX_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_NYMEX_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_CME_MGEX_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_MGEX_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_MGEX_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_ICE_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ICE_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ICE_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_OPRANBBO_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRANBBO_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRANBBO_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_ARCA_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ARCA_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ARCA_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		public Nullable<bool> SRLABS_INET_IGNORE
		{
			get
			{
				bool result = false;
				if (Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_INET_IGNORE, EnvironmentVariableTarget.Machine), out result))
				{
					return result;
				}
				return null;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_INET_IGNORE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}

		#endregion
	}
}
