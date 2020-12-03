using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace SRLabMDS
{
	public static class HelperAbout
	{
		public static string GetVersion()
		{
			string result = GetString("", "MDS");
			result = result + GetString(Application.ProductVersion.ToString(), "Version", true);

			return result;
		}

		public static string GetSRLabSetting()
		{
			string result = GetString("", "LBM Settings");
			result = result + GetString(Configuration.SRLab.Default.ConfigFile, "ConfigFile");
			result = result + GetString(Configuration.SRLab.Default.Configlocation, "Configlocation");
			result = result + GetString(Configuration.SRLab.Default.SMNames, "SMNames");
			result = result + GetString(Configuration.SRLab.Default.SMType, "SMType");
			result = result + GetString(Configuration.SRLab.Default.LocalIP.ToString(), "LocalIP");

			result = result + GetString(Configuration.SRLab.Default.Depth.ToString(), "Depth");
			result = result + GetString(Configuration.SRLab.Default.FireLevel1Only.ToString(), "FireLevel1Only");
			result = result + GetString(Configuration.SRLab.Default.DelayMCJoin.ToString(), "DelayMCJoin");
			result = result + GetString(Configuration.SRLab.Default.BusyWait.ToString(), "BusyWait");

			result = result + GetString(Configuration.SRLab.Default.LogLevel, "LogLevel");
			result = result + GetString(Configuration.SRLab.Default.MaxLogSize, "MaxLogSize", true);

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
				return string.Format("{0,-15}: {1} \r\n \r\n", name, val);
			}
			else
			{
				return string.Format("{0,-15}: {1} \r\n", name, val);
			}
		}
	}
}
