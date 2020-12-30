using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using ROMEx;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class BaseUserControl : UserControl
	{
		public BaseUserControl()
		{
			InitializeComponent();
		}

		#region - Events -

		[Description("Event raised when the UI Timer Ticked."), Category("Property Changed")]
		public event EventHandler UITimerTicked;

		[Description("Event raised when the Default UI Timer Interval Changed"), Category("Property Changed")]
		public event EventHandler DefaultUITimerIntervalChanged;

		#endregion

		private int _defaultUITimerInterval = 150;
		private Timer _uiTimer;

		#region - OnEvents -

		protected virtual void OnUITimerTicked(EventArgs e)
		{
			if (UITimerTicked != null)
			{
				UITimerTicked(this, e);
			}
		}

		protected virtual void OnDefaultUITimerIntervalChanged(EventArgs e)
		{
			if (DefaultUITimerIntervalChanged != null)
			{
				DefaultUITimerIntervalChanged(this, e);
			}
		}

		#endregion

		#region - Shared Functions -

		public void SetColumnMode(DataGridView grid, DataGridViewAutoSizeColumnMode mode)
		{
			int oldColWidth = 0;
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (mode)
				{
					case DataGridViewAutoSizeColumnMode.NotSet:
					case DataGridViewAutoSizeColumnMode.None:
						oldColWidth = col.Width;
						col.AutoSizeMode = mode;
						col.Width = oldColWidth;
						break;
					default:
						col.AutoSizeMode = mode;
						break;
				}
			}
		}

		public void ResizeToFit(DataGridView grid)
		{
			int totoalWidth = 0;
			foreach (DataGridViewColumn col in grid.Columns)
			{
				if (col.Visible)
				{
					totoalWidth = totoalWidth + col.Width;
				}
			}

			//Control 1 = Vertical Scroll Bar
			if (grid.Controls[1].Visible)
			{
				Width = totoalWidth + grid.Controls[1].Width;
			}
			else
			{
				Width = totoalWidth;
			}

			//Control 0 = Horizontal Scroll Bar
			if (MaximumSize.Width != 0)
			{
				while (grid.Controls[0].Visible && Width < MaximumSize.Width)
				{
					Width += 5;
				}
			}
			else
			{
				while (grid.Controls[0].Visible)
				{
					Width += 5;
				}
			}
		}

		public void ResizeToFitBoth(DataGridView grid)
		{
			int totoalWidth = 0;

			foreach (DataGridViewColumn col in grid.Columns)
			{
				if (col.Visible)
				{
					totoalWidth = totoalWidth + col.Width;
				}
			}

			//// Control 0 = Horizontal Scroll Bar
			//if (MaximumSize.Width != 0)
			//{
			//    while (grid.Controls[0].Visible && Width < MaximumSize.Width)
			//    {
			//        Width += 5;
			//    }
			//}
			//else
			//{
			//    while (grid.Controls[0].Visible)
			//    {
			//        Width += 5;
			//    }
			//}

			//Control 1 = Vertical Scroll Bar
			if (grid.Controls[1].Visible)
			{
				if (MaximumSize.Height != 0)
				{
					while (grid.Controls[1].Visible && Height < MaximumSize.Height)
					{
						Height += 5;
					}
				}
				else
				{
					while (grid.Controls[1].Visible)
					{
						Height += 5;
					}
				}
			}

			if (grid.Controls[1].Visible)
			{
				Width = totoalWidth + grid.Controls[1].Width + 10;
			}
			else
			{
				Width = totoalWidth + 10;
			}
		}

		#endregion

		#region - Timer -

		public void StartTimer()
		{
			StartTimer(_defaultUITimerInterval);
		}
		public void StartTimer(int interval)
		{
			if (_uiTimer == null)
			{
				_uiTimer = new Timer();
				_uiTimer.Tick += new EventHandler(uiTimer_Tick);
				_uiTimer.Interval = interval;
				_uiTimer.Enabled = true;
			}
		}

		public void StopTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Stop();
				_uiTimer.Tick -= new EventHandler(uiTimer_Tick);
			}

			_uiTimer = null;
		}

		public void PauseTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Stop();
			}
		}

		public void ResumeTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Start();
			}
		}

		private void uiTimer_Tick(object sender, EventArgs e)
		{
			PauseTimer();

			OnUITimerTicked(EventArgs.Empty);

			ResumeTimer();
		}

		#endregion

		//private Dictionary<string, string> _imSymbolNeeded;
		//public Dictionary<string, string> ImSymbolNeeded
		//{
		//    get
		//    {
		//        if (_imSymbolNeeded == null)
		//        {
		//            _imSymbolNeeded = new Dictionary<string, string>();
		//        }
		//        return _imSymbolNeeded;
		//    }
		//    set
		//    {
		//        _imSymbolNeeded = value;
		//    }
		//}

		//private BaseSecurityInfo _currentSecInfo;
		//public BaseSecurityInfo CurrentSecInfo
		//{
		//    get
		//    {
		//        if (_currentSecInfo == null)
		//        {
		//            _currentSecInfo = new BaseSecurityInfo();
		//        }
		//        return _currentSecInfo;
		//    }
		//    set
		//    {
		//        _currentSecInfo = value;
		//    }
		//}

		//public sealed class TradeTicks
		//{
		//    public const int Up = 0;
		//    public const int None = 1;
		//    public const int Down = 2;
		//}

		//private List<string> _mdSymbols = new List<string>();
		//public List<string> MDSymbols
		//{
		//    get
		//    {
		//        return _mdSymbols;
		//    }
		//    set
		//    {
		//        _mdSymbols = value;
		//    }
		//}

		//public long GetOrderTypeCode(string type)
		//{
		//    switch (type)
		//    {
		//        case "MARKET":
		//            return CSVEx.CSVFieldIDs.OrderTypes.Market;
		//        case "MOC":
		//            return CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose;
		//        case "MOO":
		//            return CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen;
		//        case "LOC":
		//            return CSVEx.CSVFieldIDs.OrderTypes.LimitOnClose;
		//        case "STOP":
		//            return CSVEx.CSVFieldIDs.OrderTypes.Stop;
		//        case "STOPLIMIT":
		//            return CSVEx.CSVFieldIDs.OrderTypes.StopLimit;
		//        case "OCO":
		//            return CSVEx.CSVFieldIDs.OrderTypes.OCO;
		//        case "LIMIT":
		//        default:
		//            return CSVEx.CSVFieldIDs.OrderTypes.Limit;
		//    }
		//}

		//public long GetDurationCode(string tif)
		//{
		//    switch (tif)
		//    {
		//        case "GTC":
		//            return CSVEx.CSVFieldIDs.TIFCodes.GTC;
		//        case "IOC":
		//            return CSVEx.CSVFieldIDs.TIFCodes.IOC;
		//        case "FOK":
		//            return CSVEx.CSVFieldIDs.TIFCodes.FOK;
		//        case "OPG":
		//            return CSVEx.CSVFieldIDs.TIFCodes.OPG;
		//        case "DAY":
		//        default:
		//            return CSVEx.CSVFieldIDs.TIFCodes.Day;
		//    }
		//}
	}
}
