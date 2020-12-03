using System;
using System.Runtime.InteropServices;

namespace LBMEx
{
	public static class API
	{
		[DllImport("kernel32.dll")]
		public static extern void Sleep(uint dwMilliseconds);

		[DllImport("kernel32.dll")]
		public static extern uint SleepEx(uint dwMilliseconds, bool bAlertable);

		[DllImport("user32.dll", CharSet = CharSet.Auto)]
		public static extern int SendMessage(IntPtr hWnd, int wMsg, IntPtr wParam, IntPtr lParam);
	}
}
