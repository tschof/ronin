using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;

namespace ClockEx
{
	public enum ColonType
	{ 
		Circular, 
		Rectangular 
	};
	
	public enum ClockFormat 
	{ 
		TwentyFourHourFormat, 
		TwelveHourFormat 
	};

	public enum TimeZoneFormat
	{
		Local,
		Atlantic,
		Eastern,
		Central,
		Mountain,
		Pacific
	}

	public partial class DigitalClockCtrl : UserControl
	{
		private sealed class ClockAreas
		{
			public const int hour1 = 0;
			public const int hour2 = 1;
			public const int min1 = 2;
			public const int min2 = 3;
			public const int sec1 = 4;
			public const int sec2 = 5;
			public const int colon1 = 6;
			public const int colon2 = 7;
			public const int ampm = 8;
			public const int ms = 9;
		}

		private sealed class ImageKeys
		{
			public const int Digit_0 = 0;
			public const int Digit_1 = 1;
			public const int Digit_2 = 2;
			public const int Digit_3 = 3;
			public const int Digit_4 = 4;
			public const int Digit_5 = 5;
			public const int Digit_6 = 6;
			public const int Digit_7 = 7;
			public const int Digit_8 = 8;
			public const int Digit_9 = 9;
			public const int Digit_Null = 10;

			public const int Digit_Small_0 = 11;
			public const int Digit_Small_1 = 12;
			public const int Digit_Small_2 = 13;
			public const int Digit_Small_3 = 14;
			public const int Digit_Small_4 = 15;
			public const int Digit_Small_5 = 16;
			public const int Digit_Small_6 = 17;
			public const int Digit_Small_7 = 18;
			public const int Digit_Small_8 = 19;
			public const int Digit_Small_9 = 20;
			public const int Digit_Small_Null = 21;

			public const int AM = 22;
			public const int PM = 23;
			public const int Colon_On = 24;
			public const int Colon_Off = 25;
		}

		#region - Local Variable -

		private Size[] _digitSize = null;
		private Point[] _digitLocation = null;
		private Dictionary<int, Image> _digitImages;

		private Timer _showTimer;
		private bool _showBusy = false;

		private Timer _colonTimer;
		private bool _colonBusy = false;

		private bool _blink = false;

		private bool _readyForResize = false;

		Graphics _graphics = null;

		#endregion

		#region - Properties -

		private ClockFormat _clockDisplayFormat = ClockFormat.TwelveHourFormat;
		public ClockFormat ClockDisplayFormat
		{
			get
			{
				return _clockDisplayFormat;
			}
			set
			{
				_clockDisplayFormat = value;
			}
		}

		private TimeZoneFormat _timeZoneFormat = TimeZoneFormat.Local;
		private int _timeZoneOffsetFromUTC = 0;
		public TimeZoneFormat TimeZoneDisplayFormat
		{
			get
			{
				return _timeZoneFormat;
			}
			set
			{
				_timeZoneOffsetFromUTC = 0;
				switch (value)
				{
					case TimeZoneFormat.Atlantic:
						_timeZoneFormat = TimeZoneFormat.Atlantic;
						_timeZoneOffsetFromUTC = -4;
						break;
					case TimeZoneFormat.Eastern:
						_timeZoneFormat = TimeZoneFormat.Eastern;
						_timeZoneOffsetFromUTC = -5;
						break;
					case TimeZoneFormat.Central:
						_timeZoneFormat = TimeZoneFormat.Central;
						_timeZoneOffsetFromUTC = -6;
						break;
					case TimeZoneFormat.Mountain:
						_timeZoneFormat = TimeZoneFormat.Mountain;
						_timeZoneOffsetFromUTC = -7;
						break;
					case TimeZoneFormat.Pacific:
						_timeZoneFormat = TimeZoneFormat.Pacific;
						_timeZoneOffsetFromUTC = -8;
						break;
					default:
						_timeZoneFormat = TimeZoneFormat.Local;
						_timeZoneOffsetFromUTC = 0;
						break;
				}

				if (_timeZoneFormat != TimeZoneFormat.Local)
				{
					if (DateTime.Now.IsDaylightSavingTime())
					{
						_timeZoneOffsetFromUTC = _timeZoneOffsetFromUTC + 1;
					}
				}
			}
		}

		private Color _digitsColor = Color.Red;
		public Color DigitsColor
		{
			get
			{
				return _digitsColor;
			}
			set
			{
				this.Invalidate();
				DigitalDisplay.SetPenColor(value);
			}
		}

		#endregion

		#region - Constructor -

		public DigitalClockCtrl()
		{
			InitializeComponent();
			Initialize();
		}

		private void Initialize()
		{
			_digitSize = new Size[10];
			_digitLocation = new Point[10];
			_digitImages = new Dictionary<int, Image>();

			DigitalDisplay.SetPenColor(_digitsColor);
			DigitalDisplay.SetBackColor(BackColor);

			StartShowTimer();
			StartColonTimer();
		}

		#endregion

		#region - Show Timer -

		private void StartShowTimer()
		{
			if (_showTimer == null)
			{
				_showTimer = new Timer();
				_showTimer.Interval = 100;
				_showTimer.Tick += new EventHandler(showTimer_Tick);
				_showTimer.Enabled = true;
			}
		}

		private void StopShowTimer()
		{
			if (_showTimer != null)
			{
				_showTimer.Stop();
				_showTimer.Tick -= new EventHandler(showTimer_Tick);
			}
			_showTimer = null;
		}

		private void showTimer_Tick(object sender, System.EventArgs e)
		{
			try
			{
				_showTimer.Stop();
				if (_graphics != null)
				{
					DisplayTime(_graphics);
				}
				_showTimer.Start();
			}
			catch (Exception ex)
			{
				Debug.Print(ex.Message);
			}
		}

		#endregion

		#region - Colon Timer -

		private void StartColonTimer()
		{
			if (_colonTimer == null)
			{
				_colonTimer = new Timer();
				_colonTimer.Interval = 1000;
				_colonTimer.Tick += new EventHandler(colonTimer_Tick);
				_colonTimer.Enabled = true;
			}
		}

		private void StopColonTimer()
		{
			if (_colonTimer != null)
			{
				_colonTimer.Stop();
				_colonTimer.Tick -= new EventHandler(colonTimer_Tick);
			}
			_colonTimer = null;
		}

		private void colonTimer_Tick(object sender, System.EventArgs e)
		{
			if (!_colonBusy)
			{
				_colonBusy = true;

				_blink = !_blink;

				_colonBusy = false;
			}
		}

		#endregion

		#region - Local Event -

		protected override void OnPaint(PaintEventArgs e)
		{
			try
			{
				if (_graphics != null)
				{
					DisplayTime(_graphics);
				}
			}
			catch (Exception ex)
			{
				Debug.Print(ex.Message);
			}

 			//base.OnPaint(e);
		}

		protected override void OnLoad(EventArgs e)
		{
			PreparePanels();

			_readyForResize = true;
			_graphics = Graphics.FromHwnd(this.Handle);

			//base.OnLoad(e);
		}

		protected override void OnResize(EventArgs e)
		{
			if (_readyForResize)
			{
				lock (this)
				{
					PreparePanels();
				}

				Invalidate();
			}
			else
			{
				base.OnResize(e);
			}
		}

		#endregion

		private void DisplayTime(Graphics g)
		{
			if (!_showBusy)
			{
				_showBusy = true;

				DateTime dt = DateTime.UtcNow;
				if (_timeZoneFormat != TimeZoneFormat.Local)
				{
					dt = dt.AddHours(_timeZoneOffsetFromUTC);
				}
				else
				{
					dt = DateTime.Now;
				}

				string dtString = "";
				if (_clockDisplayFormat == ClockFormat.TwelveHourFormat)
				{
					dtString = String.Format("{0:hh:mm:ssft}", dt);
				}
				else
				{
					dtString = String.Format("{0:HH:mm:ssft}", dt);
				}

				int index = 0;
				Point pos = new Point(0, 0);

				bool drawDigit = false;
				bool drawColon = false;
				bool drawMS = false;
				bool drawAMPM = false;
				foreach(char c in dtString.ToCharArray())
				{
					drawDigit = false;
					drawColon = false;
					drawMS = false;
					drawAMPM = false;

					switch (index)
					{
						case 0:
							pos = _digitLocation[ClockAreas.hour1];
							drawDigit = true;
							break;
						case 1:
							pos = _digitLocation[ClockAreas.hour2];
							drawDigit = true;
							break;
						case 2:
							// Colon1
							pos = _digitLocation[ClockAreas.colon1];
							drawColon = true;
							break;
						case 3:
							pos = _digitLocation[ClockAreas.min1];
							drawDigit = true;
							break;
						case 4:
							pos = _digitLocation[ClockAreas.min2];
							drawDigit = true;
							break;
						case 5:
							// Colon2
							pos = _digitLocation[ClockAreas.colon2];
							drawColon = true;
							break;
						case 6:
							pos = _digitLocation[ClockAreas.sec1];
							drawDigit = true;
							break;
						case 7:
							pos = _digitLocation[ClockAreas.sec2];
							drawDigit = true;
							break;
						case 8:
							pos = _digitLocation[ClockAreas.ms];
							drawMS = true;
							break;
						case 9:
							pos = _digitLocation[ClockAreas.ampm];
							drawAMPM = true;
							break;
					}

					if (drawDigit)
					{
						switch (c)
						{
							case '0':
								g.DrawImage(_digitImages[ImageKeys.Digit_0], pos);
								break;
							case '1':
								g.DrawImage(_digitImages[ImageKeys.Digit_1], pos);
								break;
							case '2':
								g.DrawImage(_digitImages[ImageKeys.Digit_2], pos);
								break;
							case '3':
								g.DrawImage(_digitImages[ImageKeys.Digit_3], pos);
								break;
							case '4':
								g.DrawImage(_digitImages[ImageKeys.Digit_4], pos);
								break;
							case '5':
								g.DrawImage(_digitImages[ImageKeys.Digit_5], pos);
								break;
							case '6':
								g.DrawImage(_digitImages[ImageKeys.Digit_6], pos);
								break;
							case '7':
								g.DrawImage(_digitImages[ImageKeys.Digit_7], pos);
								break;
							case '8':
								g.DrawImage(_digitImages[ImageKeys.Digit_8], pos);
								break;
							case '9':
								g.DrawImage(_digitImages[ImageKeys.Digit_9], pos);
								break;
							default:
								g.DrawImage(_digitImages[ImageKeys.Digit_Null], pos);
								break;
						}
					}

					if (drawColon)
					{
						if (_blink)
						{
							g.DrawImage(_digitImages[ImageKeys.Colon_On], pos);
						}
						else
						{
							g.DrawImage(_digitImages[ImageKeys.Colon_Off], pos);
						}
					}

					if (drawMS)
					{
						switch (c)
						{
							case '0':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_0], pos);
								break;
							case '1':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_1], pos);
								break;
							case '2':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_2], pos);
								break;
							case '3':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_3], pos);
								break;
							case '4':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_4], pos);
								break;
							case '5':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_5], pos);
								break;
							case '6':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_6], pos);
								break;
							case '7':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_7], pos);
								break;
							case '8':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_8], pos);
								break;
							case '9':
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_9], pos);
								break;
							default:
								g.DrawImage(_digitImages[ImageKeys.Digit_Small_Null], pos);
								break;
						}
					}

					if (drawAMPM)
					{
						if (_clockDisplayFormat == ClockFormat.TwelveHourFormat)
						{
							switch (c)
							{
								case 'P':
									g.DrawImage(_digitImages[ImageKeys.PM], pos);
									break;
								case 'A':
									g.DrawImage(_digitImages[ImageKeys.AM], pos);
									break;
							}
						}
						else
						{
							g.DrawImage(_digitImages[ImageKeys.Digit_Small_Null], pos);
						}
					}

					index = index + 1;
				}

				_showBusy = false;
			}
		}

		private void PreparePanels()
		{
			// from the above assumptions for height and width
			// the height should be 2.4 units and width 8.8 units :-)
			// check height and width whichever is dominant and adjust the other
			// and set up margins
			Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);

			// widths, spacings and margins
			// height of colon display is same as a digit
			int digitWidth;
			int digitHeight;
			int colonWidth; 
			int spacing;
			// left and right margin
			float hmargin = 0;
			// top and bottom margin
			float vmargin = 0;

			// Calculate a digit width (which is our unit) from the above metrics
			// and settle for the least value
			int widthUnit = (int)(rect.Width / 8.8F);
			int heightUnit = (int)(rect.Height / 2.4F);
			digitWidth = (widthUnit < heightUnit) ? widthUnit : heightUnit;

			// height is twice of width
			digitHeight = 2 * digitWidth;
			// colon width is half of a digit
			colonWidth = digitWidth / 2; 
			spacing = digitWidth / 10;
			// atleast a spacing of 1 is required
			if (spacing < 1) spacing = 1;
			hmargin = (rect.Width - (8.8F * digitWidth)) / 2;
			vmargin = (rect.Height - digitHeight) / 2;

			// This is the basic rectangle, offset it as required
			Rectangle basicRect = new Rectangle(0, 0, (int)digitWidth, (int)digitHeight);
			int XOffset = (rect.Width - (digitWidth * 6 + colonWidth * 3 + spacing * 4)) / 2;
			int YOffset = (int)(vmargin);

			// calculated rectangle for a panel
			Rectangle calcRect;
			for (int i = 0; i < 6; i++)
			{
				calcRect = basicRect;
				XOffset = (int)(hmargin + (spacing * (i + 2 + (i / 2))) + (i * digitWidth) + ((i / 2) * colonWidth));
				calcRect.Offset(XOffset, YOffset);
				_digitSize[i] = calcRect.Size;
				_digitLocation[i] = calcRect.Location;
			}

			// for first colon
			calcRect = basicRect;
			calcRect.Width = (int)colonWidth;
			XOffset = (int)(hmargin + 3 * spacing + 2 * digitWidth);
			calcRect.Offset(XOffset, YOffset);
			_digitSize[ClockAreas.colon1] = calcRect.Size;
			_digitLocation[ClockAreas.colon1] = calcRect.Location;

			// for second colon
			calcRect = basicRect;
			calcRect.Width = (int)colonWidth;
			XOffset = (int)((6 * spacing) + (4 * digitWidth) + colonWidth + hmargin);
			calcRect.Offset(XOffset, YOffset);
			_digitSize[ClockAreas.colon2] = calcRect.Size;
			_digitLocation[ClockAreas.colon2] = calcRect.Location;

			// for displaying 'A'(AM) or 'P' (PM)
			calcRect = basicRect;
			calcRect.Width = (int)colonWidth;
			calcRect.Height = calcRect.Height / 2;
			XOffset = (int)((10 * spacing) + (6 * digitWidth) + (2 * colonWidth) + hmargin);
			calcRect.Offset(XOffset, (int)(YOffset + digitHeight / 2.0 + 1));
			_digitSize[ClockAreas.ampm] = calcRect.Size;
			_digitLocation[ClockAreas.ampm] = calcRect.Location;

			// for displaying 1/10 of a second
			// reuse AM/PM display panel rectangle
			// only change Y coordinate, shift upwards
			calcRect.Y = YOffset - 1; // just to keep it apart from AM/PM display
			_digitSize[ClockAreas.ms] = calcRect.Size;
			_digitLocation[ClockAreas.ms] = calcRect.Location;

			PrepareImages();
		}

		private void PrepareImages()
		{
			_digitImages = new Dictionary<int,Image>();

			_digitImages.Add(ImageKeys.Digit_0, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 0));
			_digitImages.Add(ImageKeys.Digit_1, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 1));
			_digitImages.Add(ImageKeys.Digit_2, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 2));
			_digitImages.Add(ImageKeys.Digit_3, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 3));
			_digitImages.Add(ImageKeys.Digit_4, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 4));
			_digitImages.Add(ImageKeys.Digit_5, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 5));
			_digitImages.Add(ImageKeys.Digit_6, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 6));
			_digitImages.Add(ImageKeys.Digit_7, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 7));
			_digitImages.Add(ImageKeys.Digit_8, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 8));
			_digitImages.Add(ImageKeys.Digit_9, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1], 9));
			_digitImages.Add(ImageKeys.Digit_Null, DigitalDisplay.Draw(_digitSize[ClockAreas.hour1]));

			_digitImages.Add(ImageKeys.Digit_Small_0, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 0));
			_digitImages.Add(ImageKeys.Digit_Small_1, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 1));
			_digitImages.Add(ImageKeys.Digit_Small_2, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 2));
			_digitImages.Add(ImageKeys.Digit_Small_3, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 3));
			_digitImages.Add(ImageKeys.Digit_Small_4, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 4));
			_digitImages.Add(ImageKeys.Digit_Small_5, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 5));
			_digitImages.Add(ImageKeys.Digit_Small_6, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 6));
			_digitImages.Add(ImageKeys.Digit_Small_7, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 7));
			_digitImages.Add(ImageKeys.Digit_Small_8, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 8));
			_digitImages.Add(ImageKeys.Digit_Small_9, DigitalDisplay.Draw(_digitSize[ClockAreas.ms], 9));
			_digitImages.Add(ImageKeys.Digit_Small_Null, DigitalDisplay.Draw(_digitSize[ClockAreas.ms]));

			_digitImages.Add(ImageKeys.AM, DigitalDisplay.Draw(_digitSize[ClockAreas.ampm], 'A'));
			_digitImages.Add(ImageKeys.PM, DigitalDisplay.Draw(_digitSize[ClockAreas.ampm], 'P'));

			_digitImages.Add(ImageKeys.Colon_On, DigitalDisplay.Draw(_digitSize[ClockAreas.colon1], ColonType.Rectangular, false));
			_digitImages.Add(ImageKeys.Colon_Off, DigitalDisplay.Draw(_digitSize[ClockAreas.colon1], ColonType.Rectangular, true));
		}

		public void Stop()
		{
			StopShowTimer();
			StopColonTimer();
		}
	}
}