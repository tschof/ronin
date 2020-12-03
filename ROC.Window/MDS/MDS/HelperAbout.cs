using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace MDS
{
	public static class HelperAbout
	{
		public static string GetVersion()
		{
			string result = GetString("", "MDS");
			result = result + GetString(Application.ProductVersion.ToString(), "Version", true);

			return result;
		}

		public static string GetLBMSetting()
		{
			string result = GetString("", "LBM Settings");
			result = result + GetString(Configuration.LBM.Default.TransportName, "TransportName");
			result = result + GetString(Configuration.LBM.Default.Middleware, "Middleware");
			result = result + GetString(Configuration.LBM.Default.DictionarySourceSymbolNamespace, "DictionarySourceSymbolNamespace", true);

			result = result + GetString(Configuration.LBM.Default.MaxRetries.ToString(), "MaxRetries");
			result = result + GetString(Configuration.LBM.Default.IdelTimeBetweenRetries.ToString(), "IdelTimeBetweenRetries", true);

			result = result + GetString(Configuration.LBM.Default.ThrottleDefault.ToString(), "ThrottleDefault");
			result = result + GetString(Configuration.LBM.Default.ThrottleInitial.ToString(), "ThrottleInitial");
			result = result + GetString(Configuration.LBM.Default.ThrottleRecap.ToString(), "ThrottleRecap", true);

			result = result + GetString(Configuration.LBM.Default.SubscriptionSourceRange, "SubscriptionSourceRange");
			result = result + GetString(Configuration.LBM.Default.SubscriptionSymbolRange, "SubscriptionSymbolRange");
			result = result + GetString(Configuration.LBM.Default.SubscriptionTimeGap.ToString(), "SubscriptionTimeGap", true);

			result = result + GetString("", "LBM Latency Check Settings");
			result = result + GetString(Configuration.User.Default.CheckLatency.ToString(), "CheckLatency");
			result = result + GetString(Configuration.User.Default.DriftLimit.ToString(), "DriftLimit");
			result = result + GetString(Configuration.User.Default.LatencyLimit.ToString(), "LatencyLimit", true);

			result = result + GetString("", "LBM Scheduler For Today");
			foreach (string key in GLOBAL.HLBM.SourceBlockTimes.Keys)
			{
				result = result + GetString(GLOBAL.HLBM.SourceBlockTimes[key].ToString("HH:mm:ss.fff"), key);
			}
			result = result + GetString("", DateTime.Today.DayOfWeek.ToString(), true);

			result = result + GetString("", "LBM Soruces Avaliable Today");
			foreach (string key in GLOBAL.HLBM.SourceAvailableList)
			{
				result = result + GetString(key, "");
			}
			result = result + GetString("", DateTime.Today.DayOfWeek.ToString(), true);
			
			return result;
		}

		public static string GetMDSSetting()
		{
			string result = GetString("", "MDS Settings");

			result = result + GetString(Configuration.MDS.Default.IP, "IP");
			result = result + GetString(Configuration.MDS.Default.Port.ToString(), "Port", true);

			return result;
		}

		private static string GetString(string val, string name)
		{
			return GetString(val, name, false);
		}
		private static string GetString(string val, string name, bool doubleSpace)
		{
			if (doubleSpace)
			{
				return string.Format("{0}: {1} \r\n \r\n", name, val);
			}
			else
			{
				return string.Format("{0}: {1} \r\n", name, val);
			}
		}
	}
}
