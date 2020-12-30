using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

using Common;
using FormEx;
using ROMEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Code")]
	public partial class BaseTicket : VistaWindowBorder
	{
		internal enum PriceType
		{
			Limit,
			Stop,
			Peg,
		}

		private bool _hasFirstUpdate = false;
		internal bool HasFirstUpdate
		{
			get
			{
				return _hasFirstUpdate;
			}
			set
			{
				_hasFirstUpdate = value;
			}
		}

		private Dictionary<string, Control> _controls = new Dictionary<string, Control>();
		private Dictionary<string, PanelEx.PanelManager> _panelManagers = new Dictionary<string, PanelEx.PanelManager>();

		#region - Setup Mouse Events -

		internal void Setup_MouseEnterEvent()
		{
			foreach (Control ctl in Controls)
			{
				Setup_MouseEnterEvent(ctl);

				if (ctl.Controls.Count > 0)
				{
					Setup_MouseEnterEvent(ctl.Controls);
				}
			}
		}

		private void Setup_MouseEnterEvent(Control.ControlCollection ctls)
		{
			foreach (Control ctl in ctls)
			{
				Setup_MouseEnterEvent(ctl);

				if (ctl.Controls.Count > 0)
				{
					Setup_MouseEnterEvent(ctl.Controls);
				}
			}
		}

		private void Setup_MouseEnterEvent(Control ctl)
		{
			if (ctl is NumericUpDownEx.NumericUpDownBase)
			{
				ctl.MouseEnter += new EventHandler(ctl_MouseEnter);
			}
			else if (ctl is PanelEx.PanelManager)
			{
				_panelManagers.TryAdd(ctl.Name, (PanelEx.PanelManager)ctl);
			} 
			else if (ctl is ComboBox)
			{
				switch (ctl.Name)
				{
					case "cboStrikesToShow":
					case "cboExpirationToShow":
						break;
					default:
						ctl.MouseEnter += new EventHandler(ctl_MouseEnter);
						break;
				}
			}
			else if (ctl is Button)
			{
				ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
			}
			else if (ctl is Label)
			{
				switch (ctl.Name)
				{
					case "lblQuantity":
					case "dspQuantityIncrement":
					case "lblOrderType":
					case "lblLimitPrice":
					case "dspLimitPriceTickSize":
					case "lblLimitPriceX":
					case "dspLimitPriceIncrement":
					case "lblStopPrice":
					case "dspStopPriceTickSize":
					case "lblStopPriceX":
					case "dspStopPriceIncrement":
					case "lblDuration":
					case "lblExchange":
					case "lblDoNotTranslate":
					// Quick Ticket
					case "lblStopLimitOffset":
					case "lblShortLender":
					// Stock Ticket
					case "lblPegPrice":
					case "dspPegPriceTickSize":
					case "lblPegPriceX":
					case "dspPegPriceIncrement":
					case "dspShowIncrement":
					case "dspMinQuantityIncrement":
					case "lblAlgoType":
					// Option Ticket:
					case "lblCallPut":
					case "lblExpiration":
					case "lblStrike":
						ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
						break;
				}
			}
			else if (ctl is CheckBox)
			{
				switch (ctl.Name)
				{
					case "chkShow":
					case "chkMinQty":
					case "chkInstructions":
					case "chkAlgoExchange":
					case "chkStartTime":
					case "chkEndTime":
					case "chkMVP":
					case "chkMonthlyOnly":
						ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
						break;
				}
			}
			else if (ctl is DataGridView)
			{
				ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
			}
			else if (ctl is RichTextBox)
			{
				switch (ctl.Name)
				{
					case "dspOrderMsg":
						ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
						break;
				}
			}
			else if (ctl is Panel)
			{
				if (ctl.BackColor == Color.LightSteelBlue)
				{
					_controls.TryAdd(ctl.Name, ctl);
					ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
				}
			}
			else if (ctl is ToolStrip)
			{
				switch (ctl.Name)
				{
					case "toolStripTraderInfo":
						ctl.MouseEnter += new EventHandler(ctl_MouseLeave);
						break;
				}
			}
		}

		private void ctl_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			if (sender != null)
			{
				ActiveControl = (Control)sender;
			}
		}

		private void ctl_MouseLeave(object sender, EventArgs e)
		{
			//Activate();
			if (ActiveControl != null)
			{
				if (ActiveControl.Name != "cboSymbolDetails")
				{
					ActiveControl = null;
				}
			}
		}

		#endregion

		#region - Current Trader Account -

		private DropDownTraderAccount _ddTraderAccount;
		internal DropDownTraderAccount DDAccountDropDown
		{
			get
			{
				return _ddTraderAccount;
			}
			set
			{
				_ddTraderAccount = value;
			}
		}

		internal string CurrentTradeFor
		{
			get
			{
				return DDAccountDropDown.CurrentTradeFor;
			}
			set
			{
				DDAccountDropDown.CurrentTradeFor = value;
			}
		}

		internal string CurrentAccount
		{
			get
			{
				return DDAccountDropDown.CurrentAccount;
			}
			set
			{
				DDAccountDropDown.CurrentAccount = value;
			}
		}

		internal string CurrentExchange
		{
			get
			{
				return DDAccountDropDown.CurrentExchange;
			}
			set
			{
				DDAccountDropDown.CurrentExchange = value;
			}
		}

		#endregion

		#region - Current SymbolDeatil -

		private DropDownSymbolDetails _ddSymbolDetails;
		internal DropDownSymbolDetails DDSymbolDetails
		{
			get
			{
				return _ddSymbolDetails;
			}
			set
			{
				_ddSymbolDetails = value;
			}
		}

		// This is used to get Symbol Setting Defaults and Subscription.
		internal string CurrentSymbolDetail
		{
			get
			{
				return DDSymbolDetails.CurrentSymbolDetail;
			}
			set
			{
				DDSymbolDetails.CurrentSymbolDetail = value.ToUpper();
			}
		}

		private DataTable _currentSymbolDetails;
		internal DataTable CurrentSymbolDetails
		{
			get
			{
				if (_currentSymbolDetails == null)
				{
					_currentSymbolDetails = new DataTable();

					_currentSymbolDetails.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));

					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_currentSymbolDetails.Columns["SymbolDetail"]);
					_currentSymbolDetails.PrimaryKey = keyColumns.ToArray();

					_currentSymbolDetails.DefaultView.Sort = "SymbolDetail";
				}
				return _currentSymbolDetails;
			}
			set
			{
				_currentSymbolDetails = value;
			}
		}

		#endregion

		#region - Do Not Translate Symbols -

		private List<string> _monthCode;
		private List<string> MonthCode
		{
			get
			{
				if (_monthCode == null)
				{
					_monthCode = new List<string>();
					_monthCode.Add("F");
					_monthCode.Add("G");
					_monthCode.Add("H");
					_monthCode.Add("J");
					_monthCode.Add("K");
					_monthCode.Add("M");
					_monthCode.Add("N");
					_monthCode.Add("Q");
					_monthCode.Add("U");
					_monthCode.Add("V");
					_monthCode.Add("X");
					_monthCode.Add("Z");
				}
				return _monthCode;
			}
		}

		internal bool IsFutureBySymbol(string symbol)
		{
			string[] tArray = symbol.Split(new char[] { '.' });
			symbol = tArray[0];

			if (symbol.Length <= 0)
			{
				return false;
			}

			int year = 0;
			if (Int32.TryParse(symbol.Substring(symbol.Length - 1, 1), out year))
			{
				if (MonthCode.Contains(symbol.Substring(symbol.Length - 2, 1)))
				{
					return true;
				}
			}

			return false;
		}

		internal bool DoNotSend(string symbol)
		{
			if (IsFutureBySymbol(symbol))
			{
				if (HelperFuture.DoNotTranslate.Contains(symbol.Substring(0, symbol.Length - 2)))
				{
					return true;
				}
			}
			return false;
		}

		#endregion

		#region - Current SecInfo -

		private BaseSecurityInfo _currentSecInfo;
		internal BaseSecurityInfo CurrentSecInfo
		{
			get
			{
				if (_currentSecInfo == null)
				{
					_currentSecInfo = new BaseSecurityInfo();
				}
				return _currentSecInfo;
			}
			set
			{
				_currentSecInfo = value;
			}
		}

		private Dictionary<string, string> _imSymbolNeeded;
		internal Dictionary<string, string> ImSymbolNeeded
		{
			get
			{
				if (_imSymbolNeeded == null)
				{
					_imSymbolNeeded = new Dictionary<string, string>();
				}
				return _imSymbolNeeded;
			}
			set
			{
				_imSymbolNeeded = value;
			}
		}

		internal string LongName
		{
			get
			{
				return Caption;
			}
			set
			{
				Caption = value.ToUpper().Replace("&", "&&");
			}
		}

		#endregion

		#region - Current MarketData -

		internal sealed class TradeTicks
		{
			public const int Up = 0;
			public const int None = 1;
			public const int Down = 2;
		}

		private int _bookDepthLimit = 10;
		internal int BookDepthLimit
		{
			get
			{
				return _bookDepthLimit;
			}
			set
			{
				_bookDepthLimit = value;
			}
		}

		private List<string> _mdSymbols = new List<string>();
		internal List<string> MDSymbols
		{
			get
			{
				return _mdSymbols;
			}
			set
			{
				_mdSymbols = value;
			}
		}

		#endregion

		#region - Order Panel Resizer -

		private Graphics _localGraphics;
		private Graphics LocalGraphics
		{
			get
			{
				if (_localGraphics == null)
				{
					_localGraphics = CreateGraphics();
				}
				return _localGraphics;
			}
		}

		public void SetPanelWidth(ref Panel p, NumericUpDown num)
		{
			string value = num.Value.ToString("F" + num.DecimalPlaces.ToString());

			if (value.Length > 0)
			{
				SetPanelWidth(ref p, value, num.Font);
			}
		}

		public void SetPanelWidth(ref Panel p, ComboBox cbo)
		{
			string value = cbo.Text;

			if (value.Length > 0)
			{
				SetPanelWidth(ref p, value, cbo.Font);
			}
		}

		private void SetPanelWidth(ref Panel p, string v, Font f)
		{
			int result = p.Width;

			SizeF sf = LocalGraphics.MeasureString(v, f);
			result = Convert.ToInt32(sf.Width) + 15 + 20;  // + Pad + UpDown
			if (p.Width < result)
			{
				p.Width = result;
			}
			else if (result > p.MinimumSize.Width && result < p.Width)
			{
				p.Width = result;
			}
			else if (result < p.MinimumSize.Width)
			{
				p.Width = p.MinimumSize.Width;
			}
		}

		#endregion

		#region - Quick Button -

		private QuickButtonSupport _quickButtonSupport;
		internal QuickButtonSupport QuickButtonSupprot
		{
			get
			{
				return _quickButtonSupport;
			}
			set
			{
				_quickButtonSupport = value;
			}
		}

		#endregion

		#region - Type To Code -

		public long GetOrderTypeCode(string type)
		{
			switch (type)
			{
				case "MARKET":
					return CSVEx.CSVFieldIDs.OrderTypes.Market;
				case "MOC":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose;
				case "MOO":
					return CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen;
				case "LOC":
					return CSVEx.CSVFieldIDs.OrderTypes.LimitOnClose;
				case "STOP":
					return CSVEx.CSVFieldIDs.OrderTypes.Stop;
				case "STOPLIMIT":
					return CSVEx.CSVFieldIDs.OrderTypes.StopLimit;
				case "OCO":
					return CSVEx.CSVFieldIDs.OrderTypes.OCO;
				case "PEG MKT":
					return CSVEx.CSVFieldIDs.OrderTypes.PegMkt;
				case "PEG PRI":
					return CSVEx.CSVFieldIDs.OrderTypes.PegPri;
				case "PEG MID":
					return CSVEx.CSVFieldIDs.OrderTypes.PegMid;
				case "VWAP MKT":
					return CSVEx.CSVFieldIDs.OrderTypes.VWAPMKT;
				case "VWAP LIM":
					return CSVEx.CSVFieldIDs.OrderTypes.VWAPLIM;
				case "GVWAP":
					return CSVEx.CSVFieldIDs.OrderTypes.GVWAP;
				case "IMBOO":
					return CSVEx.CSVFieldIDs.OrderTypes.IMBOO;
				case "IMBOC":
					return CSVEx.CSVFieldIDs.OrderTypes.IMBOC;
				case "IMBOI":
					return CSVEx.CSVFieldIDs.OrderTypes.IMBOI;
				case "LIMIT":
				default:
					return CSVEx.CSVFieldIDs.OrderTypes.Limit;
			}
		}

		public long GetDurationCode(string tif)
		{
			switch (tif)
			{
				case "GTC":
					return CSVEx.CSVFieldIDs.TIFCodes.GTC;
				case "IOC":
					return CSVEx.CSVFieldIDs.TIFCodes.IOC;
				case "FOK":
					return CSVEx.CSVFieldIDs.TIFCodes.FOK;
				case "OPG":
					return CSVEx.CSVFieldIDs.TIFCodes.OPG;
				case "DAY":
				default:
					return CSVEx.CSVFieldIDs.TIFCodes.Day;
			}
		}

		public long GetAlgoCode(string algo)
		{
			switch (algo)
			{
				case "TWAP":
					return CSVEx.CSVFieldIDs.AlgoTypes.TWAP;
				case "VWAP":
					return CSVEx.CSVFieldIDs.AlgoTypes.VWAP;
				default:
					return 0;
			}
		}

		#endregion

		#region - Order Object -

		private RomBasicOrder _basicOrderMessage;
		public RomBasicOrder BasicOrderMessage
		{
			get
			{
				return _basicOrderMessage;
			}
			set
			{
				_basicOrderMessage = value;
			}
		}

		#endregion

		#region - Tool Tip -

		private ToolTip _userTip;
		public ToolTip UserTip
		{
			get
			{
				if (_userTip == null)
				{
					_userTip = new ToolTip();
				}
				return _userTip;
			}
			set
			{
				_userTip = value;
			}
		}

		private int _userTipDuration = 3000;
		public int UserTipDuration
		{
			get
			{
				return _userTipDuration;
			}
			set
			{
				_userTipDuration = value;
			}
		}

		#endregion

		#region - Ticket State -

		public bool SetState(Label obj, bool err)
		{
			return TicketSupport.SetState(obj, err);
		}
		public bool SetState(NumericUpDown obj, bool err)
		{
			return TicketSupport.SetState(obj, err);
		}
		public bool SetState(ComboBox obj, bool err)
		{
			return TicketSupport.SetState(obj, err);
		}
		public bool SetState(CheckBox obj, bool err)
		{
			return TicketSupport.SetState(obj, err);
		}
		public bool SetState(ToolStripDropDownButton obj, bool err)
		{
			return TicketSupport.SetState(obj, err);
		}

		#endregion

		#region - Ticket Event -

		protected override void OnActivated(EventArgs e)
		{
			SetActivePanelColor();
			base.OnActivated(e);
		}

		protected override void OnDeactivate(EventArgs e)
		{
			SetDeactivePanelColor();
			base.OnDeactivate(e);
		}

		private void SetActivePanelColor()
		{
			foreach (Control ctl in _controls.Values)
			{
				ctl.BackColor = Color.LightSteelBlue;
			}

			foreach (PanelEx.PanelManager ctl in _panelManagers.Values)
			{
				foreach (PanelEx.ManagedPanel mpanel in ctl.ManagedPanels)
				{
					mpanel.BackColor = Color.LightSteelBlue;
				}
			}
		}

		private void SetDeactivePanelColor()
		{
			foreach (Control ctl in _controls.Values)
			{
				ctl.BackColor = Color.LightGray;
			}

			foreach (PanelEx.PanelManager ctl in _panelManagers.Values)
			{
				foreach (PanelEx.ManagedPanel mpanel in ctl.ManagedPanels)
				{
					mpanel.BackColor = Color.LightGray;
				}
			}
		}

		#endregion
	}
}