using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

using SerializationEx;
using DataGridViewEx;
using MarketDataEx;
using CSVEx;
using RDSEx;
using ContextMenuEx;
using LabelEx;
using ROMEx;
using DateTimeEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmAutoSpreadTicket : QuickAutoSpreadSupport
	{
		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return rocAutoSpreadListSettings.Processing;
			}
			set
			{
				rocAutoSpreadListSettings.Processing = value;
			}
		}

		private List<AutoSpreadSymbolSetting> _lastSavedAutoSpreadSymbolSettingList;
		public List<AutoSpreadSymbolSetting> LastSavedAutoSpreadSymbolSettingList
		{
			get
			{
				if (_lastSavedAutoSpreadSymbolSettingList == null)
				{
					_lastSavedAutoSpreadSymbolSettingList = new List<AutoSpreadSymbolSetting>();
				}
				return _lastSavedAutoSpreadSymbolSettingList;
			}
			set
			{
				_lastSavedAutoSpreadSymbolSettingList = value;
			}
		}

		private DataView _searchView;
		[Browsable(false)]
		public DataView SearchView
		{
			get
			{
				if (_searchView == null)
				{
					_searchView = new DataView(rocAutoSpreadListSettings.RocGridTable);
					_searchView.Sort = "Symbol";
				}
				return _searchView;
			}
			set
			{
				_searchView = value;
			}
		}

		public bool EquityOnly
		{
			get
			{
				foreach (AutoSpreadItem item in _autoSpreadSupport.ItemByLegs.Values)
				{
					if (item.TradeFor == "" || item.LocalAcctAcrn == "" || item.Exchange == "")
					{
						return false;
					}
				}
				return true;
			}
		}

		#endregion

		#region - Local Variable -

		private Size _lastSize = new Size();
		private bool _widthChanged = false;
		private bool _heightChanged = false;
		private bool _resizeGridToFit = false;

		private bool _isInError = false;
		private bool _isLoadingValue = true;

		private AutoSpreadSupport _autoSpreadSupport;

		private DataView _orderTypesView;
		private DataView _durationsView;

		private NumericUpDown _crrentPriceObj;

		private bool _updateBuyAggregation = false;
		private bool _updateSellAggregation = false;

		private bool _orderLoaded = false;

		private Dictionary<string, ROCOrder> _buyOrdersByTag = new Dictionary<string, ROCOrder>();
		private Dictionary<string, ROCOrder> _sellOrdersByTag = new Dictionary<string, ROCOrder>();

		private long _openBuyQty = 0;
		private long _openSellQty = 0;
		private long _netFilledQty = 0;

		private bool _centered = false;
		private bool _ladderHasRows = false;

		private bool okToBuildQuickList
		{
			get
			{
				if (_autoSpreadSupport.HasAllLegs && _autoSpreadSupport.SyntheticBidBook.Count > 0 && _autoSpreadSupport.SyntheticAskBook.Count > 0)
				{
					return true;
				}
				return false;
			}
		}

		private bool newSymbol
		{
			set
			{
				if (value)
				{
					// New symbol refresh needed
					rocAutoSpreadListSettings.RefreshRation = true;
					rocAutoSpreadList.ReadyForTickTable = false;
					ClearLadder();
				}
			}
		}

		private bool settingsVisible
		{
			get
			{
				if (splitContainer.Panel2Collapsed)
				{
					return false;
				}
				return true;
			}
			set
			{
				int lastFirstDisplayedRow = rocAutoSpreadList.FirstDisplayedScrollingRowIndex;
				if (value)
				{
					cmdSelectLadder.Enabled = true;
					cmdSelectLadder.BackColor = Color.LimeGreen;

					cmdSelectSettingGrid.Enabled = false;
					cmdSelectSettingGrid.BackColor = Color.Gray;

					splitContainer.Panel1Collapsed = true;
					splitContainer.Panel2Collapsed = false;
				}
				else
				{
					cmdSelectLadder.Enabled = false;
					cmdSelectLadder.BackColor = Color.Gray;

					cmdSelectSettingGrid.Enabled = true;
					cmdSelectSettingGrid.BackColor = Color.LimeGreen;

					splitContainer.Panel1Collapsed = false;
					splitContainer.Panel2Collapsed = true;
				}

				if (lastFirstDisplayedRow >= 0)
				{
					rocAutoSpreadList.FirstDisplayedScrollingRowIndex = lastFirstDisplayedRow;
				}

				FitWidth(false);
			}
		}

		//private Nullable<double> _lastTopOfAskPrice;
		//private Nullable<double> _lastTopOfBidPrice;
		//private bool refreshSelectedLegPrice
		//{
		//    get
		//    {
		//        if (_lastTopOfAskPrice != rocAutoSpreadList.TopOfBookAskPrice ||
		//            _lastTopOfBidPrice != rocAutoSpreadList.TopOfBookBidPrice)
		//        {
		//            _lastTopOfAskPrice = rocAutoSpreadList.TopOfBookAskPrice;
		//            _lastTopOfBidPrice = rocAutoSpreadList.TopOfBookBidPrice;
		//            return true;
		//        }
		//        return false;
		//    }
		//}

		// To Keep Trake the Orders that are currently under replacing mode
		// Tag - Origional Total Qty, when not null split orders needs to be generated.
		private Dictionary<string, Nullable<long>> _replacingOrders = new Dictionary<string, long?>();

		// To Keep Trake the Locked parent tags
		private List<string> _lockedParentTags = new List<string>();

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private List<ROCOrder> _rocOrders = new List<ROCOrder>();
		private Dictionary<string, MDServerToClient> _deltas = new Dictionary<string, MDServerToClient>();

		// Used to store user info for faster lookup
		private Dictionary<string, RomBasicOrder> _validUserInfos = new Dictionary<string, RomBasicOrder>();

		#endregion

		#region - Construtor -

		public frmAutoSpreadTicket()
		{
			InitializeComponent();
			
			Initialize();
		}

		private void Initialize()
		{
			DDAccountDropDown = new DropDownTraderAccount(this);

			_isLoadingValue = true;

			_autoSpreadSupport = new AutoSpreadSupport(Configuration.ROC.Default.BookDepthLimit, 0, Handle);
			lblLegs.Value = _autoSpreadSupport.Legs;

			_crrentPriceObj = numLimitPrice;

			_orderTypesView = new DataView(GLOBAL.HOrders.AutoSpreadOrderTypes);
			cboOrder.DataSource = _orderTypesView;
			cboOrder.DisplayMember = "Type";
			cboOrder.Text = "LIMIT";

			_durationsView = new DataView(GLOBAL.HOrders.AutoSpreadDurations);
			cboDuration.DataSource = _durationsView;
			cboDuration.DisplayMember = "Duration";
			cboDuration.Text = "DAY";

			_isLoadingValue = false;

			Height = MinimumSize.Height;

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			SizeChanged += new EventHandler(Form_SizeChanged);
			Activated += new EventHandler(From_Activated);

			panelManagerQuick.SelectedPanel = managedPanelBook;

			rocAutoSpreadList.Initialize();
			rocAutoSpreadList.RefreshingLadder += new EventHandler(rocAutoSpreadList_RefreshingLadder);
			rocAutoSpreadList.PropertyChanged += new PropertyChangedEventHandler(rocAutoSpreadList_PropertyChanged);
			rocAutoSpreadList.MouseEnter += new EventHandler(rocAutoSpreadList_MouseEnter);

			rocAutoSpreadListSettings.Initialize();
			rocAutoSpreadListSettings.PropertyChanged += new PropertyChangedEventHandler(rocAutoSpreadListSettings_PropertyChanged);
			rocAutoSpreadListSettings.RefreshingRation += new EventHandler(rocAutoSpreadListSettings_RefreshingRation);
			rocAutoSpreadListSettings.MouseEnter += new EventHandler(rocAutoSpreadListSettings_MouseEnter);

			settingsVisible = true;

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

			if (lblLegs.Value == null || (int)lblLegs.Value != _autoSpreadSupport.Legs)
			{
				lblLegs.Value = _autoSpreadSupport.Legs;
			}

			rocAutoSpreadList.PadByLimit();

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
			if (!_resizeGridToFit)
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
					}
				}
			}
		}

		private void FitWidth(bool manual)
		{
			_widthChanged = false;
			_resizeGridToFit = manual;

			if (settingsVisible)
			{
				lock (rocAutoSpreadListSettings)
				{
					if (_resizeGridToFit)
					{
						SetColumnMode(rocAutoSpreadListSettings, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocAutoSpreadListSettings);
					}
					else
					{
						SetColumnMode(rocAutoSpreadListSettings, DataGridViewAutoSizeColumnMode.Fill);
					}
					SetColumnMode(rocAutoSpreadListSettings, DataGridViewAutoSizeColumnMode.None);
				}
			}
			else
			{
				lock (rocAutoSpreadList)
				{
					if (_resizeGridToFit)
					{
						SetColumnMode(rocAutoSpreadList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocAutoSpreadList);
					}
					else
					{
						SetColumnMode(rocAutoSpreadList, DataGridViewAutoSizeColumnMode.Fill);
					}
					SetColumnMode(rocAutoSpreadList, DataGridViewAutoSizeColumnMode.None);
				}
			}

			int diff = MinimumSize.Width - Width;

			if (diff > 0)
			{
				int deltaUpper = diff / 6;

				//panelShares.Width = panelShares.MinimumSize.Width + deltaUpper;
				//panelOrderTypes.Width = panelOrderTypes.MinimumSize.Width + deltaUpper;
				//panelLimitPrice.Width = panelLimitPrice.MinimumSize.Width + deltaUpper;
				//panelStopPrice.Width = panelStopPrice.MinimumSize.Width + deltaUpper;
				//panelDuration.Width = panelDuration.MinimumSize.Width + deltaUpper;
				//panelExchange.Width = panelExchange.MinimumSize.Width + deltaUpper;

				int deltaCommands = diff / 2;

				cmdBuy.Width = cmdBuy.MinimumSize.Width + deltaCommands;
				cmdSell.Width = cmdSell.MinimumSize.Width + deltaCommands;

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

			_resizeGridToFit = false;
		}

		private void FitHeight()
		{
			_heightChanged = false;
		}

		private void Form_Load(object sender, EventArgs e)
		{
			DDAccountDropDown.LoadUserAccounts();

			RefreshAutoSpreadSettingsList(null);
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			StopTimer();

			UITimerTicked -= new EventHandler(Form_UITimerTicked);
			FitClicked -= new EventHandler(Form_FitClicked);

			Load -= new EventHandler(Form_Load);
			FormClosing -= new FormClosingEventHandler(Form_FormClosing);
			SizeChanged -= new EventHandler(Form_SizeChanged);
			Activated -= new EventHandler(From_Activated);

			rocAutoSpreadList.Dispose();
			rocAutoSpreadList.RefreshingLadder -= new EventHandler(rocAutoSpreadList_RefreshingLadder);
			rocAutoSpreadList.PropertyChanged -= new PropertyChangedEventHandler(rocAutoSpreadList_PropertyChanged);
			rocAutoSpreadList.MouseEnter -= new EventHandler(rocAutoSpreadList_MouseEnter);

			rocAutoSpreadListSettings.Dispose();
			rocAutoSpreadListSettings.PropertyChanged -= new PropertyChangedEventHandler(rocAutoSpreadListSettings_PropertyChanged);
			rocAutoSpreadListSettings.RefreshingRation -= new EventHandler(rocAutoSpreadListSettings_RefreshingRation);
			rocAutoSpreadListSettings.MouseEnter -= new EventHandler(rocAutoSpreadListSettings_MouseEnter);

			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void From_Activated(object sender, EventArgs e)
		{
			ActiveControl = numTickSize;
			numTickSize.Select(0, numTickSize.Text.Length);
		}

		#endregion

		#region - Ladder Grid Event -

		private void rocAutoSpreadList_RefreshingLadder(object sender, EventArgs e)
		{
			if (rocAutoSpreadList.RefreshLadder)
			{
				rocAutoSpreadList.RefreshLadder = false;
			}
		}

		private void rocAutoSpreadList_PropertyChanged(object sender, PropertyChangedEventArgs e)
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
				case "ShowQtyMenu":
					ShowQuantityMenu();
					break;
				case "BuyCancel":
					CancelOrderByPrice("Buy", ((ROCQuickList)sender).PriceBook);
					break;
				case "SellCancel":
					CancelOrderByPrice("Sell", ((ROCQuickList)sender).PriceBook);
					break;
			}
		}

		private void SetStopPrice(decimal price)
		{
			if (price >= numStopPrice.Minimum)
			{
				numStopPrice.Value = price;
			}
		}

		private void SetLimitPrice(decimal price)
		{
			if (price >= numLimitPrice.Minimum)
			{
				numLimitPrice.Value = price;
			}
		}

		private void rocAutoSpreadList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket)
			{
				ActivateGrid(rocAutoSpreadList);
			}
		}

		#endregion
		
		#region - Setting Grid Event -

		// when user changed the spread setting
		private void rocAutoSpreadListSettings_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			lock (rocAutoSpreadListSettings.RocGridTable)
			{
				switch (e.PropertyName)
				{
					case "SubscribeSymbolDetail":
						UpdateDataRows(rocAutoSpreadListSettings.SubscribeSymbolDetail);
						break;
					case "ShowColumnMenu":
						ShowColumnMenu();
						break;
					case "ShowTraderMenu":
						ShowTraderMenu();
						break;
					case "ShowAccountMenu":
						ShowAccountMenu();
						break;
					case "ShowExchangeMenu":
						ShowExchangeMenu();
						break;
				}
			}
		}

		private void rocAutoSpreadListSettings_RefreshingRation(object sender, EventArgs e)
		{
			if (rocAutoSpreadListSettings.RefreshRation)
			{
				rocAutoSpreadListSettings.RefreshRation = false;
				RefreshSpreadSettings();

				// replay current
				try
				{
					UpdateMarketDataDeltas(new Dictionary<string, MDServerToClient>(GLOBAL.HMarketData.Current));
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void UpdateDataRows(string symbolDetail)
		{
			DataRow[] rows = rocAutoSpreadListSettings.RocGridTable.Select("SymbolDetail = '" + symbolDetail + "'");

			if (rows.Length > 0)
			{
				BaseSecurityInfo secInfo = null;

				foreach (DataRow row in rows)
				{
					secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

					UpdateSettingDataRow(symbolDetail, 1, 0, false, 5, "", "", "", "", row, false);
				}

				// New symbol refresh needed
				newSymbol = true;
			}
		}

		private DataRow UpdateSettingDataRow(string symbolDetail, long factor, double userRatio, bool lean, long tickLimit, string exchange, string localAccountAcrn, string traderFor, string shortLender, DataRow row, bool isRefresh)
		{
			string mdSymbol = symbolDetail;
			double tickSize = 0.01;
			string secType = "";
			string name = "";
			MDServerToClient delta = new MDServerToClient();

			UpdateAutoSpreadSettingsWithSecurityInfo(symbolDetail, ref mdSymbol, ref tickSize, ref secType, ref name);

			UpdateAutoSpreadSettingsWithCurrentMarketData(mdSymbol.ToUpper(), ref delta);

			row["Symbol"] = mdSymbol;
			if (isRefresh)
			{
				row["SymbolDetail"] = symbolDetail;
				row["Factor"] = factor;
				row["UserRatio"] = userRatio;
				row["Lean"] = lean;
				row["TickLimit"] = tickLimit;
				row["Exchange"] = exchange;
				row["LocalAccountAcrn"] = localAccountAcrn;
				row["TraderFor"] = traderFor;
				row["ShortLender"] = shortLender;
			}

			row["TickSize"] = tickSize;

			if (delta.DisplayConversionFactor != null)
			{
				row["DisplayFactor"] = (double)delta.DisplayConversionFactor;
			}
			else
			{
				row["DisplayFactor"] = 1;
			}
			row["SecType"] = secType;
			row["Valid"] = 0;

			if (symbolDetail.Contains(" "))
			{
				string[] optionDetails = symbolDetail.Split(new char[] { ' ' }, StringSplitOptions.None);
				if (optionDetails.Length > 0)
				{
					GLOBAL.HRDS.GetOptionChain(optionDetails[0]);
				}
			}
			else
			{
				GLOBAL.HRDS.GetSecurityByTicker(symbolDetail);
			}

			return row;
		}

		private void rocAutoSpreadListSettings_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket)
			{
				ActivateGrid(rocAutoSpreadListSettings);
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

		private void TickSize_ValueChanged(object sender, EventArgs e)
		{
			if (okToBuildQuickList)
			{
				double tickSize = Convert.ToDouble(numTickSize.Value);
				lock (rocAutoSpreadList)
				{
					if (rocAutoSpreadList.TickSize != tickSize)
					{
						// Changed by user
						rocAutoSpreadList.TickSize = tickSize;
						RefreshSpreadLadder();
					}
				}
			}
		}

		private void ToggleGrid_Click(object sender, EventArgs e)
		{
			settingsVisible = !settingsVisible;
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
			Nullable<double> price = _autoSpreadSupport.EndOfBookPrice("Bid");
			if (price != null)
			{
				price = rocAutoSpreadList.CalTickPriceFromSpreadPrice((double)price, "Bid");
				rocAutoSpreadList.CenterOnPrice((double)price);
			}
		}

		private void cmdCenterTrade_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			rocAutoSpreadList.CenterOnPrice(rocAutoSpreadList.TradePrice);
		}

		private void cmdCenterHigh_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			Nullable<double> price = _autoSpreadSupport.EndOfBookPrice("Ask");
			if (price != null)
			{
				price = rocAutoSpreadList.CalTickPriceFromSpreadPrice((double)price, "Ask");
				rocAutoSpreadList.CenterOnPrice((double)price);
			}
		}

		private void Control_KeyUp(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Tab)
			{
				switch (((Control)sender).Name)
				{
					case "numTickSize":
						ActiveControl = numQuantity;
						numQuantity.Select(0, numQuantity.Text.Length);
						break;
					case "numQuantity":
					case "cboOrder":
						if (panelLimitPrice.Visible)
						{
							ActiveControl = numLimitPrice;
							numLimitPrice.Select(0, numLimitPrice.Text.Length);
						}
						else if (panelStopPrice.Visible)
						{
							ActiveControl = numStopPrice;
							numStopPrice.Select(0, numStopPrice.Text.Length);
						}
						else
						{
							ActiveControl = numTickSize;
							numTickSize.Select(0, numTickSize.Text.Length);
						}
						break;
					case "numLimitPrice":
						if (panelStopPrice.Visible)
						{
							ActiveControl = numStopPrice;
							numStopPrice.Select(0, numStopPrice.Text.Length);
						}
						else
						{
							ActiveControl = numTickSize;
							numTickSize.Select(0, numTickSize.Text.Length);
						}
						break;
					case "numStopPrice":
					case "cboDuration":
					case "cboExchange":
						ActiveControl = numTickSize;
						numTickSize.Select(0, numTickSize.Text.Length);
						break;
				}
			}
		}

		#endregion

		#region - Binding -

		#region - Refresh & First Load -

		public void RefreshAutoSpreadSettingsList(object input)
		{
			UpdateAutoSpreadSettingsStart();

			RefreshLoadSymbolDetailList();

			UpdateAutoSpreadSettingsStop();
		}

		// Used by Main Refresh and Load Thread
		private delegate void RefreshLoadSymbolDetailListDelegate();
		private void RefreshLoadSymbolDetailList()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new RefreshLoadSymbolDetailListDelegate(RefreshLoadSymbolDetailList));
				return;
			}

			try
			{
				lock (rocAutoSpreadListSettings.RocGridTable)
				{
					rocAutoSpreadListSettings.SuspendLayout();

					rocAutoSpreadListSettings.RocGridTable.Rows.Clear();
					rocAutoSpreadListSettings.GridKeys.Clear();
					rocAutoSpreadListSettings.Symbols.Clear();
					rocAutoSpreadListSettings.Symbols.Add("");
					rocAutoSpreadListSettings.Details.Clear();
					rocAutoSpreadListSettings.Details.Add("");

					LoadSymbolDetailList();

					if (rocAutoSpreadListSettings.RocGridTable.Rows.Count == 0)
					{
						// Insert at least one blank row for user to edit
						rocAutoSpreadListSettings.RocGridTable.Rows.Add(rocAutoSpreadListSettings.RocGridTable.NewRow());
					}

					rocAutoSpreadListSettings.ResumeLayout();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void LoadSymbolDetailList()
		{
			if (LastSavedAutoSpreadSymbolSettingList.Count > 0)
			{
				// Check to see if the last item is null or not, if null remove from list
				while (LastSavedAutoSpreadSymbolSettingList[LastSavedAutoSpreadSymbolSettingList.Count - 1].SymbolDetail == "")
				{
					if (LastSavedAutoSpreadSymbolSettingList.Count - 1 == 0)
					{
						LastSavedAutoSpreadSymbolSettingList.Clear();
						break;
					}
					else
					{
						LastSavedAutoSpreadSymbolSettingList.RemoveAt(LastSavedAutoSpreadSymbolSettingList.Count - 1);
					}
				}

				foreach (AutoSpreadSymbolSetting symbolSetting in LastSavedAutoSpreadSymbolSettingList)
				{
					AddSymbolDetail(symbolSetting.SymbolDetail, symbolSetting.Factor, symbolSetting.UserRatio, symbolSetting.Lean, symbolSetting.TickLimit, symbolSetting.Exchange, symbolSetting.LocalAccountAcrn, symbolSetting.TraderFor, symbolSetting.ShortLender);
				}
			}
		}

		// Refresh or First Load
		private void AddSymbolDetail(string symbolDetail, long factor, double userRatio, bool lean, long tickLimit, string exchange, string localAccountAcrn, string traderFor, string shortLender)
		{
			lock (rocAutoSpreadListSettings.RocGridTable)
			{
				DataRow row = rocAutoSpreadListSettings.RocGridTable.NewRow();
				row = UpdateSettingDataRow(symbolDetail, factor, userRatio, lean, tickLimit, exchange, localAccountAcrn, traderFor, shortLender, row, true);

				rocAutoSpreadListSettings.UpdateSymbol(row["Symbol"].ToString());

				if (!rocAutoSpreadListSettings.Details.Contains(symbolDetail))
				{
					rocAutoSpreadListSettings.Details.Add(symbolDetail);
				}

				rocAutoSpreadListSettings.RocGridTable.Rows.Add(row);
			}
		}

		private void InitialOrderLoad()
		{
			UpdateOrders();
			
			_orderLoaded = true;
		}

		#endregion

		#region - Used By Process Thread -

		private delegate void UpdateTicketByProcessDelegate(bool updateIM, ROCOrder[] orders, Dictionary<string, MDServerToClient> deltas);
		public void UpdateTicketByProcess(bool updateIM, ROCOrder[] orders, Dictionary<string, MDServerToClient> deltas)
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
					lock (_deltas)
					{
						foreach (string key in deltas.Keys)
						{
							if (_deltas.ContainsKey(key))
							{
								_deltas[key].Update(deltas[key]);
							}
							else
							{
								_deltas.Add(key, new MDServerToClient(deltas[key]));
							}
						}
					}
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

					_updateBuyAggregation = false;
					_updateSellAggregation = false;

					if (!_orderLoaded && rocAutoSpreadList.ReadyForTickTable && rocAutoSpreadList.Rows.Count > 0)
					{
						InitialOrderLoad();
					}
					else if (_orderLoaded)
					{
						if (orders.Length > 0)
						{
							UpdateOrders(orders);
						}
					}

					if (deltas.Count > 0)
					{
						UpdateMarketDataDeltas(deltas);
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
				if (!_updatingUI)
				{
					_updatingUI = true;
					if (_updateIM)
					{
						_updateIM = false;
						UpdateSecurityInfo();
					}

					_updateBuyAggregation = false;
					_updateSellAggregation = false;

					if (!_orderLoaded && rocAutoSpreadList.ReadyForTickTable && rocAutoSpreadList.Rows.Count > 0)
					{
						InitialOrderLoad();
					}
					else if (_orderLoaded)
					{
						ROCOrder[] orders = new ROCOrder[0];
						lock (_rocOrders)
						{
							if (_rocOrders.Count > 0)
							{
								orders = new ROCOrder[_rocOrders.Count];
								_rocOrders.CopyTo(orders, 0);
								_rocOrders.Clear();
							}
						}
						if (orders.Length > 0)
						{
							UpdateOrders(orders);
						}
					}

					Dictionary<string, MDServerToClient> deltas = new Dictionary<string, MDServerToClient>();
					lock (_deltas)
					{
						if (_deltas.Count > 0)
						{
							deltas = new Dictionary<string, MDServerToClient>(_deltas);
							_deltas.Clear();
						}
					}
					if (deltas.Count > 0)
					{
						UpdateMarketDataDeltas(deltas);
					}

					UpdateAggregation();
				}
				_updatingUI = false;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
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

				BaseSecurityInfo secInfo = null;
				List<string> removeList = new List<string>();

				lock (rocAutoSpreadListSettings.RocGridTable)
				{
					rocAutoSpreadListSettings.RocGridTable.BeginLoadData();

					foreach (string symbolDetail in lcoImSymbolNeeded.Keys)
					{
						secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
						if (secInfo != null)
						{
							UpdateIMInfo(symbolDetail, secInfo);

							if (!removeList.Contains(symbolDetail))
							{
								removeList.Add(symbolDetail);
							}
						}
					}

					rocAutoSpreadListSettings.RocGridTable.EndLoadData();
				}

				// Clean Up
				if (removeList.Count > 0)
				{
					lock (ImSymbolNeeded)
					{
						foreach (string symbolDetail in removeList)
						{
							if (ImSymbolNeeded.ContainsKey(symbolDetail))
							{
								ImSymbolNeeded.Remove(symbolDetail);
							}
						}
					}
				}
			}
		}

		private void UpdateIMInfo(string symbolDetail, BaseSecurityInfo secInfo)
		{
			HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecutrityTypes.Equity:
					if (!MDSymbols.Contains(secInfo.MDSymbol))
					{
						MDSymbols.Add(secInfo.MDSymbol);
					}
					break;
				case CSVFieldIDs.SecutrityTypes.Future:
					HelperSubscriber.SubscribeBook(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);

					if (!MDSymbols.Contains(secInfo.MDSymbol))
					{
						MDSymbols.Add(secInfo.MDSymbol);
					}
					break;
				default:
					if (MDSymbols.Contains(secInfo.MDSymbol))
					{
						MDSymbols.Remove(secInfo.MDSymbol);
					}
					break;
			}

			DataRow[] rows = rocAutoSpreadListSettings.RocGridTable.Select(string.Concat(new object[] { "SymbolDetail = '", symbolDetail, "'" }));

			if (rows.Length > 0)
			{
				foreach (DataRow row in rows)
				{
					row["Symbol"] = secInfo.MDSymbol;
					if (!rocAutoSpreadListSettings.Symbols.Contains(secInfo.MDSymbol))
					{
						rocAutoSpreadListSettings.Symbols.Add(secInfo.MDSymbol);
					}
					row["TickSize"] = secInfo.TickSize;
					row["SecType"] = secInfo.SecType;
					row["ContractSize"] = secInfo.ContractSize;
					row["Name"] = secInfo.LongName;
					row["Valid"] = 1;
				}
 
				// New valid symbol refresh needed
				newSymbol = true;
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
		private void UpdateOrders(ROCOrder[] orders)
		{
			bool hasOrder = false;
			bool hasBuy = false;
			bool hasSell = false;

			CheckOrders(_autoSpreadSupport.ID.ToString(), orders, out hasOrder, out hasBuy, out hasSell);
			UpdateOrders(hasOrder, hasBuy, hasSell);
		}
		private void UpdateOrders(bool hasOrder, bool hasBuy, bool hasSell)
		{
			if (hasOrder)
			{
				if (EchoID != _autoSpreadSupport.ID)
				{
					EchoID = _autoSpreadSupport.ID;
				}

				if (hasBuy)
				{
					UpdateOpenOrders("Buy", GetOpenPriceQty("Buy"));
					_updateBuyAggregation = true;
				}

				if (hasSell)
				{
					UpdateOpenOrders("Sell", GetOpenPriceQty("Sell"));
					_updateSellAggregation = true;
				}
			}
		}

		private Dictionary<int, ROCOrder> CheckOrders(string ID, ROCOrder[] orders, out bool hasOrder, out bool hasBuy, out bool hasSell)
		{
			hasOrder = false;
			hasBuy = false;
			hasSell = false;
			Dictionary<int, ROCOrder> spreadOrders = new Dictionary<int,ROCOrder>();

			// Find changes for this ticket
			TagKeyItems tagItems;
			foreach (ROCOrder order in orders)
			{
				// Debug
				//GLOBAL.HROC.AddToException("order status " + CSVFieldIDs.StatusCodes.Descriptions[order.Status]);

				if (order.ClientEcho == ID)
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

					if (!_lockedParentTags.Contains(order.ParentTag))
					{
						tagItems = new TagKeyItems(order.Tag);
						spreadOrders = GetSpreadOrders(order, tagItems);

						switch (order.Status)
						{
							case CSVFieldIDs.StatusCodes.Open:
								break;
							case CSVFieldIDs.StatusCodes.Rejected:
								foreach (ROCOrder spreadOrder in spreadOrders.Values)
								{
									if (spreadOrder.Tag != order.Tag)
									{
										// Cancel All Other orders in the sparead Order
										GLOBAL.HROM.CancelSingleOrder(spreadOrder.Tag);
									}
								}
								break;
							case CSVFieldIDs.StatusCodes.Filled:
							case CSVFieldIDs.StatusCodes.PartiallyFilled:
								if (spreadOrders.Count == _autoSpreadSupport.Legs)
								{
									if (IsLockableSpreadOrder(spreadOrders))
									{
										LockSpreadOrder(spreadOrders);
									}
									else
									{
										Nullable<KeyValuePair<int, double>> largestMultiple = null;
										Nullable<KeyValuePair<int, double>> smallestMultiple = null;

										GetLeaveMultiple(spreadOrders, out largestMultiple, out smallestMultiple);
										if (largestMultiple != null)
										{
											KeyValuePair<int, double> sm = (KeyValuePair<int, double>)smallestMultiple;

											if (Math.Floor(sm.Value) == 0)
											{
												// Lock the order
												LockSpreadOrder(spreadOrders);
											}
											else if (Math.Floor(sm.Value) == sm.Value)
											{
												// Cancel order then reopen order
												CancelSpreadOrdersByLeaveMultiple(spreadOrders, Convert.ToInt64(Math.Floor(sm.Value)));
											}
											else
											{
												// Replace order qty then reopen order
												ReplaceSpreadOrdersByLeaveMultiple(spreadOrders, Convert.ToInt64(Math.Floor(sm.Value)));
											}
										}
									}
								}
								break;
							default:
								// reopen order
								SplitSpreadOrders(order);
								break;
						}
					}
				}
			}
			return spreadOrders;
		}

		#endregion

		// Update On Play back & onProcess
		private void UpdateMarketDataDeltas(Dictionary<string, MDServerToClient> deltas)
		{
			bool recalculateSpread = false;
			bool checkBook = false;

			int[] legs = new int[_autoSpreadSupport.ItemByLegs.Keys.Count];
			_autoSpreadSupport.ItemByLegs.Keys.CopyTo(legs, 0);

			foreach (int leg in legs)
			{
				AutoSpreadItem item = _autoSpreadSupport.ItemByLegs[leg];
				checkBook = false;

				if (deltas.ContainsKey(item.SecInfo.MDSymbol))
				{
					// Update Level 1
					UpdateMarketDataDeltaL1(item.SecInfo.MDSymbol, ref item, deltas[item.SecInfo.MDSymbol]);
					recalculateSpread = true;
					checkBook = true;
				}

				string bookSymbol = "";
				switch (item.SecInfo.SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Future:
						// Update Book
						bookSymbol = "b" + item.SecInfo.MDSymbol;
						if (deltas.ContainsKey(bookSymbol))
						{
							UpdateMarketDataDeltaBook(leg, bookSymbol, ref item, deltas[bookSymbol]);
							recalculateSpread = true;
							checkBook = true;
						}
						break;
				}

				if (checkBook)
				{
					// Makeup the bid and ask book to level and synthetic size
					item.CheckLegBooks();

					_autoSpreadSupport.ItemByLegs[leg] = item;
				}
			}

			if (recalculateSpread)
			{
				ReclaculateSpread(legs);
			}			
		}

		private void ReclaculateSpread(int[] legs)
		{
			CalculateSpreadData(legs);

			UpdateSpreadBook();

			UpdateSpreadL1();
		}

		#region - Level 1 Update -

		// Get L1 Market Data By Process
		private void UpdateMarketDataDeltaL1(string symbol, ref AutoSpreadItem item, MDServerToClient delta)
		{
			if (item.IsNewL1 && GLOBAL.HMarketData.Current.ContainsKey(symbol))
			{
				// First Time
				item.MarketData.Update(GLOBAL.HMarketData.Current[symbol]);
				item.IsNewL1 = false;
			}
			else
			{
				// Update
				item.MarketData.Update(delta);
			}

			UpdateMarketDataDeltaBookFromL1(ref item);
		}

		private void UpdateMarketDataDeltaBookFromL1(ref AutoSpreadItem item)
		{
			double price = 0;
			long size = 0;
			if ((item.MarketData.BidBooks.Count == 0 || item.SecInfo.SecType != CSVFieldIDs.SecutrityTypes.Future) &&
				(item.MarketData.BidPrice != null && item.MarketData.BidSize != null))
			{
				price = (double)item.MarketData.BidPrice;
				switch (item.SecInfo.SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Equity:
						size = (long)item.MarketData.BidSize * 100;
						break;
					default:
						size = (long)item.MarketData.BidSize;
						break;
				}

				if (item.MarketData.BidBooks.ContainsKey(price))
				{
					item.MarketData.BidBooks[price]= size;
				}
				else
				{
					item.MarketData.BidBooks.Add(price, size);
				}
			}

			if ((item.MarketData.AskBooks.Count == 0 || item.SecInfo.SecType != CSVFieldIDs.SecutrityTypes.Future) &&
				(item.MarketData.AskPrice != null && item.MarketData.AskSize != null))
			{
				price = (double)item.MarketData.AskPrice;
				switch (item.SecInfo.SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Equity:
						size = (long)item.MarketData.AskSize * 100;
						break;
					default:
						size = (long)item.MarketData.AskSize;
						break;
				}

				if (item.MarketData.AskBooks.ContainsKey(price))
				{
					item.MarketData.AskBooks[price] = size;
				}
				else
				{
					item.MarketData.AskBooks.Add(price, size);
				}
			}
		}

		#endregion

		#region - Book Update -

		// Get Book Market Data By Process
		private void UpdateMarketDataDeltaBook(int leg, string bookSymbol, ref AutoSpreadItem item, MDServerToClient delta)
		{
			if (item.IsNewBook && GLOBAL.HMarketData.Current.ContainsKey(bookSymbol))
			{
				// First Time
				item.MarketData.Update(GLOBAL.HMarketData.Current[bookSymbol]);
				item.IsNewBook = false;
			}
			else
			{
				// Update
				item.MarketData.Update(delta);
			}
		}

		#endregion

		#region - Spread Calculation -

		private void CalculateSpreadData(int[] legs)
		{
			// Prime Leg must have a non zero factor
			if (_autoSpreadSupport.HasAllLegs && _autoSpreadSupport.ItemByLegs[_autoSpreadSupport.PrimeLeg].Factor != 0)
			{
				//DateTimeHP _dtHP = new DateTimeHP();
				//TimeSpan _pTimeSpan = new TimeSpan();

				//DateTime _pTime = _dtHP.Now;

				_autoSpreadSupport.CalculateSyntheticBookFromLegs(legs, "Ask");
				_autoSpreadSupport.CalculateSyntheticBookFromLegs(legs, "Bid");

				//_autoSpreadSupport.CalculateSyntheticTradedPriceByLegs(legs);
				_autoSpreadSupport.CalculateSyntheticLevel1PricesByLegs(legs);

				//_pTimeSpan = _dtHP.Now.Subtract(_pTime);
				//GLOBAL.HROC.AddToStatusLogs(string.Format("CalculateSyntheticTime {0,8}ms| 3Leg", _pTimeSpan.TotalMilliseconds));

				if (okToBuildQuickList && !rocAutoSpreadList.ReadyForTickTable)
				{
					lock (rocAutoSpreadList)
					{
						// Update TickSize 
						if (rocAutoSpreadList.TickSize != _autoSpreadSupport.ItemByLegs[_autoSpreadSupport.PrimeLeg].SecInfo.TickSize)
						{
							rocAutoSpreadList.TickSize = _autoSpreadSupport.ItemByLegs[_autoSpreadSupport.PrimeLeg].SecInfo.TickSize;
						}

						// Refresh Ladder
						rocAutoSpreadList.ReadyForTickTable = true;
						RefreshSpreadLadder();
					}
				}
			}
		}

		#endregion

		// Check to See What Price/Qty needs update
		private void UpdateOpenOrders(string side, OpenOrderItems priceItems)
		{
			lock (rocAutoSpreadList.RocGridTable)
			{
				DataRow[] oldRows = rocAutoSpreadList.RocGridTable.Select(side + "OpenQty > 0");

				double oldPrice;
				foreach (DataRow oldRow in oldRows)
				{
					oldPrice = (double)oldRow["Price"];

					if (!priceItems.OpenOrderPriceQty.ContainsKey(oldPrice))
					{
						// The Qty is no longer there
						oldRow[side + "OpenQty"] = DBNull.Value;
						// Reset Flag Stop Price
						oldRow[side + "StopFlag"] = DBNull.Value;

						// Debug
						//GLOBAL.HROC.AddToException("The Qty is no longer there");						
					}
					else
					{
						// Update Qty And Remove from the update
						if (oldRow[side + "OpenQty"] == DBNull.Value || (long)oldRow[side + "OpenQty"] != priceItems.OpenOrderPriceQty[oldPrice])
						{
							oldRow[side + "OpenQty"] = priceItems.OpenOrderPriceQty[oldPrice];
						}
						priceItems.OpenOrderPriceQty.Remove(oldPrice);

						// Debug
						//GLOBAL.HROC.AddToException("Update Qty And Remove from the update");
					}
				}

				// Only new prices left here
				foreach (double price in priceItems.OpenOrderPriceQty.Keys)
				{
					DataRowView[] rows = rocAutoSpreadList.PriceSearchView.FindRows(price);

					foreach (DataRowView row in rows)
					{
						if (priceItems.OpenOrderPriceQty[price] > 0)
						{
							row[side + "OpenQty"] = priceItems.OpenOrderPriceQty[price];
							// Flag Stop Price
							if (priceItems.OpenStopOrderPrice.Contains(price))
							{
								row[side + "StopFlag"] = PriceFlages.stop;
							}

							// Debug
							//GLOBAL.HROC.AddToException("OpenQty = " + row[side + "OpenQty"].ToString());
						}
					}
				}
			}
		}

		private void UpdateAggregation()
		{
			long filledBuyQty = 0;
			long filledSellQty = 0;
			long openBuyQty = 0;
			long openSellQty = 0;

			if (_updateBuyAggregation)
			{
				_openBuyQty = openBuyQty;
			}

			if (_updateSellAggregation)
			{
				_openSellQty = openSellQty;
			}

			if (_updateBuyAggregation || _updateSellAggregation)
			{
				_netFilledQty = filledBuyQty - filledSellQty;
			}
		}

		#endregion

		#region - Order Execution -

		private void cmdBuy_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString());
		}

		private void cmdSell_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString());
		}

		private void BuildOrder(string side)
		{
			long orderType = GetOrderTypeCode(cboOrder.Text);
			long duration = GetDurationCode(cboDuration.Text);
			string masterOrderID = ROCOrderTypes.AutoSpread + GLOBAL.HROM.RomMessageMaker.GetOrderID(GLOBAL.HROM.UserName);
			string masterSide = side;

			switch (orderType)
			{
				#region - Market -

				case CSVEx.CSVFieldIDs.OrderTypes.Market:
					BuildOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(), 
						masterOrderID,
						masterSide);
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					BuildOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString(), 
						masterOrderID, 
						masterSide);
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					BuildOrder(side,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString(), 
						masterOrderID, 
						masterSide);
					break;

				#endregion

				#region - STOP -

				case CSVEx.CSVFieldIDs.OrderTypes.Stop:
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						"",
						numStopPrice.Value.ToString(),
						masterOrderID,
						masterSide);
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.StopLimit:
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						numStopPrice.Value.ToString(),
						masterOrderID,
						masterSide);
					break;

				#endregion

				default:
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						"", 
						masterOrderID,
						masterSide);
					break;
			}
		}
		private void BuildOrder(string side, string orderType, string duration, string masterOrderID, string masterSide)
		{
			BuildOrder(side, orderType, duration, "", "", masterOrderID, masterSide);
		}
		private void BuildOrder(string side, string orderType, string duration, string limitPrice, string masterOrderID, string masterSide)
		{
			BuildOrder(side, orderType, duration, limitPrice, "", masterOrderID, masterSide);
		}
		private void BuildOrder(string side, string orderType, string duration, string limitPrice, string stopPrice, string masterOrderID, string masterSide)
		{
			List<RomBasicOrder> orders = new List<RomBasicOrder>();
			
			DestinationMap dest = new DestinationMap();
			AccountMap acct = new AccountMap();

			try
			{
				if ((DDAccountDropDown.GetCurrentAccount(ref acct) && DDAccountDropDown.GetCurrentExchange(ref dest) && dest.destID != null) || EquityOnly)
				{
					bool hasError = false;
					long baseQty = Convert.ToInt64(numQuantity.Value);

					foreach (AutoSpreadItem item in _autoSpreadSupport.ItemByLegs.Values)
					{
						RomBasicOrder order = new RomBasicOrder();
						string limitMarketPrice = "";
						string stopMarketPrice = "";

						bool isShort = false;
						order.side = GetLegSideCode(side, item, ref isShort);
						if (isShort)
						{
							order.shortLender = item.ShortLender;
						}

						order.parentTag = GetParentTag(masterOrderID, limitPrice, stopPrice, masterSide, baseQty.ToString());
						order.clientEcho = _autoSpreadSupport.ID.ToString();

						switch (item.SecInfo.SecType)
						{
							case CSVEx.CSVFieldIDs.SecutrityTypes.Future:
								order.symbol = item.SymbolDetail;
								order.mdSymbol = item.SecInfo.MDSymbol;
								order.secType = item.SecInfo.SecType;
								order.underlying = item.SecInfo.Underlying;
								order.expDate = item.SecInfo.Expiration;
								break;
							case CSVEx.CSVFieldIDs.SecutrityTypes.Equity:
							default:
								order.symbol = item.SymbolDetail;
								order.mdSymbol = item.SecInfo.MDSymbol;
								order.secType = item.SecInfo.SecType;
								break;
						}

						order.trader = GLOBAL.HROM.UserName;
						order.shares = GetLegQty(baseQty, item);
						if (order.shares == "")
						{
							GLOBAL.HROC.AddToException("AutoSpread Leg Fractional Share, Orders Not Sent.");
							hasError = true;
							break;
						}
						if (limitPrice != "")
						{
							order.price = GetLegPrice(order.side, limitPrice, item, out limitMarketPrice);
							if (order.price == "" || limitMarketPrice == "")
							{
								GLOBAL.HROC.AddToException("AutoSpread Leg Price Error, Orders Not Sent.");
								hasError = true;
								break;
							}
						}
						if (stopPrice != "")
						{
							order.stopPrice = GetLegPrice(order.side, stopPrice, item, out stopMarketPrice);
							if (order.stopPrice == "" || stopMarketPrice == "")
							{
								GLOBAL.HROC.AddToException("AutoSpread Leg StopPrice Error, Orders Not Sent.");
								hasError = true;
								break;
							}
						}
						// [Type]OrderID;LegNumber;LimitMarketPrice;StopMarketPrice
						order.orderID = GetOrderID(masterOrderID, item.LegNumber.ToString(), limitMarketPrice.ToString(), stopMarketPrice.ToString());

						if (item.Exchange != "" && item.LocalAcctAcrn != "" && item.TradeFor != "")
						{
							// User Specified
							order.tradeFor = item.TradeFor;
							order.localAcctAcrn = item.LocalAcctAcrn;
							order.exchangeID = item.Exchange;
							VerifyTrader(ref order);
						}
						else
						{
							order.tradeFor = DDAccountDropDown.CurrentTradeFor;
							order.exchangeID = dest.destID.ToString();
							order.cmtaFirmID = dest.cmtaFirmID;
							order.capacity = acct.capacity;
							order.clearingAcctID = acct.clearingAcID;
							order.clearingFirmID = acct.clearingFirmID;
							order.firmArc = acct.firmAcr;
							order.localAcctAcrn = acct.localAcAcrn;
						}

						order.orderType = orderType;
						order.duration = duration;

						orders.Add(order);
					}

					if (!hasError)
					{
						foreach (RomBasicOrder order in orders)
						{
							GLOBAL.HROM.EnterOrder(order, true);
						}
					}
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
		private void BuildOrder(ROCOrder orgOrder, string newOrderID, string newParentTag, string splitOffQty)
		{
			bool useItemAccountInfo = false;
			AccountMap acct = new AccountMap();

			if (DDAccountDropDown.GetCurrentAccount(orgOrder.LocalAcct, ref acct) || EquityOnly)
			{
				// For Split off Qty
				RomBasicOrder order = new RomBasicOrder();
				order.orderID = newOrderID;
				order.parentTag = newParentTag;

				order.side = orgOrder.Side.ToString();
				order.clientEcho = orgOrder.ClientEcho;

				switch (orgOrder.SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Future:
						order.symbol = orgOrder.SymbolDetail;
						order.mdSymbol = orgOrder.Symbol;
						order.secType = orgOrder.SecType;
						order.underlying = orgOrder.Underlying;
						order.expDate = orgOrder.ExpDate;
						break;
					case CSVFieldIDs.SecutrityTypes.Equity:
					default:
						order.symbol = orgOrder.SymbolDetail;
						order.mdSymbol = orgOrder.Symbol;
						order.secType = orgOrder.SecType;
						break;
				}
				order.shares = splitOffQty;
				order.price = orgOrder.Price.ToString();
				order.stopPrice = orgOrder.StopPrice.ToString();
				order.orderType = orgOrder.OrderType.ToString();
				order.duration = orgOrder.TIF.ToString();

				order.tradeFor = orgOrder.TradeFor;
				order.trader = orgOrder.Trader;
				order.cmtaFirmID = orgOrder.CMTAFirmID;
				order.exchangeID = orgOrder.DestID.ToString();

				foreach (AutoSpreadItem item in _autoSpreadSupport.ItemByLegs.Values)
				{
					if (item.SymbolDetail == orgOrder.SymbolDetail)
					{
						if (item.Exchange != "" && item.LocalAcctAcrn != "" && item.TradeFor != "")
						{
							// User Specified
							order.tradeFor = item.TradeFor;
							order.localAcctAcrn = item.LocalAcctAcrn;
							order.exchangeID = item.Exchange;
							VerifyTrader(ref order);

							useItemAccountInfo = true;
						}

						if (item.ShortLender != "")
						{
							order.shortLender = item.ShortLender;
						}
					}
				}

				if (!useItemAccountInfo)
				{
					order.capacity = acct.capacity;
					order.clearingAcctID = acct.clearingAcID;
					order.clearingFirmID = acct.clearingFirmID;
					order.firmArc = acct.firmAcr;
					order.localAcctAcrn = acct.localAcAcrn;
				}
				
				GLOBAL.HROM.EnterOrder(order, true);
			}
		}

		private void CancelOrderByPrice(string side, double price)
		{
			lock (GLOBAL.HOrders)
			{
				// Get All Open Orders
				DataRow[] rows = GLOBAL.HOrders.Table.Select(GetOpenOrderFilter(), "Tag");
				List<string> parentTags = GetParentTags(rows);

				foreach (string parentTag in parentTags)
				{
					ParentTagKeyItems pTagItems = new ParentTagKeyItems(parentTag);
					Dictionary<int, ROCOrder> orders = GetSpreadOrders(parentTag, false);

					if (pTagItems.LimitSpreadPriceDouble != null && 
						(double)pTagItems.LimitSpreadPriceDouble == price &&
						pTagItems.MasterSideSpreadName == side)
					{
						foreach (ROCOrder order in orders.Values)
						{
							GLOBAL.HROM.CancelSingleOrder(order.Tag);
						}
					}
				}
			}
		}
		private void CancelOrderBySide(long side)
		{
			lock (GLOBAL.HOrders)
			{
				// Get All Open Orders
				DataRow[] rows = GLOBAL.HOrders.Table.Select(GetOpenOrderFilter(), "Tag");
				List<string> parentTags = GetParentTags(rows);

				foreach (string parentTag in parentTags)
				{
					Dictionary<int, ROCOrder> orders = GetSpreadOrders(parentTag, false);
					
					if (orders.ContainsKey(_autoSpreadSupport.PrimeLeg) && 
						orders[_autoSpreadSupport.PrimeLeg].Side != null &&
						(long)orders[_autoSpreadSupport.PrimeLeg].Side == side)
					{
						foreach (ROCOrder order in orders.Values)
						{
							GLOBAL.HROM.CancelSingleOrder(order.Tag);
						}
					}
				}
			}
		}

		#endregion

		#region - Ticket Display -

		private void ValidateTicketValues()
		{
			_isInError = false;

			if (!_isLoadingValue)
			{
				IsInError(SetState(cmdTradeFor, (cmdTradeFor.Text == "N/A" || cmdTradeFor.Text == "")) || EquityOnly);
				IsInError(SetState(cmdAccount, (cmdAccount.Text == "N/A" || cmdAccount.Text == "")) || EquityOnly);
				IsInError(SetState(lblExchange, (cboExchange.Text == "")) || EquityOnly);
				SetPanelWidth(ref panelExchange, cboExchange);

				try
				{
					_orderTypesView.RowFilter = TicketSupport.BuildOrderTypeFilter(cboExchange, cboOrder);
					BuildDurationTypeFilter();

					SetTicketStateByOrderType();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				IsInError(SetState(lblQuantity, numQuantity.Value <= 0));
				SetPanelWidth(ref panelShares, numQuantity);
				rocAutoSpreadList.Qty = Convert.ToInt64(numQuantity.Value);

				IsInError(SetState(numQuantity, numQuantity.Value <= 0));

				if (panelLimitPrice.Visible)
				{
					IsInError(SetState(lblLimitPrice, false));
					IsInError(SetState(numLimitPrice, false));
					SetPanelWidth(ref panelLimitPrice, numLimitPrice);
				}

				if (panelStopPrice.Visible)
				{
						IsInError(SetState(lblStopPrice, false));
						IsInError(SetState(numStopPrice, false));
						SetPanelWidth(ref panelStopPrice, numStopPrice);
				}

				IsInError(SetState(lblOrderType, cboOrder.Text == ""));
				SetPanelWidth(ref panelOrderTypes, cboOrder);

				IsInError(SetState(lblDuration, cboDuration.Text == ""));
				SetPanelWidth(ref panelDuration, cboDuration);

				BuildOrderMsg();

				GetSelectedPriceForLegs();
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

			LongName = _autoSpreadSupport.LongName;

			// 0 Quantity
			// 1 Symbol
			// 2 OrderType
			// 3 Limit Price / Stop Price
			// 4 Duration
			// 5 Exchange
			dspOrderMsg.Text = "";
			dspOrderMsg.SelectionAlignment = HorizontalAlignment.Center;
			dspOrderMsg.SelectionBackColor = dspOrderMsg.BackColor;

			//string orderSymbol = cboSymbolDetails.Text;
			string orderSymbol = "SP";

			if (panelLimitPrice.Visible && panelStopPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {5} - {6} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					numLimitPrice.Value.ToString("######0.00#####"),
					numStopPrice.Value.ToString("######0.00#####"));
			}
			else if (panelLimitPrice.Visible && !panelStopPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {5} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					numLimitPrice.Value.ToString("######0.00#####"));
			}
			else if (!panelLimitPrice.Visible && panelStopPrice.Visible)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {2} {5} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					numStopPrice.Value.ToString("######0.00#####"));
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

		private void ResetTicketValues()
		{
			_isLoadingValue = true;
			_centered = false;
			_ladderHasRows = false;

			_openBuyQty = 0;
			_openSellQty = 0;
			_netFilledQty = 0;

			dspAskPrice.Value = 0.00;
			dspBidPrice.Value = 0.00;
			dspTradedPrice.Value = 0.00;

			_autoSpreadSupport.SyntheticAskPrice = 0;
			_autoSpreadSupport.SyntheticBidPrice = 0;
			_autoSpreadSupport.SyntheticTradedPrice = 0;

			_buyOrdersByTag.Clear();
			_sellOrdersByTag.Clear();

			CurrentSecInfo = new BaseSecurityInfo();

			LongName = "Auto Spread Ticket";
			EchoID = 0;

			dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;

			//cboOrder.Text = "Limit";
			//cboDuration.Text = "DAY";
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Future, CSVEx.CSVFieldIDs.SecutrityTypes.Future));

			_isLoadingValue = false;

			rocAutoSpreadList.TradePrice = 0;
			rocAutoSpreadList.HighPrice = 0;
			rocAutoSpreadList.LowPrice = 0;
			rocAutoSpreadList.BidPrice = 0;
			rocAutoSpreadList.AskPrice = 0;
			rocAutoSpreadList.TickSize = CurrentSecInfo.TickSize;
			rocAutoSpreadList.ReadyForTickTable = false;

			if (rocAutoSpreadList.RocGridTable != null)
			{
				lock (rocAutoSpreadList.RocGridTable)
				{
					rocAutoSpreadList.ResetMaxBookSize();
					rocAutoSpreadList.RocGridTable.Clear();
				}

				((DGVQuickListNumberColumn)rocAutoSpreadList.Columns["Price"]).MaxDecimal = 2;
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

			numLimitPrice.Value = data.limitPrice;
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

		#region - Ladder Grid Functions -

		private void ClearLadder()
		{
			_orderLoaded = false;

			lock (rocAutoSpreadList.RocGridTable)
			{
				rocAutoSpreadList.RocGridTable.Rows.Clear();
			}
		}

		private void RefreshSpreadLadder()
		{
			ClearLadder();			
			UpdateTicket();
			UpdateSpreadBook();
		}

		private void UpdateTicket()
		{
			numTickSize.Value = Convert.ToDecimal(rocAutoSpreadList.TickSize);

			// Limit Price
			numLimitPrice.Increment = numTickSize.Value;
			dspLimitPriceTickSize.Value = rocAutoSpreadList.TickSize;

			// Stop Price
			numStopPrice.Increment = numTickSize.Value;
			dspStopPriceTickSize.Value = rocAutoSpreadList.TickSize;

			CurrentSecInfo.TickSize = rocAutoSpreadList.TickSize;

			LongName = _autoSpreadSupport.LongName;

			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_autoSpreadSupport.PrimeSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Future));
		}

		private void UpdateSpreadBook()
		{
			SetupTickTable(rocAutoSpreadList);
			lock (_autoSpreadSupport.SyntheticBidBook)
			{
				UpdateTickTable(rocAutoSpreadList, _autoSpreadSupport.SyntheticBidBook, "Bid");
			}
			//GLOBAL.HROC.AddToStatusLogs("_autoSpreadSupport.SyntheticBidBook.Count = " + _autoSpreadSupport.SyntheticBidBook.Count.ToString());
			lock (_autoSpreadSupport.SyntheticAskBook)
			{
				UpdateTickTable(rocAutoSpreadList, _autoSpreadSupport.SyntheticAskBook, "Ask");
			}
			//GLOBAL.HROC.AddToStatusLogs("_autoSpreadSupport.SyntheticAskBook.Count = " + _autoSpreadSupport.SyntheticAskBook.Count.ToString());
		}

		private void UpdateSpreadL1()
		{
			bool refreshSelectedLegPrice = false;

			try
			{
				if (dspBidPrice.Value.ToString() == "" || (double)dspBidPrice.Value != _autoSpreadSupport.SyntheticBidPrice)
				{
					dspBidPrice.Value = _autoSpreadSupport.SyntheticBidPrice;
					refreshSelectedLegPrice = true;
					if (_ladderHasRows)
					{
						rocAutoSpreadList.FlagBidPriceFromSpredPrice(rocAutoSpreadList.BidPrice, _autoSpreadSupport.SyntheticBidPrice);
						rocAutoSpreadList.CheckBook(_autoSpreadSupport.SyntheticBidPrice, "Bid");
					}
				}

				if (dspAskPrice.Value.ToString() == "" || (double)dspAskPrice.Value != _autoSpreadSupport.SyntheticAskPrice)
				{
					dspAskPrice.Value = _autoSpreadSupport.SyntheticAskPrice;
					refreshSelectedLegPrice = true;
					if (_ladderHasRows)
					{
						rocAutoSpreadList.FlagAskPriceFromSpredPrice(rocAutoSpreadList.AskPrice, _autoSpreadSupport.SyntheticAskPrice);
						rocAutoSpreadList.CheckBook(_autoSpreadSupport.SyntheticAskPrice, "Ask");
					}
				}

				if (dspTradedPrice.Value.ToString() == "" || (double)dspTradedPrice.Value != _autoSpreadSupport.SyntheticTradedPrice)
				{
					dspTradedPrice.Value = _autoSpreadSupport.SyntheticTradedPrice;
					if (_ladderHasRows)
					{
						if (_autoSpreadSupport.SyntheticTradedPrice > rocAutoSpreadList.TradePrice)
						{
							rocAutoSpreadList.FlagTradedPriceFromSpredPrice(rocAutoSpreadList.TradePrice, _autoSpreadSupport.SyntheticTradedPrice, "+");
						}
						else
						{
							rocAutoSpreadList.FlagTradedPriceFromSpredPrice(rocAutoSpreadList.TradePrice, _autoSpreadSupport.SyntheticTradedPrice, "-");
						}

						if (!_centered)
						{
						    _centered = true;
							rocAutoSpreadList.CenterOnPrice(rocAutoSpreadList.TradePrice);
						}
					}
				}

				if (refreshSelectedLegPrice)
				{
					GetSelectedPriceForLegs();

					// Do this on price change
					ReplaceSpreadOrders();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void SetupTickTable(ROCAutoSpreadList grid)
		{
			if (rocAutoSpreadList.ReadyForTickTable && okToBuildQuickList && rocAutoSpreadList.RocGridTable.Rows.Count == 0)
			{
				lock (grid.RocGridTable)
				{
					grid.SetupTickTable(Math.Round(_autoSpreadSupport.SyntheticAskBook[0].Price, 2), _autoSpreadSupport.BookDepthLimit);
				}
			}
		}

		private void UpdateTickTable(ROCAutoSpreadList grid, Dictionary<int, PriceVolumePair> book, string side)
		{
			if (rocAutoSpreadList.ReadyForTickTable && okToBuildQuickList)
			{
				lock (grid.RocGridTable)
				{
					double limitPrice = book[0].Price;

					rocAutoSpreadList.CheckBook(limitPrice, side);

					foreach (int level in book.Keys)
					{
						double price = Math.Round(book[level].Price, 2);

						if (grid.RocGridTable.Rows.Count > 0)
						{
							_ladderHasRows = true;
						}

						switch (side)
						{
							case "Bid":
								if (price <= limitPrice && book[level].Volume != 0)
								{
									grid.SetBookSize(grid.CalTickPriceFromSpreadPrice(price, side), book[level].Volume, side);
								}
								break;
							case "Ask":
								if (price >= limitPrice && book[level].Volume != 0)
								{
									grid.SetBookSize(grid.CalTickPriceFromSpreadPrice(price, side), book[level].Volume, side);
								}
								break;
						}
					}
				}
			}
		}

		#endregion

		#region - Auto Order Replace Function -

		private string GetOpenOrderFilter()
		{
			return GetOpenOrderFilter("", false);
		}
		private string GetOpenOrderFilter(bool zeroCumQtyOnly)
		{
			return GetOpenOrderFilter("", zeroCumQtyOnly);
		}
		private string GetOpenOrderFilter(string parentTag, bool zeroCumQtyOnly)
		{
			string result = string.Format("Echo = '{0}'", _autoSpreadSupport.ID);
			if (parentTag != "")
			{
				result = result + string.Format(" And ParentTag = '{0}'", parentTag);
			}
			result = result + string.Format(" And ({0})", CSVFieldIDs.StatusCodes.ActiveStatusFilter);
			if (zeroCumQtyOnly)
			{
				result = result + " And CumQty = 0";
			}

			return result;
		}
		private string GetOpenOrderFilter(long side)
		{
			string result = string.Format("Echo = '{0}'", _autoSpreadSupport.ID);
			result = result + string.Format(" And ({0})", CSVFieldIDs.StatusCodes.ActiveStatusFilter);
			switch (side)
			{
				case CSVFieldIDs.SideCodes.Buy:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.BuySideFilter);
					break;
				case CSVFieldIDs.SideCodes.Sell:
					result = result + string.Format(" And ({0})", CSVFieldIDs.SideCodes.SellSideFilter);
					break;
			}

			return result;
		}

		private string GetAllOrderFilter(string parentTag)
		{
			return GetAllOrderFilter(parentTag, false);
		}
		private string GetAllOrderFilter(string parentTag, bool activeOnly)
		{
			string result = string.Format("Echo = '{0}'", _autoSpreadSupport.ID);
			if (parentTag != "")
			{
				result = result + string.Format(" And ParentTag = '{0}'", parentTag);
			}
			if (activeOnly)
			{
				result = result + string.Format(" And ({0})", CSVFieldIDs.StatusCodes.ActiveStatusFilter);
			}

			return result;
		}

		// Replace Order by Market Data
		private void ReplaceSpreadOrders()
		{
			// Get All Open Orders and CumQty == 0 for This Spread
			DataRow[] rows = new DataRow[0];
			lock (GLOBAL.HOrders.Table)
			{
				rows = GLOBAL.HOrders.Table.Select(GetOpenOrderFilter(true), "Tag");
			}
			List<string> parentTags = GetParentTags(rows);

			Nullable<double> orgLimitSpreadPrice = null;
			Nullable<double> orgStopSpreadPrice = null;
			foreach (string parentTag in parentTags)
			{
				if (_lockedParentTags.Contains(parentTag))
				{
					// the spread group is already locked, skip it.
					continue;
				}

				Dictionary<int, ROCOrder> orders = GetSpreadOrders(parentTag, false);

				if (orders.Count == _autoSpreadSupport.Legs)
				{
					// Get the origional spread price from parent tag
					ParentTagKeyItems pTagItems = new ParentTagKeyItems(parentTag);
					orgLimitSpreadPrice = pTagItems.LimitSpreadPriceDouble;
					orgStopSpreadPrice = pTagItems.StopSpreadPriceDouble;

					if (orgLimitSpreadPrice != null)
					{
						ReplaceSpreadOrders(orders, (double)orgLimitSpreadPrice, orgStopSpreadPrice);
					}
				}
			}
		}
		private void ReplaceSpreadOrders(Dictionary<int, ROCOrder> orders, double orgLimitSpreadPrice, Nullable<double> orgStopSpreadPrice)
		{
			string newLimitPrice = "";
			string newStopPrice = "";
			string newLimitMarketPrice = "";
			string newStopMarketPrice = "";

			lock (_replacingOrders)
			{
				foreach (int legNumber in orders.Keys)
				{
					switch (orders[legNumber].Status)
					{
						case CSVFieldIDs.StatusCodes.Rejected:
						case CSVFieldIDs.StatusCodes.Filled:
						case CSVFieldIDs.StatusCodes.FilledAndCancelled:
						case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
							break;
						default:
							newLimitPrice = GetLegPrice((long)orders[legNumber].Side, orgLimitSpreadPrice, legNumber, out newLimitMarketPrice);
							if (newLimitMarketPrice != "")
							{
								_autoSpreadSupport.NewLimitMarketPriceFor(orders[legNumber].Tag, newLimitMarketPrice);
							}

							if (orgStopSpreadPrice != null)
							{
								newStopPrice = GetLegPrice((long)orders[legNumber].Side, (double)orgStopSpreadPrice, legNumber, out newStopMarketPrice);
								if (newStopMarketPrice != "")
								{
									_autoSpreadSupport.NewStopMarketPriceFor(orders[legNumber].Tag, newStopMarketPrice);
								}
							}

							if (newLimitPrice != "")
							{
								if (!_replacingOrders.ContainsKey(orders[legNumber].Tag))
								{
									_replacingOrders.Add(orders[legNumber].Tag, null);
									GLOBAL.HROM.ReplaceOrder(orders[legNumber].Tag, newLimitPrice, newStopPrice);
								}
							}
							break;
					}
				}
			}
		}
		private List<string> GetParentTags(DataRow[] rows)
		{
			// Make a list of Unique ParentTags
			List<string> parentTags = new List<string>();
			foreach (DataRow row in rows)
			{
				if (row["ParentTag"] != DBNull.Value && row["ParentTag"].ToString() != "")
				{
					if (!parentTags.Contains((string)row["ParentTag"]))
					{
						parentTags.Add((string)row["ParentTag"]);
					}
				}
			}

			return parentTags;
		}

		// Lock Order by Filled Order
		private bool IsLockableSpreadOrder(Dictionary<int, ROCOrder> orders)
		{
			// Check if the any of the spread leg is filled or not
			// If filled then lock down the entire spread order.
			bool result = false;

			foreach (ROCOrder order in orders.Values)
			{
				switch (order.Status)
				{
					case CSVFieldIDs.StatusCodes.Rejected:
					case CSVFieldIDs.StatusCodes.Filled:
					case CSVFieldIDs.StatusCodes.FilledAndCancelled:
					case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
						lock (_lockedParentTags)
						{
							if (!_lockedParentTags.Contains(order.ParentTag))
							{
								_lockedParentTags.Add(order.ParentTag);
							}
						}
						return true;
				}
			}

			return result;
		}
		private void LockSpreadOrder(Dictionary<int, ROCOrder> orders)
		{
			foreach (ROCOrder order in orders.Values)
			{
				switch (order.Status)
				{
					case CSVFieldIDs.StatusCodes.Rejected:
					case CSVFieldIDs.StatusCodes.Filled:
					case CSVFieldIDs.StatusCodes.FilledAndCancelled:
					case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
						break;
					default:
						TagKeyItems tagItems = new TagKeyItems(order.Tag);
						if (order.Price.ToString() != tagItems.LimitMarketPrice || order.StopPrice.ToString() != tagItems.StopMarketPrice)
						{
							lock (_replacingOrders)
							{
								// The order is already locked don't replace it anymore
								if (!_replacingOrders.ContainsKey(order.Tag))
								{
									_replacingOrders.Add(order.Tag, null);
									GLOBAL.HROM.ReplaceOrder(order.Tag, tagItems.LimitMarketPrice, tagItems.StopMarketPrice);
								}
							}
						}
						break;
				}
			}
		}

		// Replace Order by partially filled order
		private void GetLeaveMultiple(Dictionary<int, ROCOrder> orders, out Nullable<KeyValuePair<int, double>> largestMultiple, out Nullable<KeyValuePair<int, double>> smallestMultiple)
		{
			Dictionary<int, double> leaveMultiples = new Dictionary<int,double>();
			foreach (ROCOrder order in orders.Values)
			{
				TagKeyItems tagItems = new TagKeyItems(order.Tag);

				if (tagItems.LegNumberInt != null)
				{
					int legNumber = (int)tagItems.LegNumberInt;
					double multiple =  Math.Abs(order.LeaveQty / _autoSpreadSupport.ItemByLegs[legNumber].Factor);
					if (leaveMultiples.ContainsKey(legNumber))
					{
						leaveMultiples[legNumber] = multiple;
					}
					else
					{
						leaveMultiples.Add(legNumber, multiple);
					}
				}
			}

			// Get the largest multiples
			largestMultiple = null;
			smallestMultiple = null;

			bool gotLargestMultiple = false;
			bool gotSmallestMultiple = false;
			foreach (KeyValuePair<int, double> kvp in leaveMultiples)
			{
				// get the smallest multipe (when orders needs to be cancelled)
				if (smallestMultiple == null)
				{
					smallestMultiple = new KeyValuePair<int, double>(kvp.Key, kvp.Value);
					gotSmallestMultiple = true;
				}
				else
				{
					if (((KeyValuePair<int, double>)smallestMultiple).Value > kvp.Value)
					{
						smallestMultiple = new KeyValuePair<int, double>(kvp.Key, kvp.Value);
					}
				}

				if (largestMultiple == null)
				{
					largestMultiple = new KeyValuePair<int, double>(kvp.Key, kvp.Value);
					gotLargestMultiple = true;
				}
				else
				{
					if (((KeyValuePair<int, double>)largestMultiple).Value < kvp.Value)
					{
						largestMultiple = new KeyValuePair<int, double>(kvp.Key, kvp.Value);
					}
				}
			}

			if (!gotLargestMultiple)
			{
				smallestMultiple = null;
			}

			if (!gotSmallestMultiple)
			{
				smallestMultiple = null;
			}
		}
		private long GetOrigionalMultiple(long orginalQty, int legNumber)
		{
			return Convert.ToInt64(Math.Abs(Math.Floor(orginalQty / _autoSpreadSupport.ItemByLegs[legNumber].Factor)));
		}

		private void ReplaceSpreadOrdersByLeaveMultiple(Dictionary<int, ROCOrder> orders, long replaceMultiple)
		{
			if (replaceMultiple > 0)
			{
				string replaceQty = "";

				lock (_replacingOrders)
				{
					foreach (int legNumber in orders.Keys)
					{
						replaceQty = GetQtyFromMultiple(orders[legNumber], replaceMultiple, _autoSpreadSupport.ItemByLegs[legNumber].Factor, false);
						if (replaceQty != "")
						{
							if (!_replacingOrders.ContainsKey(orders[legNumber].Tag))
							{
								// save the origional qty of the order
								_replacingOrders.Add(orders[legNumber].Tag, orders[legNumber].Qty);
								TagKeyItems tagItems = new TagKeyItems(orders[legNumber].Tag);
								// reduce qty to multiple and lock price to market
								GLOBAL.HROM.ReplaceOrder(orders[legNumber].Tag, replaceQty, tagItems.LimitMarketPrice, tagItems.StopMarketPrice);
							}
						}
					}
				}
			}
		}
		private void CancelSpreadOrdersByLeaveMultiple(Dictionary<int, ROCOrder> orders, long leaveMultiple)
		{
			string newQty = "";

			lock (_replacingOrders)
			{
				foreach (int legNumber in orders.Keys)
				{
					newQty = GetQtyFromMultiple(orders[legNumber], leaveMultiple, _autoSpreadSupport.ItemByLegs[legNumber].Factor, true);
					if (newQty == "")
					{
						// Can not replace qty to factor, cancell the order and then reopen
						if (!_replacingOrders.ContainsKey(orders[legNumber].Tag))
						{
							_replacingOrders.Add(orders[legNumber].Tag, orders[legNumber].Qty);
							// save the origional qty of the order
							if (orders[legNumber].Qty.ToString() != newQty)
							{
								GLOBAL.HROM.CancelSingleOrder(orders[legNumber].Tag);
								// Debug
								//GLOBAL.HROC.AddToStatusLogs("CancelSingleOrder " + orders[legNumber].Tag);
							}
						}
					}
					else
					{
						// Still can replace qty
						if (!_replacingOrders.ContainsKey(orders[legNumber].Tag))
						{
							_replacingOrders.Add(orders[legNumber].Tag, orders[legNumber].Qty);
							TagKeyItems tagItems = new TagKeyItems(orders[legNumber].Tag);
							// reduce qty to multiple and lock price to market
							GLOBAL.HROM.ReplaceOrder(orders[legNumber].Tag, newQty, tagItems.LimitMarketPrice, tagItems.StopMarketPrice);
							// Debug
							//GLOBAL.HROC.AddToStatusLogs("ReplaceOrder " + orders[legNumber].Tag);
						}
					}
				}
			}
		}
		private string GetQtyFromMultiple(ROCOrder order, long multiple, double factor, bool forCancel)
		{
			long result = order.Qty - Convert.ToInt64(Math.Floor(Math.Abs(factor) * multiple));
			if (forCancel)
			{
				result = result - order.CumQty;
			}

			if (result > 0)
			{
				return result.ToString();
			}

			return "";
		}

		private void SplitSpreadOrders(ROCOrder order)
		{
			Nullable<long> orgTotalQty = null;
			lock (_replacingOrders)
			{
				if (_replacingOrders.ContainsKey(order.Tag))
				{
					orgTotalQty = _replacingOrders[order.Tag];
					_replacingOrders.Remove(order.Tag);
				}
			}

			if (orgTotalQty != null)
			{
				// Make a new order out of qty need to refill the origional qty
				long currentTotalQty = order.CumQty + order.LeaveQty;
				long newQty = (long)orgTotalQty - currentTotalQty;

				if (newQty > 0)
				{
					TagKeyItems tagItems = new TagKeyItems(order.Tag);
					string newOrderID = tagItems.MasterOrderID + "+";
					ParentTagKeyItems ptagItems = new ParentTagKeyItems(order.ParentTag);
					string newParentTag = GetParentTag(newOrderID, ptagItems);
					newOrderID = GetOrderID(newOrderID, tagItems);
					BuildOrder(order, newOrderID, newParentTag, newQty.ToString());
					// Debug
					//GLOBAL.HROC.AddToStatusLogs("SplitSpreadOrders " + order.Tag);
				}
				else
				{
					// Debug
					//GLOBAL.HROC.AddToStatusLogs("Did not SplitSpreadOrders" + order.Tag);
				}
			}
		}

		private Dictionary<int, ROCOrder> GetSpreadOrders(ROCOrder currentOrder, TagKeyItems tagItems)
		{
			if (tagItems.LegNumberInt == null)
			{
				// This is not a spread order without leg number
				//GLOBAL.HROC.AddToException(currentOrder.Tag + " Can not find leg number!");
				return new Dictionary<int, ROCOrder>();
			}

			return GetSpreadOrders(currentOrder.ParentTag, true);
		}
		private Dictionary<int, ROCOrder> GetSpreadOrders(string parentTag, bool getAllOrders)
		{
			DataRow[] rows = new DataRow[0];

			lock (GLOBAL.HOrders.Table)
			{
				// Get All Open Orders for This Spread
				if (getAllOrders)
				{
					rows = GLOBAL.HOrders.Table.Select(GetAllOrderFilter(parentTag, false), "Tag");
					// Check To See if We have equal number of orders to legs
					if (rows.Length != _autoSpreadSupport.Legs)
					{
						// Missing Legs
						//GLOBAL.HROC.AddToException(parentTag + " Can not find all legs in data table!");
						return new Dictionary<int, ROCOrder>();
					}
				}
				else
				{
					// Used by Market Data Replace
					rows = GLOBAL.HOrders.Table.Select(GetOpenOrderFilter(parentTag, true), "Tag");
					if (rows.Length != _autoSpreadSupport.Legs)
					{
						// Is a locked order don't do replace on it anymore
						// remember the locked parentTags
						lock (_lockedParentTags)
						{
							if (!_lockedParentTags.Contains(parentTag))
							{
								_lockedParentTags.Add(parentTag);
							}
						}
						return new Dictionary<int, ROCOrder>();
					}
				}
			}

			// Leg - Order
			return GetLegOrders(rows);
		}
		private Dictionary<int, ROCOrder> GetLegOrders(DataRow[] rows)
		{
			Dictionary<int, ROCOrder> result = new Dictionary<int, ROCOrder>();

			lock (GLOBAL.HOrders.RocItems)
			{
				foreach (DataRow row in rows)
				{
					string orderID = row["Tag"].ToString();
					if (GLOBAL.HOrders.RocItems.ContainsKey(orderID))
					{
						ROCOrder order = GLOBAL.HOrders.RocItems[orderID];
						TagKeyItems tagItems = new TagKeyItems(orderID);

						if (tagItems.LegNumberInt != null)
						{
							if (!result.ContainsKey((int)tagItems.LegNumberInt))
							{
								result.Add((int)tagItems.LegNumberInt, order);
							}
						}
					}
				}
			}

			return result;
		}

		#endregion

		#region - Leg Price\Qty\Side\OrderID Functions -

		// Used when displaying selected prices
		private void GetSelectedPriceForLegs()
		{
		    if (rocAutoSpreadList.ReadyForTickTable && okToBuildQuickList && rocAutoSpreadList.RocGridTable.Rows.Count > 0)
		    {
		        if (panelLimitPrice.Visible)
		        {
					_autoSpreadSupport.CalculateLegPriceFromSelectedPrice("Bid", "Limit", Convert.ToDouble(numLimitPrice.Value));
					_autoSpreadSupport.CalculateLegPriceFromSelectedPrice("Ask", "Limit", Convert.ToDouble(numLimitPrice.Value));
				}

				if (panelStopPrice.Visible)
		        {
					_autoSpreadSupport.CalculateLegPriceFromSelectedPrice("Bid", "Stop", Convert.ToDouble(numStopPrice.Value));
					_autoSpreadSupport.CalculateLegPriceFromSelectedPrice("Ask", "Stop", Convert.ToDouble(numStopPrice.Value));
				}
			}

			//string legPrices = "[LIM] \r\n";
			string legPrices = "";
			foreach (AutoSpreadItem item in _autoSpreadSupport.ItemByLegs.Values)
			{
				if (item.LegNumber == _autoSpreadSupport.PrimeLeg)
				{
					legPrices = legPrices + string.Format("{0, -6}  BID {1, 8}  ASK {2, 8} Last {3, 8} \r\n", new object[] { item.SymbolDetail, item.BidPriceSTR, item.AskPriceSTR, item.TradedPriceSTR });
					legPrices = legPrices + string.Format("{0, -6}  BUY {1, 8} SELL {2, 8} \r\n", new object[] { "", item.BuyLimitPriceSTR, item.SellLimitPriceSTR });
					legPrices = legPrices + string.Format("({0, 4}) TICK {1, 8} TICK {2, 8} \r\n", new object[] { item.TickLimit, item.BuyLimitPriceFromBidPrice, item.SellLimitPriceFromAskPrice });
				}
				else
				{
					if (_autoSpreadSupport.IsFlipped(_autoSpreadSupport.PrimeFactor, item.Factor))
					{
						legPrices = legPrices + string.Format("{0, -6}  ASK {1, 8}  BID {2, 8} Last {3, 8} \r\n", new object[] { item.SymbolDetail, item.AskPriceSTR, item.BidPriceSTR, item.TradedPriceSTR });
						legPrices = legPrices + string.Format("{0, -6} SELL {1, 8}  BUY {2, 8} \r\n", new object[] { "", item.SellLimitPriceSTR, item.BuyLimitPriceSTR });
						legPrices = legPrices + string.Format("({0, 4}) TICK {1, 8} TICK {2, 8} \r\n", new object[] { item.TickLimit, item.SellLimitPriceFromAskPrice, item.BuyLimitPriceFromBidPrice });
					}
					else
					{
						legPrices = legPrices + string.Format("{0, -6}  BID {1, 8}  ASK {2, 8} Last {3, 8} \r\n", new object[] { item.SymbolDetail, item.BidPriceSTR, item.AskPriceSTR, item.TradedPriceSTR });
						legPrices = legPrices + string.Format("{0, -6}  BUY {1, 8} SELL {2, 8} \r\n", new object[] { "", item.BuyLimitPriceSTR, item.SellLimitPriceSTR });
						legPrices = legPrices + string.Format("({0, 4}) TICK {1, 8} TICK {2, 8} \r\n", new object[] { item.TickLimit, item.BuyLimitPriceFromBidPrice, item.SellLimitPriceFromAskPrice });
					}
				}
			}
			//legPrices = legPrices + "[STP] \r\n";
			//foreach (AutoSpreadItem item in _autoSpreadSupport.ItemByLegs.Values)
			//{
			//    legPrices = legPrices + string.Format("{0, -6} Bid {1, -8} Ask {2, -8} \r\n", new object[] { item.SecInfo.MDSymbol, item.BidStopPrice, item.AskStopPrice });
			//}
			if (legPrices.Length > 4)
			{
				dspSelectedLegPrice.Text = legPrices.Substring(0, legPrices.Length - 3);
			}
		}

		private string GetLegSideCode(string sideCode, AutoSpreadItem item, ref bool isShort)
		{
			return GetLegSideCode(Convert.ToInt64(sideCode), item, ref isShort);
		}
		private string GetLegSideCode(long sideCode, AutoSpreadItem item, ref bool isShort)
		{
			bool flip = false;

			if (_autoSpreadSupport.PrimeFactor > 0)
			{
				if (item.Factor < 0)
				{
					flip = true;
				}
			}
			else if (_autoSpreadSupport.PrimeFactor < 0)
			{
				if (item.Factor > 0)
				{
					flip = true;
				}
			}

			if (flip)
			{
				switch (sideCode)
				{
					case CSVEx.CSVFieldIDs.SideCodes.Buy:
						if (item.SecInfo.SecType == CSVFieldIDs.SecutrityTypes.Equity &&
							item.ShortLender != "")
						{
							isShort = true;
							return CSVEx.CSVFieldIDs.SideCodes.Short.ToString();
						}
						else
						{
							return CSVEx.CSVFieldIDs.SideCodes.Sell.ToString();
						}
					case CSVEx.CSVFieldIDs.SideCodes.Sell:
						return CSVEx.CSVFieldIDs.SideCodes.Buy.ToString();
				}
			}

			return sideCode.ToString();
		}

		private string GetLegQty(long baseQty, AutoSpreadItem item)
		{
			decimal qty = baseQty * Math.Abs(item.FactorDec);
			long result = Convert.ToInt64(qty);

			if (qty != result)
			{
				return "";
			}
			else
			{
				return result.ToString();
			}
		}

		// Used When executing orders
		private string GetLegPrice(string sideCode, string selPrice, AutoSpreadItem item, out string marketPrice)
		{
			return GetLegPrice(Convert.ToInt64(sideCode), Convert.ToDouble(selPrice), item.LegNumber, out marketPrice);
		}
		private string GetLegPrice(string sideCode, double selPrice, AutoSpreadItem item, out string marketPrice)
		{
			return GetLegPrice(Convert.ToInt64(sideCode), selPrice, item.LegNumber, out marketPrice);
		}
		private string GetLegPrice(long sideCode, double selPrice, int legNumber, out string marketPrice)
		{
			Nullable<double> legPrice = null;
			marketPrice = "";

			switch (sideCode)
			{
				case CSVFieldIDs.SideCodes.Buy:
					legPrice = _autoSpreadSupport.CalculateLegPriceFromSelectedPriceByLeg("Bid", selPrice, legNumber, out marketPrice);
					break;
				case CSVFieldIDs.SideCodes.Short:
				case CSVFieldIDs.SideCodes.Sell:
					legPrice = _autoSpreadSupport.CalculateLegPriceFromSelectedPriceByLeg("Ask", selPrice, legNumber, out marketPrice);
					break;
			}

			if (legPrice != null)
			{
				return legPrice.ToString();
			}

			return "";
		}

		#endregion

		#region - Setting Grid Functions -

		private void ClearSpreadBooks()
		{
			lock (_autoSpreadSupport)
			{
				_autoSpreadSupport.HasAllLegs = false;
				_autoSpreadSupport.ItemByLegs.Clear();

				lock (_autoSpreadSupport.SyntheticBidBook)
				{
					_autoSpreadSupport.SyntheticBidBook.Clear();
				}
				lock (_autoSpreadSupport.SyntheticAskBook)
				{
					_autoSpreadSupport.SyntheticAskBook.Clear();
				}

				if (rocAutoSpreadList.ReadyForTickTable)
				{
					// Perpare for Ladder Grid Refresh
					//rocAutoSpreadList.ReadyForTickTable = false;
					ResetTicketValues();
				}
			}
		}

		private void RefreshSpreadSettings()
		{
			double factorBase = 1;
			double contractSizeBase = 1;

			double factor = 1;
			double contractSize = 1;
			double ratio = 0;

			int legCount = 0;

			ClearSpreadBooks();

			foreach (DataGridViewRow row in rocAutoSpreadListSettings.Rows)
			{
				if (row.Cells["SymbolDetail"].Value != null &&
					row.Cells["SymbolDetail"].Value.ToString() != "" &&
					row.Cells["Symbol"].Value != null &&
					row.Cells["Symbol"].Value.ToString() != "")
				{
					switch (row.Index)
					{
						case 0:
							row.Cells["Ratio"].Value = 1;
							if (row.Cells["Factor"].Value != null)
							{
								factorBase = (double)row.Cells["Factor"].Value;
							}
							if (row.Cells["ContractSize"].Value != null)
							{
								contractSizeBase = (double)row.Cells["ContractSize"].Value;
							}
							break;
						default:
							if (row.Cells["Factor"].Value != null)
							{
								factor = (double)row.Cells["Factor"].Value;
							}
							if (row.Cells["ContractSize"].Value != null)
							{
								contractSize = (double)row.Cells["ContractSize"].Value;
							}
							ratio = (factor * contractSize) / (factorBase * contractSizeBase);
							row.Cells["Ratio"].Value = Math.Round(ratio, 7);
							break;
					}

					RefreshSpreadItemsBySettingRow(legCount, row);

					legCount = legCount + 1;
				}
			}

			_autoSpreadSupport.Legs = legCount;
		}

		private void RefreshSpreadItemsBySettingRow(int leg, DataGridViewRow row)
		{
			if (!_autoSpreadSupport.ItemByLegs.ContainsKey(leg))
			{
				_autoSpreadSupport.ItemByLegs.Add(leg, new AutoSpreadItem(leg, 3));
			}

			_autoSpreadSupport.ItemByLegs[leg].RowIndex = leg;
			if (row.Cells["Factor"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].Factor = (double)row.Cells["Factor"].Value;
			}
			if (row.Cells["Ratio"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].Ratio = (double)row.Cells["Ratio"].Value;
			}
			if (row.Cells["UserRatio"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].UserRatio = (double)row.Cells["UserRatio"].Value;
			}
			if (row.Cells["Lean"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].IsLean = (bool)row.Cells["Lean"].Value;
			}
			if (row.Cells["TickLimit"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].TickLimit = (long)row.Cells["TickLimit"].Value;
			}
			if (row.Cells["SymbolDetail"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].SymbolDetail = (string)row.Cells["SymbolDetail"].Value;
			}
			if (row.Cells["Exchange"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].Exchange = (string)row.Cells["Exchange"].Value;
			}
			if (row.Cells["LocalAccountAcrn"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].LocalAcctAcrn = (string)row.Cells["LocalAccountAcrn"].Value;
			}
			if (row.Cells["TraderFor"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].TradeFor = (string)row.Cells["TraderFor"].Value;
			}
			if (row.Cells["ShortLender"].Value != DBNull.Value)
			{
				_autoSpreadSupport.ItemByLegs[leg].ShortLender = (string)row.Cells["ShortLender"].Value;
			}

			_autoSpreadSupport.ItemByLegs[leg].MarketData = new MDServerToClient();
			_autoSpreadSupport.ItemByLegs[leg].IsNewL1 = true;
			_autoSpreadSupport.ItemByLegs[leg].IsNewBook = true;

			// Make a copy
			ItemByLegs = new Dictionary<int, AutoSpreadItem>(_autoSpreadSupport.ItemByLegs);
			PrimeLeg = _autoSpreadSupport.PrimeLeg;
		}

		// Update with Play back & onLoad
		private void UpdateAutoSpreadSettingsWithCurrentMarketData(string mdSymbol, ref MDServerToClient delta)
		{
			if (mdSymbol != null && mdSymbol != "")
			{
				if (GLOBAL.HMarketData.Current.ContainsKey(mdSymbol))
				{
					delta.Update(GLOBAL.HMarketData.Current[mdSymbol]);
				}
			}
		}

		// Update with Security Info On Play back & onLoad
		private bool UpdateAutoSpreadSettingsWithSecurityInfo(string symbolDetail, ref string mdSymbol, ref double tickSize, ref string secType, ref string name)
		{
			bool result = false;

			BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
			if (secInfo != null)
			{
				mdSymbol = secInfo.MDSymbol;
				tickSize = secInfo.TickSize;
				secType = secInfo.SecType;
				name = secInfo.LongName;

				switch (secType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
						rocAutoSpreadListSettings.UpdateSymbol(mdSymbol);
						rocAutoSpreadListSettings.UpdateTickSize(mdSymbol, tickSize);
						break;
					default:
						rocAutoSpreadListSettings.UpdateTickSize(mdSymbol, tickSize);
						break;
				}

				result = true;
			}

			lock (ImSymbolNeeded)
			{
				if (!ImSymbolNeeded.ContainsKey(symbolDetail))
				{
					ImSymbolNeeded.Add(symbolDetail, mdSymbol);
				}
				else
				{
					ImSymbolNeeded[symbolDetail] = mdSymbol;
				}
			}

			return result;
		}

		private delegate void UpdateAutoSpreadSettingsStartDelegate();
		private void UpdateAutoSpreadSettingsStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateAutoSpreadSettingsStartDelegate(UpdateAutoSpreadSettingsStart));
				return;
			}

			IsProcessing = true;

			rocAutoSpreadListSettings.RocGridTable.DefaultView.RowFilter = "";
			rocAutoSpreadListSettings.RocGridTable.DefaultView.Sort = "";

			rocAutoSpreadListSettings.RocGridTable.BeginLoadData();
		}

		private delegate void UpdateAutoSpreadSettingsStopDelegate();
		private void UpdateAutoSpreadSettingsStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateAutoSpreadSettingsStopDelegate(UpdateAutoSpreadSettingsStop));
				return;
			}

			rocAutoSpreadListSettings.RocGridTable.EndLoadData();

			IsProcessing = false;

			//rocAutoSpreadListSettings.RefreshRation = true;
			rocAutoSpreadListSettings.RefreshSharedRows = true;
			rocAutoSpreadListSettings.RefreshAggragation = true;
			rocAutoSpreadListSettings.ShouldScrollToLastRow = true;
		}

		#endregion

		#region - Export & Import -

		public string ExportAutoSpreadSettingsGrid()
		{
			ROCAutoSpreadListSettingsProfile prof = new ROCAutoSpreadListSettingsProfile(rocAutoSpreadListSettings);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public string ExportAutoSpreadGrid()
		{
			ROCAutoSpreadListProfile prof = new ROCAutoSpreadListProfile(rocAutoSpreadList);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public string ExportTicket()
		{
			ROCTicketProfile prof = new ROCTicketProfile(this);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportAutoSpreadSettingsGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCAutoSpreadListSettingsProfile prof = (ROCAutoSpreadListSettingsProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					LastSavedAutoSpreadSymbolSettingList.AddRange(prof.AutoSpreadSymbolSettingList.ToArray());
					prof = new ROCAutoSpreadListSettingsProfile(rocAutoSpreadListSettings, prof);
				}
			}
		}

		public void ImportAutoSpreadGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCAutoSpreadListProfile prof = (ROCAutoSpreadListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCAutoSpreadListProfile(rocAutoSpreadList, prof);
				}
			}
		}

		public void ImportTicket(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCTicketProfile prof = (ROCTicketProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCTicketProfile(this, prof);
				}
			}
		}

		#endregion

		#region - Columns Menu -

		private menuBaseFilter _menuColumns;
		private PropertyContainer _menuColumnsContainer;
		internal void ShowColumnMenu()
		{
			if (_menuColumns == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();
				items.Add("MDSymbol", new FilterItem("MDSymbol", !rocAutoSpreadListSettings.Columns["Symbol"].Visible));
				//items.Add("Symbol", new FilterItem("Symbol", !rocAutoSpreadListSettings.Columns["SymbolDetail"].Visible));
				//items.Add("Qty", new FilterItem("Qty", !rocAutoSpreadListSettings.Columns["Factor"].Visible));
				items.Add("MULT", new FilterItem("MULT", !rocAutoSpreadListSettings.Columns["ContractSize"].Visible));
				items.Add("TSize", new FilterItem("TSize", !rocAutoSpreadListSettings.Columns["TickSize"].Visible));
				//items.Add("DEF", new FilterItem("DEF", !rocAutoSpreadListSettings.Columns["Ratio"].Visible));
				items.Add("OVRD", new FilterItem("OVRD", !rocAutoSpreadListSettings.Columns["UserRatio"].Visible));
				items.Add("Lean", new FilterItem("Lean", !rocAutoSpreadListSettings.Columns["Lean"].Visible));
				items.Add("TickLimit", new FilterItem("TickLimit", !rocAutoSpreadListSettings.Columns["TickLimit"].Visible));
				items.Add("Exch", new FilterItem("Exch", !rocAutoSpreadListSettings.Columns["Exchange"].Visible));
				items.Add("Acct", new FilterItem("Acct", !rocAutoSpreadListSettings.Columns["LocalAccountAcrn"].Visible));
				items.Add("Trader", new FilterItem("Trader", !rocAutoSpreadListSettings.Columns["TraderFor"].Visible));
				items.Add("SLender", new FilterItem("SLender", !rocAutoSpreadListSettings.Columns["ShortLender"].Visible));
				items.Add("Name", new FilterItem("Name", !rocAutoSpreadListSettings.Columns["Name"].Visible));
				items.Add("Valid", new FilterItem("Valid", !rocAutoSpreadListSettings.Columns["Valid"].Visible));
				items.Add("DisplayFactor", new FilterItem("DisplayFactor", !rocAutoSpreadListSettings.Columns["DisplayFactor"].Visible));
				items.Add("SecType", new FilterItem("SecType", !rocAutoSpreadListSettings.Columns["SecType"].Visible));

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
				case "MDSymbol":
					rocAutoSpreadListSettings.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				//case "Symbol":
				//    rocAutoSpreadListSettings.Columns["SymbolDetail"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
				//    break;
				//case "Qty":
				//    rocAutoSpreadListSettings.Columns["Factor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
				//    break;
				case "MULT":
					rocAutoSpreadListSettings.Columns["ContractSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TSize":
					rocAutoSpreadListSettings.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				//case "DEF":
				//    rocAutoSpreadListSettings.Columns["Ratio"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
				//    break;
				case "OVRD":
					rocAutoSpreadListSettings.Columns["UserRatio"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Lean":
					rocAutoSpreadListSettings.Columns["Lean"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TickLimit":
					rocAutoSpreadListSettings.Columns["TickLimit"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Exch":
					rocAutoSpreadListSettings.Columns["Exchange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Acct":
					rocAutoSpreadListSettings.Columns["LocalAccountAcrn"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Trader":
					rocAutoSpreadListSettings.Columns["TraderFor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SLender":
					rocAutoSpreadListSettings.Columns["ShortLender"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Name":
					rocAutoSpreadListSettings.Columns["Name"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Valid":
					rocAutoSpreadListSettings.Columns["Valid"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DisplayFactor":
					rocAutoSpreadListSettings.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecType":
					rocAutoSpreadListSettings.Columns["SecType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
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

		#region - Row TraderFor Menu -

		private menuBaseAction _menuTraderActions;
		private PropertyContainer _menuTraderActionsContainer;
		internal void ShowTraderMenu()
		{
			if (_menuTraderActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					items.Add(trader.tradeFor.ToUpper(), trader.tradeFor.ToUpper());
				}
				items.Add("", "");

				_menuTraderActions = new menuBaseAction(items, true);
				_menuTraderActions.PropertyChanged += new PropertyChangedEventHandler(menuTraderActions_PropertyChanged);
			}

			if (_menuTraderActionsContainer == null)
			{
				_menuTraderActionsContainer = new PropertyContainer(_menuTraderActions);
				_menuTraderActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuTraderActionsContainer_Closed);
			}

			_menuTraderActionsContainer.Show(Cursor.Position);
		}

		private void menuTraderActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			bool found = false;

			RomBasicOrder order = new RomBasicOrder();

			order.tradeFor = e.PropertyName;

			if (order.tradeFor != "")
			{
				// Get the current account for selected row
				if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value != null)
				{
					order.localAcctAcrn = rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString();
				}
				else
				{
					order.localAcctAcrn = "";
				}

				// Get the current destination short name for selected row
				if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value != null)
				{
					order.exchangeID = rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value.ToString();
				}
				else
				{
					order.exchangeID = "";
				}

				string key = MakeUserInfoKey(order);
				if (_validUserInfos.ContainsKey(key))
				{
					order.capacity = _validUserInfos[key].capacity;
					order.clearingAcctID = _validUserInfos[key].clearingAcctID;
					order.clearingFirmID = _validUserInfos[key].clearingFirmID;
					order.firmArc = _validUserInfos[key].firmArc;
					order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

					order.exchangeID = _validUserInfos[key].exchangeID;
					found = true;
				}
				else
				{
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor == e.PropertyName)
						{
							if (VerifyExchange(ref order, trader.CSAccounts) || VerifyExchange(ref order, trader.FUTAccounts) || VerifyExchange(ref order, trader.OPTAccounts))
							{
								found = true;
								break;
							}
						}
					}
				}

				// Update the TradeFor and clear all account and exchange field
				if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value == null)
				{
					ReplaceValue("TraderFor", "", order.tradeFor);
				}
				else
				{
					ReplaceValue("TraderFor", rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString(), order.tradeFor);
				}

				if (!found)
				{
					// Update the TradeFor and clear all account and exchange field
					if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value == null)
					{
						ReplaceValue("LocalAccountAcrn", "", "");
					}
					else
					{
						ReplaceValue("LocalAccountAcrn", rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString(), "");
					}

					// Update the TradeFor and clear all account and exchange field
					if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value == null)
					{
						ReplaceValue("Exchange", "", "");
					}
					else
					{
						ReplaceValue("Exchange", rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value.ToString(), "");
					}
				}
			}
			else
			{
				// Clear
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value = "";
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value = "";
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value = "";
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["ShortLender"].Value = "";
			}


			RefreshSpreadSettings();

			_menuTraderActionsContainer.Close();
		}

		private void menuTraderActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuTraderActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuTraderActionsContainer_Closed);
			_menuTraderActionsContainer = null;
			_menuTraderActions.PropertyChanged -= new PropertyChangedEventHandler(menuTraderActions_PropertyChanged);
			_menuTraderActions = null;
		}

		#endregion

		#region - Row Local Account Acrn Menu -

		private menuBaseAction _menuAccountActions;
		private PropertyContainer _menuAccountActionsContainer;
		internal void ShowAccountMenu()
		{
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value != null &&
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString() != "")
			{
				if (_menuAccountActions == null)
				{
					Dictionary<string, string> items = new Dictionary<string, string>();
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor.ToUpper() == rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString().ToUpper())
						{
							foreach (AccountMap acctMap in trader.CSAccounts.Values)
							{
								if (!items.ContainsKey(acctMap.account))
								{
									items.Add(acctMap.account, acctMap.account);
								}
							}

							foreach (AccountMap acctMap in trader.FUTAccounts.Values)
							{
								if (!items.ContainsKey(acctMap.account))
								{
									items.Add(acctMap.account, acctMap.account);
								}
							}

							//foreach (AccountMap acctMap in trader.OPTAccounts.Values)
							//{
							//    if (!items.ContainsKey(acctMap.account))
							//    {
							//        items.Add(acctMap.account, acctMap.account);
							//    }
							//}

							break;
						}
					}

					_menuAccountActions = new menuBaseAction(items, true);
					_menuAccountActions.PropertyChanged += new PropertyChangedEventHandler(menuAccountActions_PropertyChanged);
				}

				if (_menuAccountActionsContainer == null)
				{
					_menuAccountActionsContainer = new PropertyContainer(_menuAccountActions);
					_menuAccountActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuAccountActionsContainer_Closed);
				}

				_menuAccountActionsContainer.Show(Cursor.Position);
			}
		}

		private void menuAccountActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			bool found = false;

			RomBasicOrder order = new RomBasicOrder();

			// Get the current account for selected row
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value != null)
			{
				order.tradeFor = rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString();
			}
			else
			{
				order.tradeFor = "";
			}

			// Load Account into localAcctAcrn first, then it will be replaced by the Correct LocalAcctAcrn
			order.localAcctAcrn = e.PropertyName;

			// Get the current destination short name for selected row
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value != null)
			{
				order.exchangeID = rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value.ToString();
			}
			else
			{
				order.exchangeID = "";
			}

			string key = MakeUserInfoKey(order);
			if (_validUserInfos.ContainsKey(key))
			{
				order.capacity = _validUserInfos[key].capacity;
				order.clearingAcctID = _validUserInfos[key].clearingAcctID;
				order.clearingFirmID = _validUserInfos[key].clearingFirmID;
				order.firmArc = _validUserInfos[key].firmArc;
				order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

				order.exchangeID = _validUserInfos[key].exchangeID;
				found = true;
			}
			else
			{
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor == e.PropertyName)
					{
						if (VerifyExchange(ref order, trader.CSAccounts) || VerifyExchange(ref order, trader.FUTAccounts) || VerifyExchange(ref order, trader.OPTAccounts))
						{
							found = true;
							break;
						}
					}
				}
			}

			// Update the TradeFor and clear all account and exchange field
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value == null)
			{
				ReplaceValue("LocalAccountAcrn", "", order.localAcctAcrn);
			}
			else
			{
				ReplaceValue("LocalAccountAcrn", rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString(), order.localAcctAcrn);
			}

			if (!found)
			{
				// Update the TradeFor and clear all account and exchange field
				if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value == null)
				{
					ReplaceValue("Exchange", "", "");
				}
				else
				{
					ReplaceValue("Exchange", rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value.ToString(), "");
				}
			}

			RefreshSpreadSettings();

			_menuAccountActionsContainer.Close();
		}

		private void menuAccountActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuAccountActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuAccountActionsContainer_Closed);
			_menuAccountActionsContainer = null;
			_menuAccountActions.PropertyChanged -= new PropertyChangedEventHandler(menuAccountActions_PropertyChanged);
			_menuAccountActions = null;
		}

		#endregion

		#region - Row Exchange Menu -

		private menuBaseAction _menuExchangeActions;
		private PropertyContainer _menuExchangeActionsContainer;
		internal void ShowExchangeMenu()
		{
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value != null &&
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString() != "" &&
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value != null &&
				rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString() != "")
			{
				if (_menuExchangeActions == null)
				{
					Dictionary<string, string> items = new Dictionary<string, string>();
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor.ToUpper() == rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString().ToUpper())
						{
							foreach (AccountMap acctMap in trader.CSAccounts.Values)
							{
								if (acctMap.account.ToUpper() == rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString().ToUpper())
								{
									foreach (DestinationMap destMap in acctMap.Destinations.Values)
									{
										if (!items.ContainsKey(destMap.shortName))
										{
											items.Add(destMap.shortName, destMap.shortName);
										}
									}
								}
							}

							foreach (AccountMap acctMap in trader.FUTAccounts.Values)
							{
								if (acctMap.account.ToUpper() == rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString().ToUpper())
								{
									foreach (DestinationMap destMap in acctMap.Destinations.Values)
									{
										if (!items.ContainsKey(destMap.shortName))
										{
											items.Add(destMap.shortName, destMap.shortName);
										}
									}
								}
							}

							foreach (AccountMap acctMap in trader.OPTAccounts.Values)
							{
								if (acctMap.account.ToUpper() == rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString().ToUpper())
								{
									foreach (DestinationMap destMap in acctMap.Destinations.Values)
									{
										if (!items.ContainsKey(destMap.shortName))
										{
											items.Add(destMap.shortName, destMap.shortName);
										}
									}
								}
							}

							break;
						}
					}

					_menuExchangeActions = new menuBaseAction(items, true);
					_menuExchangeActions.PropertyChanged += new PropertyChangedEventHandler(menuExchangeActions_PropertyChanged);
				}

				if (_menuExchangeActionsContainer == null)
				{
					_menuExchangeActionsContainer = new PropertyContainer(_menuExchangeActions);
					_menuExchangeActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuExchangeActionsContainer_Closed);
				}

				_menuExchangeActionsContainer.Show(Cursor.Position);
			}
		}

		private void menuExchangeActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			bool found = false;

			RomBasicOrder order = new RomBasicOrder();

			// Get the current account for selected row
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value != null)
			{
				order.tradeFor = rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["TraderFor"].Value.ToString();
			}
			else
			{
				order.tradeFor = "";
			}

			// Get the current account for selected row
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value != null)
			{
				order.localAcctAcrn = rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["LocalAccountAcrn"].Value.ToString();
			}
			else
			{
				order.localAcctAcrn = "";
			}

			order.exchangeID = e.PropertyName;

			string key = MakeUserInfoKey(order);
			if (_validUserInfos.ContainsKey(key))
			{
				order.capacity = _validUserInfos[key].capacity;
				order.clearingAcctID = _validUserInfos[key].clearingAcctID;
				order.clearingFirmID = _validUserInfos[key].clearingFirmID;
				order.firmArc = _validUserInfos[key].firmArc;
				order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

				//order.exchangeID = _validUserInfos[key].exchangeID;
				found = true;
			}
			else
			{
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor == e.PropertyName)
					{
						if (VerifyExchange(ref order, trader.CSAccounts) || VerifyExchange(ref order, trader.FUTAccounts) || VerifyExchange(ref order, trader.OPTAccounts))
						{
							found = true;
							break;
						}
					}
				}
			}

			// Update the TradeFor and clear all account and exchange field
			if (rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value == null)
			{
				ReplaceValue("Exchange", "", order.exchangeID);
			}
			else
			{
				ReplaceValue("Exchange", rocAutoSpreadListSettings.Rows[rocAutoSpreadListSettings.RowLocation].Cells["Exchange"].Value.ToString(), order.exchangeID);
			}

			if (!found)
			{
			}

			RefreshSpreadSettings();

			_menuExchangeActionsContainer.Close();
		}

		private void menuExchangeActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuExchangeActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuExchangeActionsContainer_Closed);
			_menuExchangeActionsContainer = null;
			_menuExchangeActions.PropertyChanged -= new PropertyChangedEventHandler(menuExchangeActions_PropertyChanged);
			_menuExchangeActions = null;
		}

		#endregion

		#region - Row Menu Support -

		internal void ReplaceValue(string colName, string orgValue, string newValue)
		{
			ReplaceValue(colName, orgValue, newValue, true);
		}
		internal void ReplaceValue(string colName, string orgValue, string newValue, bool selectedRowsOnly)
		{
			try
			{
				lock (rocAutoSpreadListSettings)
				{
					orgValue = orgValue.ToUpper();
					newValue = newValue.ToUpper();

					if (selectedRowsOnly)
					{
						foreach (DataGridViewRow row in rocAutoSpreadListSettings.SelectedRows)
						{
							ReplaceValue(row, colName, orgValue, newValue);
						}
					}
					else
					{
						foreach (DataGridViewRow row in rocAutoSpreadListSettings.Rows)
						{
							ReplaceValue(row, colName, orgValue, newValue);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}
		internal void ReplaceValue(DataGridViewRow row, string colName, string orgValue, string newValue)
		{
			if (row.Cells[colName].Value != null && row.Cells[colName].Value.ToString() != "")
			{
				if (orgValue == row.Cells[colName].Value.ToString().ToUpper())
				{
					row.Cells[colName].Value = newValue;
				}
			}
			else
			{
				if (orgValue == "")
				{
					row.Cells[colName].Value = newValue;
				}
			}
		}

		#endregion

		#region - Row Menu Support - Verify Trader -

		private bool VerifyTrader(ref RomBasicOrder order)
		{
			if (GLOBAL.HRDS.GotUserProfiles)
			{
				string key = MakeUserInfoKey(order);
				if (_validUserInfos.ContainsKey(key))
				{
					order.capacity = _validUserInfos[key].capacity;
					order.clearingAcctID = _validUserInfos[key].clearingAcctID;
					order.clearingFirmID = _validUserInfos[key].clearingFirmID;
					order.firmArc = _validUserInfos[key].firmArc;
					order.localAcctAcrn = _validUserInfos[key].localAcctAcrn;

					order.exchangeID = _validUserInfos[key].exchangeID;
					if (order.secType == "")
					{
						order.secType = _validUserInfos[key].secType;
					}

					return true;
				}
				else
				{
					// Search for a match
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						if (trader.tradeFor == order.tradeFor)
						{
							switch (order.secType)
							{
								case CSVFieldIDs.SecutrityTypes.Option:
									if (VerifyExchange(ref order, trader.OPTAccounts))
									{
										return true;
									}
									break;
								case CSVFieldIDs.SecutrityTypes.OptionFuture:
									if (VerifyExchange(ref order, trader.OPTAccounts))
									{
										return true;
									}
									if (VerifyExchange(ref order, trader.FUTAccounts))
									{
										return true;
									}
									break;
								case CSVFieldIDs.SecutrityTypes.Future:
									if (VerifyExchange(ref order, trader.FUTAccounts))
									{
										return true;
									}
									break;
								default:
									if (VerifyExchange(ref order, trader.CSAccounts))
									{
										return true;
									}

									if (VerifyExchange(ref order, trader.FUTAccounts))
									{
										return true;
									}

									if (VerifyExchange(ref order, trader.OPTAccounts))
									{
										return true;
									}
									break;
							}
						}
					}
				}
			}
			return false;
		}

		private bool VerifyExchange(ref RomBasicOrder order, Dictionary<string, AccountMap> accounts)
		{
			foreach (AccountMap acctMap in accounts.Values)
			{
				if (order.localAcctAcrn == "")
				{
					// User didn't specify an account, use the first destination found
					foreach (DestinationMap destMap in acctMap.Destinations.Values)
					{
						if (destMap.shortName == order.exchangeID)
						{
							StoreUserInfo(ref order, acctMap, destMap);
							return true;
						}
					}
				}
				else
				{
					if (acctMap.account == order.localAcctAcrn)
					{
						foreach (DestinationMap destMap in acctMap.Destinations.Values)
						{
							if (destMap.shortName == order.exchangeID)
							{
								StoreUserInfo(ref order, acctMap, destMap);
								return true;
							}
						}
					}
				}
			}
			return false;
		}

		private void StoreUserInfo(ref RomBasicOrder order, AccountMap acctMap, DestinationMap destMap)
		{
			order.capacity = acctMap.capacity;
			order.clearingAcctID = acctMap.clearingAcID;
			order.clearingFirmID = acctMap.clearingFirmID;
			order.firmArc = acctMap.firmAcr;
			order.localAcctAcrn = acctMap.localAcAcrn;
			order.occAcct = acctMap.occAcr;
			if (acctMap.homeExchange != null)
			{
				order.homeExchange = acctMap.homeExchange;
			}

			if (destMap.destID != null)
			{
				order.exchangeID = destMap.destID.ToString();
				order.cmtaFirmID = destMap.cmtaFirmID;
				if (order.secType == "")
				{
					order.secType = destMap.secType;
				}
			}

			string key = order.tradeFor + "," + order.localAcctAcrn + "," + destMap.shortName;
			if (_validUserInfos.ContainsKey(key))
			{
				_validUserInfos[key] = order;
			}
			else
			{
				_validUserInfos.Add(key, order);
			}
		}

		private string MakeUserInfoKey(RomBasicOrder order)
		{
			// Starting with exchangeID as the destination short name
			return order.tradeFor + "," + order.localAcctAcrn + "," + order.exchangeID;
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