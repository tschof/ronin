using System;
using System.Collections.Generic;
using System.Text;

namespace SRLabTest
{
	public sealed class SRLabEnviromentKeys
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

		public const string SRLABS_FIRE_LEVEL1_ONLY = "SRLABS_FIRE_LEVEL1_ONLY";
		public const string SRLABS_LAST_VALUE = "SRLABS_LAST_VALUE";
		public const string SRLABS_LOG_LEVEL = "SRLABS_LOG_LEVEL";
		public const string SRLABS_MAX_LOG_SIZE = "SRLABS_MAX_LOG_SIZE";
	}

	public class HelperSRLabEnvironments : HelperStatusBase
	{
		public static bool _isProcessing = false;

		public void Check()
		{
			if (!_isProcessing)
			{
				_isProcessing = true;
				System.Threading.ThreadPool.QueueUserWorkItem(Check);
			}
		}

		public void Check(object input)
		{
			try
			{
				if (SRLABS_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_ARCA_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_ARCA_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_CME_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_CME_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_CTA_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_CTA_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_ICE_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_ICE_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_INET_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_INET_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_INET_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_INET_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}
				if (SRLABS_OPRA_RCV_INTFC != Configuration.SRLab.Default.LocalIP)
				{
					SRLABS_OPRA_RCV_INTFC = Configuration.SRLab.Default.LocalIP;
				}

				if (SRLABS_DELAY_MCJOIN != Configuration.SRLab.Default.SRLABS_DELAY_MCJOIN)
				{
					SRLABS_DELAY_MCJOIN = Configuration.SRLab.Default.SRLABS_DELAY_MCJOIN;
				}
				if (SRLABS_FIRE_LEVEL1_ONLY != Configuration.SRLab.Default.SRLABS_FIRE_LEVEL1_ONLY)
				{
					SRLABS_FIRE_LEVEL1_ONLY = Configuration.SRLab.Default.SRLABS_FIRE_LEVEL1_ONLY;
				}
				if (SRLABS_LAST_VALUE != Configuration.SRLab.Default.SRLABS_LAST_VALUE)
				{
					SRLABS_LAST_VALUE = Configuration.SRLab.Default.SRLABS_LAST_VALUE;
				}

				if (SRLABS_LOG_LEVEL != Configuration.SRLab.Default.SRLABS_LOG_LEVEL)
				{
					SRLABS_LOG_LEVEL = Configuration.SRLab.Default.SRLABS_LOG_LEVEL;
				}
				if (SRLABS_MAX_LOG_SIZE != Configuration.SRLab.Default.SRLABS_MAX_LOG_SIZE)
				{
					SRLABS_MAX_LOG_SIZE = Configuration.SRLab.Default.SRLABS_MAX_LOG_SIZE;
				}
			}
			catch (Exception ex)
			{
				AddToExecption(ex);
			}

			_isProcessing = false;
		}


		public string SRLABS_RCV_INTFC
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ARCA_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CME_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_CTA_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_ICE_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_INET_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_NASDAQ_RCV_INTFC);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_OPRA_RCV_INTFC);
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
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_DELAY_MCJOIN), out result);
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
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_FIRE_LEVEL1_ONLY), out result);
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
				Boolean.TryParse(Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LAST_VALUE), out result);
				return result;
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LAST_VALUE, value.ToString(), EnvironmentVariableTarget.Machine);
			}
		}


		// DEBUG, INFO, WARN
		public string SRLABS_LOG_LEVEL
		{
			get
			{
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_LOG_LEVEL);
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
				return Environment.GetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_MAX_LOG_SIZE);
			}
			set
			{
				Environment.SetEnvironmentVariable(SRLabEnviromentKeys.SRLABS_MAX_LOG_SIZE, value, EnvironmentVariableTarget.Machine);
			}
		}
	}
}
