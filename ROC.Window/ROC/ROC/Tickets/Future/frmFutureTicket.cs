using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

using RDSEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using LabelEx;
using ROMEx;
using CSVEx;
using MarketData;

using Price = Common.Price;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	internal partial class frmFutureTicket : FutureSupport
	{
		#region - Properties -

		private bool gotSSFChain
		{
			get
			{
				if (CurrentSecInfo != null &&
					CurrentSecInfo.SSFChain.Count > 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		#endregion

		#region - Local Variable -

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

		internal Price? HighPrice => getPrice(dspHighPrice);
		internal Price? LowPrice => getPrice(dspLowPrice);
		internal Price? AskPrice => getPrice(dspAskPrice);
		internal Price? BidPrice => getPrice(dspBidPrice);

		private const int _defaultBook = 454;
		private const int _defaultSSF = 354;

		//private int _bookDepthLimit = 10;

		private string _lastSymbolDetail = "";
		private bool _lastIsBook = true;

		private Size _lastSize = new Size();
		private bool _widthChanged = false;
		private bool _heightChanged = false;

		private bool _isInError = false;
		private bool _isLoadingValue = true;

		private DataView _orderTypesView;
		private DataView _durationsView;

		private DataView _bookBidSearchView;
		private DataView bookBidSearchView
		{
			get
			{
				if (_bookBidSearchView == null)
				{
					_bookBidSearchView = new DataView(rocBookListBid.RocGridTable);
					_bookBidSearchView.Sort = "Price";
				}
				return _bookBidSearchView;
			}
		}

		private DataView _bookAskSearchView;
		private DataView bookAskSearchView
		{
			get
			{
				if (_bookAskSearchView == null)
				{
					_bookAskSearchView = new DataView(rocBookListAsk.RocGridTable);
					_bookAskSearchView.Sort = "Price";
				}
				return _bookAskSearchView;
			}
		}

		private DataView _bookBuySearchView;
		private DataView bookBuySearchView
		{
			get
			{
				if (_bookBuySearchView == null)
				{
					_bookBuySearchView = new DataView(rocBookListBid.RocGridTable);
					_bookBuySearchView.Sort = "Tag";
				}
				return _bookBuySearchView;
			}
		}

		private DataView _bookSellSearchView;
		private DataView bookSellSearchView
		{
			get
			{
				if (_bookSellSearchView == null)
				{
					_bookSellSearchView = new DataView(rocBookListAsk.RocGridTable);
					_bookSellSearchView.Sort = "Tag";
				}
				return _bookSellSearchView;
			}
		}

		private NumericUpDown _crrentPriceObj;

		private bool _updateBookBidAggregation = false;
		private bool _updateBookAskAggregation = false;

		private bool _orderLoaded = false;

		private string _currentSSFSymbol = "";
		private string _currentSSFSecType = "";
		private string _currentSSFExpiration = "";

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private List<ROCOrder> _rocOrders = new List<ROCOrder>();
		private Market _deltas = new Market();

		#endregion

		#region - Constructor -

		internal frmFutureTicket()
		{
			InitializeComponent();

			Initialize();
		}

		internal frmFutureTicket(string symbolDetail)
		{
			InitializeComponent();

			Initialize();

			CurrentSymbolDetail = symbolDetail;
		}

		internal void Initialize()
		{
			DDSymbolDetails = new DropDownSymbolDetails(this);
			DDAccountDropDown = new DropDownTraderAccount(this);

			QuickButtonSupprot = new QuickButtonSupport(this);
			QuickButtonSupprot.QuickButtonClicked += new QuickButtonClickedEventHandler(QuickButtonSupprot_QuickButtonClicked);

			CurrentSecInfo = new BaseSecurityInfo();

			BookDepthLimit = Configuration.ROC.Default.BookDepthLimit;

			_isLoadingValue = true;

			_crrentPriceObj = numLimitPrice;

			cboSymbolDetails.DataSource = CurrentSymbolDetails;
			cboSymbolDetails.DisplayMember = "SymbolDetail";

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

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			SizeChanged += new EventHandler(Form_SizeChanged);
			Activated += new EventHandler(From_Activated);

			panelManagerFuture.SelectedPanel = managedPanelBook;

			rocBookListBid.Initialize(ROCBookList.GridSides.Bid);
			rocBookListBid.PropertyChanged += new PropertyChangedEventHandler(rocBookList_PropertyChanged);

			rocBookListBid.MouseEnter += new EventHandler(rocBookListBid_MouseEnter);

			rocBookListAsk.Initialize(ROCBookList.GridSides.Ask);
			rocBookListAsk.PropertyChanged += new PropertyChangedEventHandler(rocBookList_PropertyChanged);

			rocBookListAsk.MouseEnter += new EventHandler(rocBookListAsk_MouseEnter);

			rocSSFList.Initialize();
			rocSSFList.PropertyChanged += new PropertyChangedEventHandler(rocSSFList_PropertyChanged);

			rocSSFList.MouseEnter += new EventHandler(rocSSFList_MouseEnter);

			_isLoadingValue = false;

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
			if (_lastIsBook && IsStock)
			{
				_lastIsBook = false;
				reloadUserAccounts = true;
			}
			else if (!_lastIsBook && IsFuture)
			{
				_lastIsBook = true;
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

			rocBookListBid.ScrollToFirstRow();
			rocBookListAsk.ScrollToFirstRow();

			CheckMarketData();
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

		private void Form_SizeChanged(object sender, EventArgs e)
		{
			_widthChanged = (_lastSize.Width != Width);
			_heightChanged = (_lastSize.Height != Height);

			if (_widthChanged || _heightChanged)
			{
				_lastSize = Size;
				if (_widthChanged)
				{
					FitWidth(false);
				}
				if (_heightChanged)
				{
					FitHeight();

					rocBookListAsk.Invalidate();
					rocBookListBid.Invalidate();
				}
			}
		}

		private void FitWidth(bool manual)
		{
			_widthChanged = false;

			int diff = MinimumSize.Width - Width;

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

			if (!gotSSFChain)
			{
				splitContainerBooks.SplitterDistance = (splitContainerBooks.Width - splitContainerBooks.SplitterWidth) / 2;

				lock (rocBookListBid)
				{
					SetColumnMode(rocBookListBid, DataGridViewAutoSizeColumnMode.Fill);
					SetColumnMode(rocBookListBid, DataGridViewAutoSizeColumnMode.None);
				}

				lock (rocBookListAsk)
				{
					SetColumnMode(rocBookListAsk, DataGridViewAutoSizeColumnMode.Fill);
					SetColumnMode(rocBookListAsk, DataGridViewAutoSizeColumnMode.None);
				}
			}
			else
			{
				if (manual)
				{
					lock (rocSSFList)
					{
						SetColumnMode(rocSSFList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocSSFList);
						SetColumnMode(rocSSFList, DataGridViewAutoSizeColumnMode.None);
					}
				}
			}
		}

		private void FitHeight()
		{
			_heightChanged = false;
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

			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void From_Activated(object sender, EventArgs e)
		{
			ActiveControl = cboSymbolDetails;
			cboSymbolDetails.Select();
		}

		#endregion

		#region - Grid Event -

		private void rocBookList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ShowToolTip":
					ShowGridTip(((ROCBookList)sender).ShowTip);
					break;
				case "SizeSelected":
					_crrentPriceObj.Value = Convert.ToDecimal(((ROCBookList)sender).PriceBook);
					//numQuantity.Value = Convert.ToDecimal(((ROCBookList)sender).SizeBook);
					numQuantity.Value = numQuantity.Increment;
					TogglePriceSelection();
					break;
				case "PriceSelected":
					_crrentPriceObj.Value = Convert.ToDecimal(((ROCBookList)sender).PriceBook);
					TogglePriceSelection();
					break;
				case "ShowOrderMenu":
					ShowOrderModification(((ROCBookList)sender).ShowOrderID);
					break;
			}
		}

		private void rocSSFList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "SizeSelected":
					_crrentPriceObj.Value = Convert.ToDecimal(((ROCSSFList)sender).PriceSSF);
					//numQuantity.Value = Convert.ToDecimal(((ROCSSFList)sender).SizeSSF);
					numQuantity.Value = numQuantity.Increment;
					TogglePriceSelection();
					break;
				case "PriceSelected":
					_crrentPriceObj.Value = Convert.ToDecimal(((ROCSSFList)sender).PriceSSF);
					TogglePriceSelection();
					break;
			}

			if (_currentSSFSymbol != ((ROCSSFList)sender).SymbolSSF)
			{
				_currentSSFSymbol = ((ROCSSFList)sender).SymbolSSF;
				_currentSSFSecType = ((ROCSSFList)sender).SecTypeSSF;
				_currentSSFExpiration = ((ROCSSFList)sender).ExpirationSSF;
				ValidateTicketValues();
			}
		}

		private void rocBookListBid_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusFutureTicket)
			{
				ActivateGrid(rocBookListBid);
			}
		}

		private void rocBookListAsk_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusFutureTicket)
			{
				ActivateGrid(rocBookListAsk);
			}
		}

		private void rocSSFList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusFutureTicket)
			{
				ActivateGrid(rocSSFList);
			}
		}

		#endregion

		#region - Grid Tip -

		private void ShowGridTip(string tip)
		{
			if (tip != null && tip != "")
			{
				UserTip.Show(tip, this, PointToClient(Cursor.Position), UserTipDuration);
			}
		}

		#endregion

		#region - Order Modification Menu -

		private menuBaseOrderModification _menuOrderModification;
		private PropertyContainer _menuOrderModificationContainer;
		private void ShowOrderModification(string orderID)
		{
			if (_menuOrderModification == null)
			{
				_menuOrderModification = new menuBaseOrderModification(
					orderID,
					numQuantity.Increment,
					0,
					numLimitPrice.Increment,
					numStopPrice.Increment,
					0.01M);
				_menuOrderModification.PropertyChanged += new PropertyChangedEventHandler(menuOrderModification_PropertyChanged);
			}

			if (_menuOrderModificationContainer == null)
			{
				_menuOrderModificationContainer = new PropertyContainer(_menuOrderModification);
				_menuOrderModificationContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOrderModificationContainer_Closed);
			}

			_menuOrderModificationContainer.Show(Cursor.Position);
		}

		private void menuOrderModification_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Close":
					break;
				case "Cancel":
					GLOBAL.HROM.CancelSingleOrder(_menuOrderModification.OrderID);
					break;
				case "Replace":
					GLOBAL.HROM.ReplaceOrder(_menuOrderModification.OrderID, _menuOrderModification.DeltaShare, _menuOrderModification.NewPrice, _menuOrderModification.NewStopPrice, _menuOrderModification.NewPegPrice, _menuOrderModification.NewDuration);
					break;
				case "CancelAll":
					GLOBAL.HROM.CancelAllBySide(_menuOrderModification.OrderID, _menuOrderModification.Side);
					break;
			}

			_menuOrderModificationContainer.Close();
		}

		private void menuOrderModificationContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuOrderModificationContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuOrderModificationContainer_Closed);
			_menuOrderModificationContainer = null;
			_menuOrderModification.PropertyChanged -= new PropertyChangedEventHandler(menuOrderModification_PropertyChanged);
			_menuOrderModification = null;
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
					if (CurrentSecInfo != null)
					{
						_crrentPriceObj.Value = Math.Round(Convert.ToDecimal(((LabelBase)sender).Value), CurrentSecInfo.DecimalPlaces);
					}
					else
					{
						_crrentPriceObj.Value = Convert.ToDecimal(((LabelBase)sender).Value);
					}
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
			_orderLoaded = false;
			_updateBookBidAggregation = false;
			_updateBookAskAggregation = false;
			
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
			if (IsFuture)
			{
				if (delta != null)
				{
					UpdateMarketDataL1(delta);
					UpdateMarketDataDeltaBook(delta);
				}
				InitialOrderLoad(symbolDetail);
			}
			else
			{
				if (delta != null)
				{
					UpdateMarketDataL1(delta);
				}
				UpdateMarketDataDeltaSSF(GLOBAL.HMarketData.Current);
			}
		}

		private void InitialOrderLoad(string symbolDetail)
		{
			_orderLoaded = true;
			UpdateOrders(symbolDetail, new Dictionary<string, ROCOrder>(GLOBAL.HOrders.RocItems));

			if (_updateBookBidAggregation)
			{
				UpdateAggregation(rocBookListBid, (double)dspBidPrice.Value, "Bid");
			}
			if (_updateBookAskAggregation)
			{
				UpdateAggregation(rocBookListAsk, (double)dspAskPrice.Value, "Ask");
			}
		}

		private void UpdateOrders(string symbolDetail, Dictionary<string, ROCOrder> orders)
		{
			foreach (ROCOrder order in orders.Values)
			{
				if (order.SymbolDetail == symbolDetail)
				{
					switch (order.Side)
					{
						case CSVEx.CSVFieldIDs.SideCodes.Buy:
							UpdateOrderBook(rocBookListBid, bookBuySearchView, order);
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Sell:
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							UpdateOrderBook(rocBookListAsk, bookSellSearchView, order);
							break;
						default:
							break;
					}
				}
			}
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
					_updateIM = updateIM;
					lock (_rocOrders)
					{
						_rocOrders.AddRange(orders);
					}
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

					if (IsFuture)
					{
						if (!_orderLoaded)
						{
							InitialOrderLoad(CurrentSymbolDetail);
						}
						else
						{
							if (orders.Count > 0)
							{
								UpdateOrders(CurrentSymbolDetail, orders);
							}
						}
					}

					if (!deltas.Empty)
					{
						try
						{
							Book delta = UpdateMarketData(deltas, AskPrice, BidPrice, HighPrice, LowPrice);
							if (IsFuture)
							{
								if (delta != null)
								{
									_updateBookBidAggregation = false;
									_updateBookAskAggregation = false;

									UpdateMarketDataL1(delta);
									UpdateMarketDataDeltaBook(delta);

									if (_updateBookBidAggregation && dspBidPrice.Value != null && dspBidPrice.Value.ToString() != "")
									{
										UpdateAggregation(rocBookListBid, (double)dspBidPrice.Value, "Bid");
									}
									if (_updateBookAskAggregation && dspAskPrice.Value != null && dspAskPrice.Value.ToString() != "")
									{
										UpdateAggregation(rocBookListAsk, (double)dspAskPrice.Value, "Ask");
									}
								}
							}
							else
							{
								if (delta != null)
								{
									UpdateMarketDataL1(delta);
								}
								UpdateMarketDataDeltaSSF(deltas);
							}
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		internal void UpdateTicketByProcessByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;
					if (_updateIM)
					{
						_updateIM = false;
						UpdateSecurityInfo();
					}

					if (IsFuture)
					{
						if (!_orderLoaded)
						{
							InitialOrderLoad(CurrentSymbolDetail);
						}
						else
						{
							List<ROCOrder> orders = new List<ROCOrder>();
							lock (_rocOrders)
							{
								if (_rocOrders.Count > 0)
								{
									orders = new List<ROCOrder>(_rocOrders);
									_rocOrders.Clear();
								}
							}
							if (orders.Count > 0)
							{
								UpdateOrders(CurrentSymbolDetail, orders);
							}
						}
					}

					Market deltas = Market.Replace(_deltas);
					if (!deltas.Empty)
					{
						try
						{
							Book delta = UpdateMarketData(deltas, AskPrice, BidPrice, HighPrice, LowPrice);
							if (IsFuture)
							{
								if (delta != null)
								{
									_updateBookBidAggregation = false;
									_updateBookAskAggregation = false;

									UpdateMarketDataL1(delta);
									UpdateMarketDataDeltaBook(delta);

									if (_updateBookBidAggregation && dspBidPrice.Value != null && dspBidPrice.Value.ToString() != "")
									{
										UpdateAggregation(rocBookListBid, (double)dspBidPrice.Value, "Bid");
									}
									if (_updateBookAskAggregation && dspAskPrice.Value != null && dspAskPrice.Value.ToString() != "")
									{
										UpdateAggregation(rocBookListAsk, (double)dspAskPrice.Value, "Ask");
									}
								}
							}
							else
							{
								if (delta != null)
								{
									UpdateMarketDataL1(delta);
								}
								UpdateMarketDataDeltaSSF(deltas);
							}
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
			_updatingUI = false;
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
						UpdateIMInfo(symbolDetail, CurrentSecInfo);

						if (!IsFuture && !gotSSFChain)
						{
							if (!GettingSSFChainSymbols.Contains(symbolDetail))
							{
								GLOBAL.HRDS.GetSSFutureChain(symbolDetail);
							}
						}
						else
						{
							if (!removeList.Contains(symbolDetail))
							{
								removeList.Add(symbolDetail);
							}
						}

						if (!GettingSSFChainSymbols.Contains(symbolDetail))
						{
							GettingSSFChainSymbols.Add(symbolDetail);
						}

						UpdateTicket();

						if (IsFuture)
						{
							// Replay data from memory
							Book delta = UpdateMarketData(GLOBAL.HMarketData.Current);
							if (delta != null)
							{
								UpdateMarketDataL1(delta);
								UpdateMarketDataDeltaBook(delta);
							}
						}
						else
						{
							UpdateMarketDataDeltaSSF(GLOBAL.HMarketData.Current);
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

		private void UpdateIMInfo(string _, BaseSecurityInfo secInfo)
		{
			HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Equity:
					if (secInfo.SSFChain.Count > 0)
					{
						foreach (var entry in secInfo.SSFChain)
							HelperSubscriber.SubscribeSSF(entry.Value.MDSymbol, entry.Value.MDSource, entry.Value.SecType);
					}
					break;
				case CSVFieldIDs.SecurityTypes.Future:
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

		private void UpdateOrders(string symbolDetail, List<ROCOrder> orders)
		{
			foreach (ROCOrder order in orders)
			{
				if (order.SymbolDetail == symbolDetail)
				{
					switch (order.Side)
					{
						case CSVEx.CSVFieldIDs.SideCodes.Buy:
							UpdateOrderBook(rocBookListBid, bookBuySearchView, order);
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Sell:
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							UpdateOrderBook(rocBookListAsk, bookSellSearchView, order);
							break;
						default:
							break;
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
				UpdateFutureTicketWithSecurityInfo(symbolDetail);
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
		private void UpdateFutureTicketWithSecurityInfo(string symbolDetail)
		{
			CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[symbolDetail] = CurrentSecInfo.MDSymbol;
			}
		}

		// Update with matching market data Level 1
		private void UpdateMarketDataL1(Book delta)
		{
			double price;
			long size;
			string text;

			lock (panelTicker)
			{
				//SRLabsPriceCheck(delta);

				panelTicker.SuspendLayout();

				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
				{
					dspBidPrice.Value = price;
					_updateBookBidAggregation = true;
				}

				if (delta.TryGetField(Book.FieldEnum.BidSize, out size))
				{
					dspBidSize.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
					dspAskPrice.Value = price;
					_updateBookAskAggregation = true;
				}

				if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
					dspAskSize.Value = size;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price)) {
					dspTradedPrice.Value = price;
					if (!HasFirstUpdate)
					{
						HasFirstUpdate = true;
						numLimitPrice.Value = Math.Round(Convert.ToDecimal(dspTradedPrice.Value), CurrentSecInfo.DecimalPlaces);
					}
				}

				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size)) {
					dspVolume.Value = size;
				}

				if (delta.TryGetNonEmpty(Book.FieldEnum.TradeTick, out text))
				{
					switch (text)
					{
						case "+":
						case "+0":
							picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Up];
							break;
						case "-":
						case "-0":
							picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Down];
							break;
						default:
							// Leave the last tick state
							//picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.None];
							break;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.TradeVolume, out size)) {
					dspTradeVolume.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.NetChange, out price)) {
					dspNetChange.Value = price;
					dspTradedPrice.ForeColor = dspNetChange.ForeColor;
				}

				if (delta.TryGetField(Book.FieldEnum.PctChange, out price)) {
					dspPctChange.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.HighPrice, out price)) {
					dspHighPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.LowPrice, out price)) {
					dspLowPrice.Value = price;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out price)) {
					dspClosePrice.Value = price;
				}
				else
				{
					switch (CurrentSecInfo.SecType)
					{
						case "F":
							if (delta.TryGetNonZero(Book.FieldEnum.SettlePrice, out price)) {
								dspClosePrice.Value = price;
							}
							else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
								dspClosePrice.Value = price;
							}
							break;
						default:
							if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
								dspClosePrice.Value = price;
							}
							else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
								dspClosePrice.Value = price;
							}
							break;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out text)) {
					switch (text) {
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
							lblSecurityStatus.Text = text;
							if (!lblSecurityStatus.Visible) {
								lblSecurityStatus.Visible = true;
								lblSecurityStatus.Blink = true;
							}
							break;
					}
				}

				panelTicker.ResumeLayout();
			}
		}

		private void UpdateMarketDataDeltaBook(Book delta)
		{
			if (!delta.Bids.Empty)
			{
				UpdateMarketDataDeltaBook(rocBookListBid, bookBidSearchView, delta.Bids, "Bid");
				_updateBookBidAggregation = true;
			}

			if (!delta.Asks.Empty)
			{
				UpdateMarketDataDeltaBook(rocBookListAsk, bookAskSearchView, delta.Asks, "Ask");
				_updateBookAskAggregation = true;
			}
		}

		private void UpdateMarketDataDeltaBook(ROCBookList grid, DataView searchView, QuoteCollection quotes, string side)
		{
			lock (grid.RocGridTable)
			{
				grid.SuspendLayout();

				DataRowView[] rows = new DataRowView[0];

				double currentPrice = 0;
				foreach (Quote quote in quotes) {
					currentPrice = Math.Round(quote.QuotePrice.Value, 7);

					if (searchView.Table.Rows.Count > 0)
					{
						rows = searchView.FindRows(currentPrice);
					}

					if (rows.Length == 0)
					{
						DataRow row = grid.RocGridTable.NewRow();

						row["Tag"] = "";

						row["TickSize"] = CurrentSecInfo.TickSize;
						row["DisplayFactor"] = 1;

						row["Price"] = currentPrice;
						row["Size"] = quote.Size;

						row["SecType"] = CurrentSecInfo.SecType;

						grid.RocGridTable.Rows.Add(row);
					}
					else
					{
						foreach (DataRowView row in rows)
						{
							if (row["Tag"].ToString() == "")
							{
								row["Size"] = quote.Size;
							}
						}
					}
				}

				grid.ResumeLayout();
			}
		}

		private void UpdateMarketDataDeltaSSF(Market deltas)
		{
			if (rocSSFList.GridKeys.Count > 0)
			{
				lock (rocSSFList.GridKeys)
				{
					foreach (string ssfSymbol in rocSSFList.GridKeys)
					{
						if (deltas.TryGet(ssfSymbol, out Book delta))
						{
							UpdateMarketDataDeltaSSF(ssfSymbol, delta);
						}
					}
				}
			}
		}

		private void UpdateMarketDataDeltaSSF(string ssfSymbol, Book delta)
		{
			double price;
			long size;

			DataRowView[] rows = rocSSFList.SearchView.FindRows(ssfSymbol);
			foreach (DataRowView row in rows)
			{
				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
				{
					row["BidPrice"] = price;
				}
				if (delta.TryGetField(Book.FieldEnum.BidSize, out size))
				{
					row["BidSize"] = size;
				}
				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
					row["AskPrice"] = price;
				}
				if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
					row["AskSize"] = size;
				}
				if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price)) {
					row["LastTraded"] = price;
				}
				if (delta.TryGetField(Book.FieldEnum.NetChange, out price)) {
					row["NetChange"] = price;
				}
				if (delta.TryGetField(Book.FieldEnum.PctChange, out price)) {
					row["PctChange"] = price;
				}
				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size)) {
					row["TotalVolume"] = size;
				}
			}
		}

		private void UpdateOrderBook(ROCBookList grid, DataView searchView, ROCOrder order)
		{
			grid.SuspendLayout();

			lock (grid)
			{
				DataRowView[] rows = new DataRowView[0];

				if (searchView.Table.Rows.Count > 0)
				{
					rows = searchView.FindRows(order.Tag);
				}

				if (rows.Length == 0)
				{
					DataRow row = grid.RocGridTable.NewRow();

					row["Tag"] = order.Tag;

					row["TickSize"] = CurrentSecInfo.TickSize;
					row["DisplayFactor"] = 1;

					row["Price"] = (double)order.Price;
					row["Size"] = (long)order.LeaveQty;

					row["OrderStatus"] = order.Status;
					row["SecType"] = CSVEx.CSVFieldIDs.SecurityTypes.Equity;

					grid.RocGridTable.Rows.Add(row);
				}
				else
				{
					foreach (DataRowView row in rows)
					{
						row["Price"] = (double)order.Price;
						row["Size"] = (long)order.LeaveQty;

						row["OrderStatus"] = order.Status;
					}
				}
			}
		}

		private void UpdateAggregation(ROCBookList grid, double price, string side)
		{
			DataRow[] rows = new DataRow[0];
			double limitPrice = 0;
			price = Math.Round(price, 7);

			// Reapply filter for Books
			try
			{
				lock (grid.RocGridTable)
				{
					if (price == 0)
					{
						// Not valid for books
						grid.RocGridTable.Clear();
					}
					else
					{
						if (side == "Bid")
						{
							rows = grid.RocGridTable.Select(grid.DefaultRowFilter + " And Price <= " + price);
						}
						else
						{
							rows = grid.RocGridTable.Select(grid.DefaultRowFilter + " And Price >= " + price);
						}

						if (rows.Length > BookDepthLimit)
						{
							if (side == "Bid")
							{
								limitPrice = (double)rows[rows.Length - BookDepthLimit]["Price"];
							}
							else
							{
								limitPrice = (double)rows[BookDepthLimit]["Price"];
							}
						}

						if (side == "Bid")
						{
							if (limitPrice > 0)
							{
								grid.RocGridTable.DefaultView.RowFilter = grid.DefaultRowFilter + " And Price <= " + price + " And Price >= " + limitPrice;
							}
							else
							{
								grid.RocGridTable.DefaultView.RowFilter = grid.DefaultRowFilter + " And Price <= " + price;
							}
						}
						else
						{
							if (limitPrice > 0)
							{
								grid.RocGridTable.DefaultView.RowFilter = grid.DefaultRowFilter + " And Price >= " + price + " And Price < " + limitPrice;
							}
							else
							{
								grid.RocGridTable.DefaultView.RowFilter = grid.DefaultRowFilter + " And Price >= " + price;
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

					switch (CurrentSecInfo.SecType)
					{
						case CSVFieldIDs.SecurityTypes.Equity:
							dspAskPrice.MaxDecimal = 2;
							dspBidPrice.MaxDecimal = 2;
							dspNetChange.MaxDecimal = 2;
							dspLowPrice.MaxDecimal = 2;
							dspHighPrice.MaxDecimal = 2;
							dspClosePrice.MaxDecimal = 2;
							dspTradedPrice.MaxDecimal = 2;

							numLimitPrice.DecimalPlaces = 2;
							numStopPrice.DecimalPlaces = 2;
							((DGVSSFListNumberColumn)rocSSFList.Columns["BidPrice"]).MaxDecimal = 2;
							((DGVSSFListNumberColumn)rocSSFList.Columns["AskPrice"]).MaxDecimal = 2;
							((DGVSSFListNumberColumn)rocSSFList.Columns["LastTraded"]).MaxDecimal = 2;
							break;
						default:
							dspAskPrice.MaxDecimal = 7;
							dspBidPrice.MaxDecimal = 7;
							dspNetChange.MaxDecimal = 7;
							dspLowPrice.MaxDecimal = 7;
							dspHighPrice.MaxDecimal = 7;
							dspClosePrice.MaxDecimal = 7;
							dspTradedPrice.MaxDecimal = 7;

							numLimitPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
							numStopPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
							((DGVBookListNumberColumn)rocBookListAsk.Columns["Price"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVBookListNumberColumn)rocBookListBid.Columns["Price"]).MaxDecimal = CurrentSecInfo.DecimalPlaces; 
							break;
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

					LoadSSFChain();
				}

				UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void LoadSSFChain()
		{
			if (!IsFuture && gotSSFChain)
			{
				panelManagerFuture.SelectedPanel = managedPanelSSF;
				MinimumSize = new Size(MinimumSize.Width, _defaultSSF);

				foreach (IMSSFutureInfo ssf in CurrentSecInfo.SSFChain.Values)
				{
					lock (rocSSFList.RocGridTable)
					{
						if (!rocSSFList.GridKeys.Contains(ssf.MDSymbol))
						{
							rocSSFList.GridKeys.Add(ssf.MDSymbol);

							DataRow row = rocSSFList.RocGridTable.NewRow();

							row["Symbol"] = ssf.MDSymbol;
							row["Expiration"] = ssf.ExpDate;
							row["TickSize"] = CurrentSecInfo.TickSize;
							row["DisplayFactor"] = 1;
							row["SecType"] = ssf.SecType;

							rocSSFList.RocGridTable.Rows.Add(row);
						}
						else
						{
							DataRowView[] rows = rocSSFList.SearchView.FindRows(ssf.MDSymbol);
							foreach (DataRowView row in rows)
							{
								row["Symbol"] = ssf.MDSymbol;
								row["Expiration"] = ssf.ExpDate;
								row["TickSize"] = CurrentSecInfo.TickSize;
								row["DisplayFactor"] = 1;
								row["SecType"] = ssf.SecType;
							}
						}
					}
				}

				FitWidth(true);
				Height = MinimumSize.Height;
			}
			else
			{
				panelManagerFuture.SelectedPanel = managedPanelBook;
				MinimumSize = new Size(MinimumSize.Width, _defaultBook);
				if (Width < MinimumSize.Width)
				{
					Width = MinimumSize.Width;
				}
			}
		}

		#endregion

		#region - Order Execution -

		private void QuickButtonSupprot_QuickButtonClicked(object sender, QuickButtonClickedEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.Data.secType)
			{
				case CSVFieldIDs.SecurityTypes.Future:
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
			if (_isInError)
			{
				ValidateTicketValues();
			}

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString());
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Future Buy Order", CurrentSymbolDetail));
			}
		}

		private void cmdSell_Click(object sender, EventArgs e)
		{
			if (_isInError)
			{
				ValidateTicketValues();
			}

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString());
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Future Sell Order", CurrentSymbolDetail));
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

					ocoOrderID = GLOBAL.HROM.RomMessageMaker.GetOrderID(GLOBAL.HROM.UserName);

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
				case CSVEx.CSVFieldIDs.OrderTypes.StopLimit:
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
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

			DestinationMap dest = new DestinationMap();
			AccountMap acct = new AccountMap();

			try
			{
				if (DDAccountDropDown.GetCurrentAccount(ref acct) && DDAccountDropDown.GetCurrentExchange(ref dest) && dest.destID != null)
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
					else
					{
						order.symbol = _currentSSFSymbol;
						order.mdSymbol = _currentSSFSymbol;
						order.secType = _currentSSFSecType;
						order.underlying = _currentSSFSymbol;
						order.expDate = _currentSSFExpiration;
					}
					order.tradeFor = DDAccountDropDown.CurrentTradeFor;
					order.trader = GLOBAL.HROM.UserName;
					order.shares = numQuantity.Value.ToString();
					order.price = limitPrice;
					order.stopPrice = stopPrice;
					order.side = side;
					order.orderType = orderType;
					order.duration = duration;
					order.exchangeID = dest.destID.ToString();
					order.cmtaFirmID = dest.cmtaFirmID;
					order.capacity = acct.capacity;
					order.clearingAcctID = acct.clearingAcID;
					order.clearingFirmID = acct.clearingFirmID;
					order.firmArc = acct.firmAcr;
					order.localAcctAcrn = acct.localAcAcrn;
					order.occAcct = acct.occAcr;
					if (order.homeExchange != null)
					{
						order.homeExchange = acct.homeExchange;
					}

					if (IsSpread)
					{
						order.cplxOrderType = "5";
					}

					GLOBAL.HROM.EnterOrder(order, true);
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

		#endregion

		#region - Ticket Display -

		private void ValidateTicketValues()
		{
			_isInError = false;

			if (!_isLoadingValue)
			{
				if (!IsFuture)
				{
					IsInError(_currentSSFSymbol == "");
					IsInError(_currentSSFExpiration == "");
				}
				IsInError(SetState(cmdTradeFor, (cmdTradeFor.Text == "N/A" || cmdTradeFor.Text == "")));
				IsInError(SetState(cmdAccount, (cmdAccount.Text == "N/A" || cmdAccount.Text == "")));
				IsInError(SetState(lblExchange, (cboExchange.Text == "")));
				SetPanelWidth(ref panelExchange, cboExchange);

				try
				{
					_orderTypesView.RowFilter = TicketSupport.BuildOrderTypeFilter(cboExchange,cboOrder);
					BuildDurationTypeFilter();

					SetTicketStateByOrderType();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				IsInError(SetState(lblQuantity, numQuantity.Value <= 0));
				IsInError(SetState(numQuantity, numQuantity.Value <= 0));
				SetPanelWidth(ref panelShares, numQuantity);

				if (panelLimitPrice.Visible)
				{
					if (!IsSpread)
					{
						IsInError(SetState(lblLimitPrice, numLimitPrice.Value <= 0));
						IsInError(SetState(numLimitPrice, numLimitPrice.Value <= 0));
					}
					else
					{
						IsInError(SetState(lblLimitPrice, false));
						IsInError(SetState(numLimitPrice, false));
					}

					SetPanelWidth(ref panelLimitPrice, numLimitPrice);
					SetPriceDisplay(PriceType.Limit, false);
				}

				if (panelStopPrice.Visible)
				{
					if (!IsSpread)
					{
						IsInError(SetState(lblStopPrice, numStopPrice.Value <= 0));
						IsInError(SetState(numStopPrice, numStopPrice.Value <= 0));
					}
					else
					{
						IsInError(SetState(lblStopPrice, false));
						IsInError(SetState(numStopPrice, false));
					}

					SetPanelWidth(ref panelStopPrice, numStopPrice);
					SetPriceDisplay(PriceType.Stop, false);
				}

				IsInError(SetState(lblOrderType, cboOrder.Text == ""));
				SetPanelWidth(ref panelOrderTypes, cboOrder);

				IsInError(SetState(lblDuration, cboDuration.Text == ""));
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

		private void IsInError(bool isError)
		{
			if (!_isInError)
			{
				_isInError = isError;
			}
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

			//string durationFilter = "";

			//if (cboOrder.Text != "MOO")
			//{
			//    durationFilter = "Duration <> 'OPG'";
			//}

			//_durationsView.RowFilter = durationFilter;
			//cboOrder.DataSource = _durationsView;
		}

		private void BuildOrderMsg()
		{
			if (_isInError)
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
				LongName = "Future Ticket";
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
			if (!IsFuture)
			{
				orderSymbol = _currentSSFSymbol;
			}

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
			_isLoadingValue = true;

			HasFirstUpdate = false;

			CurrentSecInfo = new BaseSecurityInfo();

			LongName = CurrentSecInfo.LongName;

			GettingSSFChainSymbols.Clear();

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

			_isLoadingValue = false;
			
			_currentSSFSymbol = "";

			if (rocBookListAsk.RocGridTable != null)
			{
				lock (rocBookListAsk.RocGridTable)
				{
					rocBookListAsk.RocGridTable.Rows.Clear();
					rocBookListAsk.RocGridTable.DefaultView.RowFilter = "";
					rocBookListAsk.Columns["Price"].HeaderText = "Price";
					rocBookListAsk.Columns["Size"].HeaderText = "Size";
				}

				((DGVBookListNumberColumn)rocBookListAsk.Columns["Price"]).MaxDecimal = 7;
			}

			if (rocBookListBid.RocGridTable != null)
			{
				lock (rocBookListBid.RocGridTable)
				{
					rocBookListBid.RocGridTable.Rows.Clear();
					rocBookListBid.RocGridTable.DefaultView.RowFilter = "";
					rocBookListBid.Columns["Price"].HeaderText = "Price";
					rocBookListBid.Columns["Size"].HeaderText = "Size";
				}

				((DGVBookListNumberColumn)rocBookListBid.Columns["Price"]).MaxDecimal = 7;
			}

			if (panelManagerFuture.SelectedPanel != managedPanelBook)
			{
				panelManagerFuture.SelectedPanel = managedPanelBook;
			}

			lock (rocSSFList.GridKeys)
			{
				rocSSFList.GridKeys.Clear();
			}

			lock (rocSSFList.RocGridTable)
			{
				rocSSFList.RocGridTable.Clear();
			}

			ValidateTicketValues();
		}

		// Used to load symbol defaults from last saved settings, else it loads the Ticket Default Value;
		private void UILoadSymbolDefaults(SymbolSettingData data)
		{
			_isLoadingValue = true;

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

			_isLoadingValue = false;

			ValidateTicketValues();
		}

		#endregion

		#region - Export & Import -

		internal string ExportBidGrid()
		{
			ROCBookListProfile prof = new ROCBookListProfile(rocBookListBid);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		internal void ImportBidGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCBookListProfile prof = (ROCBookListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCBookListProfile(rocBookListBid, prof);
				}
			}
		}

		internal string ExportAskGrid()
		{
			ROCBookListProfile prof = new ROCBookListProfile(rocBookListAsk);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		internal void ImportAskGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCBookListProfile prof = (ROCBookListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCBookListProfile(rocBookListAsk, prof);
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
			lock (GLOBAL.HWindows.FutureTicketWindows)
			{
				frmFutureTicket w = new frmFutureTicket();

				w.Width = this.Width;
				w.Height = this.Height;

				w.Clone(new ROCTicketProfile(this), new ROCBookListProfile(rocBookListBid), new ROCBookListProfile(rocBookListAsk));

				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}
		internal void Clone(ROCTicketProfile prof, ROCBookListProfile gridBidProf, ROCBookListProfile gridAskProf)
		{
			if (prof != null)
			{
				prof = new ROCTicketProfile(this, prof);
			}
			if (gridBidProf != null)
			{
				gridBidProf = new ROCBookListProfile(rocBookListBid, gridBidProf);
			}
			if (gridAskProf != null)
			{
				gridAskProf = new ROCBookListProfile(rocBookListAsk, gridAskProf);
			}
		}

		#endregion

		#region - Settings DropDown -

		private void itemSaveAsDefault_Click(object sender, EventArgs e)
		{
			SymbolSettingData data = GLOBAL.HSymbolSettingData.GetSymbolDefaults(CurrentSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Future);

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

			GLOBAL.HSymbolSettingData.SetSymbolDefaults(data);
		}

		private void itemResetToSystemDefault_Click(object sender, EventArgs e)
		{
			GLOBAL.HSymbolSettingData.RemoveSymbolDefault(CurrentSymbolDetail, CurrentSecInfo.SecType);
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CurrentSecInfo.SecType));
		}

		#endregion

		#region - Trader Account DropDown -

		//private DropDownTraderAccount _ddTraderAccount;
		//private DropDownTraderAccount ddAccountDropDown
		//{
		//    get
		//    {
		//        if (_ddTraderAccount == null)
		//        {
		//            _ddTraderAccount = new DropDownTraderAccount(this);
		//        }
		//        return _ddTraderAccount;
		//    }
		//}

		//internal string CurrentTradeFor
		//{
		//    get
		//    {
		//        return ddAccountDropDown.CurrentTradeFor;
		//    }
		//    set
		//    {
		//        ddAccountDropDown.CurrentTradeFor = value;
		//    }
		//}

		//internal string CurrentAccount
		//{
		//    get
		//    {
		//        return ddAccountDropDown.CurrentAccount;
		//    }
		//    set
		//    {
		//        ddAccountDropDown.CurrentAccount = value;
		//    }
		//}

		//internal string CurrentExchange
		//{
		//    get
		//    {
		//        return ddAccountDropDown.CurrentExchange;
		//    }
		//    set
		//    {
		//        ddAccountDropDown.CurrentExchange = value;
		//    }
		//}

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
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

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
	}
}