using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Drawing;

namespace ROC
{
	public static class FutureMatrixColor
	{
		private static int _colorShiftInterval = 18;
		[Browsable(false)]
		internal static int ColorShiftInterval
		{
			get
			{
				return _colorShiftInterval;
			}
			set
			{
				_colorShiftInterval = value;
			}
		}

		private static int _colorShiftIntervalMax = 10;
		[Browsable(false)]
		internal static int ColorShiftIntervalMax
		{
			get
			{
				return _colorShiftIntervalMax;
			}
			set
			{
				_colorShiftIntervalMax = value;
			}
		}

		internal static Color ShiftColor(Color orgColor, int interval)
		{
			try
			{
				if (interval > ColorShiftIntervalMax)
				{
					interval = ColorShiftIntervalMax;
				}

				int shiftValue = ColorShiftInterval * interval;

				int red = 0;
				int green = 0;
				int blue = 0;

				if (orgColor.R - shiftValue >= 0 && orgColor.R - shiftValue <= 255)
				{
					red = orgColor.R - shiftValue;
				}
				if (orgColor.G - shiftValue >= 0 && orgColor.G - shiftValue <= 255)
				{
					green = orgColor.G - shiftValue;
				}
				if (orgColor.B - shiftValue >= 0 && orgColor.B - shiftValue <= 255)
				{
					blue = orgColor.B - shiftValue;
				}

				return Color.FromArgb(red, green, blue);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return orgColor;
		}
	}
}
