using System;
using System.Collections.Generic;
using System.Text;
using LoggerEx;
using BenchMarkEx;
using System.Windows.Forms;
using System.Diagnostics;

namespace SRLabMDS
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
			public static Logger SRLab;		// SRLAB Log
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

		private static HelperSRLab _hSRLab;
		public static HelperSRLab HSRLab
		{
			get
			{
				if (_hSRLab == null)
				{
					_hSRLab = new HelperSRLab();
				}
				return _hSRLab;
			}
			set
			{
				_hSRLab = value;
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
