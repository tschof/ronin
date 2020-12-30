using System;
using System.Windows.Forms;

using FormEx;
using System.ComponentModel;

using DataGridViewEx;
using System.Collections.Generic;
using ContextMenuEx;
using System.Diagnostics;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmClock : VistaWindowBorder
	{
		public string ClockDisplayFormat
		{
			get
			{
				switch (digitalClock.ClockDisplayFormat)
				{
					case ClockEx.ClockFormat.TwentyFourHourFormat:
						return "24Hour";
					default:
						return "12Hour";
				}
			}
			set
			{
				switch (value)
				{
					case "24Hour":
						digitalClock.ClockDisplayFormat = ClockEx.ClockFormat.TwentyFourHourFormat;
						break;
					default:
						digitalClock.ClockDisplayFormat = ClockEx.ClockFormat.TwelveHourFormat;
						break;
				}

				Caption = String.Format("{0} {1} Cycle", ClockTimeZoneDisplayFormat, ClockDisplayFormat);
			}
		}

		public string ClockTimeZoneDisplayFormat
		{
			get
			{
				switch (digitalClock.TimeZoneDisplayFormat)
				{
					case ClockEx.TimeZoneFormat.Atlantic:
						return "Atlantic";
					case ClockEx.TimeZoneFormat.Eastern:
						return "Eastern";
					case ClockEx.TimeZoneFormat.Central:
						return "Central";
					case ClockEx.TimeZoneFormat.Mountain:
						return "Mountain";
					case ClockEx.TimeZoneFormat.Pacific:
						return "Pacific";
					case ClockEx.TimeZoneFormat.Local:
					default:
						return "Local";
				}
			}
			set
			{
				switch (value)
				{
					case "Atlantic":
						digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Atlantic;
						break;
					case "Eastern":
						digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Eastern;
						break;
					case "Central":
						digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Central;
						break;
					case "Mountain":
						digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Mountain;
						break;
					case "Pacific":
						digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Pacific;
						break;
					case "Local":
					default:
						digitalClock.TimeZoneDisplayFormat = ClockEx.TimeZoneFormat.Local;
						break;
				}

				Caption = String.Format("{0} {1} Cycle", ClockTimeZoneDisplayFormat, ClockDisplayFormat);
			}
		}

		#region - Constructor -

		public frmClock()
		{
			InitializeComponent();
			Initialize();
		}

		private void Initialize()
		{
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			Caption = ClockTimeZoneDisplayFormat;
		}

		#endregion

		#region - Form Events -

		private void Form_ShowAllClicked(object sender, EventArgs e)
		{
			try
			{
				GLOBAL.HWindows.ShowAll();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_FormClosing(object sender, EventArgs e)
		{
			try
			{
				digitalClock.Stop();
				GLOBAL.HWindows.CloseWindow(this);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Clock Menu -

		private menuBaseAction _menuClockWindow;
		private PropertyContainer _menuClockWindowContainer;
		private void digitalClock_MouseClick(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				if (_menuClockWindow == null)
				{
					Dictionary<string, string> items = new Dictionary<string, string>();
					items.Add("12Hour", "12 Hour");
					items.Add("24Hour", "24 Hour");
					items.Add("Atlantic", "Atlantic");
					items.Add("Eastern", "Eastern");
					items.Add("Central", "Central");
					items.Add("Mountain", "Mountain");
					items.Add("Pacific", "Pacific");
					items.Add("Local", "Local");

					_menuClockWindow = new menuBaseAction(items, true);
					_menuClockWindow.PropertyChanged += new PropertyChangedEventHandler(menuClockWindow_PropertyChanged);
				}

				if (_menuClockWindowContainer == null)
				{
					_menuClockWindowContainer = new PropertyContainer(_menuClockWindow);
					_menuClockWindowContainer.Closed += new ToolStripDropDownClosedEventHandler(menuClockWindowContainer_Closed);
				}

				_menuClockWindowContainer.Show(Cursor.Position);
			}
		}

		private void menuClockWindow_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuClockWindowContainer.Close();
			switch (e.PropertyName)
			{
				case "12Hour":
				case "24Hour":
					ClockDisplayFormat = e.PropertyName;
					break;
				case "Atlantic":
				case "Eastern":
				case "Central":
				case "Mountain":
				case "Pacific":
				case "Local":
					ClockTimeZoneDisplayFormat = e.PropertyName;
					break;
			}
		}

		private void menuClockWindowContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuClockWindowContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuClockWindowContainer_Closed);
			_menuClockWindowContainer = null;
			_menuClockWindow.PropertyChanged -= new PropertyChangedEventHandler(menuClockWindow_PropertyChanged);
			_menuClockWindow = null;
		}

		#endregion
	}
}