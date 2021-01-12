using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

using Common;
using RDSEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using LabelEx;
using ROMEx;
using CSVEx;
using MarketData;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	internal partial class frmQuickTicket : QuickSupport
	{
		#region - Ticket Properties -

		internal double TicketOpacity {
			get {
				return Opacity;
			}
			set {
				if (Opacity != value) {
					Opacity = value;
				}
			}
		}

		private Color _ticketBackColor = Color.LightSteelBlue;
		internal Color TicketBackColor {
			get {
				return _ticketBackColor;
			}
			set {
				if (_ticketBackColor != value) {
					_ticketBackColor = value;
					SetTicketBackColor();
				}
			}
		}
		private void SetTicketBackColor()
		{
			panelOrderInfoUpper.BackColor = _ticketBackColor;
			panelOrderInfoMsg.BackColor = _ticketBackColor;
			panelCommands.BackColor = _ticketBackColor;
			managedPanelBook.BackColor = _ticketBackColor;
		}

		internal bool ShowBidPrice {
			get {
				return lblBidPrice.Visible;
			}
			set {
				lblBidPrice.Visible = value;
				dspBidPrice.Visible = value;
			}
		}

		internal bool ShowBidSize {
			get {
				return lblBidSize.Visible;
			}
			set {
				lblBidSize.Visible = value;
				dspBidSize.Visible = value;
			}
		}

		internal bool ShowAskPrice {
			get {
				return lblAskPrice.Visible;
			}
			set {
				lblAskPrice.Visible = value;
				dspAskPrice.Visible = value;
			}
		}

		internal bool ShowAskSize {
			get {
				return lblAskSize.Visible;
			}
			set {
				lblAskSize.Visible = value;
				dspAskSize.Visible = value;
			}
		}

		internal bool ShowNetChange {
			get {
				return lblNetChange.Visible;
			}
			set {
				lblNetChange.Visible = value;
				dspNetChange.Visible = value;
			}
		}

		internal bool ShowPctChange {
			get {
				return lblPctChange.Visible;
			}
			set {
				lblPctChange.Visible = value;
				dspPctChange.Visible = value;
			}
		}

		internal bool ShowTotalVolume {
			get {
				return lblVolume.Visible;
			}
			set {
				lblVolume.Visible = value;
				dspVolume.Visible = value;
			}
		}

		internal bool ShowTradedVolume {
			get {
				return lblTradeVolume.Visible;
			}
			set {
				lblTradeVolume.Visible = value;
				dspTradeVolume.Visible = value;
			}
		}

		internal bool ShowLowPrice {
			get {
				return lblLowPrice.Visible;
			}
			set {
				lblLowPrice.Visible = value;
				dspLowPrice.Visible = value;
			}
		}

		internal bool ShowHighPrice {
			get {
				return lblHighPrice.Visible;
			}
			set {
				lblHighPrice.Visible = value;
				dspHighPrice.Visible = value;
			}
		}

		internal bool ShowPrevClosePrice {
			get {
				return lblClosePrice.Visible;
			}
			set {
				lblClosePrice.Visible = value;
				dspClosePrice.Visible = value;
			}
		}

		internal bool ShowTicker {
			get {
				return panelTicker.Visible;
			}
			set {
				panelTicker.Visible = value;
			}
		}

		internal bool ShowQty {
			get {
				return panelShares.Visible;
			}
			set {
				panelShares.Visible = value;
			}
		}

		internal bool ShowOrder {
			get {
				return panelOrderTypes.Visible;
			}
			set {
				panelOrderTypes.Visible = value;
			}
		}

		internal bool ShowLimitPrice {
			get {
				return panelLimitPrice.Visible;
			}
			set {
				panelLimitPrice.Visible = value;
			}
		}

		internal bool ShowStopPrice {
			get {
				return panelStopPrice.Visible;
			}
			set {
				panelStopPrice.Visible = value;
			}
		}

		internal bool ShowDuration {
			get {
				return panelDuration.Visible;
			}
			set {
				panelDuration.Visible = value;
			}
		}

		internal bool ShowExchange {
			get {
				return panelExchange.Visible;
			}
			set {
				panelExchange.Visible = value;
			}
		}

		internal bool ShowOrderInfo {
			get {
				return panelOrderInfoMsg.Visible;
			}
			set {
				panelOrderInfoMsg.Visible = value;
			}
		}

		internal bool ShowCommand {
			get {
				return panelCommands.Visible;
			}
			set {
				panelCommands.Visible = value;
			}
		}

		internal bool ShowIncrement {
			get {
				return dspQuantityIncrement.Visible;
			}
			set {
				dspQuantityIncrement.Visible = value;

				dspLimitPriceIncrement.Visible = value;
				dspLimitPriceTickSize.Visible = value;
				lblLimitPriceX.Visible = value;

				dspStopPriceIncrement.Visible = value;
				dspStopPriceTickSize.Visible = value;
				lblStopPriceX.Visible = value;
			}
		}

		#endregion

		#region - Local Variable -

		internal Price? HighPrice => getPrice(dspHighPrice);
		internal Price? LowPrice => getPrice(dspLowPrice);
		internal Price? AskPrice => getPrice(dspAskPrice);
		internal Price? BidPrice => getPrice(dspBidPrice);
		internal Price? TradedPrice => getPrice(dspTradedPrice);
		internal Price CenterPrice => firstOf(TradedPrice, AskPrice, BidPrice);

		private Price? getPrice(LabelEx.LabelBase field)
		{
			string text = field.Value as string;
			if (!string.IsNullOrEmpty(text) && double.TryParse(text, out double value)) {
				return value;
			} else {
				GLOBAL.HROC.AddToException($"Bad High Price {text}.");
				return null;
			}
		}

		private Price firstOf(params Price?[] values)
		{
			foreach (Price? value in values) {
				if (value.HasValue && !value.Value.IsZero)
					return value.Value;
			}
			return Price.UnsetPrice;
		}

		private class FormFlags
		{
			[System.Flags]
			internal enum ID : ushort { 
				None = 0,
				// changed flags
				Width = 0x0001, Height = 0x0002, Grid = 0x0004, BuyAgg = 0x0008, SellAgg = 0x0010, UpdateIM = 0x0020,
				// state flags
				LastIsBook = 0x0040, HasError = 0x0080, OrderTypesView = 0x0100, DurationView = 0x0200, Centered = 0x0400, OrderLoaded = 0x0800,
				// lock flags
				UpdatingUI = 0x1000, LoadingValue = 0x200
			};

			private volatile ID _flags;

			internal FormFlags(ID initial)
			{
				_flags = initial;
			}

			// You can use joined flags for 'get' and 'set'.
			// Eg. get: 'State[ID.Width | ID.Height | ID.Grid]' will return true if any of those flags are set.
			// Eg. set: 'State[ID.Width | ID.Height] = true' will set both the Width and Height flags ('false' will clear both).
			internal bool this[ID which] { 
				get => (_flags & which) != 0;
				set {
					if (value)
						_flags |= which; // set flag
					else
						_flags &= ~which; // clear flag
				}
			}
		}

		private FormFlags _flags = new FormFlags(FormFlags.ID.LastIsBook | FormFlags.ID.LoadingValue);
		private string _lastSymbolDetail = "";
		private Size _lastSize = new Size();
		private DataView _orderTypesView;
		private DataView _durationsView;
		private NumericUpDown _crrentPriceObj;
		private long _openBuyQty = 0;
		private long _openSellQty = 0;
		private long _netFilledQty = 0;
		private LockList<ROCOrder> _rocOrders = new LockList<ROCOrder>();
		private Market _deltas = new Market();

		// Double Mouse Click Time
		private int _doubleMouseClickInterval = 250;
		private DateTime _lastCenterOnCenterTime = DateTime.Now;

		#endregion

		#region - Constructor -

		internal frmQuickTicket()
		{
			InitializeComponent();

			Initialize();
		}

		internal frmQuickTicket(string symbolDetail)
		{
			InitializeComponent();

			Initialize();

			CurrentSymbolDetail = symbolDetail;
		}

		internal void Initialize()
		{
			DDSymbolDetails = new DropDownSymbolDetails(this);
			DDAccountDropDown = new DropDownTraderAccount(this);
			DDAccountDropDown.PropertyChanged += new PropertyChangedEventHandler(DDAccountDropDown_PropertyChanged);

			QuickButtonSupprot = new QuickButtonSupport(this);
			QuickButtonSupprot.QuickButtonClicked += new QuickButtonClickedEventHandler(QuickButtonSupport_QuickButtonClicked);

			CurrentSecInfo = new BaseSecurityInfo();

			BookDepthLimit = Configuration.ROC.Default.BookDepthLimit;

			rocQuickListBook.ReadyForTickTable = false;
			rocQuickListBook.TickSize = 0.01;

			_flags[FormFlags.ID.LoadingValue] = true;

			_crrentPriceObj = numLimitPrice;

			cboSymbolDetails.DataSource = CurrentSymbolDetails;
			cboSymbolDetails.DisplayMember = "SymbolDetail";

			cboShortLenders.DataSource = ShortLenders;
			cboShortLenders.DisplayMember = "ShortLender";

			_orderTypesView = new DataView(GLOBAL.HOrders.FutureOrderTypes);
			cboOrder.DataSource = _orderTypesView;
			cboOrder.DisplayMember = "Type";
			cboOrder.Text = "LIMIT";

			_durationsView = new DataView(GLOBAL.HOrders.Durations);
			cboDuration.DataSource = _durationsView;
			cboDuration.DisplayMember = "Duration";
			cboDuration.Text = "DAY";

			_lastSymbolDetail = "";
			CurrentSymbolDetail = "";

			Height = MinimumSize.Height;

			if (Configuration.User.Default.AutoFocusOnControls)
			{
				Setup_MouseEnterEvent();

				dspLimitPrice.MouseEnter += new EventHandler(dspLimitPrice_MouseEnter);
				dspLimitPrice.MouseWheel += new MouseEventHandler(dspLimitPrice_MouseEnter);

				dspStopPrice.MouseEnter += new EventHandler(dspStopPrice_MouseEnter);
				dspStopPrice.MouseWheel += new MouseEventHandler(dspStopPrice_MouseEnter);
			}

			SetPriceDisplay(PriceType.Limit, true);
			dspLimitPrice.LostFocus += new EventHandler(dspLimitPrice_Leave);

			SetPriceDisplay(PriceType.Stop, true);
			dspStopPrice.LostFocus += new EventHandler(dspStopPrice_Leave);

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			IconClicked += new EventHandler(Form_IconClicked);
			DisplayClicked += new EventHandler(Form_DisplayClicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			SizeChanged += new EventHandler(Form_SizeChanged);
			Activated += new EventHandler(Form_Activated);

			panelManagerQuick.SelectedPanel = managedPanelBook;

			rocQuickListBook.Initialize();
			rocQuickListBook.PropertyChanged += new PropertyChangedEventHandler(rocQuickListBook_PropertyChanged);
			rocQuickListBook.MouseEnter += new EventHandler(rocQuickListBook_MouseEnter);
			rocQuickListBook.AutoCenterOnTraded = Configuration.ROC.Default.EnableQuickTicketAutoCenter;
			rocQuickListBook.AutoCenterRange = Configuration.ROC.Default.QuickTicketAutoCenterRange;

			_flags[FormFlags.ID.LoadingValue] = false;

			cboSymbolDetails.LostFocus += new EventHandler(cboSymbolDetails_LostFocus);
			cboSymbolDetails.SelectedValueChanged += new EventHandler(cboSymbolDetails_LostFocus);

			StartTimer();
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				UpdateTicketByProcessByTimer();
			}

			if (cboSymbolDetails.BackColor != panelTicker.BackColor)
			{
				cboSymbolDetails.BackColor = panelTicker.BackColor;
			}

			//if (panelCommands.Enabled == _isInError)
			//{
			//    panelCommands.Enabled = !_isInError;
			//}

			panelCommands.Enabled = (CurrentSymbolDetail != "");
			panelCommandsEx.Enabled = (CurrentSymbolDetail != "");

			if (_lastSymbolDetail != CurrentSymbolDetail)
			{
				_lastSymbolDetail = "";
				ResetTicketValues();

				if (DDSymbolDetails.NewSymbolDetail)
				{
					_lastSymbolDetail = CurrentSymbolDetail;

					SubscribeToSymbol(_lastSymbolDetail);
					DDSymbolDetails.NewSymbolDetail = false;
				}
			}
			else
			{
				if (DDSymbolDetails.NewSymbolDetail)
				{
					DDSymbolDetails.NewSymbolDetail = false;
				}
			}

			bool reloadUserAccounts = false;
			if (_flags[FormFlags.ID.LastIsBook] && IsStock)
			{
				_flags[FormFlags.ID.LastIsBook] = false;
				reloadUserAccounts = true;
			}
			else if (!_flags[FormFlags.ID.LastIsBook] && IsFuture)
			{
				_flags[FormFlags.ID.LastIsBook] = true;
				reloadUserAccounts = true;
			}

			if (reloadUserAccounts)
			{
				if (_lastSymbolDetail == "")
				{
					//DDAccountDropDown.SelectedExchange = "";
					//DDAccountDropDown.SelectedLocalAccountAcrn = "";
					//DDAccountDropDown.SelectedTraderFor = "";
					//DDAccountDropDown.LoadUserAccounts();

					UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Future, CSVEx.CSVFieldIDs.SecurityTypes.Future));
				}
				else
				{
					if (CurrentSecInfo != null)
					{
						UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
					}
					else
					{
						UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Future, CSVEx.CSVFieldIDs.SecurityTypes.Future));
					}
				}
			}

			rocQuickListBook.PadByLimit();

			dspBuyOpenQty.Value = _openBuyQty;
			dspSellOpenQty.Value = _openSellQty;
			dspNetOpen.Value = _netFilledQty;

			CheckMarketData();

			CheckAutoCenter();
		}

		private void CheckAutoCenter()
		{
			if (rocQuickListBook.AutoCenterOnTraded)
			{
				CenterOnCenter();
			}
		}

		private void CheckMarketData()
		{
			if (Configuration.User.Default.UseMarketData && !GLOBAL.MDSsConnected)
			{
				lblMarketDataError.Visible = true;
				lblMarketDataError.Blink = true;
			}
			else
			{
				lblMarketDataError.Visible = false;
				lblMarketDataError.Blink = false;
			}
		}

		private void Form_IconClicked(object sender, EventArgs e)
		{
			try
			{
				Clone();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_FitClicked(object sender, EventArgs e)
		{
			try
			{
				_lastSize = Size;
				FitWidth(true);
				FitHeight();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

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

		#region - Display -

		private frmQuickTicketDisplay _quickTicketDisplay;
		private frmQuickTicketDisplay quickTicketDisplay
		{
			get
			{
				if (_quickTicketDisplay == null)
				{
					_quickTicketDisplay = new frmQuickTicketDisplay(new ROCQuickListProfile(rocQuickListBook), new ROCTicketProfile(this));
					_quickTicketDisplay.PropertyChanged += new PropertyChangedEventHandler(quickTicketDisplay_PropertyChanged);
					_quickTicketDisplay.Disposed += new EventHandler(quickTicketDisplay_Disposed);
				}
				return _quickTicketDisplay;
			}
			set
			{
				_quickTicketDisplay = value;
			}
		}
		private void Form_DisplayClicked(object sender, EventArgs e)
		{
			GLOBAL.MainForm.TopMost = false;
			bool orgTopMost = TopMost;
			TopMost = false;

			quickTicketDisplay.ShowDialog();
			
			TopMost = orgTopMost;
			GLOBAL.MainForm.TopMost = true;
		}

		private void quickTicketDisplay_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Apply":
					SetProfile(quickTicketDisplay.GetProfile(new ROCQuickListProfile(rocQuickListBook)));
					SetProfile(quickTicketDisplay.GetProfile(new ROCTicketProfile(this)));
					break;
				case "ApplyToAll":
					ROCQuickListProfile gridProf = quickTicketDisplay.GetProfile(new ROCQuickListProfile(rocQuickListBook));
					ROCTicketProfile ticketProf = quickTicketDisplay.GetProfile(new ROCTicketProfile(this));
					foreach (frmQuickTicket w in GLOBAL.HWindows.QuickTicketWindows.Values)
					{
						w.SetProfile(quickTicketDisplay.GetProfile(gridProf));
						w.SetProfile(quickTicketDisplay.GetProfile(ticketProf), false);
					}
					// Set Global
					Configuration.ROC.Default.EnableQuickTicketAutoCenter = gridProf.GridAutoCenterOnTraded;
					Configuration.ROC.Default.Save();
					Configuration.ROC.Default.QuickTicketAutoCenterRange = gridProf.GridAutoCenterRange;
					Configuration.ROC.Default.Save();
					break;
			}
		}

		private void quickTicketDisplay_Disposed(object sender, EventArgs e)
		{
			if (_quickTicketDisplay != null)
			{
				_quickTicketDisplay.PropertyChanged -= new PropertyChangedEventHandler(quickTicketDisplay_PropertyChanged);
				_quickTicketDisplay.Disposed -= new EventHandler(quickTicketDisplay_Disposed);
			}

			_quickTicketDisplay = null;
		}

		internal void SetProfile(ROCQuickListProfile prof)
		{
			new ROCQuickListProfile(rocQuickListBook, prof);
		}

		internal void SetProfile(ROCTicketProfile prof)
		{
			new ROCTicketProfile(this, prof);
			UpdateOrders();
		}

		internal void SetProfile(ROCTicketProfile prof, bool clone)
		{
			new ROCTicketProfile(this, prof, clone);
			UpdateOrders();
		}

		#endregion

		private void Form_SizeChanged(object sender, EventArgs e)
		{
			if (!_flags[FormFlags.ID.Grid])
			{
				_flags[FormFlags.ID.Width] = _lastSize.Width != Width;
				_flags[FormFlags.ID.Height] = _lastSize.Height != Height;

				if (_flags[FormFlags.ID.Width | FormFlags.ID.Height])
				{
					_lastSize = Size;
					if (_flags[FormFlags.ID.Width])
						FitWidth(false);

					if (_flags[FormFlags.ID.Height])
						FitHeight();
				}
			}
		}

		private void FitWidth(bool manual)
		{
			_flags[FormFlags.ID.Width] = false;
			_flags[FormFlags.ID.Grid] = manual;

			lock (rocQuickListBook)
			{
				if (manual)
				{
					SetColumnMode(rocQuickListBook, DataGridViewAutoSizeColumnMode.DisplayedCells);
					ResizeToFit(rocQuickListBook);
					SetColumnMode(rocQuickListBook, DataGridViewAutoSizeColumnMode.None);
				}
				else
				{
					SetColumnMode(rocQuickListBook, DataGridViewAutoSizeColumnMode.Fill);
					SetColumnMode(rocQuickListBook, DataGridViewAutoSizeColumnMode.None);
				}
			}

			int diff = MinimumSize.Width - Width;

			if (diff > 0)
			{
				//int deltaUpper = diff / 6;

				//panelShares.Width = panelShares.MinimumSize.Width + deltaUpper;
				//panelOrderTypes.Width = panelOrderTypes.MinimumSize.Width + deltaUpper;
				//panelLimitPrice.Width = panelLimitPrice.MinimumSize.Width + deltaUpper;
				//panelStopPrice.Width = panelStopPrice.MinimumSize.Width + deltaUpper;
				//panelDuration.Width = panelDuration.MinimumSize.Width + deltaUpper;
				//panelExchange.Width = panelExchange.MinimumSize.Width + deltaUpper;

				int deltaCommands = diff / 3;

				cmdBuy.Width = cmdBuy.MinimumSize.Width + deltaCommands;
				cmdSell.Width = cmdSell.MinimumSize.Width + deltaCommands;
				cmdShort.Width = cmdShort.MinimumSize.Width + deltaCommands;

				int deltaQuickCommands = diff / 9;

				dspBuyOpenQty.Width = dspBuyOpenQty.MinimumSize.Width + deltaQuickCommands;
				dspSellOpenQty.Width = dspSellOpenQty.MinimumSize.Width + deltaQuickCommands;
			}
			else
			{
				//panelShares.Width = panelShares.MinimumSize.Width;
				//panelOrderTypes.Width = panelOrderTypes.MinimumSize.Width;
				//panelLimitPrice.Width = panelLimitPrice.MinimumSize.Width;
				//panelStopPrice.Width = panelStopPrice.MinimumSize.Width;
				//panelDuration.Width = panelDuration.MinimumSize.Width;
				//panelExchange.Width = panelExchange.MinimumSize.Width;

				cmdBuy.Width = cmdBuy.MinimumSize.Width;
				cmdSell.Width = cmdSell.MinimumSize.Width;

				dspBuyOpenQty.Width = dspBuyOpenQty.MinimumSize.Width;
				dspSellOpenQty.Width = dspSellOpenQty.MinimumSize.Width;
			}

			_flags[FormFlags.ID.Grid] = false;
		}

		private void FitHeight()
		{
			_flags[FormFlags.ID.Height] = false;
		}

		private void Form_Load(object sender, EventArgs e)
		{
			try
			{
				DDAccountDropDown.LoadUserAccounts();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			StopTimer();

			_orderTypesView = null;
			_durationsView = null;

			FitClicked -= new EventHandler(Form_FitClicked);

			Load -= new EventHandler(Form_Load);
			FormClosing -= new FormClosingEventHandler(Form_FormClosing);
			SizeChanged -= new EventHandler(Form_SizeChanged);

			rocQuickListBook.Dispose();
			rocQuickListBook.PropertyChanged -= new PropertyChangedEventHandler(rocQuickListBook_PropertyChanged);
			rocQuickListBook.MouseEnter -= new EventHandler(rocQuickListBook_MouseEnter);

			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void Form_Activated(object sender, EventArgs e)
		{
			ActiveControl = cboSymbolDetails;
			cboSymbolDetails.Select();
		}

		#endregion

		#region - Grid Event -

		private void rocQuickListBook_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "PriceSelected":
					if (panelLimitPrice.Visible && panelStopPrice.Visible)
					{
						_crrentPriceObj.Value = Convert.ToDecimal(((ROCQuickList)sender).PriceBook);

						TogglePriceSelection();
					}
					else if (panelStopPrice.Visible)
					{
						SetStopPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
					}
					else if (panelLimitPrice.Visible)
					{
						SetLimitPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
					}
					break;
				case "BuySelected":
					if (panelLimitPrice.Visible)
					{
						SetLimitPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString());
					}
					else if (panelStopPrice.Visible)
					{
						SetStopPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString());
					}
					else
					{
						MessageBox.Show("Can not be in Market Order mode.");
					}
					break;
				case "SellSelected":
					if (panelLimitPrice.Visible)
					{
						SetLimitPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString());
					}
					else if (panelStopPrice.Visible)
					{
						SetStopPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString());
					}
					else
					{
						MessageBox.Show("Can not be in Market Order mode.");
					}
					break;
				case "BuyStopSelected":
					SetStopPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
					if (IsFuture && CurrentSecInfo != null && numStopLimitOffset.Value > 0)
					{
						SetLimitPrice(Convert.ToDecimal(numStopPrice.Value + (CurrentSecInfo.TickSizeDec * numStopLimitOffset.Value)));
						BuildOrder(
							CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(),
							CSVEx.CSVFieldIDs.OrderTypes.StopLimit.ToString(),
							CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
							numLimitPrice.Value.ToString(),
							numStopPrice.Value.ToString());
					}
					else
					{
						BuildOrder(
							CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(),
							CSVEx.CSVFieldIDs.OrderTypes.Stop.ToString(),
							CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
							"",
							numStopPrice.Value.ToString());
					}
					break;
				case "SellStopSelected":
					SetStopPrice(Convert.ToDecimal(((ROCQuickList)sender).PriceBook));
					if (IsFuture && CurrentSecInfo != null && numStopLimitOffset.Value > 0)
					{
						SetLimitPrice(Convert.ToDecimal(numStopPrice.Value - (CurrentSecInfo.TickSizeDec * numStopLimitOffset.Value)));
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(),
							CSVEx.CSVFieldIDs.OrderTypes.StopLimit.ToString(),
							CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
							numLimitPrice.Value.ToString(),
							numStopPrice.Value.ToString());
					}
					else
					{
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(),
							CSVEx.CSVFieldIDs.OrderTypes.Stop.ToString(),
							CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
							"",
							numStopPrice.Value.ToString());
					}
					break;
				case "ShowQtyMenu":
					ShowQuantityMenu();
					break;
				case "ShowColumnMenu":
					ShowColumnMenu();
					break;
				case "BuyCancel":
					CancelOrderByPrice("Buy", ((ROCQuickList)sender).PriceBook);
					((ROCQuickList)sender).ClearStopLimit("BuyStopLimitFlag");
					break;
				case "SellCancel":
					CancelOrderByPrice("Sell", ((ROCQuickList)sender).PriceBook);
					((ROCQuickList)sender).ClearStopLimit("SellStopLimitFlag");
					break;
				case "BuyReplace":
					ReplaceOrderByPrice("Buy", ((ROCQuickList)sender).StartReplacePrice, ((ROCQuickList)sender).EndReplacePrice);
					((ROCQuickList)sender).ClearStopLimit("BuyStopLimitFlag");
					break;
				case "SellReplace":
					ReplaceOrderByPrice("Sell", ((ROCQuickList)sender).StartReplacePrice, ((ROCQuickList)sender).EndReplacePrice);
					((ROCQuickList)sender).ClearStopLimit("SellStopLimitFlag");
					break;
				case "ShowBuyStopLimit":
					double buyStopPrice = ((ROCQuickList)sender).StopPriceBook;
					Price? buyStopLimitPrice = GetStopLimitPrice("Buy", buyStopPrice);
					if (buyStopLimitPrice.HasValue)
					{
						((ROCQuickList)sender).FlagStopLimitPrice("BuyStopLimitFlag", buyStopPrice, buyStopLimitPrice.Value);
					}
					break;
				case "ClearBuyStopLimit":
					((ROCQuickList)sender).ClearStopLimit("BuyStopLimitFlag");
					break;
				case "ShowSellStopLimit":
					double sellStopPrice = ((ROCQuickList)sender).StopPriceBook;
					Price? sellStopLimitPrice = GetStopLimitPrice("Sell", sellStopPrice);
					if (sellStopLimitPrice != null)
					{
						((ROCQuickList)sender).FlagStopLimitPrice("SellStopLimitFlag", sellStopLimitPrice.Value, sellStopPrice);
					}
					break;
				case "ClearSellStopLimit":
					((ROCQuickList)sender).ClearStopLimit("SellStopLimitFlag");
					break;
			}
		}

		private void SetStopPrice(decimal price)
		{
			if (price >= numStopPrice.Minimum)
			{
				if (CurrentSecInfo != null)
				{
					numStopPrice.Value = Math.Round(price, CurrentSecInfo.DecimalPlaces);
				}
				else
				{
					numStopPrice.Value = price;
				}
			}
		}

		private void SetLimitPrice(decimal price)
		{
			if (price >= numLimitPrice.Minimum)
			{
				if (CurrentSecInfo != null)
				{
					numLimitPrice.Value = Math.Round(price, CurrentSecInfo.DecimalPlaces);
				}
				else
				{
					numLimitPrice.Value = price;
				}
			}
		}

		private void rocQuickListBook_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusQuickTicket)
			{
				ActivateGrid(rocQuickListBook);
			}
		}

		#endregion

		#region - Price Event -

		private void PriceSelection_Click(object sender, EventArgs e)
		{
			try
			{
				if (((Control)sender).Name.ToUpper().Contains("LIMITPRICE"))
				{
					SelectLimitPrice();
				}
				else if (((Control)sender).Name.ToUpper().Contains("STOPPRICE"))
				{
					SelectStopPrice();
				}
			} 
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Price_MouseDown(object sender, MouseEventArgs e)
		{
			try
			{
				((LabelBase)sender).ForeColor = Color.LimeGreen;
				if (((LabelBase)sender).Value != null && ((LabelBase)sender).Value.ToString() != "")
				{
					_crrentPriceObj.Value = Convert.ToDecimal(((LabelBase)sender).Value);
				}

				TogglePriceSelection();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Price_MouseUp(object sender, MouseEventArgs e)
		{
			try
			{
				((LabelBase)sender).ForeColor = Color.Gainsboro;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void TogglePriceSelection()
		{
			if (panelLimitPrice.Visible && panelStopPrice.Visible)
			{
				if (_crrentPriceObj.Name.ToUpper().Contains("LIMITPRICE"))
				{
					SelectStopPrice();
				}
				else
				{
					SelectLimitPrice();
				}
			}
		}

		private void SelectLimitPrice()
		{
			_crrentPriceObj = numLimitPrice;
			panelLimitPrice.BackColor = Color.CornflowerBlue;
			panelStopPrice.BackColor = Color.Transparent;
		}

		private void SelectStopPrice()
		{
			_crrentPriceObj = numStopPrice;
			panelLimitPrice.BackColor = Color.Transparent;
			panelStopPrice.BackColor = Color.CornflowerBlue;
		}

		#endregion

		#region - Ticket Event -

		// When any of the User selected fields are changed
		private void Ticket_ValueChanged(object sender, EventArgs e)
		{
			try
			{
				ValidateTicketValues();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cmdCancelAllBuy_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			CancelOrderBySide(CSVFieldIDs.SideCodes.Buy);
		}

		private void cmdCancelAll_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			CancelOrderBySide(CSVFieldIDs.SideCodes.Buy);
			CancelOrderBySide(CSVFieldIDs.SideCodes.Sell);
		}

		private void cmdCancelAllSell_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			CancelOrderBySide(CSVFieldIDs.SideCodes.Sell);
		}

		private void cmdCenterLow_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			if (dspLowPrice.Value != null && dspLowPrice.Value.ToString() != "")
			{
				rocQuickListBook.CenterOnPrice(Convert.ToDouble(dspLowPrice.Value));
			}
		}

		private void cmdCenterTrade_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;

			if (((TimeSpan)DateTime.Now.Subtract(_lastCenterOnCenterTime)).TotalMilliseconds > _doubleMouseClickInterval)
			{
				CenterOnCenter();
			}
			else
			{
				CenterOnCenterAll();
			}

			_lastCenterOnCenterTime = DateTime.Now;
		}

		private void cmdCenterHigh_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			if (dspHighPrice.Value != null && dspHighPrice.Value.ToString() != "")
			{
				rocQuickListBook.CenterOnPrice(Convert.ToDouble(dspHighPrice.Value));
			}
		}

		private void dspLimitPrice_KeyDown(object sender, KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyData)
			{
				case Keys.Enter:
				case Keys.Tab:
					SetPriceDisplayFromUserInput(PriceType.Limit);
					break;
			}
		}

		private void dspLimitPrice_Leave(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			SetPriceDisplayFromUserInput(PriceType.Limit);
		}

		//private void dspLimitPrice_Leave(object sender, MouseEventArgs e)
		//{
		//    GLOBAL.Interrupt = true;
		//    SetPriceDisplayFromUserInput(PriceType.Limit);
		//}

		private void dspLimitPrice_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			ActiveControl = numLimitPrice;
		}

		private void dspStopPrice_KeyDown(object sender, KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyData)
			{
				case Keys.Enter:
				case Keys.Tab:
					SetPriceDisplayFromUserInput(PriceType.Stop);
					break;
			}
		}

		private void dspStopPrice_Leave(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			SetPriceDisplayFromUserInput(PriceType.Stop);
		}

		//private void dspStopPrice_Leave(object sender, MouseEventArgs e)
		//{
		//    GLOBAL.Interrupt = true;
		//    SetPriceDisplayFromUserInput(PriceType.Stop);
		//}

		private void dspStopPrice_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			ActiveControl = numStopPrice;
		}

		private void Control_KeyUp(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Tab)
			{
				switch (((Control)sender).Name)
				{
					case "cboSymbolDetails":
						ActiveControl = numQuantity;
						numQuantity.Select(0, numQuantity.Text.Length);
						break;
					case "numQuantity":
					case "cboOrder":
						if (panelLimitPrice.Visible)
						{
							ActiveControl = dspLimitPrice;
							dspLimitPrice.Select();
						}
						else if (panelStopPrice.Visible)
						{
							ActiveControl = dspStopPrice;
							dspStopPrice.Select();
						}
						else
						{
							ActiveControl = cboSymbolDetails;
							cboSymbolDetails.Select();
						}
						break;
					case "numLimitPrice":
					case "dspLimitPrice":
						if (panelStopPrice.Visible)
						{
							ActiveControl = dspStopPrice;
							dspStopPrice.Select();
						}
						else
						{
							ActiveControl = cboSymbolDetails;
							cboSymbolDetails.Select();
						}
						break;
					case "numStopPrice":
					case "dspStopPrice":
					case "cboDuration":
					case "cboExchange":
						ActiveControl = cboSymbolDetails;
						cboSymbolDetails.Select();
						break;
				}
			}
		}

		#endregion

		#region - Binding -

		#region - Refresh & First Load -

		private void SubscribeToSymbol(string symbolDetail)
		{
			try
			{
				_flags[FormFlags.ID.BuyAgg | FormFlags.ID.SellAgg | FormFlags.ID.Centered | FormFlags.ID.OrderLoaded] = false;

				if (IsSpread)
				{
					numLimitPrice.Minimum = -numLimitPrice.Maximum;
					numStopPrice.Minimum = -numStopPrice.Maximum;
				}
				else
				{
					numLimitPrice.Minimum = 0;
					numStopPrice.Minimum = 0;
				}

				UpdateSecurityInfo(symbolDetail);

				Book delta = UpdateMarketData(GLOBAL.HMarketData.Current);
				if (delta != null)
				{
					UpdateMarketDataL1(delta);
					UpdateMarketDataDeltaBook(delta);
				}

				if (!_flags[FormFlags.ID.OrderLoaded] && rocQuickListBook.ReadyForTickTable && rocQuickListBook.Rows.Count > 0)
				{
					InitialOrderLoad(symbolDetail);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void InitialOrderLoad(string symbolDetail)
		{
			_flags[FormFlags.ID.OrderLoaded] = true;

			ROCOrder[] orders = new ROCOrder[GLOBAL.HOrders.RocItems.Count];
			GLOBAL.HOrders.RocItems.Values.CopyTo(orders, 0);
			UpdateOrders(symbolDetail, orders);
		}

		#endregion

		#region - Used By Process Thread -

		private delegate void UpdateTicketByProcessDelegate(bool updateIM, List<ROCOrder> orders, Market deltas);
		internal void UpdateTicketByProcess(bool updateIM, List<ROCOrder> orders, Market deltas)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				try
				{
					_flags[FormFlags.ID.UpdateIM] = updateIM;
					_rocOrders.AddRange(orders);
					_deltas.Merge(deltas);
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
			else
			{
				if (InvokeRequired)
				{
					BeginInvoke(new UpdateTicketByProcessDelegate(UpdateTicketByProcess), new object[] { updateIM, orders, deltas });
					return;
				}
				try
				{
					if (updateIM)
					{
						UpdateSecurityInfo();
					}

					_flags[FormFlags.ID.BuyAgg | FormFlags.ID.SellAgg] = false;

					if (!_flags[FormFlags.ID.OrderLoaded] && rocQuickListBook.ReadyForTickTable && rocQuickListBook.Rows.Count > 0)
					{
						InitialOrderLoad(CurrentSymbolDetail);
					}
					else if (_flags[FormFlags.ID.OrderLoaded])
					{
						if (orders.Count > 0)
						{
							UpdateOrders(CurrentSymbolDetail, orders);
						}
					}

					if (!deltas.Empty)
					{
						try
						{
							//DateTimeHP _dtHP = new DateTimeHP();
							//TimeSpan _pTimeSpan = new TimeSpan();
							//DateTime _pTime = _dtHP.Now;
							Book delta = UpdateMarketData(deltas, AskPrice, BidPrice, HighPrice, LowPrice);
							if (delta != null)
							{
								UpdateMarketDataL1(delta);
								UpdateMarketDataDeltaBook(delta);
							}
							//_pTimeSpan = _dtHP.Now.Subtract(_pTime);
							//GLOBAL.HROC.AddToStatusLogs(string.Format("QuickBookUpdateTime {0,8}ms| 3Leg", _pTimeSpan.TotalMilliseconds));
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}

					UpdateAggregation();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void UpdateTicketByProcessByTimer()
		{
			try
			{
				if (!_flags[FormFlags.ID.UpdatingUI])
				{
					_flags[FormFlags.ID.UpdatingUI] = true;
					if (_flags[FormFlags.ID.UpdateIM])
					{
						_flags[FormFlags.ID.UpdateIM] = false;
						UpdateSecurityInfo();
					}

					_flags[FormFlags.ID.BuyAgg | FormFlags.ID.SellAgg] = false;

					if (!_flags[FormFlags.ID.OrderLoaded] && rocQuickListBook.ReadyForTickTable && rocQuickListBook.Rows.Count > 0)
					{
						InitialOrderLoad(CurrentSymbolDetail);
					}
					else if (_flags[FormFlags.ID.OrderLoaded])
					{
						List<ROCOrder> orders = _rocOrders.TakeAll();
						if (orders.Count > 0)
							UpdateOrders(CurrentSymbolDetail, orders);
					}

					Market deltas = _deltas.Release();
					if (!deltas.Empty)
					{
						try
						{
							//DateTimeHP _dtHP = new DateTimeHP();
							//TimeSpan _pTimeSpan = new TimeSpan();
							//DateTime _pTime = _dtHP.Now;
							Book delta = UpdateMarketData(deltas, AskPrice, BidPrice, HighPrice, LowPrice);
							if (delta != null)
							{
								UpdateMarketDataL1(delta);
								UpdateMarketDataDeltaBook(delta);
							}
							//_pTimeSpan = _dtHP.Now.Subtract(_pTime);
							//GLOBAL.HROC.AddToStatusLogs(string.Format("QuickBookUpdateTime {0,8}ms| 3Leg", _pTimeSpan.TotalMilliseconds));
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
					UpdateAggregation();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
			_flags[FormFlags.ID.UpdatingUI] = false;
		}

		private void UpdateSecurityInfo()
		{
			Dictionary<string, string> lcoImSymbolNeeded = new Dictionary<string, string>();
			if (ImSymbolNeeded.Count > 0)
			{
				lock (ImSymbolNeeded)
				{
					lcoImSymbolNeeded = new Dictionary<string, string>(ImSymbolNeeded);
				}

				List<string> removeList = new List<string>();

				foreach (string symbolDetail in lcoImSymbolNeeded.Keys)
				{
					CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
					if (CurrentSecInfo != null && CurrentSecInfo.SecType != "")
					{
						if (!removeList.Contains(symbolDetail))
						{
							removeList.Add(symbolDetail);
						}

						UpdateIMInfo(symbolDetail, CurrentSecInfo);
						
						UpdateTicket();

						// Replay data from memory
						Book delta = UpdateMarketData(GLOBAL.HMarketData.Current);
						if (delta != null)
						{
							UpdateMarketDataL1(delta);
							UpdateMarketDataDeltaBook(delta);
						}
					}
				}

				// Clean Up
				if (removeList.Count > 0)
				{
					lock (ImSymbolNeeded)
					{
						foreach (string symbolDetail in removeList)
							ImSymbolNeeded.Remove(symbolDetail);
					}
				}
			}
		}

		private void UpdateIMInfo(string symbolDetail, BaseSecurityInfo secInfo)
		{
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Equity:
					HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);

					if (!MDSymbols.Contains(secInfo.MDSymbol))
					{
						MDSymbols.Add(secInfo.MDSymbol);
					}
					break;
				case CSVFieldIDs.SecurityTypes.Future:
					HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
					HelperSubscriber.SubscribeBook(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);

					if (!MDSymbols.Contains(secInfo.MDSymbol))
					{
						MDSymbols.Add(secInfo.MDSymbol);
					}
					break;
				default:
					DDSymbolDetails.DeleteSymbolDetail(CurrentSymbolDetail);
					break;
			}
		}

		private void UpdateOrders()
		{
			if (DDAccountDropDown.CurrentAcctountInfo != null)
			{
				UpdateOrders(true, true, true);
				UpdateAggregation();
			}
		}
		private void UpdateOrders(string symbolDetail, IEnumerable<ROCOrder> orders)
		{
			CheckOrders(symbolDetail, orders, out bool hasOrder, out bool hasBuy, out bool hasSell);
			UpdateOrders(hasOrder, hasBuy, hasSell);
		}
		private void UpdateOrders(bool hasOrder, bool hasBuy, bool hasSell)
		{
			if (hasOrder)
			{
				if (hasBuy)
				{
					UpdateOpenOrders("Buy", GetOpenPriceQty("Buy"));
					_flags[FormFlags.ID.BuyAgg] = true;

				}

				if (hasSell)
				{
					UpdateOpenOrders("Sell", GetOpenPriceQty("Sell"));
					_flags[FormFlags.ID.SellAgg] = true;
				}
			}
		}

		private void CheckOrders(string symbolDetail, IEnumerable<ROCOrder> orders, out bool hasOrder, out bool hasBuy, out bool hasSell)
		{
			hasOrder = false;
			hasBuy = false;
			hasSell = false;

			// Find changes for this ticket
			foreach (ROCOrder order in orders)
			{
				if (order.SymbolDetail == symbolDetail)
				{
					hasOrder = true;
					switch (order.Side)
					{
						case CSVEx.CSVFieldIDs.SideCodes.Buy:
							hasBuy = true;
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Sell:
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							hasSell = true;
							break;
						default:
							break;
					}
				}

				if (hasOrder && hasBuy && hasSell)
				{
					// Refresh All, Don't loop anymore.
					break;
				}
			}
		}

		internal void CenterOnCenter()
		{
			if (dspTradedPrice.Value != null && dspTradedPrice.Value.ToString() != "")
			{
				rocQuickListBook.CenterOnPrice(CenterPrice, true);
			}
		}

		private void CenterOnCenterAll()
		{
			IntPtr[] keys = new IntPtr[0];
			lock (GLOBAL.HWindows.QuickTicketWindows)
			{
				keys = new IntPtr[GLOBAL.HWindows.QuickTicketWindows.Count];
				GLOBAL.HWindows.QuickTicketWindows.Keys.CopyTo(keys, 0);
			}

			foreach (IntPtr key in keys)
			{
				if (GLOBAL.HWindows.QuickTicketWindows.TryGetValue(key, out frmQuickTicket form))
				{
					try
					{
						form.CenterOnCenter();
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}
			}
		}

		#endregion

		// Update the ticket with all current IM info
		private void UpdateSecurityInfo(string symbolDetail)
		{
			if (!DoNotSend(symbolDetail))
			{
				UpdateQuickTicketWithSecurityInfo(symbolDetail);
				GLOBAL.HRDS.GetSecurityByTicker(symbolDetail);
			}
			else
			{
				lblDoNotTranslate.Dock = DockStyle.Fill;
				lblDoNotTranslate.Visible = true;
				lblDoNotTranslate.Blink = true;
			}
		}

		// Update with Security Info On Play back & onLoad
		private void UpdateQuickTicketWithSecurityInfo(string symbolDetail)
		{
			CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

			lock (ImSymbolNeeded)
			{
				if (!ImSymbolNeeded.ContainsKey(symbolDetail))
				{
					// Should have only the current symbol in the list;
					ImSymbolNeeded.Clear();
					ImSymbolNeeded.Add(symbolDetail, CurrentSecInfo.MDSymbol);
				}
				else
				{
					ImSymbolNeeded[symbolDetail] = CurrentSecInfo.MDSymbol;
				}
			}
		}

		private void UpdateMarketDataL1(Book delta)
		{
			double price;
			long size;

			lock (panelTicker)
			{
				//SRLabsPriceCheck(delta);

				panelTicker.SuspendLayout();

				rocQuickListBook.HasFlagChange = false;

				#region - Traded Price & Volume  And (Builds the Ladder) -

				if (delta.TryGetField(Book.FieldEnum.TradePrice, out price))
				{
					SetupTickTable(rocQuickListBook, price, Convert.ToInt64(numQuantity.Value));
				}

				UpdateTradedPrice(rocQuickListBook, delta);

				#endregion

				#region - L1 Data Update -


				// Update the Prices after the trade and ladder is created
				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price)) {
					if (!Price.Same(rocQuickListBook.BidPrice, price))
					{
						dspBidPrice.Value = price;
						rocQuickListBook.FlagBidPrice(rocQuickListBook.BidPrice, price);
						rocQuickListBook.HasFlagChange = true;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.BidSize, out size)) {
					dspBidSize.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
					if (rocQuickListBook.AskPrice != price)
					{
						dspAskPrice.Value = price;
						rocQuickListBook.FlagAskPrice(rocQuickListBook.AskPrice, price);
						rocQuickListBook.HasFlagChange = true;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
					dspAskSize.Value = size;
				}

				double change;

				if (delta.TryGetField(Book.FieldEnum.NetChange, out change)) {
					dspNetChange.Value = change;
					dspTradedPrice.ForeColor = dspNetChange.ForeColor;
				}

				if (delta.TryGetField(Book.FieldEnum.PctChange, out change)) {
					dspPctChange.Value = change;
				}

				if (delta.TryGetField(Book.FieldEnum.HighPrice, out price)) {
					dspHighPrice.Value = price;

					rocQuickListBook.Pad(price, true);
					rocQuickListBook.FlagHighPrice(rocQuickListBook.HighPrice, price);
					rocQuickListBook.HasFlagChange = true;
				}

				if (delta.TryGetField(Book.FieldEnum.LowPrice, out price)) {
					dspLowPrice.Value = price;

					rocQuickListBook.Pad(price, true);
					rocQuickListBook.FlagLowPrice(rocQuickListBook.LowPrice, price);
					rocQuickListBook.HasFlagChange = true;
				}

				if (delta.TryGetField(Book.FieldEnum.ClosePrice, out price) && !Price.IsZeroValue(price)) {
					dspClosePrice.Value = price;
				} else if (CurrentSecInfo.SecType == "F") {
					if (delta.TryGetField(Book.FieldEnum.SettlePrice, out price) && !Price.IsZeroValue(price)) {
						dspClosePrice.Value = price;
					} else if (delta.TryGetField(Book.FieldEnum.PrevClosePrice, out price) && !Price.IsZeroValue(price)) {
						dspClosePrice.Value = price;
					}
				} else {
					if (delta.TryGetField(Book.FieldEnum.AdjPrevClosePrice, out price) && !Price.IsZeroValue(price)) {
						dspClosePrice.Value = price;
					} else if (delta.TryGetField(Book.FieldEnum.PrevClosePrice, out price) && !Price.IsZeroValue(price)) {
						dspClosePrice.Value = price;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size)) {
					dspVolume.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out string status)) {
					switch (status.ToLower()) {
						case "none":
							break;
						case "normal":
							if (lblSecurityStatus.Visible) {
								lblSecurityStatus.Visible = false;
								lblSecurityStatus.Blink = false;
								lblSecurityStatus.Text = "STA";
							}
							break;
						default:
							lblSecurityStatus.Text = status;
							if (!lblSecurityStatus.Visible) {
								lblSecurityStatus.Visible = true;
								lblSecurityStatus.Blink = true;
							}
							break;
					}
				}

				#endregion

				#region - Cumulation Traded Volume -

				// Update Traded Volumes
				switch (CurrentSecInfo.SecType) {
					case CSVFieldIDs.SecurityTypes.Future:
						foreach (Quote quote in delta.TradedVolumes) {
							rocQuickListBook.UpdateTotalTradedVolumeByPrice(quote.QuotePrice.Value, quote.Size);
						}
						break;
					case CSVFieldIDs.SecurityTypes.Equity:
						foreach (Quote quote in delta.TradedVolumes) {
							rocQuickListBook.UpdateTotalTradedVolumeByPrice(Math.Round(quote.QuotePrice.Value, 2), quote.Size);
						}
						break;
				}

				#endregion

				if (rocQuickListBook.HasFlagChange)
				{
					rocQuickListBook.Invalidate();
				}

				panelTicker.ResumeLayout();
			}
		}

		private void SetupTickTable(ROCQuickList grid, double tradedPrice, long qtyBuySell)
		{
			try
			{
				if (grid.ReadyForTickTable && grid.RocGridTable.Rows.Count == 0)
				{
					lock (grid.RocGridTable)
					{
						grid.Qty = qtyBuySell;
						if (IsStock)
						{
							grid.SetupTickTable(tradedPrice, 1);
						}
						else
						{
							grid.SetupTickTable(tradedPrice, BookDepthLimit);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void UpdateTradedPrice(ROCQuickList grid, Book delta)
		{
			// Default to Trade Up
			if (delta.TryGetField(Book.FieldEnum.TradeTick, out string tradeTick) && !string.IsNullOrEmpty(tradeTick)) {
				switch (tradeTick)
				{
					case "+":
					case "+0":
						picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Up];
						grid.TradeTick = "+";
						break;
					case "-":
					case "-0":
						picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Down];
						grid.TradeTick = "-";
						break;
					default:
						break;
				}
			}
			else
			{
				if (grid.TradeTick == "")
				{
					picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Up];
					grid.TradeTick = "+";
				}
			}

			if (delta.TryGetField(Book.FieldEnum.TradeVolume, out long size)) {
				dspTradeVolume.Value = size;
			}


			if (delta.TryGetField(Book.FieldEnum.TradePrice, out double price)) {
				dspTradedPrice.Value = price;
				if (!HasFirstUpdate)
				{
					HasFirstUpdate = true;
					numLimitPrice.Value = Math.Round(Convert.ToDecimal(dspTradedPrice.Value), CurrentSecInfo.DecimalPlaces);
				}

				grid.FlagTradePrice(grid.TradePrice, price, grid.TradeTick);
				if (dspTradeVolume.Value != null && dspTradeVolume.Value.ToString() != "")
				{
					switch (CurrentSecInfo.SecType)
					{
						case CSVFieldIDs.SecurityTypes.Future:
							grid.UpdateTotalTradedVolumeByPrice(grid.TradePrice, Convert.ToInt64(dspTradeVolume.Value), true);
							break;
						case CSVFieldIDs.SecurityTypes.Equity:
							grid.UpdateTotalTradedVolumeByPrice(Math.Round(grid.TradePrice, 2), Convert.ToInt64(dspTradeVolume.Value), true);
							break;
					}
				}

				if (!_flags[FormFlags.ID.Centered] && grid.RocGridTable.Rows.Count > 0)
				{
					FirstTimeCenter();
				}
			}
		}

		private void FirstTimeCenter()
		{
			_flags[FormFlags.ID.Centered] = true;

			// First Center after a symbol load
			rocQuickListBook.CenterOnPrice(CenterPrice);
		}

		private void UpdateMarketDataDeltaBook(Book delta)
		{
			lock (rocQuickListBook.RocGridTable)
			{
				if (rocQuickListBook.RocGridTable.Rows.Count > 0)
				{
					if (!delta.Bids.Empty)
					{
						rocQuickListBook.UpdateBook(delta.Bids, "Bid");
					}

					if (!delta.Asks.Empty)
					{
						rocQuickListBook.UpdateBook(delta.Asks, "Ask");
					}
				}
			}
		}

		// Check to See What Price/Qty needs update
		private void UpdateOpenOrders(string side, OpenOrderItems priceItems)
		{
			lock (rocQuickListBook.RocGridTable)
			{
				DataRow[] oldRows = rocQuickListBook.RocGridTable.Select(side + "OpenQty > 0");

				double oldPrice;
				foreach (DataRow oldRow in oldRows)
				{
					oldPrice = (double)oldRow["Price"];

					if (!priceItems.OpenOrderPriceQty.TryGetValue(oldPrice, out long quantity))
					{
						// The Qty is no longer there
						oldRow[side + "OpenQty"] = DBNull.Value;
						// Reset Flag Stop Price
						oldRow[side + "StopFlag"] = DBNull.Value;
					}
					else
					{
						// Update Qty And Remove from the update
						if (oldRow[side + "OpenQty"] == DBNull.Value || (long)oldRow[side + "OpenQty"] != quantity)
						{
							oldRow[side + "OpenQty"] = quantity;
						}
						priceItems.OpenOrderPriceQty.Remove(oldPrice);
					}
				}

				// Only new prices left here
				foreach (Price price in priceItems.OpenOrderPriceQty.Keys)
				{
					DataRowView[] rows = rocQuickListBook.PriceSearchView.FindRows(price);

					foreach (DataRowView row in rows)
					{
						row[side + "OpenQty"] = priceItems.OpenOrderPriceQty[price];
						// Flag Stop Price
						if (priceItems.OpenStopOrderPrice.Contains(price))
						{
							row[side + "StopFlag"] = PriceFlages.stop;
						}
					}
				}
			}
		}

		private void UpdateAggregation()
		{
			if (_flags[FormFlags.ID.BuyAgg])
			{
				_openBuyQty = GetOpenQty(CSVFieldIDs.SideCodes.Buy);
			}

			if (_flags[FormFlags.ID.SellAgg])
			{
				_openSellQty = GetOpenQty(CSVFieldIDs.SideCodes.Sell);
			}

			if (_flags[FormFlags.ID.BuyAgg | FormFlags.ID.SellAgg])
			{
				_netFilledQty = GetFilledQty(CSVFieldIDs.SideCodes.Buy) - GetFilledQty(CSVFieldIDs.SideCodes.Sell);
			}
		}

		private void UpdateTicket()
		{
			try
			{
				if (CurrentSecInfo != null)
				{
					if (CurrentSecInfo.LongName != null && CurrentSecInfo.LongName != "")
					{
						LongName = CurrentSecInfo.LongName;
					}

					if (IsStock)
					{
						dspAskPrice.MaxDecimal = 2;
						dspBidPrice.MaxDecimal = 2;
						dspNetChange.MaxDecimal = 2;
						dspLowPrice.MaxDecimal = 2;
						dspHighPrice.MaxDecimal = 2;
						dspClosePrice.MaxDecimal = 2;
						dspTradedPrice.MaxDecimal = 2;

						numLimitPrice.DecimalPlaces = 2;
						numStopPrice.DecimalPlaces = 2;
						((DGVQuickListNumberColumn)rocQuickListBook.Columns["Price"]).MaxDecimal = 2;
					}
					else
					{
						dspAskPrice.MaxDecimal = 7;
						dspBidPrice.MaxDecimal = 7;
						dspNetChange.MaxDecimal = 7;
						dspLowPrice.MaxDecimal = 7;
						dspHighPrice.MaxDecimal = 7;
						dspClosePrice.MaxDecimal = 7;
						dspTradedPrice.MaxDecimal = 7;

						numLimitPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
						numStopPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
						((DGVQuickListNumberColumn)rocQuickListBook.Columns["Price"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
					}

					dspAskPrice.TickSize = CurrentSecInfo.TickSize;
					dspBidPrice.TickSize = CurrentSecInfo.TickSize;
					dspNetChange.TickSize = CurrentSecInfo.TickSize;
					dspLowPrice.TickSize = CurrentSecInfo.TickSize;
					dspHighPrice.TickSize = CurrentSecInfo.TickSize;
					dspClosePrice.TickSize = CurrentSecInfo.TickSize;
					dspTradedPrice.TickSize = CurrentSecInfo.TickSize;

					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec;
					dspLimitPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;

					numStopPrice.Increment = CurrentSecInfo.TickSizeDec;
					dspStopPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;

					rocQuickListBook.TickSize = CurrentSecInfo.TickSize;
					rocQuickListBook.ReadyForTickTable = true;
				}

				UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Order Execution -

		private void QuickButtonSupport_QuickButtonClicked(object sender, QuickButtonClickedEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.Data.secType)
			{
				case CSVFieldIDs.SecurityTypes.Future:
				case CSVFieldIDs.SecurityTypes.Equity:
					numQuantity.Value = e.Data.qty;

					switch (e.Data.basePriceSource)
					{
						case QuickButtonBasePriceSource.Bid:
							SetPriceByData(dspBidPrice.Value, e.Data.basePriceOffset);
							break;
						case QuickButtonBasePriceSource.Ask:
							SetPriceByData(dspAskPrice.Value, e.Data.basePriceOffset);
							break;
						case QuickButtonBasePriceSource.Last:
							SetPriceByData(dspTradedPrice.Value, e.Data.basePriceOffset);
							break;
						case QuickButtonBasePriceSource.High:
							SetPriceByData(dspHighPrice.Value, e.Data.basePriceOffset);
							break;
						case QuickButtonBasePriceSource.Low:
							SetPriceByData(dspLowPrice.Value, e.Data.basePriceOffset);
							break;
					}

					if (IsStock)
					{
						if (e.Data.shortLocate != "")
						{
							cboShortLenders.Text = e.Data.shortLocate;
							cboShortLenders_KeyUp(null, new KeyEventArgs(Keys.Enter));
						}
					}

					if (!e.Data.loadOnly)
					{
						switch (e.Data.side)
						{
							case QuickButtonSides.Buy:
								cmdBuy_Click(null, EventArgs.Empty);
								break;
							case QuickButtonSides.Sell:
								cmdSell_Click(null, EventArgs.Empty);
								break;
							case QuickButtonSides.Short:
								cmdShort_Click(null, EventArgs.Empty);
								break;
						}
					}
					break;
			}
		}

		private void SetPriceByData(object value, long offset)
		{
			try
			{
				decimal offsetPrice = 0;
				if (offset != 0 && CurrentSecInfo != null)
				{
					offsetPrice = offset * CurrentSecInfo.TickSizeDec;
				}

				if (value != null)
				{
					decimal price = Convert.ToDecimal(value) + offsetPrice;
					if (panelLimitPrice.Visible)
					{
						if (price >= numLimitPrice.Minimum && price <= numLimitPrice.Maximum)
						{
							if (CurrentSecInfo != null)
							{
								numLimitPrice.Value = Math.Round(price, CurrentSecInfo.DecimalPlaces);
							}
							else
							{
								numLimitPrice.Value = price;
							}
						}
					}
					else if (panelStopPrice.Visible)
					{
						if (price >= numStopPrice.Minimum && price <= numStopPrice.Maximum)
						{
							if (CurrentSecInfo != null)
							{
								numStopPrice.Value = Math.Round(price, CurrentSecInfo.DecimalPlaces);
							}
							else
							{
								numStopPrice.Value = price;
							}
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cmdBuy_Click(object sender, EventArgs e)
		{
			if (_flags[FormFlags.ID.HasError])
			{
				ValidateTicketValues();
			}

			if (!_flags[FormFlags.ID.HasError])
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString());
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Quick Buy Order", CurrentSymbolDetail));
			}
		}

		private void cmdSell_Click(object sender, EventArgs e)
		{
			if (_flags[FormFlags.ID.HasError])
			{
				ValidateTicketValues();
			}

			if (!_flags[FormFlags.ID.HasError])
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString());
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Quick Sell Order", CurrentSymbolDetail));
			}
		}

		private void cmdShort_Click(object sender, EventArgs e)
		{
			if (_flags[FormFlags.ID.HasError])
			{
				ValidateTicketValues();
			}

			if (!_flags[FormFlags.ID.HasError])
			{
				GLOBAL.Interrupt = true;
				if (IsStock)
				{
					BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Short.ToString());
				}
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Quick Short Order", CurrentSymbolDetail));
			}
		}

		private void BuildOrder(string side)
		{
			long orderType = GetOrderTypeCode(cboOrder.Text);
			long duration = GetDurationCode(cboDuration.Text);
			string ocoOrderID = "";

			switch (orderType)
			{
				#region - Market -

				case CSVEx.CSVFieldIDs.OrderTypes.Market:
					BuildOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					BuildOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					BuildOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;

				#endregion

				#region - OCO -

				case CSVEx.CSVFieldIDs.OrderTypes.OCO:

					ocoOrderID = GLOBAL.OrderManagers.RomMessageMaker.GetOrderID(GLOBAL.OrderManagers.UserName);

					// Limit Price - primary side
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						"",
						ocoOrderID + "M");

					// Stop Price - other side
					if (side == CSVEx.CSVFieldIDs.SideCodes.Buy.ToString())
					{
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(),
							orderType.ToString(),
							duration.ToString(),
							numStopPrice.Value.ToString(),
							"",
							ocoOrderID + "S");
					}
					else
					{
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(),
							orderType.ToString(),
							duration.ToString(),
							numStopPrice.Value.ToString(),
							"",
							ocoOrderID + "S");
					}
					break;

				#endregion

				#region - STOP -

				case CSVEx.CSVFieldIDs.OrderTypes.Stop:
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						"",
						numStopPrice.Value.ToString());
					break;

				#endregion

				default:
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						"");
					break;
			}
		}
		private void BuildOrder(string side, string orderType, string duration)
		{
			BuildOrder(side, orderType, duration, "", "", "");
		}
		private void BuildOrder(string side, string orderType, string duration, string limitPrice, string stopPrice)
		{
			BuildOrder(side, orderType, duration, limitPrice, stopPrice, "");
		}
		private void BuildOrder(string side, string orderType, string duration, string limitPrice, string stopPrice, string ocoOrderID)
		{
			RomBasicOrder order = new RomBasicOrder();

			//DestinationMap dest = new DestinationMap();
			//AccountMap acct = new AccountMap();

			try
			{
				if (DDAccountDropDown.CurrentAcctountInfo != null && 
					DDAccountDropDown.CurrentDestinationInfo != null && 
					DDAccountDropDown.CurrentDestinationInfo.destID != null)
				{
					if (ocoOrderID != "")
					{
						order.orderID = ocoOrderID;
					}

					if (IsFuture)
					{
						order.symbol = CurrentSymbolDetail;
						order.mdSymbol = CurrentSecInfo.MDSymbol;
						order.secType = CurrentSecInfo.SecType;
						order.underlying = CurrentSecInfo.Underlying;
						order.expDate = CurrentSecInfo.Expiration;
						order.multiplier = CurrentSecInfo.ContractSize.ToString();
					}
					else if (IsStock)
					{
						order.symbol = CurrentSymbolDetail;
						order.mdSymbol = CurrentSecInfo.MDSymbol;
						order.secType = CurrentSecInfo.SecType;
						if (cboShortLenders.Text != "")
						{
							order.shortLender = cboShortLenders.Text;
						}
					}
					order.tradeFor = DDAccountDropDown.CurrentTradeFor;
					order.trader = GLOBAL.OrderManagers.UserName;
					order.shares = numQuantity.Value.ToString();
					order.price = limitPrice;
					order.stopPrice = stopPrice;
					order.side = side;
					order.orderType = orderType;
					order.duration = duration;
					order.exchangeID = DDAccountDropDown.CurrentDestinationInfo.destID.ToString();
					order.cmtaFirmID = DDAccountDropDown.CurrentDestinationInfo.cmtaFirmID;
					order.duration = duration;
					order.capacity = DDAccountDropDown.CurrentAcctountInfo.capacity;
					order.clearingAcctID = DDAccountDropDown.CurrentAcctountInfo.clearingAcID;
					order.clearingFirmID = DDAccountDropDown.CurrentAcctountInfo.clearingFirmID;
					order.firmArc = DDAccountDropDown.CurrentAcctountInfo.firmAcr;
					order.localAcctAcrn = DDAccountDropDown.CurrentAcctountInfo.localAcAcrn;
					order.occAcct = DDAccountDropDown.CurrentAcctountInfo.occAcr;
					if (DDAccountDropDown.CurrentAcctountInfo.homeExchange != null)
					{
						order.homeExchange = DDAccountDropDown.CurrentAcctountInfo.homeExchange;
					}

					if (IsSpread)
					{
						order.cplxOrderType = "5";
					}

					GLOBAL.OrderManagers.EnterOrder(order, true);
				}
				else
				{
					ValidateTicketValues();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void CancelOrderByPrice(string side, double price)
		{
			Dictionary<string, ROCOrder> orders = GetOpenOrder(side, price);
			foreach (ROCOrder order in orders.Values)
			{
				GLOBAL.OrderManagers.CancelSingleOrder(order.Tag);
			}
		}

		private void CancelOrderBySide(long side)
		{
			Dictionary<string, ROCOrder> orders = GetOpenOrder(side, null);
			foreach (ROCOrder order in orders.Values)
			{
				GLOBAL.OrderManagers.CancelSingleOrder(order.Tag);
			}
		}

		private void ReplaceOrderByPrice(string side, double startReplacePrice, double endReplacePrice)
		{
			if (startReplacePrice != endReplacePrice)
			{
				Dictionary<string, ROCOrder> orders = GetOpenOrder(side, startReplacePrice);
				foreach (ROCOrder order in orders.Values)
				{
					switch (order.OrderType)
					{
						case CSVFieldIDs.OrderTypes.Stop:
						case CSVFieldIDs.OrderTypes.StopLimit:
							GLOBAL.OrderManagers.ReplaceOrder(order.Tag, "", "", endReplacePrice.ToString(), "", "");
							break;
						default:
							GLOBAL.OrderManagers.ReplaceOrder(order.Tag, "", endReplacePrice.ToString(), "", "", "");
							break;
					}
				}
			}
		}

		#endregion

		#region - Ticket Display -

		private void ValidateTicketValues()
		{
			_flags[FormFlags.ID.HasError] = false;

			if (!_flags[FormFlags.ID.LoadingValue])
			{
				setHasError(SetState(cmdTradeFor, (cmdTradeFor.Text == "N/A" || cmdTradeFor.Text == "")));
				setHasError(SetState(cmdAccount, (cmdAccount.Text == "N/A" || cmdAccount.Text == "")));
				setHasError(SetState(lblExchange, (cboExchange.Text == "")));
				SetPanelWidth(ref panelExchange, cboExchange);

				try
				{
					if (IsStock && _orderTypesView.Table.TableName != "STK")
					{
						_orderTypesView = new DataView(GLOBAL.HOrders.StockOrderTypes);
						_orderTypesView.RowFilter = TicketSupport.BuildOrderTypeFilter(cboExchange, cboOrder);
						cboOrder.DataSource = _orderTypesView;
					}
					else if (IsFuture && _orderTypesView.Table.TableName != "FUT")
					{
						_orderTypesView = new DataView(GLOBAL.HOrders.FutureOrderTypes);
						_orderTypesView.RowFilter = TicketSupport.BuildOrderTypeFilter(cboExchange, cboOrder);
						cboOrder.DataSource = _orderTypesView;
					}

					_orderTypesView.RowFilter = TicketSupport.BuildOrderTypeFilter(cboExchange,cboOrder);
					BuildDurationTypeFilter();

					SetTicketStateByOrderType();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				setHasError(SetState(lblQuantity, numQuantity.Value <= 0));
				SetPanelWidth(ref panelShares, numQuantity);
				rocQuickListBook.Qty = Convert.ToInt64(numQuantity.Value);

				setHasError(SetState(numQuantity, numQuantity.Value <= 0));

				if (panelLimitPrice.Visible)
				{
					if (!IsSpread)
					{
						setHasError(SetState(lblLimitPrice, numLimitPrice.Value <= 0));
						setHasError(SetState(numLimitPrice, numLimitPrice.Value <= 0));
					}
					else
					{
						setHasError(SetState(lblLimitPrice, false));
						setHasError(SetState(numLimitPrice, false));
					}

					SetPanelWidth(ref panelLimitPrice, numLimitPrice);
					SetPriceDisplay(PriceType.Limit, false);
				}

				if (panelStopPrice.Visible)
				{
					if (!IsSpread)
					{
						setHasError(SetState(lblStopPrice, numStopPrice.Value <= 0));
						setHasError(SetState(numStopPrice, numStopPrice.Value <= 0));
					}
					else
					{
						setHasError(SetState(lblStopPrice, false));
						setHasError(SetState(numStopPrice, false));
					}

					SetPanelWidth(ref panelStopPrice, numStopPrice);
					SetPriceDisplay(PriceType.Stop, false);
				}

				if (IsStock)
				{
					if (cboShortLenders.Text == "")
					{
						cmdShort.BackColor = Color.Gray;
						cmdShort.Enabled = false;
					}
					else
					{
						cmdShort.BackColor = Color.Orchid;
						cmdShort.Enabled = true;
					}

					SetPanelWidth(ref panelShortLender, cboShortLenders);

					panelStopLimitOffset.Visible = false;
					panelShortLender.Visible = true;
					cmdShort.Visible = true;
				}
				else
				{
					panelStopLimitOffset.Visible = true;
					panelShortLender.Visible = false;
					cmdShort.Visible = false;
				}

				setHasError(SetState(lblOrderType, cboOrder.Text == ""));
				SetPanelWidth(ref panelOrderTypes, cboOrder);

				setHasError(SetState(lblDuration, cboDuration.Text == ""));
				SetPanelWidth(ref panelDuration, cboDuration);

				BuildOrderMsg();
			}
		}

		private void SetPriceDisplay(PriceType type, bool firstTime)
		{
			string formatedPrice = "";
			switch (type)
			{
				case PriceType.Limit:
					if (firstTime)
					{
						dspLimitPrice.Font = numLimitPrice.Font;
					}
					if (dspLimitPrice.ForeColor != numLimitPrice.ForeColor)
					{
						dspLimitPrice.ForeColor = numLimitPrice.ForeColor;
					}
					formatedPrice = GetDoubleFormat(Convert.ToDouble(numLimitPrice.Value));
					if (dspLimitPrice.Text != formatedPrice)
					{
						dspLimitPrice.Text = formatedPrice;
					}
					break;
				case PriceType.Stop:
					if (firstTime)
					{
						dspStopPrice.Font = numStopPrice.Font;
					}
					if (dspStopPrice.ForeColor != numStopPrice.ForeColor)
					{
						dspStopPrice.ForeColor = numStopPrice.ForeColor;
					}
					formatedPrice = GetDoubleFormat(Convert.ToDouble(numStopPrice.Value));
					if (dspStopPrice.Text != formatedPrice)
					{
						dspStopPrice.Text = formatedPrice;
					}
					break;
			}
		}
		private void SetPriceDisplayFromUserInput(PriceType type)
		{
			decimal price = 0;

			switch (type)
			{
				case PriceType.Limit:
					price = Convert.ToDecimal(GetPriceFromUserInput(dspLimitPrice.Text));
					if (numLimitPrice.Value != price)
					{
						if (CurrentSecInfo != null)
						{
							numLimitPrice.Value = Math.Round(price, CurrentSecInfo.DecimalPlaces);
						}
						else
						{
							numLimitPrice.Value = price;
						}
					}
					else
					{
						SetPriceDisplay(PriceType.Limit, false);
					}
					break;
				case PriceType.Stop:
					price = Convert.ToDecimal(GetPriceFromUserInput(dspStopPrice.Text));
					if (numStopPrice.Value != price)
					{
						if (CurrentSecInfo != null)
						{
							numStopPrice.Value = Math.Round(price, CurrentSecInfo.DecimalPlaces);
						}
						else
						{
							numStopPrice.Value = price;
						}
					}
					else
					{
						SetPriceDisplay(PriceType.Stop, false);
					}
					break;
			}
		}

		private void setHasError(bool isError)
		{
			if (isError)
				_flags[FormFlags.ID.HasError] = true;
		}

		private void SetTicketStateByOrderType()
		{
			switch (cboOrder.Text)
			{
				case "MARKET":
				case "MOC":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = false;
					cboDuration.Enabled = false;
					cboDuration.Text = "DAY";
					break;
				case "MOO":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = false;
					cboDuration.Enabled = false;
					cboDuration.Text = "OPG";
					break;
				case "STOPLIMIT":
				case "OCO":
					panelLimitPrice.Visible = true;
					panelStopPrice.Visible = true;
					EnableDuration();
					break;
				case "STOP":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = true;
					EnableDuration();
					PriceSelection_Click(lblStopPrice, EventArgs.Empty);
					break;
				case "LOC":
					panelLimitPrice.Visible = true;
					panelStopPrice.Visible = false;
					EnableDuration();
					PriceSelection_Click(lblLimitPrice, EventArgs.Empty);
					break;
				default:
					panelLimitPrice.Visible = true;
					panelStopPrice.Visible = false;
					EnableDuration();
					PriceSelection_Click(lblLimitPrice, EventArgs.Empty);
					break;
			}
		}

		private void EnableDuration()
		{
			if (!cboDuration.Enabled)
			{
				cboDuration.Enabled = true;
				cboDuration.Text = "DAY";
			}
		}

		private void BuildDurationTypeFilter()
		{
			string durationFilter = TicketSupport.BuildDurationTypeFilter(cboExchange);
			if (_durationsView.RowFilter != durationFilter)
			{
				_durationsView.RowFilter = durationFilter;
			}
		}

		private void BuildOrderMsg()
		{
			try
			{
				if (_flags[FormFlags.ID.HasError])
				{
					dspOrderMsg.ForeColor = Color.Red;
				}
				else
				{
					dspOrderMsg.ForeColor = Color.Black;
				}

				if (CurrentSymbolDetail != "")
				{
					if (CurrentSecInfo.LongName == "")
					{
						LongName = string.Format("- {0} -", CurrentSymbolDetail.ToUpper().Trim());
					}
				}
				else
				{
					LongName = "Quick Ticket";
				}

				// 0 Quantity
				// 1 Symbol
				// 2 OrderType
				// 3 Limit Price / Stop Price
				// 4 Duration
				// 5 Exchange
				dspOrderMsg.Text = "";
				dspOrderMsg.SelectionAlignment = HorizontalAlignment.Center;
				dspOrderMsg.SelectionBackColor = dspOrderMsg.BackColor;

				string orderSymbol = cboSymbolDetails.Text;

				if (panelLimitPrice.Visible && panelStopPrice.Visible)
				{
					dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {5} - {6} {3} {4}",
						numQuantity.Value,
						orderSymbol,
						cboOrder.Text,
						cboDuration.Text,
						cboExchange.Text,
						GetDisplayLimitPrice(),
						GetDisplayStopPrice());
				}
				else if (panelLimitPrice.Visible && !panelStopPrice.Visible)
				{
					dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {5} {3} {4}",
						numQuantity.Value,
						orderSymbol,
						cboOrder.Text,
						cboDuration.Text,
						cboExchange.Text,
						GetDisplayLimitPrice());
				}
				else if (!panelLimitPrice.Visible && panelStopPrice.Visible)
				{
					dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {5} {3} {4}",
						numQuantity.Value,
						orderSymbol,
						cboOrder.Text,
						cboDuration.Text,
						cboExchange.Text,
						GetDisplayStopPrice());
				}
				else
				{
					dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {3} {4}",
						numQuantity.Value,
						orderSymbol,
						cboOrder.Text,
						cboDuration.Text,
						cboExchange.Text);
				}

				if (cboOrder.Text != "LIMIT")
				{
					dspOrderMsg.Select(dspOrderMsg.Find(cboOrder.Text), cboOrder.Text.Length);
					dspOrderMsg.SelectionBackColor = Color.Yellow;
				}

				if (cboDuration.Text != "DAY")
				{
					dspOrderMsg.SelectAll();
					dspOrderMsg.Select(dspOrderMsg.Find(cboDuration.Text), cboDuration.Text.Length);
					dspOrderMsg.SelectionBackColor = Color.Yellow;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private string GetDisplayLimitPrice()
		{
			if (dspLimitPrice.Text != numLimitPrice.Value.ToString("######0.00#####"))
			{
				return string.Format("{0} ({1})", new object[] { dspLimitPrice.Text, numLimitPrice.Value.ToString("######0.00#####") });
			}
			else
			{
				return numLimitPrice.Value.ToString("######0.00#####");
			}
		}

		private string GetDisplayStopPrice()
		{
			if (dspStopPrice.Text != numStopPrice.Value.ToString("######0.00#####"))
			{
				return string.Format("{0} ({1})", new object[] { dspStopPrice.Text, numStopPrice.Value.ToString("######0.00#####") });
			}
			else
			{
				return numStopPrice.Value.ToString("######0.00#####");
			}
		}

		private void ResetTicketValues()
		{
			_flags[FormFlags.ID.LoadingValue] = true;
			_flags[FormFlags.ID.Centered] = false;

			_openBuyQty = 0;
			_openSellQty = 0;
			_netFilledQty = 0;

			HasFirstUpdate = false;

			CurrentSecInfo = new BaseSecurityInfo();

			LongName = CurrentSecInfo.LongName;

			dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;

			dspBidPrice.Value = 0.0;
			dspBidSize.Value = 0;
			dspAskPrice.Value = 0.0;
			dspAskSize.Value = 0;
			dspNetChange.Value = 0.0;
			dspPctChange.Value = 0.0;
			dspTradedPrice.Value = 0.0;
			dspTradeVolume.Value = 0;
			picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.None];
			dspVolume.Value = 0;
			dspLowPrice.Value = 0.0;
			dspHighPrice.Value = 0.0;
			dspClosePrice.Value = 0.0;

			lblSecurityStatus.Visible = false;
			lblSecurityStatus.Blink = false;
			lblSecurityStatus.Text = "STA";

			lblMarketDataError.Visible = false;
			lblMarketDataError.Blink = false;

			lblDoNotTranslate.Visible = false;
			lblDoNotTranslate.Blink = false;

			numLimitPrice.Value = 0;

			//cboOrder.Text = "Limit";
			//cboDuration.Text = "DAY";
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Future, CSVEx.CSVFieldIDs.SecurityTypes.Future));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(TicketDefaults.Future, CSVEx.CSVFieldIDs.SecurityTypes.Future), false);

			_flags[FormFlags.ID.LoadingValue] = false;

			rocQuickListBook.TradePrice = 0;
			rocQuickListBook.HighPrice = 0;
			rocQuickListBook.LowPrice = 0;
			rocQuickListBook.BidPrice = 0;
			rocQuickListBook.AskPrice = 0;
			rocQuickListBook.TickSize = CurrentSecInfo.TickSize;
			rocQuickListBook.ReadyForTickTable = false;

			if (rocQuickListBook.RocGridTable != null)
			{
				lock (rocQuickListBook.RocGridTable)
				{
					rocQuickListBook.ResetMaxBookSize();
					rocQuickListBook.RocGridTable.Clear();
				}

				((DGVQuickListNumberColumn)rocQuickListBook.Columns["Price"]).MaxDecimal = 7;
			}

			if (panelManagerQuick.SelectedPanel != managedPanelBook)
			{
				panelManagerQuick.SelectedPanel = managedPanelBook;
			}

			ValidateTicketValues();
		}

		// Used to load symbol defaults from last saved settings, else it loads the Ticket Default Value;
		private void UILoadSymbolDefaults(SymbolSettingData data)
		{
			_flags[FormFlags.ID.LoadingValue] = true;

			bool reloadUserAccounts = false;

			if (data.tradeFor != "")
			{
				reloadUserAccounts = true;
				DDAccountDropDown.SelectedTraderFor = data.tradeFor;
			}

			if (data.account != "")
			{
				reloadUserAccounts = true;
				DDAccountDropDown.SelectedLocalAccountAcrn = data.account;
			}

			if (data.exchange != "")
			{
				reloadUserAccounts = true;
				DDAccountDropDown.SelectedExchange = data.exchange;
			}

			if (reloadUserAccounts)
			{
				DDAccountDropDown.LoadUserAccounts();
			}

			if (data.orderType != "")
			{
				cboOrder.Text = data.orderType;
			}
			else
			{
				cboOrder.Text = "LIMIT";
			}

			if (data.duration != "")
			{
				cboDuration.Text = data.duration;
			}
			else
			{
				cboDuration.Text = "DAY";
			}

			numQuantity.Value = data.qty;
			numQuantity.Increment = data.qtyIncrement;
			dspQuantityIncrement.Value = numQuantity.Increment;

			if ((data.limitPriceIncrement % CurrentSecInfo.TickSizeDec) != 0)
			{
				data.limitPriceIncrement = CurrentSecInfo.TickSizeDec;
			}

			// Override with current traded price
			//numLimitPrice.Value = data.limitPrice;
			numLimitPrice.Increment = data.limitPriceIncrement;
			dspLimitPriceIncrement.Value = data.limitPriceIncrement / CurrentSecInfo.TickSizeDec;

			if ((data.stopPriceIncrement % CurrentSecInfo.TickSizeDec) != 0)
			{
				data.stopPriceIncrement = CurrentSecInfo.TickSizeDec;
			}

			numStopPrice.Value = data.stopPrice;
			numStopPrice.Increment = data.stopPriceIncrement;
			dspStopPriceIncrement.Value = data.stopPriceIncrement / CurrentSecInfo.TickSizeDec;

			if (data.stopLimitOffset > 0)
			{
				numStopLimitOffset.Value = data.stopLimitOffset;
			}
			else
			{
				numStopLimitOffset.Value = 0;
			}

			if (data.shortLender != "")
			{
				CurrentShortLender = data.shortLender;
			}

			_flags[FormFlags.ID.LoadingValue] = false;

			ValidateTicketValues();
		}

		#endregion

		#region - Export & Import -

		internal string ExportQuickGrid()
		{
			ROCQuickListProfile prof = new ROCQuickListProfile(rocQuickListBook);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		internal void ImportQuickGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCQuickListProfile prof = (ROCQuickListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCQuickListProfile(rocQuickListBook, prof);
				}
			}
		}

		internal string ExportTicket()
		{
			ROCTicketProfile prof = new ROCTicketProfile(this);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		internal void ImportTicket(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCTicketProfile prof = (ROCTicketProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					foreach (string symbolDetail in prof.SymbolDetails)
					{
						CurrentSymbolDetails.Rows.Add(symbolDetail);
					}

					foreach (string shortLender in prof.ShortLenders)
					{
						ShortLenders.Rows.Add(shortLender);
					}

					prof = new ROCTicketProfile(this, prof);
				}
			}
		}

		internal void ReloadTicket()
		{
			if (CurrentSymbolDetail != "" && CurrentSecInfo != null)
			{
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
			}
			else
			{
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(TicketDefaults.Future, CSVFieldIDs.SecurityTypes.Future));
			}
			ValidateTicketValues();
		}

		internal void Clone()
		{
			lock (GLOBAL.HWindows.QuickTicketWindows)
			{
				frmQuickTicket w = new frmQuickTicket() {
					Width = this.Width,
					Height = this.Height
				};
				w.Clone(new ROCTicketProfile(this), new ROCQuickListProfile(rocQuickListBook));

				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}
		internal void Clone(ROCTicketProfile prof, ROCQuickListProfile gridProf)
		{
			if (prof != null)
			{
				prof = new ROCTicketProfile(this, prof);
			}
			if (gridProf != null)
			{
				gridProf = new ROCQuickListProfile(rocQuickListBook, gridProf);
			}
		}

		#endregion

		#region - Trader Account Drop Down -

		private void DDAccountDropDown_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "TradeForChanged":
				case "AccountChanged":
					UpdateOrders();
					break;
			}
		}

		#endregion

		#region - Settings DropDown -

		private void itemSaveAsDefault_Click(object sender, EventArgs e)
		{
			SymbolSettingData data = GLOBAL.HSymbolSettingData.GetSymbolDefaults(CurrentSymbolDetail, CurrentSecInfo.SecType);

			data.tradeFor = cmdTradeFor.Text;
			data.account = cmdAccount.Text;
			data.exchange = cboExchange.Text;

			data.qty = numQuantity.Value;
			data.qtyIncrement = numQuantity.Increment;

			data.orderType = cboOrder.Text;
			data.limitPrice = numLimitPrice.Value;
			data.limitPriceIncrement = numLimitPrice.Increment;

			data.stopPrice = numStopPrice.Value;
			data.stopPriceIncrement = numStopPrice.Increment;

			data.duration = cboDuration.Text;
			data.stopLimitOffset = numStopLimitOffset.Value;

			GLOBAL.HSymbolSettingData.SetSymbolDefaults(data);
		}

		private void itemResetToSystemDefault_Click(object sender, EventArgs e)
		{
			GLOBAL.HSymbolSettingData.RemoveSymbolDefault(CurrentSymbolDetail, CurrentSecInfo.SecType);
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
		}

		#endregion

		#region - SymbolDetails DropDown -

		private void cboSymbolDetails_LostFocus(object sender, EventArgs e)
		{
			try
			{
				DDSymbolDetails.SymbolDetailsKeyUp(new KeyEventArgs(Keys.Enter));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cboSymbolDetails_KeyUp(object sender, KeyEventArgs e)
		{
			try
			{
				DDSymbolDetails.SymbolDetailsKeyUp(e);
				Control_KeyUp(sender, e);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cboSymbolDetails_KeyDown(object sender, KeyEventArgs e)
		{
			try
			{
				DDSymbolDetails.SymbolDetailsKeyDown(e);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - ShortLenders DropDown -

		private DropDownShortLenders _ddShortLenders;
		private DropDownShortLenders ddShortLenders
		{
			get
			{
				if (_ddShortLenders == null)
				{
					_ddShortLenders = new DropDownShortLenders(this);
				}
				return _ddShortLenders;
			}
		}

		internal string CurrentShortLender
		{
			get
			{
				return cboShortLenders.Text;
			}
			set
			{
				cboShortLenders.Text = value;
			}
		}

		private DataTable _shortLenders;
		internal DataTable ShortLenders
		{
			get
			{
				if (_shortLenders == null)
				{
					_shortLenders = new DataTable();

					_shortLenders.Columns.Add(new DataColumn("ShortLender", Type.GetType("System.String")));

					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_shortLenders.Columns["ShortLender"]);
					_shortLenders.PrimaryKey = keyColumns.ToArray();

					_shortLenders.DefaultView.Sort = "ShortLender";
				}
				return _shortLenders;
			}
			set
			{
				_shortLenders = value;
			}
		}

		private void cboShortLenders_KeyUp(object sender, KeyEventArgs e)
		{
			ddShortLenders.ShortLendersKeyUp(e);
		}

		private void cboShortLenders_KeyDown(object sender, KeyEventArgs e)
		{
			ddShortLenders.ShortLendersKeyDown(e);
		}

		#endregion

		#region - Qty Menu -

		private menuBaseAction _menuQuantity;
		private PropertyContainer _menuQuantityContainer;
		private void ShowQuantityMenu()
		{
			if (_menuQuantity == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				if (IsStock)
				{
					items.Add("100", "100");
					items.Add("500", "500");
					items.Add("1000", "1000");
					items.Add("5000", "5000");
				}
				else
				{
					items.Add("1", "1");
					items.Add("2", "2");
					items.Add("5", "5");
					items.Add("10", "10");
					items.Add("20", "20");
					items.Add("25", "25");
					items.Add("30", "30");
					items.Add("40", "40");
					items.Add("50", "50");
					items.Add("60", "60");
					items.Add("70", "70");
					items.Add("75", "75");
					items.Add("80", "80");
					items.Add("90", "90");
					items.Add("100", "100");
				}

				_menuQuantity = new menuBaseAction(items, true);
				_menuQuantity.PropertyChanged += new PropertyChangedEventHandler(menuQuantity_PropertyChanged);
			}

			if (_menuQuantityContainer == null)
			{
				_menuQuantityContainer = new PropertyContainer(_menuQuantity);
				_menuQuantityContainer.Closed += new ToolStripDropDownClosedEventHandler(menuQuantityContainer_Closed);
			}

			_menuQuantityContainer.Show(Cursor.Position);
		}

		private void menuQuantity_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuQuantityContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numQuantity.Value = 1;
					break;
				case "2":
					numQuantity.Value = 2;
					break;
				case "5":
					numQuantity.Value = 5;
					break;
				case "10":
					numQuantity.Value = 10;
					break;
				case "20":
					numQuantity.Value = 20;
					break;
				case "25":
					numQuantity.Value = 25;
					break;
				case "30":
					numQuantity.Value = 30;
					break;
				case "40":
					numQuantity.Value = 40;
					break;
				case "50":
					numQuantity.Value = 50;
					break;
				case "60":
					numQuantity.Value = 60;
					break;
				case "70":
					numQuantity.Value = 70;
					break;
				case "75":
					numQuantity.Value = 75;
					break;
				case "80":
					numQuantity.Value = 80;
					break;
				case "90":
					numQuantity.Value = 90;
					break;
				case "100":
					numQuantity.Value = 100;
					break;
				case "500":
					if (IsStock)
					{
						numQuantity.Value = 500;
					}
					break;
				case "1000":
					if (IsStock)
					{
						numQuantity.Value = 1000;
					}
					break;
				case "5000":
					if (IsStock)
					{
						numQuantity.Value = 5000;
					}
					break;
			}
		}

		private void menuQuantityContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuQuantityContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuQuantityContainer_Closed);
			_menuQuantityContainer = null;
			_menuQuantity.PropertyChanged -= new PropertyChangedEventHandler(menuQuantity_PropertyChanged);
			_menuQuantity = null;
		}

		#endregion

		#region - Qty Increment Menu -

		private menuBaseAction _menuQtyIncrement;
		private PropertyContainer _menuQtyIncrementContainer;
		private void dspQuantityIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuQtyIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						if (IsStock)
						{
							items.Add("100", "100");
							items.Add("500", "500");
							items.Add("1000", "1000");
							items.Add("5000", "5000");
						}
						else
						{
							items.Add("1", "1");
							items.Add("5", "5");
							items.Add("10", "10");
							items.Add("25", "25");
							items.Add("50", "50");
							items.Add("75", "75");
							items.Add("100", "100");
						}

						_menuQtyIncrement = new menuBaseAction(items, true);
						_menuQtyIncrement.PropertyChanged += new PropertyChangedEventHandler(menuQtyIncrement_PropertyChanged);
					}

					if (_menuQtyIncrementContainer == null)
					{
						_menuQtyIncrementContainer = new PropertyContainer(_menuQtyIncrement);
						_menuQtyIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuQtyIncrementContainer_Closed);
					}

					_menuQtyIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuQtyIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuQtyIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numQuantity.Increment = 1;
					break;
				case "5":
					numQuantity.Increment = 5;
					break;
				case "10":
					numQuantity.Increment = 10;
					break;
				case "25":
					numQuantity.Increment = 25;
					break;
				case "50":
					numQuantity.Increment = 50;
					break;
				case "75":
					numQuantity.Increment = 75;
					break;
				case "100":
					numQuantity.Increment = 100;
					break;
				case "500":
					if (IsStock)
					{
						numQuantity.Increment = 500;
					}
					break;
				case "1000":
					if (IsStock)
					{
						numQuantity.Increment = 1000;
					}
					break;
				case "5000":
					if (IsStock)
					{
						numQuantity.Increment = 5000;
					}
					break;
			}
			dspQuantityIncrement.Value = numQuantity.Increment;
		}

		private void menuQtyIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuQtyIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuQtyIncrementContainer_Closed);
			_menuQtyIncrementContainer = null;
			_menuQtyIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuQtyIncrement_PropertyChanged);
			_menuQtyIncrement = null;
		}

		#endregion

		#region - Limit Price Increment Menu -

		private menuBaseAction _menuLimitPriceIncrement;
		private PropertyContainer _menuLimitPriceIncrementContainer;
		private void dspLimitPriceIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuLimitPriceIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

						_menuLimitPriceIncrement = new menuBaseAction(items, true);
						_menuLimitPriceIncrement.PropertyChanged += new PropertyChangedEventHandler(menuLimitPriceIncrement_PropertyChanged);
					}

					if (_menuLimitPriceIncrementContainer == null)
					{
						_menuLimitPriceIncrementContainer = new PropertyContainer(_menuLimitPriceIncrement);
						_menuLimitPriceIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuLimitPriceIncrementContainer_Closed);
					}

					_menuLimitPriceIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuLimitPriceIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuLimitPriceIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 1;
					break;
				case "5":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 5;
					break;
				case "10":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 10;
					break;
				case "25":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 25;
					break;
				case "50":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 50;
					break;
				case "75":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 75;
					break;
				case "100":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 100;
					break;
			}
			dspLimitPriceIncrement.Value = numLimitPrice.Increment / CurrentSecInfo.TickSizeDec;
		}

		private void menuLimitPriceIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuLimitPriceIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuLimitPriceIncrementContainer_Closed);
			_menuLimitPriceIncrementContainer = null;
			_menuLimitPriceIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuLimitPriceIncrement_PropertyChanged);
			_menuLimitPriceIncrement = null;
		}

		#endregion

		#region - Stop Price Increment Menu -

		private menuBaseAction _menuStopPriceIncrement;
		private PropertyContainer _menuStopPriceIncrementContainer;
		private void dspStopPriceIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuStopPriceIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

						_menuStopPriceIncrement = new menuBaseAction(items, true);
						_menuStopPriceIncrement.PropertyChanged += new PropertyChangedEventHandler(menuStopPriceIncrement_PropertyChanged);
					}

					if (_menuStopPriceIncrementContainer == null)
					{
						_menuStopPriceIncrementContainer = new PropertyContainer(_menuStopPriceIncrement);
						_menuStopPriceIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuStopPriceIncrementContainer_Closed);
					}

					_menuStopPriceIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuStopPriceIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuStopPriceIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 1;
					break;
				case "5":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 5;
					break;
				case "10":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 10;
					break;
				case "25":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 25;
					break;
				case "50":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 50;
					break;
				case "75":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 75;
					break;
				case "100":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 100;
					break;
			}
			dspStopPriceIncrement.Value = numStopPrice.Increment / CurrentSecInfo.TickSizeDec;
		}

		private void menuStopPriceIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuStopPriceIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuStopPriceIncrementContainer_Closed);
			_menuStopPriceIncrementContainer = null;
			_menuStopPriceIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuStopPriceIncrement_PropertyChanged);
			_menuStopPriceIncrement = null;
		}

		#endregion

		#region - Columns Menu -

		private menuBaseFilter _menuColumns;
		private PropertyContainer _menuColumnsContainer;
		private void ShowColumnMenu()
		{
			if (_menuColumns == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();

				items.Add("Volume", new FilterItem("Volume", !rocQuickListBook.Columns["Volume"].Visible));
				items.Add("TradedVolume", new FilterItem("TradedVolume", !rocQuickListBook.Columns["TradedVolume"].Visible));
				
				_menuColumns = new menuBaseFilter(items, true);
				_menuColumns.PropertyChanged += new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			}

			if (_menuColumnsContainer == null)
			{
				_menuColumnsContainer = new PropertyContainer(_menuColumns);
				_menuColumnsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			}

			_menuColumnsContainer.Show(Cursor.Position);
		}

		private void menuColumns_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Volume":
					rocQuickListBook.Columns["Volume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TradedVolume":
					rocQuickListBook.Columns["TradedVolume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
			}
		}

		private void menuColumnsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuColumnsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			_menuColumnsContainer = null;
			_menuColumns.PropertyChanged -= new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			_menuColumns = null;
		}

		#endregion
	}
}