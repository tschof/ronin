using System;
using System.Runtime.InteropServices;

namespace FormEx
{
	internal sealed class DockWindowHelper
	{
		//public const int WM_NCLBUTTONDOWN = 0xA1;
		//public const int HTCAPTION = 0x2;

		[DllImport("User32.dll")]
		public static extern bool ReleaseCapture();

		[DllImport("User32.dll")]
		public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);

		[DllImport("user32.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		public static extern short GetAsyncKeyState(int vKey);

		[DllImport("user32.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
		public static extern IntPtr GetDesktopWindow();

		public class VK
		{
			public const int VK_SHIFT = 0x10;
			public const int VK_CONTROL = 0x11;
			public const int VK_MENU = 0x12;
			public const int VK_ESCAPE = 0x1B;

			public static bool IsKeyPressed(int KeyCode)
			{
				return (GetAsyncKeyState(KeyCode) & 0x0800) == 0;
			}
		}

		public class WM
		{
			public const int WM_HTCAPTION = 0x2;

			public const int WM_NCCREATE = 0x0081;
			public const int WM_NCDESTROY = 0x0082;
			public const int WM_NCCALCSIZE = 0x0083;
			public const int WM_NCHITTEST = 0x84;
			public const int WM_NCPAINT = 0x0085;
			public const int WM_NCACTIVATE = 0x0086;

			public const int WM_NCMOUSEMOVE = 0x00A0;
			public const int WM_NCLBUTTONDOWN = 0x00A1;
			public const int WM_NCLBUTTONUP = 0x00A2;
			public const int WM_NCLBUTTONDBLCLK = 0x00A3;
			public const int WM_NCRBUTTONDOWN = 0x00A4;
			public const int WM_NCRBUTTONUP = 0x00A5;
			public const int WM_NCRBUTTONDBLCLK = 0x00A6;
			public const int WM_NCMBUTTONDOWN = 0x00A7;
			public const int WM_NCMBUTTONUP = 0x00A8;
			public const int WM_NCMBUTTONDBLCLK = 0x00A9;

			public const int WM_MOUSEMOVE = 0x0200;
			public const int WM_LBUTTONDOWN = 0x0201;
			public const int WM_LBUTTONUP = 0x0202;
			public const int WM_KEYDOWN = 0x0100;

			public const int WM_CREATE = 0x0001;
			public const int WM_DESTROY = 0x0002;
			public const int WM_SHOWWINDOW = 0x0018;
			public const int WM_CLOSE = 0x0010;
			public const int WM_SETTEXT = 0x000C;
			public const int WM_GETTEXT = 0x000D;
			public const int WM_GETMINMAXINFO = 0x0024;
			public const int WM_WINDOWPOSCHANGED = 0x0047;
		}

		public class HT
		{
			public const int HTERROR = (-2);
			public const int HTTRANSPARENT = (-1);
			public const int HTNOWHERE = 0;
			public const int HTCLIENT = 1;
			public const int HTCAPTION = 2;
			public const int HTSYSMENU = 3;
			public const int HTGROWBOX = 4;
			public const int HTSIZE = HTGROWBOX;
			public const int HTMENU = 5;
			public const int HTHSCROLL = 6;
			public const int HTVSCROLL = 7;
			public const int HTMINBUTTON = 8;
			public const int HTMAXBUTTON = 9;
			public const int HTLEFT = 10;
			public const int HTRIGHT = 11;
			public const int HTTOP = 12;
			public const int HTTOPLEFT = 13;
			public const int HTTOPRIGHT = 14;
			public const int HTBOTTOM = 15;
			public const int HTBOTTOMLEFT = 16;
			public const int HTBOTTOMRIGHT = 17;
			public const int HTBORDER = 18;
			public const int HTREDUCE = HTMINBUTTON;
			public const int HTZOOM = HTMAXBUTTON;
			public const int HTSIZEFIRST = HTLEFT;
			public const int HTSIZELAST = HTBOTTOMRIGHT;

			public const int HTOBJECT = 19;
			public const int HTCLOSE = 20;
			public const int HTHELP = 21;
		}

		public class Bit
		{
			public static int HiWord(int iValue)
			{
				return ((iValue >> 16) & 0xFFFF);
			}
			public static int LoWord(int iValue)
			{
				return (iValue & 0xFFFF);
			}

			public static IntPtr MakeLParam(int x, int y)
			{
				return (IntPtr)((y << 16) | (x & 0xffff));
			}
		}
	}
}
