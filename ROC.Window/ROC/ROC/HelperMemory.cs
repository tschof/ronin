using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace ROC
{
	public static class HelperMemory
	{
		[DllImportAttribute("kernel32.dll", EntryPoint = "SetProcessWorkingSetSize", ExactSpelling = true, CharSet = CharSet.Ansi, SetLastError = true)]

		private static extern int SetProcessWorkingSetSize(IntPtr process, int minimumWorkingSetSize, int maximumWorkingSetSize);

		public static void Flush()
		{
			//if (Process.GetCurrentProcess().PrivateMemorySize64 > Configuration.ROC.Default.MemoryFlushLimit * 100000)
			//{
			//    GC.Collect();
			//    GC.WaitForPendingFinalizers();
			//    if (Environment.OSVersion.Platform == PlatformID.Win32NT)
			//    {
			//        SetProcessWorkingSetSize(System.Diagnostics.Process.GetCurrentProcess().Handle, -1, -1);
			//    }
			//}
		}
	}
}
