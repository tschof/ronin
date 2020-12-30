using System;
using System.Runtime.InteropServices;

namespace FormEx
{
	public sealed class AnimatedWindowHelper
	{
		public enum SlideTypes
		{
			HOR_POS,
			HOR_NEG,
			VER_POS,
			VER_NEG,
			CENTER,
		}

		[Flags]
		private enum AnimateWindowFlags
		{
			AW_HOR_POSITIVE = 0x00000001,
			AW_HOR_NEGATIVE = 0x00000002,
			AW_VER_POSITIVE = 0x00000004,
			AW_VER_NEGATIVE = 0x00000008,
			AW_CENTER = 0x00000010,
			AW_HIDE = 0x00010000,
			AW_ACTIVATE = 0x00020000,
			AW_SLIDE = 0x00040000,
			AW_BLEND = 0x00080000
		}

		[DllImport("user32")]
		private static extern bool AnimateWindow(IntPtr hwnd, int time, AnimateWindowFlags flags);

		public static void FadeIn(IntPtr hwnd)
		{
			FadeIn(hwnd, 1000);
		}
		public static void FadeIn(IntPtr hwnd, int time)
		{
			AnimateWindow(hwnd, time, AnimateWindowFlags.AW_BLEND);
		}

		public static void FadeOut(IntPtr hwnd)
		{
			FadeOut(hwnd, 1000);
		}
		public static void FadeOut(IntPtr hwnd, int time)
		{
			AnimateWindow(hwnd, time, AnimateWindowFlags.AW_HIDE | AnimateWindowFlags.AW_BLEND);
		}

		public static void SlidIn(IntPtr hwnd)
		{
			SlidIn(hwnd, 1000, SlideTypes.CENTER);
		}
		public static void SlidIn(IntPtr hwnd, int time)
		{
			SlidIn(hwnd, time, SlideTypes.CENTER);
		}
		public static void SlidIn(IntPtr hwnd, SlideTypes type)
		{
			SlidIn(hwnd, 1000, type);
		}
		public static void SlidIn(IntPtr hwnd, int time, SlideTypes type)
		{
			switch (type)
			{
				case SlideTypes.HOR_NEG:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_HOR_NEGATIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_ACTIVATE);
					break;
				case SlideTypes.HOR_POS:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_HOR_POSITIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_ACTIVATE);
					break;
				case SlideTypes.VER_NEG:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_VER_NEGATIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_ACTIVATE);
					break;
				case SlideTypes.VER_POS:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_VER_POSITIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_ACTIVATE);
					break;
				case SlideTypes.CENTER:
				default:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_CENTER | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_ACTIVATE);
					break;
			}
		}

		public static void SlideOut(IntPtr hwnd)
		{
			SlideOut(hwnd, 1000, SlideTypes.CENTER);
		}
		public static void SlideOut(IntPtr hwnd, int time)
		{
			SlideOut(hwnd, time, SlideTypes.CENTER);
		}
		public static void SlideOut(IntPtr hwnd, SlideTypes type)
		{
			SlideOut(hwnd, 1000, type);
		}
		public static void SlideOut(IntPtr hwnd, int time, SlideTypes type)
		{
			switch (type)
			{
				case SlideTypes.HOR_NEG:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_HOR_NEGATIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_HIDE);
					break;
				case SlideTypes.HOR_POS:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_HOR_POSITIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_HIDE);
					break;
				case SlideTypes.VER_NEG:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_VER_NEGATIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_HIDE);
					break;
				case SlideTypes.VER_POS:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_VER_POSITIVE | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_HIDE);
					break;
				case SlideTypes.CENTER:
				default:
					AnimateWindow(hwnd, time, AnimateWindowFlags.AW_CENTER | AnimateWindowFlags.AW_SLIDE | AnimateWindowFlags.AW_HIDE);
					break;
			}
		}

	}
}
