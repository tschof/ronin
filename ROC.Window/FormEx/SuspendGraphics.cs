using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace FormEx
{
	public sealed class SuspendGraphics
	{
		[DllImport("User32")]
		private static extern IntPtr SendMessage(IntPtr hWnd, Int32 msg, Int32 wParam, Int32 lParam);

		private const int WM_SETREDRAW = 0x000B;

		public static void SuspendDrawing(Control c)
		{
			if (c != null)
			{
				SendMessage(c.Handle, WM_SETREDRAW, 0, 0);
			}
		}

		public static void ResumeDrawing(Control c)
		{
			if (c != null)
			{
				SendMessage(c.Handle, WM_SETREDRAW, 1, 0);
				//c.Invalidate();
			}
		}
	}
}
