using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

using RDSEx;
using SerializationEx;
using LabelEx;
using ContextMenuEx;
using DataGridViewEx;
using ROMEx;
using CSVEx;
using MarketData;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	internal partial class frmStockTicket : StockSupport
	{
		#region - Local Variable -

		private const int _defaultLevel1 = 265;
		private const int _defaultLevel2 = 420;

		private int minimumHighOffset
		{
			get
			{
				int result = 0;

				try
				{
					if (panelCommandsEx.Visible)
					{
						result = result + 24;
					}
					if (DDAccountDropDown.CurrentDestinationInfo != null)
					{
						if (DDAccountDropDown.CurrentDestinationInfo.isAlgo || cboExchange.Text.ToUpper().Contains("_ALGO"))
						{
							result = result + 40;
						}
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				return result;
			}
		}

		private string _lastSymbolDetail = "";
		private string _lastLevel = "";

		private Size _lastSize = new Size();
		private bool _widthChanged = false;
		private bool _heightChanged = false;
		private bool _resizeGridToFit = false;

		private bool _isInError = false;
		private bool _isLoadingValue = true;
		internal bool IsLoadingValue
		{
			get
			{
				return _isLoadingValue;
			}
			set
			{
				_isLoadingValue = value;
			}
		}

		private DataView _orderTypesView;
		private DataView _durationsView;
		private DataView _algoTypesView;

		private DataView _level2BidSearchView;
		private DataView level2BidSearchView
		{
			get
			{
				if (_level2BidSearchView == null)
				{
					_level2BidSearchView = new DataView(rocLevel2ListBid.RocGridTable);
					_level2BidSearchView.Sort = "PartID";
				}
				return _level2BidSearchView;
			}
		}

		private DataView _level2AskSearchView;
		private DataView level2AskSearchView
		{
			get
			{
				if (_level2AskSearchView == null)
				{
					_level2AskSearchView = new DataView(rocLevel2ListAsk.RocGridTable);
					_level2AskSearchView.Sort = "PartID";
				}
				return _level2AskSearchView;
			}
		}

		private DataView _level2BuySearchView;
		private DataView level2BuySearchView
		{
			get
			{
				if (_level2BuySearchView == null)
				{
					_level2BuySearchView = new DataView(rocLevel2ListBid.RocGridTable);
					_level2BuySearchView.Sort = "Tag";
				}
				return _level2BuySearchView;
			}
		}

		private DataView _level2SellSearchView;
		private DataView level2SellSearchView
		{
			get
			{
				if (_level2SellSearchView == null)
				{
					_level2SellSearchView = new DataView(rocLevel2ListAsk.RocGridTable);
					_level2SellSearchView.Sort = "Tag";
				}
				return _level2SellSearchView;
			}
		}

		private NumericUpDown _crrentPriceObj;

		private bool _updateL2BidAggregation = false;
		private bool _updateL2AskAggregation = false;

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private List<ROCOrder> _rocOrders = new List<ROCOrder>();
		private Market _deltas = new Market();

		#endregion

		#region - Constructor -

		internal frmStockTicket()
		{
			Initialize("", false);
		}

		internal frmStockTicket(string symbolDetail)
		{
			Initialize(symbolDetail, false);
		}

		internal frmStockTicket(string symbolDetail, bool showLevel2)
		{
			Initialize(symbolDetail, showLevel2);
		}

		private void Initialize(string symbolDetail, bool showLevel2)
		{
			InitializeComponent();

			Initialize();

			if (symbolDetail != "")
			{
				CurrentSymbolDetail = symbolDetail;
			}

			if (showLevel2)
			{
				Level_Changed(itemLevel2, EventArgs.Empty);
			}
		}

		private void Initialize()
		{
			DDSymbolDetails = new DropDownSymbolDetails(this);
			DDAccountDropDown = new DropDownTraderAccount(this);

			QuickButtonSupprot = new QuickButtonSupport(this);
			QuickButtonSupprot.QuickButtonClicked += new QuickButtonClickedEventHandler(QuickButtonSupprot_QuickButtonClicked);

			CurrentSecInfo = new BaseSecurityInfo();

			_isLoadingValue = true;

			_crrentPriceObj = numLimitPrice;

			cboSymbolDetails.DataSource = CurrentSymbolDetails;
			cboSymbolDetails.DisplayMember = "SymbolDetail";

			cboShortLenders.DataSource = ShortLenders;
			cboShortLenders.DisplayMember = "ShortLender";

			cboInstructions.DataSource = Instructions;
			cboInstructions.DisplayMember = "Instruction";

			_orderTypesView = new DataView(GLOBAL.HOrders.StockOrderTypes);
			cboOrder.DataSource = _orderTypesView;
			cboOrder.DisplayMember = "Type";
			cboOrder.Text = "LIMIT";

			_durationsView = new DataView(GLOBAL.HOrders.StockDurations);
			cboDuration.DataSource = _durationsView;
			cboDuration.DisplayMember = "Duration";
			cboDuration.Text = "DAY";

			_algoTypesView = new DataView(GLOBAL.HOrders.StokAlgoTypes);
			cboAlgoType.DataSource = _algoTypesView;
			cboAlgoType.DisplayMember = "AlgoType";
			cboAlgoType.Text = "";

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

				dspPegPrice.MouseEnter += new EventHandler(dspPegPrice_MouseEnter);
				dspPegPrice.MouseWheel += new MouseEventHandler(dspPegPrice_MouseEnter);
			}

			SetPriceDisplay(PriceType.Limit, true);
			dspLimitPrice.LostFocus += new EventHandler(dspLimitPrice_Leave);

			SetPriceDisplay(PriceType.Stop, true);
			dspStopPrice.LostFocus += new EventHandler(dspStopPrice_Leave);

			SetPriceDisplay(PriceType.Peg, true);
			dspPegPrice.LostFocus += new EventHandler(dspPegPrice_Leave);

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			IconClicked += new EventHandler(Form_IconClicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(From_FormClosing);
			SizeChanged += new EventHandler(Form_SizeChanged);
			Activated += new EventHandler(Form_Activated);

			rocLevel2ListBid.Initialize(ROCLevel2List.GridSides.Bid);
			rocLevel2ListBid.PropertyChanged += new PropertyChangedEventHandler(rocLevel2List_PropertyChanged);

			rocLevel2ListBid.MouseEnter += new EventHandler(rocLevel2ListBid_MouseEnter);
			rocLevel2ListBid.MouseUp += new MouseEventHandler(rocLevel2ListBid_MouseUp);
			rocLevel2ListBid.MouseMove += new MouseEventHandler(rocLevel2ListBid_MouseMove);

			rocLevel2ListAsk.Initialize(ROCLevel2List.GridSides.Ask);
			rocLevel2ListAsk.PropertyChanged += new PropertyChangedEventHandler(rocLevel2List_PropertyChanged);

			rocLevel2ListAsk.MouseEnter += new EventHandler(rocLevel2ListAsk_MouseEnter);
			rocLevel2ListAsk.MouseUp += new MouseEventHandler(rocLevel2ListAsk_MouseUp);
			rocLevel2ListAsk.MouseMove += new MouseEventHandler(rocLevel2ListAsk_MouseMove);

			_isLoadingValue = false;

			cboSymbolDetails.LostFocus += new EventHandler(cboSymbolDetails_LostFocus);
			cboSymbolDetails.SelectedValueChanged += new EventHandler(cboSymbolDetails_LostFocus);

			SetTicketLevel(true);

			rocLevel2ListBid.PairDGV = rocLevel2ListAsk;
			rocLevel2ListAsk.PairDGV = rocLevel2ListBid;

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

			if (chkShow.BackColor != BackColor)
			{
				chkShow.BackColor = panelOrderInfo.BackColor;
			}

			if (chkInstructions.BackColor != BackColor)
			{
				chkInstructions.BackColor = panelOrderInfo.BackColor;
			}

			//if (panelCommands.Enabled == _isInError)
			//{
			//    panelCommands.Enabled = !_isInError;
			//}

			panelCommands.Enabled = (CurrentSymbolDetail != "");
			panelCommandsEx.Enabled = (CurrentSymbolDetail != "");

			if (_lastSymbolDetail != CurrentSymbolDetail || _lastLevel != CurrentLevel)
			{
				_lastSymbolDetail = "";
				ResetTicketValues();

				if (_lastLevel != CurrentLevel)
				{
					_lastLevel = CurrentLevel;
					DDSymbolDetails.NewSymbolDetail = true;
				}

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

			rocLevel2ListBid.ScrollToFirstRow();
			rocLevel2ListAsk.ScrollToFirstRow();

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
					//GLOBAL.HROC.AddToStatusLogs("Form_SizeChanged::_heightChanged::StockWindow: " + LongName + " " + Height);
					FitHeight();
				}
			}
		}

		private void FitWidth(bool manual)
		{
			_widthChanged = false;
			_resizeGridToFit = manual;

			//if (_resizeGridToFit)
			//{
			//    lock (rocLevel2ListBid)
			//    {
			//        SetColumnMode(rocLevel2ListBid, DataGridViewAutoSizeColumnMode.DisplayedCells);
			//    }

			//    lock (rocLevel2ListAsk)
			//    {
			//        SetColumnMode(rocLevel2ListAsk, DataGridViewAutoSizeColumnMode.DisplayedCells);
			//    }

			//    ResizeToFit(splitContainerLevel2);
			//}

			int diff = Width - MinimumSize.Width;

			//int deltaUpper = diff / 5;

			//panelShares.Width = panelShares.MinimumSize.Width + deltaUpper;
			//panelOrderTypes.Width = panelOrderTypes.MinimumSize.Width + deltaUpper;
			//panelLimitPrice.Width = panelLimitPrice.MinimumSize.Width + deltaUpper;
			//panelStopPrice.Width = panelStopPrice.MinimumSize.Width + deltaUpper;

			//int deltaLower = diff / 5;

			//panelShow.Width = panelShow.MinimumSize.Width + deltaLower;
			//panelExchange.Width = panelExchange.MinimumSize.Width + deltaLower;
			//panelShortLender.Width = panelShortLender.MinimumSize.Width + deltaLower;
			//panelAlgoType.Width = panelAlgoType.MinimumSize.Width + deltaLower;

			int deltaCommands = diff / 3;

			cmdBuy.Width = cmdBuy.MinimumSize.Width + deltaCommands;
			cmdSell.Width = cmdSell.MinimumSize.Width + deltaCommands;

			splitContainerLevel2.SplitterDistance = (splitContainerLevel2.Width - splitContainerLevel2.SplitterWidth) / 2;

			lock (rocLevel2ListBid)
			{
				SetColumnMode(rocLevel2ListBid, DataGridViewAutoSizeColumnMode.Fill);
				SetColumnMode(rocLevel2ListBid, DataGridViewAutoSizeColumnMode.None);
			}

			lock (rocLevel2ListAsk)
			{
				SetColumnMode(rocLevel2ListAsk, DataGridViewAutoSizeColumnMode.Fill);
				SetColumnMode(rocLevel2ListAsk, DataGridViewAutoSizeColumnMode.None);
			}
		}

		private void FitHeight()
		{
			_heightChanged = false;
		}

		private void Form_Load(object sender, EventArgs e)
		{
			DDAccountDropDown.LoadUserAccounts();
		}

		private void From_FormClosing(object sender, FormClosingEventArgs e)
		{
			StopTimer();

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

		private void rocLevel2List_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ShowToolTip":
					ShowGridTip(((ROCLevel2List)sender).ShowTip);
					break;
				//case "SizeL2Selected":
				//    _crrentPriceObj.Value = Convert.ToDecimal(((ROCLevel2List)sender).PriceL2);
				//    numQuantity.Value = Convert.ToDecimal(((ROCLevel2List)sender).SizeL2) * 100M;

				//    TogglePriceSelection();
				//    break;
				case "PriceL2Selected":
					_crrentPriceObj.Value = Convert.ToDecimal(((ROCLevel2List)sender).PriceL2);
					TogglePriceSelection();
					break;
				case "ShowOrderMenu":
					ShowOrderModification(((ROCLevel2List)sender).ShowOrderID);
					break;
				case "ReplaceOrderNow":
					ROCLevel2List grid = (ROCLevel2List)sender;

					if (grid.WorkingOrderID != "")
					{
						if (grid.NewPrice != grid.OrgPrice)
						{
							GLOBAL.OrderManagers.ReplaceOrder(grid.WorkingOrderID, grid.NewPrice.ToString("F2"));
						}
						else
						{
							GLOBAL.OrderManagers.CancelSingleOrder(grid.WorkingOrderID);
						}
						rocLevel2ListBid.ReplaceOrderCleanUp();
						rocLevel2ListAsk.ReplaceOrderCleanUp();
					}
					break;
			}
		}

		private void rocLevel2ListBid_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusStockTicket)
			{
				ActivateGrid(rocLevel2ListBid);
			}
		}

		private void rocLevel2ListBid_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				if (rocLevel2ListAsk.PairDGV.WorkingOrderID != "")
				{
					int rowIndex = rocLevel2ListAsk.GetRowIndex(Cursor.Position);
					if (rowIndex > -1)
					{
						if (rocLevel2ListAsk.Rows[rowIndex].Cells["Price"].Value != null)
						{
							double newPrice = (double)rocLevel2ListAsk.Rows[rowIndex].Cells["Price"].Value;
							if (rocLevel2ListAsk.PairDGV.OrgPrice != newPrice)
							{
								GLOBAL.OrderManagers.ReplaceOrder(rocLevel2ListAsk.PairDGV.WorkingOrderID, newPrice.ToString("F2"));
							}
							else
							{
								GLOBAL.OrderManagers.CancelSingleOrder(rocLevel2ListAsk.PairDGV.WorkingOrderID);
							}
						}
					}
					rocLevel2ListBid.ReplaceOrderCleanUp();
					rocLevel2ListAsk.ReplaceOrderCleanUp();
				}
				EndDrag();
			}
		}

		private void rocLevel2ListBid_MouseMove(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				Drag(rocLevel2ListBid, e);
			}
		}

		private void rocLevel2ListAsk_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusStockTicket)
			{
				ActivateGrid(rocLevel2ListAsk);
			}
		}

		private void rocLevel2ListAsk_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				if (rocLevel2ListBid.PairDGV.WorkingOrderID != "")
				{
					int rowIndex = rocLevel2ListBid.GetRowIndex(Cursor.Position);
					if (rowIndex > -1)
					{
						if (rocLevel2ListBid.Rows[rowIndex].Cells["Price"].Value != null)
						{
							double newPrice = (double)rocLevel2ListBid.Rows[rowIndex].Cells["Price"].Value;
							if (rocLevel2ListBid.PairDGV.OrgPrice != newPrice)
							{
								GLOBAL.OrderManagers.ReplaceOrder(rocLevel2ListBid.PairDGV.WorkingOrderID, newPrice.ToString("F2"));
							}
							else
							{
								GLOBAL.OrderManagers.CancelSingleOrder(rocLevel2ListBid.PairDGV.WorkingOrderID);
							}
						}
					}
					rocLevel2ListBid.ReplaceOrderCleanUp();
					rocLevel2ListAsk.ReplaceOrderCleanUp();
				}
				EndDrag();
			}
		}

		private void rocLevel2ListAsk_MouseMove(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				Drag(rocLevel2ListAsk, e);
			}
		}

		#endregion

		#region - Grid Drag Tip -

		private dndStock _dnd = null;
		private void Drag(ROCLevel2List list, MouseEventArgs e)
		{
			if (_dnd == null)
			{
				Color backcolor = list.BidColor;
				if (list.Name.Contains("Ask"))
				{
					backcolor = list.AskColor;
				}

				double price;
				long size;
				if (list.PairDGV.WorkingOrderID != "")
				{
					price = list.PairDGV.OrgPrice;
					size = list.PairDGV.OrgSize;
				}
				else if (list.WorkingOrderID != "")
				{
					price = list.OrgPrice;
					size = list.OrgSize;
				}
				else
				{
					EndDrag();
					return;
				}

				_dnd = new dndStock(backcolor, price, size);
				_dnd.Show();
			}

			Point s = list.PointToScreen(new Point(e.X, e.Y));
			_dnd.Top = s.Y + 10;
			_dnd.Left = s.X + 10;
		}

		private void EndDrag()
		{
			if (_dnd != null)
			{
				_dnd.Hide();
			}
			_dnd = null;
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
					numShow.Increment,
					numLimitPrice.Increment,
					numStopPrice.Increment,
					numPegPrice.Increment);
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
					GLOBAL.OrderManagers.CancelSingleOrder(_menuOrderModification.OrderID);
					break;
				case "Replace":
					GLOBAL.OrderManagers.ReplaceOrder(_menuOrderModification.OrderID, _menuOrderModification.DeltaShare, _menuOrderModification.NewPrice, _menuOrderModification.NewStopPrice, _menuOrderModification.NewPegPrice, _menuOrderModification.NewDuration);
					break;
				case "CancelAll":
					GLOBAL.OrderManagers.CancelAllBySide(_menuOrderModification.OrderID, _menuOrderModification.Side);
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
						_crrentPriceObj.Value = Math.Round(Convert.ToDecimal(((LabelBase)sender).Value), 2);
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

		private void AlgoType_SelectedIndexChanged(object sender, EventArgs e)
		{
			//if (cboAlgoType.Text != "" && panelAlgo.Visible)
			//{
			//    if (cboOrder.Text != "MARKET")
			//    {
			//        cboOrder.Text = "MARKET";
			//        return;
			//    }
			//}
			//else
			//{
			//    if (cboOrder.Text != "LIMIT")
			//    {
			//        cboOrder.Text = "LIMIT";
			//        return;
			//    }
			//}

			//ValidateTicketValues();
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

		private void dspStopPrice_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			ActiveControl = numStopPrice;
		}

		//private void dspStopPrice_Leave(object sender, MouseEventArgs e)
		//{
		//    GLOBAL.Interrupt = true;
		//    SetPriceDisplayFromUserInput(PriceType.Stop);
		//}

		private void dspPegPrice_KeyDown(object sender, KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyData)
			{
				case Keys.Enter:
				case Keys.Tab:
					SetPriceDisplayFromUserInput(PriceType.Peg);
					break;
			}
		}

		private void dspPegPrice_Leave(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			SetPriceDisplayFromUserInput(PriceType.Peg);
		}

		//private void dspPegPrice_Leave(object sender, MouseEventArgs e)
		//{
		//    GLOBAL.Interrupt = true;
		//    SetPriceDisplayFromUserInput(PriceType.Peg);
		//}

		private void dspPegPrice_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			ActiveControl = numPegPrice;
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
						else if (panelPegPrice.Visible)
						{
							ActiveControl = dspPegPrice;
							dspPegPrice.Select();
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
						else if (panelPegPrice.Visible)
						{
							ActiveControl = dspPegPrice;
							dspPegPrice.Select();
						}
						else
						{
							ActiveControl = cboSymbolDetails;
							cboSymbolDetails.Select();
						}
						break;
					case "numStopPrice":
						if (panelPegPrice.Visible)
						{
							ActiveControl = dspPegPrice;
							dspPegPrice.Select();
						}
						else
						{
							ActiveControl = cboSymbolDetails;
							cboSymbolDetails.Select();
						}
						break;
					case "numShow":
					case "cboDuration":
					case "cboExchange":
					case "numPegPrice":
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
			if (symbolDetail != "")
			{
				_updateL2BidAggregation = false;
				_updateL2AskAggregation = false;

				UpdateSecurityInfo(symbolDetail);

				UpdateMarketDataDeltas(symbolDetail, GLOBAL.HMarketData.Current);

				if (IsLevel2)
				{
					UpdateOrders(symbolDetail, new Dictionary<string, ROCOrder>(GLOBAL.HOrders.RocItems));

					if (_updateL2BidAggregation)
					{
						UpdateAggregation(rocLevel2ListBid, (double)dspBidPrice.Value, "Bid");
					}
					if (_updateL2AskAggregation)
					{
						UpdateAggregation(rocLevel2ListAsk, (double)dspAskPrice.Value, "Ask");
					}
				}
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
							UpdateOrderLevel2(rocLevel2ListBid, level2BuySearchView, order);
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Sell:
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							UpdateOrderLevel2(rocLevel2ListAsk, level2SellSearchView, order);
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
					if (IsLevel2)
					{
						lock (_rocOrders)
						{
							_rocOrders.AddRange(orders);
						}
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
					BeginInvoke(new UpdateTicketByProcessDelegate(UpdateTicketByProcess), updateIM, orders, deltas);
					return;
				}
				try
				{
					if (updateIM)
					{
						UpdateSecurityInfo();
					}
					if (!deltas.Empty)
					{
						_updateL2BidAggregation = false;
						_updateL2AskAggregation = false;

						UpdateMarketDataDeltas(CurrentSymbolDetail, deltas);

						if (IsLevel2)
						{
							if (_updateL2BidAggregation && dspBidPrice.Value != null && dspBidPrice.Value.ToString() != "")
							{
								UpdateAggregation(rocLevel2ListBid, (double)dspBidPrice.Value, "Bid");
							}
							if (_updateL2AskAggregation && dspAskPrice.Value != null && dspAskPrice.Value.ToString() != "")
							{
								UpdateAggregation(rocLevel2ListAsk, (double)dspAskPrice.Value, "Ask");
							}
						}
					}
					if (IsLevel2)
					{
						if (orders.Count > 0)
						{
							UpdateOrders(CurrentSymbolDetail, orders);
						}
					}
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
				if (!_updatingUI)
				{
					_updatingUI = true;
					if (_updateIM)
					{
						_updateIM = false;
						UpdateSecurityInfo();
					}

					Market deltas = _deltas.Release();
					if (!deltas.Empty)
					{
						_updateL2BidAggregation = false;
						_updateL2AskAggregation = false;

						UpdateMarketDataDeltas(CurrentSymbolDetail, deltas);

						if (IsLevel2)
						{
							if (_updateL2BidAggregation && dspBidPrice.Value != null && dspBidPrice.Value.ToString() != "")
							{
								UpdateAggregation(rocLevel2ListBid, (double)dspBidPrice.Value, "Bid");
							}
							if (_updateL2AskAggregation && dspAskPrice.Value != null && dspAskPrice.Value.ToString() != "")
							{
								UpdateAggregation(rocLevel2ListAsk, (double)dspAskPrice.Value, "Ask");
							}
						}
					}
					if (IsLevel2)
					{
						List<ROCOrder> orders = new List<ROCOrder>();
						lock (_rocOrders)
						{
							if (_rocOrders.Count > 0)
							{
								orders = new List<ROCOrder>(orders);
								_rocOrders.Clear();
							}
						}
						if (orders.Count > 0)
						{
							UpdateOrders(CurrentSymbolDetail, orders);
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
			if (ImSymbolNeeded.Count > 0)
			{
				Dictionary<string, string> lcoImSymbolNeeded;

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

						if (!removeList.Contains(symbolDetail))
						{
							removeList.Add(symbolDetail);
						}

						UpdateTicket();
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

					if (IsLevel2)
					{
						HelperSubscriber.SubscribeL2(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
					}

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
							UpdateOrderLevel2(rocLevel2ListBid, level2BuySearchView, order);
							break;
						case CSVEx.CSVFieldIDs.SideCodes.Sell:
						case CSVEx.CSVFieldIDs.SideCodes.Short:
							UpdateOrderLevel2(rocLevel2ListAsk, level2SellSearchView, order);
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
			UpdateStockTicketWithSecurityInfo(symbolDetail);

			GLOBAL.HRDS.GetSecurityByTicker(symbolDetail);
		}

		// Update with Security Info On Play back & onLoad
		private void UpdateStockTicketWithSecurityInfo(string symbolDetail)
		{
			CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[symbolDetail] = CurrentSecInfo.MDSymbol;
			}
		}

		// Update with Security Info On Play back & onProcess
		private void UpdateMarketDataDeltas(string symbolDetail, Market deltas)
		{
			// Update Level 1
			if (deltas.TryGet(symbolDetail, out Book book))
			{
				UpdateMarketDataDelta(book);
			}

			if (IsLevel2)
			{
				// Update Level 2
				string symbolDetailL2 = "";
				if (CurrentSecInfo != null)
				{
					switch (CurrentSecInfo.MDSource.ToUpper())
					{
						case "CTA":
							foreach (string key in Constants.L2PartipcantCodeCTA.Codes)
							{
								symbolDetailL2 = symbolDetail + "." + key;
								if (deltas.TryGet(symbolDetailL2, out Book found))
								{
									UpdateMarketDataDeltaLevel2(found);
								}
							}
							break;
						case "NASDAQ":
							foreach (string key in Constants.L2PartipcantCodeNasdaq.Codes)
							{
								symbolDetailL2 = symbolDetail + "." + key;
								if (deltas.TryGet(symbolDetailL2, out Book found))
								{
									UpdateMarketDataDeltaLevel2(found);
								}
							}
							break;
					}
				}
			}
		}

		// Update with matching market data Level 1
		private void UpdateMarketDataDelta(Book delta)
		{
			double dval;
			long lval;
			string sval;

			lock (panelTicker)
			{
				panelTicker.SuspendLayout();

				if (delta.TryGetField(Book.FieldEnum.BidPrice, out dval))
				{
					dspBidPrice.Value = dval;
					_updateL2BidAggregation = true;
				}

				if (delta.TryGetField(Book.FieldEnum.BidSize, out lval))
				{
					dspBidSize.Value = lval;
					_updateL2BidAggregation = true;
				}

				if (delta.TryGetField(Book.FieldEnum.AskPrice, out dval)) {
					dspAskPrice.Value = dval;
					_updateL2AskAggregation = true;
				}

				if (delta.TryGetField(Book.FieldEnum.AskSize, out lval)) {
					dspAskSize.Value = lval * 100;
					_updateL2AskAggregation = true;
				}

				if (delta.TryGetField(Book.FieldEnum.TradePrice, out dval)) {
					dspTradedPrice.Value = dval;
					if (!HasFirstUpdate)
					{
						HasFirstUpdate = true;
						numLimitPrice.Value = Math.Round(Convert.ToDecimal(dspTradedPrice.Value), CurrentSecInfo.DecimalPlaces);
					}
				}
				//else
				//{
				//    if (delta.ClosePrice != null && delta.ClosePrice != 0)
				//    {
				//        dspTradedPrice.Value = (double)delta.ClosePrice;
				//    }
				//    else if (delta.PrevClosePrice != null && delta.PrevClosePrice != 0)
				//    {
				//        dspTradedPrice.Value = (double)delta.PrevClosePrice;
				//    }
				//}

				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out lval)) {
					dspVolume.Value = lval;
				}

				if (delta.TryGetNonEmpty(Book.FieldEnum.TradeTick, out sval))
				{
					switch (sval)
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

				if (delta.TryGetField(Book.FieldEnum.TradeVolume, out lval))
				{
					dspTradeVolume.Value = lval;
				}

				if (delta.TryGetField(Book.FieldEnum.NetChange, out dval))
				{
					dspNetChange.Value = dval;
					dspTradedPrice.ForeColor = dspNetChange.ForeColor;
				}

				if (delta.TryGetField(Book.FieldEnum.PctChange, out dval)) {
					dspPctChange.Value = dval;
				}

				if (delta.TryGetField(Book.FieldEnum.HighPrice, out dval)) {
					dspHighPrice.Value = dval;
				}

				if (delta.TryGetField(Book.FieldEnum.LowPrice, out dval)) {
					dspLowPrice.Value = dval;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out dval)) {
					dspClosePrice.Value = dval;
				}
				else
				{
					if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out dval)) {
						dspClosePrice.Value = dval;
					}
					else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out dval)) {
						dspClosePrice.Value = dval;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out sval)) {
					switch (sval.ToLower()) {
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
							lblSecurityStatus.Text = sval;
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

		private void UpdateMarketDataDeltaLevel2(Book delta)
		{
			if (delta.TryGetField(Book.FieldEnum.BidPrice, out double _) || delta.TryGetField(Book.FieldEnum.BidSize, out long _))
			{
				UpdateMarketDataDeltaLevel2(rocLevel2ListBid, level2BidSearchView, delta, "Bid");
				_updateL2BidAggregation = true;
			}
			if (delta.TryGetField(Book.FieldEnum.AskPrice, out double _) || delta.TryGetField(Book.FieldEnum.AskSize, out long _))
			{
				UpdateMarketDataDeltaLevel2(rocLevel2ListAsk, level2AskSearchView, delta, "Ask");
				_updateL2AskAggregation = true;
			}
		}

		private void UpdateMarketDataDeltaLevel2(ROCLevel2List grid, DataView searchView, Book delta, string side)
		{
			lock (grid.RocGridTable)
			{
				grid.SuspendLayout();

				DataRowView[] rows = null;
				string participantSymbol = "", text;

				if (searchView.Table.Rows.Count > 0)
				{
					if (delta.TryGetNonEmpty(Book.FieldEnum.uParticipantSymbol, out participantSymbol))
						rows = searchView.FindRows(participantSymbol);
				}

				if ((rows != null) && (rows.GetLength(0) > 0))
				{
					DataRow row = grid.RocGridTable.NewRow();

					if (delta.TryGetField(Book.FieldEnum.uSymbol, out text))
						row["Symbol"] = text;

					if (delta.TryGetField(Book.FieldEnum.IssueSymbol, out text))
						row["SymbolDetail"] = text;

					row["Tag"] = "";
					row["PartID"] = participantSymbol;

					if (CurrentSecInfo != null)
					{
						string name;
						switch (CurrentSecInfo.MDSource.ToUpper())
						{
							case "CTA":
								row["PartName"] = Constants.L2PartipcantCodeCTA.TryGetName(participantSymbol, out name) ? name : "";
								break;
							case "NASDAQ":
								row["PartName"] = Constants.L2PartipcantCodeNasdaq.TryGetName(participantSymbol, out name) ? name : "";
								break;
						}
					}

					row["TickSize"] = CurrentSecInfo.TickSize;
					row["DisplayFactor"] = 1;

					if (side == "Ask")
					{
						if (delta.TryGetField(Book.FieldEnum.AskPrice, out double price))
						{
							row["Price"] = price;
						}
						if (delta.TryGetField(Book.FieldEnum.AskSize, out long size))
						{
							row["Size"] = size;
							row["RealSize"] = size * 100;
						}
					}
					else
					{
						if (delta.TryGetField(Book.FieldEnum.BidPrice, out double price))
						{
							row["Price"] = price;
						}
						if (delta.TryGetField(Book.FieldEnum.BidSize, out long size))
						{
							row["Size"] = size;
							row["RealSize"] = size * 100;
						}
					}

					row["SecType"] = CSVEx.CSVFieldIDs.SecurityTypes.Equity;

					grid.RocGridTable.Rows.Add(row);
				}
				else
				{
					double price;
					long size;

					foreach (DataRowView row in rows)
					{
						if (row["Tag"].ToString() == "")
						{
							if (side == "Ask")
							{
								if (delta.TryGetField(Book.FieldEnum.AskPrice, out price))
								{
									row["Price"] = price;
								}
								if (delta.TryGetField(Book.FieldEnum.AskSize, out size))
								{
									row["Size"] = size;
									row["RealSize"] = size * 100;
								}
							}
							else
							{
								if (delta.TryGetField(Book.FieldEnum.BidPrice, out price)) {
									row["Price"] = price;
								}
								if (delta.TryGetField(Book.FieldEnum.BidSize, out size)) {
									row["Size"] = size;
									row["RealSize"] = size * 100;
								}
							}
						}
					}
				}

				grid.ResumeLayout();
			}
		}

		private void UpdateOrderLevel2(ROCLevel2List grid, DataView searchView, ROCOrder order)
		{
			lock (grid.RocGridTable)
			{
				grid.SuspendLayout();

				DataRowView[] rows = new DataRowView[0];

				if (searchView.Table.Rows.Count > 0)
				{
					rows = searchView.FindRows(order.Tag);
				}

				if (rows.Length == 0)
				{
					DataRow row = grid.RocGridTable.NewRow();

					row["Symbol"] = order.Symbol;
					row["SymbolDetail"] = order.SymbolDetail;

					row["Tag"] = order.Tag;
					row["PartID"] = "*";

					row["TickSize"] = CurrentSecInfo.TickSize;
					row["DisplayFactor"] = 1;

					row["Price"] = (double)order.OrderPrice.Value;
					row["Size"] = order.LeaveQty;
					row["RealSize"] = order.LeaveQty;

					row["OrderStatus"] = order.Status;
					row["SecType"] = CSVEx.CSVFieldIDs.SecurityTypes.Equity;

					grid.RocGridTable.Rows.Add(row);
				}
				else
				{
					foreach (DataRowView row in rows)
					{
						row["Price"] = (double)order.OrderPrice.Value;
						row["Size"] = order.LeaveQty;
						row["RealSize"] = order.LeaveQty;

						row["OrderStatus"] = order.Status;
					}
				}

				grid.ResumeLayout();
			}
		}

		private void UpdateAggregation(ROCLevel2List grid, double price, string side)
		{
			// Reapply filter for L2
			grid.SuspendLayout();

			try
			{
				if (side == "Bid")
				{
					grid.RocGridTable.DefaultView.RowFilter = grid.DefaultRowFilter + " And Price <= " + price;
				}
				else
				{
					grid.RocGridTable.DefaultView.RowFilter = grid.DefaultRowFilter + " And Price >= " + price;
				}

				if (grid.RocGridTable.Rows.Count > 0)
				{
					grid.Columns["Price"].HeaderText = price.ToString("N2");
					grid.Columns["Size"].HeaderText = grid.GetTotalSizeAtPrice(price).ToString("N0");
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			grid.ResumeLayout();
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
							numPegPrice.DecimalPlaces = 2;
							((DGVLevel2ListNumberColumn)rocLevel2ListBid.Columns["Price"]).MaxDecimal = 2;
							((DGVLevel2ListNumberColumn)rocLevel2ListAsk.Columns["Price"]).MaxDecimal = 2;
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
							numPegPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
							((DGVLevel2ListNumberColumn)rocLevel2ListBid.Columns["Price"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVLevel2ListNumberColumn)rocLevel2ListAsk.Columns["Price"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
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

					numPegPrice.Increment = CurrentSecInfo.TickSizeDec;
					dspPegPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspPegPriceTickSize.Value = CurrentSecInfo.TickSize;
				}

				UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Equity));
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Equity));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Order Execution -

		private void QuickButtonSupprot_QuickButtonClicked(object sender, QuickButtonClickedEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.Data.secType)
			{
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

					if (e.Data.shortLocate != "")
					{
						cboShortLenders.Text = e.Data.shortLocate;
						cboShortLenders_KeyUp(null, new KeyEventArgs(Keys.Enter));
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
								numLimitPrice.Value = Math.Round(price, 2);
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
								numStopPrice.Value = Math.Round(price, 2);
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
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Stock Buy Order", CurrentSymbolDetail));
			}
		}

		private void cmdShort_Click(object sender, EventArgs e)
		{
			if (_isInError)
			{
				ValidateTicketValues();
			}

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Short.ToString());
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Stock Short Order", CurrentSymbolDetail));
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
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Stock Sell Order", CurrentSymbolDetail));
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
					BuildMktOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					BuildMktOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					BuildMktOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.VWAPMKT:
					BuildMktOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.GVWAP:
					BuildMktOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;

				#endregion

				#region - Imbalance Only -

				case CSVEx.CSVFieldIDs.OrderTypes.IMBOC:
					BuildIOCOrder(side,
						CSVEx.CSVFieldIDs.OrderTypes.Limit.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.IMBOO:
					BuildIOOOrder(side,
						CSVEx.CSVFieldIDs.OrderTypes.Limit.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.IMBOI:
					BuildIOIOrder(side,
						CSVEx.CSVFieldIDs.OrderTypes.Limit.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString());
					break;

				#endregion

				#region - Peg -

				case CSVEx.CSVFieldIDs.OrderTypes.PegMkt:
					BuildPegOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
						numPegPrice.Value.ToString(),
						CSVEx.CSVFieldIDs.PegOrderExecInstructions.MarketPeg);
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.PegPri:
					BuildPegOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
						numPegPrice.Value.ToString(),
						CSVEx.CSVFieldIDs.PegOrderExecInstructions.PrimaryPeg);
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.PegMid:
					BuildPegOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(),
						numPegPrice.Value.ToString(),
						CSVEx.CSVFieldIDs.PegOrderExecInstructions.MidpointPeg);
					break;

				#endregion

				#region - OCO -

				case CSVEx.CSVFieldIDs.OrderTypes.OCO:

					ocoOrderID = GLOBAL.OrderManagers.RomMessageMaker.GetOrderID(GLOBAL.OrderManagers.UserName);

					// Limit Price - primary side
					BuildOcoOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						ocoOrderID + "M");

					// Stop Price - other side
					if (side == CSVEx.CSVFieldIDs.SideCodes.Buy.ToString())
					{
						BuildOcoOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(),
							orderType.ToString(),
							duration.ToString(),
							numStopPrice.Value.ToString(),
							ocoOrderID + "S");
					}
					else
					{
						BuildOcoOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(),
							orderType.ToString(),
							duration.ToString(),
							numStopPrice.Value.ToString(),
							ocoOrderID + "S");
					}
					break;

				#endregion

				#region - STOP -

				case CSVEx.CSVFieldIDs.OrderTypes.Stop:
					BuildStpOrder(side,
						orderType.ToString(),
						duration.ToString(),
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

		private void BuildMktOrder(string side, string orderType, string duration)
		{
			BuildOrder(side, orderType, duration, "", "", "", "", "", "", "");
		}

		private void BuildIOCOrder(string side, string orderType, string duration, string limitPrice)
		{
			BuildOrder(side, orderType, duration, limitPrice, "", "", "", "", "I", CSVEx.CSVFieldIDs.InstructionCmds.IOC);
		}
		private void BuildIOOOrder(string side, string orderType, string duration, string limitPrice)
		{
			BuildOrder(side, orderType, duration, limitPrice, "", "", "", "", "I", CSVEx.CSVFieldIDs.InstructionCmds.IOO);
		}
		private void BuildIOIOrder(string side, string orderType, string duration, string limitPrice)
		{
			BuildOrder(side, orderType, duration, limitPrice, "", "", "", "", "I", CSVEx.CSVFieldIDs.InstructionCmds.IOI);
		}
		
		private void BuildPegOrder(string side, string orderType, string duration, string pegPrice, string execInstruction)
		{
			BuildOrder(side, orderType, duration, "", "", pegPrice, "", execInstruction, "", "");
		}
		private void BuildOcoOrder(string side, string orderType, string duration, string limitPrice, string ocoOrderID)
		{
			BuildOrder(side, orderType, duration, limitPrice, "", "", ocoOrderID, "", "", "");
		}
		private void BuildStpOrder(string side, string orderType, string duration, string stopPrice)
		{
			BuildOrder(side, orderType, duration, "", stopPrice, "", "", "", "", "");
		}

		private void BuildOrder(string side, string orderType, string duration, string limitPrice, string stopPrice)
		{
			BuildOrder(side, orderType, duration, limitPrice, stopPrice, "", "", "", "", "");
		}
		private void BuildOrder(string side, string orderType, string duration, string limitPrice, string stopPrice, string pegPrice, string ocoOrderID, string execInstruction, string displayInstruction, string instruction)
		{
			RomBasicOrder order = new RomBasicOrder();

			DestinationMap dest = new DestinationMap();
			DestinationMap destAlgo = new DestinationMap();
			AccountMap acct = new AccountMap();

			try
			{
				if (DDAccountDropDown.GetCurrentAccount(ref acct) && DDAccountDropDown.GetCurrentExchange(ref dest) && dest.destID != null)
				{
					if (ocoOrderID != "")
					{
						order.orderID = ocoOrderID;
					}

					order.symbol = CurrentSymbolDetail;
					order.mdSymbol = CurrentSecInfo.MDSymbol;
					order.secType = CurrentSecInfo.SecType;
					//order.multiplier = CurrentSecInfo.ContractSize.ToString();
					order.tradeFor = DDAccountDropDown.CurrentTradeFor;
					order.trader = GLOBAL.OrderManagers.UserName;
					order.shares = numQuantity.Value.ToString();
					order.price = limitPrice;
					order.stopPrice = stopPrice;
					order.pegPrice = pegPrice;
					order.execInstructions = execInstruction;
					if (chkShow.Checked)
					{
						order.show = numShow.Value.ToString();
					}
					if (chkMinQty.Checked)
					{
						order.minQty = numMinQuantity.Value.ToString();
					}
					if (cboShortLenders.Text != "")
					{
						order.shortLender = cboShortLenders.Text;
					}
					order.side = side;
					order.orderType = orderType;
					order.duration = duration;
					order.displayInstruction = displayInstruction;

					// Algo Special Fields
					if (panelAlgo.Visible)
					{
						switch (Convert.ToInt64(order.orderType))
						{
							case CSVEx.CSVFieldIDs.OrderTypes.VWAPMKT:
								order.orderType = CSVEx.CSVFieldIDs.OrderTypes.Market.ToString();
								order.algoType = CSVEx.CSVFieldIDs.AlgoTypes.VWAP.ToString();
								order.startTime = TWAPStartTime;
								order.endTime = TWAPEndTime;
								if (numMVP.Enabled)
								{
									order.instructions = string.Format("MVP={0}", Math.Round(numMVP.Value, 0) / 100);
								}
								break;
							case CSVEx.CSVFieldIDs.OrderTypes.VWAPLIM:
								order.orderType = CSVEx.CSVFieldIDs.OrderTypes.Limit.ToString();
								order.algoType = CSVEx.CSVFieldIDs.AlgoTypes.VWAP.ToString();
								order.startTime = TWAPStartTime;
								order.endTime = TWAPEndTime;
								if (numMVP.Enabled)
								{
									order.instructions = string.Format("MVP={0}", Math.Round(numMVP.Value, 0) / 100);
								}
								break;
							case CSVEx.CSVFieldIDs.OrderTypes.GVWAP:
								order.orderType = CSVEx.CSVFieldIDs.OrderTypes.Market.ToString();
								order.algoType = CSVEx.CSVFieldIDs.AlgoTypes.GVWAP.ToString();
								break;
						}
					}

					order.exchangeID = dest.destID.ToString();
					order.cmtaFirmID = dest.cmtaFirmID;
					if (instruction != "")
					{
						order.instructions = instruction;
					}
					if (chkInstructions.Checked && cboInstructions.Text!= "")
					{
						order.instructions = cboInstructions.Text;
					}
					order.duration = duration;
					order.capacity = acct.capacity;
					order.clearingAcctID = acct.clearingAcID;
					order.clearingFirmID = acct.clearingFirmID;
					order.firmArc = acct.firmAcr;
					order.localAcctAcrn = acct.localAcAcrn;
					order.occAcct = acct.occAcr;
					if (acct.homeExchange != null)
					{
						order.homeExchange = acct.homeExchange;
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

		private string TWAPStartTime
		{
			get
			{
				if (chkStartTime.Checked)
				{
					DateTime dt = dtpStartTime.Value.ToUniversalTime();
					return dt.ToString("yyyyMMdd-HH:mm:ss");
				}
				return "";
			}
		}

		private string TWAPEndTime
		{
			get
			{
				if (chkEndTime.Checked)
				{
					DateTime dt = dtpEndTime.Value.ToUniversalTime();
					return dt.ToString("yyyyMMdd-HH:mm:ss");
				}
				return "";
			}
		}

		private void chkStartTime_CheckedChanged(object sender, EventArgs e)
		{
			dtpStartTime.Enabled = chkStartTime.Checked;
		}

		private void chkEndTime_CheckedChanged(object sender, EventArgs e)
		{
			dtpEndTime.Enabled = chkEndTime.Checked;
		}

		private void chkAlgoExchange_CheckedChanged(object sender, EventArgs e)
		{
			cboAlgoExchange.Enabled = chkAlgoExchange.Checked;
		}

		private void chkMVP_CheckedChanged(object sender, EventArgs e)
		{
			numMVP.Enabled = chkMVP.Checked;
		}

		#endregion

		#region - Ticket Display -

		private void ValidateTicketValues()
		{
			GLOBAL.Interrupt = true;

			_isInError = false;

			if (!_isLoadingValue)
			{
				IsInError(SetState(cmdTradeFor, (cmdTradeFor.Text == "N/A" || cmdTradeFor.Text == "")));
				IsInError(SetState(cmdAccount, (cmdAccount.Text == "N/A" || cmdAccount.Text == "")));
				IsInError(SetState(lblExchange, (cboExchange.Text == "")));				
				SetPanelWidth(ref panelExchange, cboExchange);

				try
				{
					string orderTypeFilter = TicketSupport.BuildOrderTypeFilter(cboExchange, cboOrder);
					if (_orderTypesView.RowFilter != orderTypeFilter)
					{
						_orderTypesView.RowFilter = orderTypeFilter;
						if (_orderTypesView.RowFilter.Contains("VWAP%"))
						{
							cboOrder.Text = "VWAP MKT";
						}
						else
						{
							cboOrder.Text = "LIMIT";
						}
					}
					
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
					IsInError(SetState(lblLimitPrice, numLimitPrice.Value <= 0));
					IsInError(SetState(numLimitPrice, numLimitPrice.Value <= 0));

					SetPanelWidth(ref panelLimitPrice, numLimitPrice);
					SetPriceDisplay(PriceType.Limit, false);
				}

				if (panelStopPrice.Visible)
				{
					IsInError(SetState(lblStopPrice, numStopPrice.Value <= 0));
					IsInError(SetState(numStopPrice, numStopPrice.Value <= 0));

					SetPanelWidth(ref panelStopPrice, numStopPrice);
					SetPriceDisplay(PriceType.Stop, false);
				}

				if (panelPegPrice.Visible)
				{
					//IsInError(SetState(lblPegPrice, numPegPrice.Value <= 0));
					//IsInError(SetState(numPegPrice, numPegPrice.Value <= 0));

					SetPanelWidth(ref panelPegPrice, numPegPrice);
					SetPriceDisplay(PriceType.Peg, false);
				}

				IsInError(SetState(lblOrderType, cboOrder.Text == ""));
				SetPanelWidth(ref panelOrderTypes, cboOrder);

				IsInError(SetState(lblDuration, cboDuration.Text == ""));
				SetPanelWidth(ref panelDuration, cboDuration);

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

				numShow.Enabled = chkShow.Checked;
				dspShowIncrement.Enabled = chkShow.Checked;
				if (numShow.Enabled)
				{
					IsInError(SetState(chkShow, numShow.Value <= 0));
				}
				else
				{
					SetState(chkShow, false);
				}

				SetPanelWidth(ref panelShow, numShow);

				numMinQuantity.Enabled = chkMinQty.Checked;
				dspMinQuantityIncrement.Enabled = chkMinQty.Checked;
				if (numMinQuantity.Enabled)
				{
					IsInError(SetState(chkMinQty, numMinQuantity.Value <= 0));
				}
				else
				{
					SetState(chkMinQty, false);
				}

				SetPanelWidth(ref panelMinShares, numMinQuantity);

				cboInstructions.Enabled = chkInstructions.Checked;
				if (cboInstructions.Enabled)
				{
					IsInError(SetState(chkInstructions, cboInstructions.Text == ""));
				}
				else
				{
					SetState(chkInstructions, false);
				}

				SetPanelWidth(ref panelInstructions, cboInstructions);

				SetPanelWidth(ref panelAlgoType, cboAlgoType);
				
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
				case PriceType.Peg:
					if (firstTime)
					{
						dspPegPrice.Font = numPegPrice.Font;
					}
					if (dspPegPrice.ForeColor != numPegPrice.ForeColor)
					{
						dspPegPrice.ForeColor = numPegPrice.ForeColor;
					}
					formatedPrice = GetDoubleFormat(Convert.ToDouble(numPegPrice.Value));
					if (dspPegPrice.Text != formatedPrice)
					{
						dspPegPrice.Text = formatedPrice;
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
				case "VWAP MKT":
				case "GVWAP":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = false;
					panelPegPrice.Visible = false;
					cboDuration.Enabled = false;
					cboDuration.Text = "DAY";
					break;
				case "MOO":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = false;
					panelPegPrice.Visible = false;
					cboDuration.Enabled = false;
					cboDuration.Text = "OPG";
					break;
				case "STOPLIMIT":
				case "OCO":
					panelLimitPrice.Visible = true;
					panelStopPrice.Visible = true;
					panelPegPrice.Visible = false;
					EnableDuration();
					break;
				case "PEG MKT":
				case "PEG PRI":
				case "PEG MID":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = false;
					panelPegPrice.Visible = true;
					EnableDuration();
					break;
				case "STOP":
					panelLimitPrice.Visible = false;
					panelStopPrice.Visible = true;
					panelPegPrice.Visible = false;
					EnableDuration();
					PriceSelection_Click(lblStopPrice, EventArgs.Empty);
					break;
				case "LOC":
					panelLimitPrice.Visible = true;
					panelStopPrice.Visible = false;
					panelPegPrice.Visible = false;
					EnableDuration();
					PriceSelection_Click(lblLimitPrice, EventArgs.Empty);
					break;
				case "IMBOO":
				case "IMBOC":
				case "IMBOI":
				case "VWAP LIM":
				default:
					panelLimitPrice.Visible = true;
					panelStopPrice.Visible = false;
					panelPegPrice.Visible = false;
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
			//	durationFilter = "Duration <> 'OPG'";
			//}

			//_durationsView.RowFilter = durationFilter;
			//cboOrder.DataSource = _durationsView;`
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
				LongName = "Stock Ticket";
			}

			// 0 Quantity
			// 1 Symbol
			// 2 OrderType
			// 3 Limit Price / Stop Price / Peg Price
			// 4 Duration
			// 5 Exchange
			dspOrderMsg.Text = "";
			dspOrderMsg.SelectionAlignment = HorizontalAlignment.Center;
			dspOrderMsg.SelectionBackColor = dspOrderMsg.BackColor;

			string orderSymbol = cboSymbolDetails.Text;

			if (panelLimitPrice.Visible && panelStopPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {6} - {7} {3} {5} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboAlgoType.Text,
					cboExchange.Text,
					numLimitPrice.Value.ToString("######0.00#####"),
					numStopPrice.Value.ToString("######0.00#####"));
			}
			else if (panelLimitPrice.Visible && !panelStopPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {6} {3} {4} {5}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboAlgoType.Text,
					cboExchange.Text,
					numLimitPrice.Value.ToString("######0.00#####"));
			}
			else if (!panelLimitPrice.Visible && panelStopPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {6} {3} {4} {5}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboAlgoType.Text,
					cboExchange.Text,
					numStopPrice.Value.ToString("######0.00#####"));
			}
			else if (panelPegPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {6} {3} {4} {5}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboAlgoType.Text,
					cboExchange.Text,
					numPegPrice.Value.ToString("######0.00#####"));
			}
			else
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {3} {4} {5}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboAlgoType.Text,
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

			if (cboAlgoType.Text != "")
			{
				dspOrderMsg.SelectAll();
				dspOrderMsg.Select(dspOrderMsg.Find(cboAlgoType.Text), cboAlgoType.Text.Length);
				dspOrderMsg.SelectionBackColor = Color.Yellow;
			}

			CheckTicketMode();
		}

		private void CheckTicketMode()
		{
			if (minimumHighOffset > 0)
			{
				if (!IsLevel2)
				{
					Height = MinimumSize.Height + minimumHighOffset;
				}
				else
				{
					if (Height < MinimumSize.Height + minimumHighOffset)
					{
						Height = MinimumSize.Height + minimumHighOffset;
					}
				}
			}
			else if (!IsLevel2)
			{
				if (Height != MinimumSize.Height)
				{
					Height = MinimumSize.Height;
				}
			}

			try
			{
				if (DDAccountDropDown.CurrentDestinationInfo != null)
				{
					if (DDAccountDropDown.CurrentDestinationInfo.isAlgo || cboExchange.Text.ToUpper().Contains("_ALGO"))
					{
						panelOrderInfo.Height = panelOrderInfo.MinimumSize.Height + 40;
						panelAlgo.Visible = true;

						dtpStartTime.Value = DateTime.Now;
						dtpEndTime.Value = DateTime.Now.AddHours(1);
					}
					else
					{
						panelOrderInfo.Height = panelOrderInfo.MinimumSize.Height;
						panelAlgo.Visible = false;
					}
				}
				else
				{
					panelOrderInfo.Height = panelOrderInfo.MinimumSize.Height;
					panelAlgo.Visible = false;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void ResetTicketValues()
		{
			_isLoadingValue = true;

			HasFirstUpdate = false;

			CurrentSecInfo = new BaseSecurityInfo();

			LongName = CurrentSecInfo.LongName;

			dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspPegPriceTickSize.Value = CurrentSecInfo.TickSize;

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

			numLimitPrice.Value = 0;

			//cboOrder.Text = "Limit"; 
			//cboDuration.Text = "DAY";
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Stock, CSVEx.CSVFieldIDs.SecurityTypes.Equity));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(TicketDefaults.Stock, CSVEx.CSVFieldIDs.SecurityTypes.Equity), false);

			cboAlgoType.Text = "";

			_isLoadingValue = false;

			if (rocLevel2ListAsk.RocGridTable != null)
			{
				lock (rocLevel2ListAsk.RocGridTable)
				{
					rocLevel2ListAsk.RocGridTable.Rows.Clear();
					rocLevel2ListAsk.RocGridTable.DefaultView.RowFilter = "";
					rocLevel2ListAsk.Columns["Price"].HeaderText = "Price";
					rocLevel2ListAsk.Columns["Size"].HeaderText = "Size";
				}
			}

			if (rocLevel2ListBid.RocGridTable != null)
			{
				lock (rocLevel2ListBid.RocGridTable)
				{
					rocLevel2ListBid.RocGridTable.Rows.Clear();
					rocLevel2ListBid.RocGridTable.DefaultView.RowFilter = "";
					rocLevel2ListBid.Columns["Price"].HeaderText = "Price";
					rocLevel2ListBid.Columns["Size"].HeaderText = "Size";
				}
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

			if ((data.pegPriceIncrement % CurrentSecInfo.TickSizeDec) != 0)
			{
				data.pegPriceIncrement = CurrentSecInfo.TickSizeDec;
			}

			numPegPrice.Value = data.pegPrice;
			numPegPrice.Increment = data.pegPriceIncrement;
			dspPegPriceIncrement.Value = data.pegPriceIncrement / CurrentSecInfo.TickSizeDec;

			chkShow.Checked = data.useShow;
			numShow.Enabled = data.useShow;
			numShow.Value = data.show;
			numShow.Increment = data.showIncrement;
			dspShowIncrement.Value = numShow.Increment;

			chkMinQty.Checked = data.useMinQty;
			numMinQuantity.Enabled = data.useMinQty;
			numMinQuantity.Value = data.minQty;
			numMinQuantity.Increment = data.minQtyIncrement;
			dspMinQuantityIncrement.Value = numMinQuantity.Increment;

			CurrentShortLender = data.shortLender;

			chkInstructions.Checked = data.useInstruction;
			cboInstructions.Enabled = data.useInstruction;
			CurrentInstruction = data.instruction;

			_isLoadingValue = false;

			ValidateTicketValues();
		}

		#endregion

		#region - Export & Import -

		internal string ExportBidGrid()
		{
			ROCLevel2ListProfile prof = new ROCLevel2ListProfile(rocLevel2ListBid);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		internal void ImportBidGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCLevel2ListProfile prof = (ROCLevel2ListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCLevel2ListProfile(rocLevel2ListBid, prof);
				}
			}
		}

		internal string ExportAskGrid()
		{
			ROCLevel2ListProfile prof = new ROCLevel2ListProfile(rocLevel2ListAsk);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		internal void ImportAskGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCLevel2ListProfile prof = (ROCLevel2ListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCLevel2ListProfile(rocLevel2ListAsk, prof);
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

					foreach (string instruction in prof.Instructions)
					{
						Instructions.Rows.Add(instruction);
					}

					prof = new ROCTicketProfile(this, prof);
				}
			}
		}

		internal void ReloadTicket()
		{
			if (CurrentSymbolDetail != "" && CurrentSecInfo != null)
			{
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVFieldIDs.SecurityTypes.Equity));
			}
			else
			{
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(TicketDefaults.Stock, CSVFieldIDs.SecurityTypes.Equity));
			}
			ValidateTicketValues();
		}

		internal void Clone()
		{
			lock (GLOBAL.HWindows.StockTicketWindows)
			{
				frmStockTicket w = new frmStockTicket();

				w.Width = this.Width;
				w.Height = this.Height;

				w.Clone(new ROCTicketProfile(this), new ROCLevel2ListProfile(rocLevel2ListBid), new ROCLevel2ListProfile(rocLevel2ListAsk));
				
				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}
		internal void Clone(ROCTicketProfile prof, ROCLevel2ListProfile gridBidProf, ROCLevel2ListProfile gridAskProf)
		{
			if (prof != null)
			{
				prof = new ROCTicketProfile(this, prof);
			}
			if (gridBidProf != null)
			{
				gridBidProf = new ROCLevel2ListProfile(rocLevel2ListBid, gridBidProf);
			}
			if (gridAskProf != null)
			{
				gridAskProf = new ROCLevel2ListProfile(rocLevel2ListAsk, gridAskProf);
			}
		}

		#endregion

		#region - Level DropDown -

		private bool _isLevel2 = false;
		internal bool IsLevel2
		{
			get
			{
				switch (cmdLevel.Text)
				{
					case "LEVEL 2":
						return true;
					case "LEVEL 1":
					default:
						return false;
				}
			}
			set
			{
				_isLevel2 = value;
				if (_isLevel2)
				{
					cmdLevel.Text = "LEVEL 2";
				}
				else
				{
					cmdLevel.Text = "LEVEL 1";
				}

				// Set during importing only, ticket resize should be disabled
				SetTicketLevel(false);
			}
		}

		internal string CurrentLevel
		{
			get
			{
				return cmdLevel.Text.ToUpper();
			}
		}

		private void Level_Changed(object sender, EventArgs e)
		{
			cmdLevel.Text = ((ToolStripMenuItem)sender).Text.ToUpper();

			if (_lastLevel != CurrentLevel)
			{
				// Set by user selection, reset ticket size
				SetTicketLevel(true);
			}
		}

		private void SetTicketLevel(bool resetSize)
		{
			if (IsLevel2)
			{
				MinimumSize = new Size(MinimumSize.Width, _defaultLevel2);
				//if (resetSize)
				//{
				//    Width = MinimumSize.Width;
				//    if (Height < MinimumSize.Height)
				//    {
				//        Height = MinimumSize.Height;
				//    }
				//    //Size = MinimumSize;
				//}

				splitContainerLevel2.Visible = true;
			}
			else
			{
				MinimumSize = new Size(MinimumSize.Width, _defaultLevel1);
				if (resetSize)
				{
					Size = MinimumSize;
				}

				splitContainerLevel2.Visible = false;
			}
		}

		#endregion

		#region - Settings DropDown -

		private void itemSaveAsDefault_Click(object sender, EventArgs e)
		{
			SymbolSettingData data = GLOBAL.HSymbolSettingData.GetSymbolDefaults(CurrentSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Equity);

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

			data.pegPrice = numPegPrice.Value;
			data.pegPriceIncrement = numPegPrice.Increment;

			data.duration = cboDuration.Text;

			data.show = numShow.Value;
			data.showIncrement = numShow.Increment;
			data.useShow = chkShow.Checked;

			data.minQty = numMinQuantity.Value;
			data.minQtyIncrement = numMinQuantity.Increment;
			data.useMinQty = chkMinQty.Checked;

			data.shortLender = cboShortLenders.Text;

			data.instruction = cboInstructions.Text;
			data.useInstruction = chkInstructions.Checked;

			GLOBAL.HSymbolSettingData.SetSymbolDefaults(data);
		}

		private void itemResetToSystemDefault_Click(object sender, EventArgs e)
		{
			GLOBAL.HSymbolSettingData.RemoveSymbolDefault(CurrentSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Equity);
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Equity));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecurityTypes.Equity));
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

		#region - Instructions DropDown -

		// This would reset the combox after every Enter and Delete key stroke
		private bool _newInstruction = true;

		internal string CurrentInstruction
		{
			get
			{
				return cboInstructions.Text;
			}
			set
			{
				cboInstructions.Text = value;
			}
		}

		private DataTable _instructions;
		internal DataTable Instructions
		{
			get
			{
				if (_instructions == null)
				{
					_instructions = new DataTable();

					_instructions.Columns.Add(new DataColumn("Instruction", Type.GetType("System.String")));

					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_instructions.Columns["Instruction"]);
					_instructions.PrimaryKey = keyColumns.ToArray();

					_instructions.DefaultView.Sort = "Instruction";
				}
				return _instructions;
			}
			set
			{
				_instructions = value;
			}
		}

		private void cboInstructions_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (CurrentInstruction != cboInstructions.Text)
			{
				CurrentInstruction = cboInstructions.Text;
			}
		}

		private void cboInstructions_KeyUp(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					if (cboInstructions.Text != "")
					{
						AddInstruction(cboInstructions.Text.ToUpper());
					}
					_newInstruction = true;
					break;
				default:
					_newInstruction = false;
					break;
			}
		}

		private void AddInstruction(string instruction)
		{
			if (Instructions.DefaultView.FindRows(instruction).Length == 0)
			{
				Instructions.Rows.Add(instruction);
			}
			else
			{
				cboInstructions.Text = instruction;
			}

			CurrentInstruction = instruction;
		}

		private void cboInstructions_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Delete:
					DeleteInstruction(cboInstructions.Text.ToUpper());
					break;
				default:
					if (_newInstruction)
					{
						// Set the Text to null
						cboInstructions.Text = "";
					}
					break;
			}
		}

		private void DeleteInstruction(string instruction)
		{
			DataRowView[] rows = Instructions.DefaultView.FindRows(instruction);

			foreach (DataRowView row in rows)
			{
				Instructions.Rows.Remove(row.Row);
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
						items.Add("100", "100");
						items.Add("500", "500");
						items.Add("1000", "1000");
						items.Add("5000", "5000");

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
				case "100":
					numQuantity.Increment = 100;
					break;
				case "500":
					numQuantity.Increment = 500;
					break;
				case "1000":
					numQuantity.Increment = 1000;
					break;
				case "5000":
					numQuantity.Increment = 5000;
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

		#region - Min Qty Increment Menu -

		private menuBaseAction _menuMinQtyIncrement;
		private PropertyContainer _menuMinQtyIncrementContainer;
		private void dspMinQuantityIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuMinQtyIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("100", "100");
						items.Add("500", "500");
						items.Add("1000", "1000");
						items.Add("5000", "5000");

						_menuMinQtyIncrement = new menuBaseAction(items, true);
						_menuMinQtyIncrement.PropertyChanged += new PropertyChangedEventHandler(menuMinQtyIncrement_PropertyChanged);
					}

					if (_menuMinQtyIncrementContainer == null)
					{
						_menuMinQtyIncrementContainer = new PropertyContainer(_menuMinQtyIncrement);
						_menuMinQtyIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuMinQtyIncrementContainer_Closed);
					}

					_menuMinQtyIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuMinQtyIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuMinQtyIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "100":
					numMinQuantity.Increment = 100;
					break;
				case "500":
					numMinQuantity.Increment = 500;
					break;
				case "1000":
					numMinQuantity.Increment = 1000;
					break;
				case "5000":
					numMinQuantity.Increment = 5000;
					break;
			}
			dspMinQuantityIncrement.Value = numMinQuantity.Increment;
		}

		private void menuMinQtyIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuMinQtyIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuMinQtyIncrementContainer_Closed);
			_menuMinQtyIncrementContainer = null;
			_menuMinQtyIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuMinQtyIncrement_PropertyChanged);
			_menuMinQtyIncrement = null;
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

		#region - Peg Price Increment Menu -

		private menuBaseAction _menuPegPriceIncrement;
		private PropertyContainer _menuPegPriceIncrementContainer;
		private void dspPegPriceIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuPegPriceIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

						_menuPegPriceIncrement = new menuBaseAction(items, true);
						_menuPegPriceIncrement.PropertyChanged += new PropertyChangedEventHandler(menuPegPriceIncrement_PropertyChanged);
					}

					if (_menuPegPriceIncrementContainer == null)
					{
						_menuPegPriceIncrementContainer = new PropertyContainer(_menuPegPriceIncrement);
						_menuPegPriceIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuPegPriceIncrementContainer_Closed);
					}

					_menuPegPriceIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuPegPriceIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuPegPriceIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 1;
					break;
				case "5":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 5;
					break;
				case "10":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 10;
					break;
				case "25":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 25;
					break;
				case "50":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 50;
					break;
				case "75":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 75;
					break;
				case "100":
					numPegPrice.Increment = CurrentSecInfo.TickSizeDec * 100;
					break;
			}
			dspPegPriceIncrement.Value = numPegPrice.Increment / CurrentSecInfo.TickSizeDec;
		}

		private void menuPegPriceIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuPegPriceIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuPegPriceIncrementContainer_Closed);
			_menuPegPriceIncrementContainer = null;
			_menuPegPriceIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuPegPriceIncrement_PropertyChanged);
			_menuPegPriceIncrement = null;
		}

		#endregion

		#region - Show Incremnt Menu -

		private menuBaseAction _menuShowIncrement;
		private PropertyContainer _menuShowIncrementContainer;
		private void dspShowIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuShowIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("100", "100");
						items.Add("500", "500");
						items.Add("1000", "1000");
						items.Add("5000", "5000");

						_menuShowIncrement = new menuBaseAction(items, true);
						_menuShowIncrement.PropertyChanged += new PropertyChangedEventHandler(menuShowIncrement_PropertyChanged);
					}

					if (_menuShowIncrementContainer == null)
					{
						_menuShowIncrementContainer = new PropertyContainer(_menuShowIncrement);
						_menuShowIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuShowIncrementContainer_Closed);
					}

					_menuShowIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuShowIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuShowIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "100":
					numShow.Increment = 100;
					break;
				case "500":
					numShow.Increment = 500;
					break;
				case "1000":
					numShow.Increment = 1000;
					break;
				case "5000":
					numShow.Increment = 5000;
					break;
			}
			dspShowIncrement.Value = numShow.Increment;
		}

		private void menuShowIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuShowIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuShowIncrementContainer_Closed);
			_menuShowIncrementContainer = null;
			_menuShowIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuShowIncrement_PropertyChanged);
			_menuShowIncrement = null;
		}

		#endregion
	}
}