using System;
using System.Diagnostics;

using LoggerEx;
using BenchMarkEx;
using System.Windows.Forms;

namespace MDS
{
	public static class GLOBAL
	{
		public static bool AutoStartMDS = false;
		public static bool AutoStartLBM = false;
		public static bool AutoHide = false;
		public static bool AutoLoad = false;

		public static class DEBUG
		{
			public static bool ShownDM
			{
				get
				{
					return !Debugger.IsAttached;
				}
			}
		}

		public static class LOG
		{
			public static Logger Log;
			public static Logger TCP;
			public static Logger MDS;		// MDS Log
			public static Logger LBM;		// LBM Log
			public static Logger BMK;		// Performance Log
		}

		public static void AddToExecption(Exception ex)
		{
			try
			{
				LOG.Log.Out(ex.Message);
				LOG.Log.Out(ex.StackTrace);
			}
			catch
			{
				MessageBox.Show(ex.StackTrace, ex.Message);
			}
		}

		public static HelperEMail HMail = new HelperEMail();

		public static class BENCHAMRK
		{
			public static Benchmark Mark;
		}

		private static HelperLBM _hlbm;
		public static HelperLBM HLBM
		{
			get
			{
				if (_hlbm == null)
				{
					_hlbm = new HelperLBM();
				}
				return _hlbm;
			}
			set
			{
				_hlbm = value;
			}
		}

		private static HelperMDS _hMDS;
		public static HelperMDS HMDS
		{
			get
			{
				if (_hMDS == null)
				{
					_hMDS = new HelperMDS();
				}
				return _hMDS;
			}
			set
			{
				_hMDS = value;
			}
		}
	}
}
