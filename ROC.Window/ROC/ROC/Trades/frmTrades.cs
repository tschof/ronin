using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Windows.Forms;

using RDSEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using ButtonEx;
using CSVEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmTrades : BaseList
	{
		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return rocTradesList.Processing;
			}
			set
			{
				rocTradesList.Processing = value;
			}
		}

		#endregion

		#region - Local Variable -

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private LockList<ROCExecution> _rocTrades = new LockList<ROCExecution>();
		private LockList<TPOSExecution> _tposTrades = new LockList<TPOSExecution>();

		#endregion

		#region - Constructor -

		public frmTrades()
		{
			InitializeComponent();

			Initialize();

			StartTimer(Configuration.User.Default.UIUpdateRate);
		}

		private void Initialize()
		{
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			IconClicked += new EventHandler(Form_IconClicked);

			RefreshClicked += new EventHandler(Form_RefreshClicked);
			DisplayClicked += new EventHandler(Form_DisplayClicked);
			UITimerTicked += new EventHandler(Form_UITimerTicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);

			rocTradesList.Initialize(GLOBAL.HExecutions.Table);
			foreach (int key in GLOBAL.HRDS.DestIDToName.Keys)
			{
				((DGVTradeDestinationColumn)rocTradesList.Columns["Exchange"]).DestIDToName.Add(key, GLOBAL.HRDS.DestIDToName[key]);
			}

			rocTradesList.PropertyChanged += new PropertyChangedEventHandler(rocTradesList_PropertyChanged);

			rocTradesList.MouseEnter += new EventHandler(rocTradesList_MouseEnter);

			rocTradesList.ProcessingChanged += new EventHandler(rocTradesList_ProcessingChanged);
			rocTradesList.ShouldScrollToLastRowChanged += new EventHandler(rocTradesList_ShouldScrollToLastRowChanged);
			rocTradesList.RefreshAggragationChanged += new EventHandler(rocTradesList_RefreshAggragationChanged);
			rocTradesList.RefreshSharedRowsChanged += new EventHandler(rocTradesList_RefreshSharedRowsChanged);
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (IsProcessing)
			{
				if (menuProcessing.Caption != rocTradesList.GridKeys.Count.ToString())
				{
					menuProcessing.Caption = rocTradesList.GridKeys.Count.ToString();
				}
			}

			if (GLOBAL.UseDelayedUpdate)
			{
				AddUpdateTradesByProcessByTimer();
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
				if (!IsProcessing)
				{
					lock (rocTradesList)
					{
						SetColumnMode(rocTradesList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocTradesList);
						SetColumnMode(rocTradesList, DataGridViewAutoSizeColumnMode.None);
					}
				}
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

		private void Form_RefreshClicked(object sender, EventArgs e)
		{
			if (!IsProcessing)
			{
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshTrades);
			}
		}

		#region - Display -

		private frmTradesDisplay _tradesDisplay;
		private frmTradesDisplay tradesDisplay
		{
			get
			{
				if (_tradesDisplay == null)
				{
					_tradesDisplay = new frmTradesDisplay(new ROCTradeProfile(rocTradesList));
					_tradesDisplay.PropertyChanged += new PropertyChangedEventHandler(tradesDisplay_PropertyChanged);
					_tradesDisplay.Disposed += new EventHandler(tradesDisplay_Disposed);
				}
				return _tradesDisplay;
			}
			set
			{
				_tradesDisplay = value;
			}
		}
		private void Form_DisplayClicked(object sender, EventArgs e)
		{
			GLOBAL.MainForm.TopMost = false;
			bool orgTopMost = TopMost;
			TopMost = false;

			tradesDisplay.ShowDialog();

			TopMost = orgTopMost;
			GLOBAL.MainForm.TopMost = true;
		}

		private void tradesDisplay_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Apply":
					SetProfile(tradesDisplay.GetProfile(new ROCTradeProfile(rocTradesList)));
					break;
				case "ApplyToAll":
					ROCTradeProfile prof = tradesDisplay.GetProfile(new ROCTradeProfile(rocTradesList));
					foreach (frmTrades w in GLOBAL.HWindows.TradeWindows.Values)
					{
						w.SetProfile(tradesDisplay.GetProfile(prof));
					}
					break;
			}
		}

		private void tradesDisplay_Disposed(object sender, EventArgs e)
		{
			if (_tradesDisplay != null)
			{
				_tradesDisplay.PropertyChanged -= new PropertyChangedEventHandler(tradesDisplay_PropertyChanged);
				_tradesDisplay.Disposed -= new EventHandler(tradesDisplay_Disposed);
			}

			_tradesDisplay = null;
		}

		public void SetProfile(ROCTradeProfile prof)
		{
			new ROCTradeProfile(rocTradesList, prof);
		}

		#endregion

		private void Form_Load(object sender, EventArgs e)
		{
			SetSourcesDisplay();
			SetSecuritiesDisplay();
			SetSymbolDisplay();
			SetAccountDisplay();
			RefreshTrades(null);
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void cmdShowAll_Click(object sender, EventArgs e)
		{
			rocTradesList.FilterReset();

			SetSourcesDisplay();
			SetSecuritiesDisplay();
			SetSymbolDisplay();
			SetAccountDisplay();
			RefreshTrades(null);
		}

		#endregion

		#region - Grid Event -

		private void rocTradesList_ProcessingChanged(object sender, EventArgs e)
		{
			rocTradesList.Enabled = !IsProcessing;
			//CaptionBar.Enabled = !IsProcessing;
			//panelTradeCommands.Enabled = !IsProcessing;

			if (IsProcessing)
			{
				menuProcessing.Show();
				FormBorderStyle = FormBorderStyle.FixedDialog;
			}
			else
			{
				menuProcessing.Hide();
				FormBorderStyle = FormBorderStyle.Sizable;
			}

			menuProcessing.Invalidate();
		}

		private void rocTradesList_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocTradesList.ShouldScrollToLastRow)
			{
				rocTradesList.ShouldScrollToLastRow = false;
				rocTradesList.ScrollToLastRow();
			}
		}

		private void rocTradesList_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocTradesList.RefreshAggragation)
			{
				//rocTradesList.UnshareRows();

				rocTradesList.RefreshAggragation = false;
				UpdateAggragation();
			}
		}

		private void rocTradesList_RefreshSharedRowsChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocTradesList.RefreshSharedRows)
			{
				rocTradesList.RefreshSharedRows = false;
				//rocTradesList.UnshareRows();
			}
		}

		private void rocTradesList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ShowColumnMenu":
					ShowColumnMenu();
					break;
				case "ShowActionMenu":
					ShowActionMenu();
					break;
				case "FilterOutSymbols":
					SetSymbolDisplay();
					break;
			}
		}

		private void rocTradesList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusTrades)
			{
				ActivateGrid(rocTradesList);
			}
		}

		#endregion

		#region - Export & Import -

		public string ExportGrid()
		{
			ROCTradeProfile prof = new ROCTradeProfile(rocTradesList);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCTradeProfile prof = (ROCTradeProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCTradeProfile(rocTradesList, prof);
				}
			}
		}

		public void Clone()
		{
			lock (GLOBAL.HWindows.TradeWindows)
			{
				frmTrades w = new frmTrades();
				
				w.ImportGrid(ExportGrid());

				w.Width = this.Width;
				w.Height = this.Height;

				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}

		#endregion

		#region - Binding -

		private void RefreshTrades(object input)
		{
			UpdateTradeStart();

			ROCExecution[] rocTrades;
			TPOSExecution[] tposTrades;

			lock (GLOBAL.HExecutions)
			{
				rocTrades = new ROCExecution[GLOBAL.HExecutions.RocItems.Values.Count];
				GLOBAL.HExecutions.RocItems.Values.CopyTo(rocTrades, 0);

				tposTrades = new TPOSExecution[GLOBAL.HExecutions.TposItems.Values.Count];
				GLOBAL.HExecutions.TposItems.Values.CopyTo(tposTrades, 0);
			}

			AddUpdateTrades(rocTrades, tposTrades);
			
			UpdateTradeStop();
		}

		// Used by Main Refresh and Load Thread
		private delegate void AddUpdateTradesDelegate(ROCExecution[] rocTrades, TPOSExecution[] tposTrades);
		private void AddUpdateTrades(ROCExecution[] rocTrades, TPOSExecution[] tposTrades)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new AddUpdateTradesDelegate(AddUpdateTrades), new object[] { rocTrades, tposTrades });
				return;
			}

			try
			{
				lock (rocTradesList.RocGridTable)
				{
					rocTradesList.SuspendLayout();
					rocTradesList.RocGridTable.BeginLoadData();

					rocTradesList.RocGridTable.Rows.Clear();
					rocTradesList.GridKeys.Clear();
					rocTradesList.Symbols.Clear();
					rocTradesList.Symbols.Add("");
					rocTradesList.Details.Clear();
					rocTradesList.Details.Add("");

					if (rocTrades.Length > 0)
					{
						foreach (ROCExecution rocTrade in rocTrades)
						{
							LoadRocTrade(rocTrade);
						}
					}

					if (Extended && tposTrades.Length > 0)
					{
						foreach (TPOSExecution tposTrade in tposTrades)
						{
							LoadTposTrade(tposTrade);
						}
					}

					rocTradesList.RocGridTable.EndLoadData();
					rocTradesList.ResumeLayout();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#region - Used by Process Thread -

		private delegate void AddUpdateTradesByProcessDelegate(bool updateIM, List<ROCExecution> rocTrades, List<TPOSExecution> tposTrades);
		public void AddUpdateTradesByProcess(bool updateIM, List<ROCExecution> rocTrades, List<TPOSExecution> tposTrades)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				try
				{
					_updateIM = updateIM;
					_rocTrades.AddRange(rocTrades);
					_tposTrades.AddRange(tposTrades);
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
					BeginInvoke(new AddUpdateTradesByProcessDelegate(AddUpdateTradesByProcess), new object[] { updateIM, rocTrades, tposTrades });
					return;
				}

				try
				{
					if (!IsProcessing)
					{
						lock (rocTradesList.RocGridTable)
						{
							if (updateIM)
							{
								UpdateSecurityInfo();
							}
							if (rocTrades.Count > 0)
							{
								foreach (ROCExecution rocTrade in rocTrades)
								{
									LoadRocTrade(rocTrade);
								}
							}
							if (Extended && tposTrades.Count > 0)
							{
								foreach (TPOSExecution tposTrade in tposTrades)
								{
									LoadTposTrade(tposTrade);
								}
							}
						}
					}
					if (updateIM || rocTrades.Count > 0 || tposTrades.Count > 0)
					{
						rocTradesList.RefreshAggragation = true;
						rocTradesList.ShouldScrollToLastRow = true;
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		public void AddUpdateTradesByProcessByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;

					bool updateIM = _updateIM, haveRocTrades = false, haveTposTrades = false;
					_updateIM = false;

					if (!IsProcessing)
					{
						lock (rocTradesList.RocGridTable)
						{
							if (updateIM)
							{
								UpdateSecurityInfo();
							}

							List<ROCExecution> rocTrades = _rocTrades.TakeAll();
							if (rocTrades.Count > 0)
							{
								haveRocTrades = true;
								foreach (ROCExecution rocTrade in rocTrades)
									LoadRocTrade(rocTrade);
							}

							if (Extended) {
								List<TPOSExecution> tposTrades = _tposTrades.TakeAll();
								if (tposTrades.Count > 0) {
									haveTposTrades = true;
									foreach (TPOSExecution tposTrade in tposTrades)
										LoadTposTrade(tposTrade);
								}
							}
						}
					}

					if (updateIM || haveRocTrades || haveTposTrades)
					{
						rocTradesList.RefreshAggragation = true;
						rocTradesList.ShouldScrollToLastRow = true;
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

				BaseSecurityInfo secInfo = null;
				List<string> removeList = new List<string>();

				lock (rocTradesList.RocGridTable)
				{
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
			DataRow[] rows = rocTradesList.RocGridTable.Select(string.Concat(new object[] { "SymbolDetail = '", symbolDetail, "'" }));
			foreach (DataRow row in rows)
			{
				row["Symbol"] = secInfo.MDSymbol;
				if (!rocTradesList.Symbols.Contains(secInfo.MDSymbol))
				{
					rocTradesList.Symbols.Add(secInfo.MDSymbol);
				}
				row["TickSize"] = secInfo.TickSize;
				if ((double)row["ContractSize"] != secInfo.ContractSize)
				{
					row["ContractSize"] = secInfo.ContractSize;
					row["TradeValue"] = Math.Round(GetTradedValue(row), 7);
				}
			}
		}

		private double GetTradedValue(DataRow row)
		{
			try
			{
				if (row["Qty"] != null && row["Price"] != null && row["ContractSize"] != null)
				{
					return (long)row["Qty"] * (double)row["Price"] * (double)row["ContractSize"];
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return 0;
		}

		#endregion

		private void LoadRocTrade(ROCExecution trade)
		{
			lock (rocTradesList)
			{
				if (!rocTradesList.Symbols.Contains(trade.Symbol))
				{
					rocTradesList.Symbols.Add(trade.Symbol);
				}

				if (!rocTradesList.Details.Contains(trade.SymbolDetail))
				{
					rocTradesList.Details.Add(trade.SymbolDetail);
				}

				trade = UpdateRocTradeWithSecurityInfo(trade);

				if (!rocTradesList.GridKeys.Contains(trade.OmExecTag))
				{
					rocTradesList.GridKeys.Add(trade.OmExecTag);
				
					rocTradesList.RocGridTable.Rows.Add(new object[] {
						trade.OmExecTag,
						trade.Symbol,
						trade.SymbolDetail,
						trade.SymbolDisplay,
						trade.Side,
						trade.Qty,
						trade.Price,
						trade.TradeValue,
						trade.DestID,
						trade.TradeTime,
						trade.ClearingAcct,
						trade.ContractSize,
						trade.TickSize,
						trade.DisplayFactor,
						trade.OmTag,
						trade.SecType,
						0});

					switch (trade.SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
							GLOBAL.HRDS.GetOptionChain(trade.Underlying);
							break;
						case CSVFieldIDs.SecurityTypes.SingleStockFuture:
							GLOBAL.HRDS.GetSSFutureChain(trade.Underlying);
							break;
						default:
							GLOBAL.HRDS.GetSecurityByTicker(trade.SymbolDetail);
							break;
					}
				}
				else
				{
					trade = UpdateRocTradeWithSecurityInfo(trade);

					rocTradesList.RocGridTable.LoadDataRow(new object[] {
						trade.OmExecTag,
						trade.Symbol,
						trade.SymbolDetail,
						trade.SymbolDisplay,
						trade.Side,
						trade.Qty,
						trade.Price,
						trade.TradeValue,
						trade.DestID,
						trade.TradeTime,
						trade.ClearingAcct,
						trade.ContractSize,
						trade.TickSize,
						trade.DisplayFactor,
						trade.OmTag,
						trade.SecType,
						0},  LoadOption.OverwriteChanges);
				}
			}
		}

		private ROCExecution UpdateRocTradeWithSecurityInfo(ROCExecution trade)
		{
			BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(trade.SymbolDetail);

			if (secInfo != null)
			{
				if (trade.Symbol == "")
				{
					trade.Symbol = secInfo.MDSymbol;
				}
				trade.TickSize = secInfo.TickSize;
				trade.ContractSize = secInfo.ContractSize;

				switch (trade.SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
						rocTradesList.UpdateSymbol(trade.Symbol);
						rocTradesList.UpdateTickSize(trade.Symbol, trade.TickSize);
						break;
					default:
						rocTradesList.UpdateTickSize(trade.Symbol, trade.TickSize);
						break;
				}
			}

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[trade.SymbolDetail] = trade.Symbol;
			}

			return trade;
		}

		private void LoadTposTrade(TPOSExecution trade)
		{
			lock (rocTradesList)
			{
				if (!rocTradesList.Symbols.Contains(trade.Symbol))
				{
					rocTradesList.Symbols.Add(trade.Symbol);
				}

				if (!rocTradesList.Details.Contains(trade.SymbolDetail))
				{
					rocTradesList.Details.Add(trade.SymbolDetail);
				}

				trade = UpdateTposTradeWithSecurityInfo(trade);

				if (!rocTradesList.GridKeys.Contains(trade.TradeID))
				{
					rocTradesList.GridKeys.Add(trade.TradeID);

					rocTradesList.RocGridTable.Rows.Add(new object[] {
						trade.TradeID,
						trade.Symbol,
						trade.SymbolDetail,
						trade.SymbolDisplay,
						trade.Side,
						trade.Qty,
						trade.Price,
						trade.TradeValue,
						trade.DestID,
						trade.TradeTime,
						trade.ClearingAcct,
						trade.ContractSize,
						trade.TickSize,
						trade.DisplayFactor,
						trade.OmTag,
						trade.SecType,
						1});

					switch (trade.SecType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
							GLOBAL.HRDS.GetOptionChain(trade.Underlying);
							break;
						case CSVFieldIDs.SecurityTypes.SingleStockFuture:
							GLOBAL.HRDS.GetSSFutureChain(trade.Underlying);
							break;
						default:
							GLOBAL.HRDS.GetSecurityByTicker(trade.SymbolDetail);
							break;
					}
				}
				else
				{
					trade = UpdateTposTradeWithSecurityInfo(trade);
				
					rocTradesList.RocGridTable.LoadDataRow(new object[] {
						trade.TradeID,
						trade.Symbol,
						trade.SymbolDetail,
						trade.SymbolDisplay,
						trade.Side,
						trade.Qty,
						trade.Price,
						trade.TradeValue,
						trade.DestID,
						trade.TradeTime,
						trade.ClearingAcct,
						trade.ContractSize,
						trade.TickSize,
						trade.DisplayFactor,
						trade.OmTag,
						trade.SecType,
						1}, LoadOption.OverwriteChanges);
				}
			}
		}

		private TPOSExecution UpdateTposTradeWithSecurityInfo(TPOSExecution trade)
		{
			BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(trade.SymbolDetail);

			if (secInfo != null)
			{
				if (trade.Symbol == "")
				{
					trade.Symbol = secInfo.MDSymbol;
				}
				trade.TickSize = secInfo.TickSize;
				trade.ContractSize = secInfo.ContractSize;

				switch (trade.SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
						rocTradesList.UpdateSymbol(trade.Symbol);
						rocTradesList.UpdateTickSize(trade.Symbol, trade.TickSize);
						break;
					case CSVFieldIDs.SecurityTypes.Future:
						rocTradesList.UpdateTickSize(trade.Symbol, trade.TickSize);
						break;
				}
			}
			else
			{
				trade.Symbol = "";
			}

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[trade.SymbolDetail] = trade.Symbol;
			}

			return trade;
		}

		private delegate void UpdateTradeStartDelegate();
		private void UpdateTradeStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateTradeStartDelegate(UpdateTradeStart));
				return;
			}

			IsProcessing = true;

			rocTradesList.LastFilter = rocTradesList.RocGridTable.DefaultView.RowFilter;
			rocTradesList.LastSort = rocTradesList.RocGridTable.DefaultView.Sort;

			rocTradesList.RocGridTable.BeginLoadData();
			rocTradesList.RocGridTable.DefaultView.RowFilter = "";
			rocTradesList.RocGridTable.DefaultView.Sort = "";
		}

		private delegate void UpdateTradeStopDelegate();
		private void UpdateTradeStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateTradeStopDelegate(UpdateTradeStop));
				return;
			}

			rocTradesList.RocGridTable.DefaultView.Sort = rocTradesList.LastSort;
			rocTradesList.RocGridTable.DefaultView.RowFilter = rocTradesList.LastFilter;
			rocTradesList.RocGridTable.EndLoadData();

			IsProcessing = false;
			rocTradesList.ImportDone = true;

			rocTradesList.RefreshSharedRows = true;
			rocTradesList.RefreshAggragation = true;
			rocTradesList.ShouldScrollToLastRow = true;
		}

		private delegate void UpdateAggragationDelegate();
		private void UpdateAggragation()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateAggragationDelegate(UpdateAggragation));
				return;
			}

			lock (rocTradesList.RocGridTable)
			{
				lblTradesQty.Value = rocTradesList.Rows.Count;

				// Buy
				lblBuyQty.Value = rocTradesList.BuyQty;
				lblBuyValue.Value = rocTradesList.BuyValue;

				// Sell
				lblSellQty.Value = rocTradesList.SellQty;
				lblSellValue.Value = rocTradesList.SellValue;

				// Short
				lblShortQty.Value = rocTradesList.ShortQty;
				lblShortValue.Value = rocTradesList.ShortValue;

				// Total
				lblNetQty.Value = (long)lblBuyQty.Value - (long)lblSellQty.Value - (long)lblShortQty.Value;
				lblNetValue.Value = (double)lblBuyValue.Value - (double)lblSellValue.Value - (double)lblShortValue.Value;
			}
		}

		#endregion

		#region - Actions Menu -

		private menuBaseAction _menuActions;
		private PropertyContainer _menuActionsContainer;
		private void ShowActionMenu()
		{
			if (_menuActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("Clone", "Clone");
				items.Add("Copy", "Copy");
				items.Add("Print", "Print");
				items.Add("SaveAs", "SaveAs");

				_menuActions = new menuBaseAction(items, true);
				_menuActions.PropertyChanged += new PropertyChangedEventHandler(menuActions_PropertyChanged);
			}

			if (_menuActionsContainer == null)
			{
				_menuActionsContainer = new PropertyContainer(_menuActions);
				_menuActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuActionsContainer_Closed);
			}

			_menuActionsContainer.Show(Cursor.Position);
		}

		private void menuActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Clone":
					Clone();
					break;
				case "Copy":
					rocTradesList.CopyToClipboard(true);
					break;
				case "Print":
					rocTradesList.Printer.Owner = this;
					rocTradesList.Printer.PrintPreviewDataGridView(rocTradesList, true);
					break;
				case "SaveAs":
					rocTradesList.Save();
					break;
			}
		}

		private void menuActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuActionsContainer_Closed);
			_menuActionsContainer = null;
			_menuActions.PropertyChanged -= new PropertyChangedEventHandler(menuActions_PropertyChanged);
			_menuActions = null;
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
				items.Add("ExecID", new FilterItem("ExecID", !rocTradesList.Columns["ExecID"].Visible));
				items.Add("BaseSymbol", new FilterItem("BaseSymbol", !rocTradesList.Columns["Symbol"].Visible));
				items.Add("Symbol", new FilterItem("Symbol", !rocTradesList.Columns["SymbolDetail"].Visible));
				items.Add("Display", new FilterItem("Display", !rocTradesList.Columns["SymbolDisplay"].Visible));
				items.Add("Side", new FilterItem("Side", !rocTradesList.Columns["Side"].Visible));
				items.Add("Qty", new FilterItem("Qty", !rocTradesList.Columns["Qty"].Visible));
				items.Add("Price", new FilterItem("Price", !rocTradesList.Columns["Price"].Visible));
				items.Add("Value", new FilterItem("Value", !rocTradesList.Columns["Value"].Visible));
				items.Add("Exchange", new FilterItem("Exchange", !rocTradesList.Columns["Exchange"].Visible));
				items.Add("Time", new FilterItem("Time", !rocTradesList.Columns["Time"].Visible));
				items.Add("Account", new FilterItem("Account", !rocTradesList.Columns["Account"].Visible));
				items.Add("Mult", new FilterItem("Mult", !rocTradesList.Columns["ContractSize"].Visible));
				items.Add("TSize", new FilterItem("TSize", !rocTradesList.Columns["TickSize"].Visible));
				items.Add("DFactor", new FilterItem("DFactor", !rocTradesList.Columns["DisplayFactor"].Visible));

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
				case "ExecID":
					rocTradesList.Columns["ExecID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BaseSymbol":
					rocTradesList.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Symbol":
					rocTradesList.Columns["SymbolDetail"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Display":
					rocTradesList.Columns["SymbolDisplay"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Side":
					rocTradesList.Columns["Side"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Qty":
					rocTradesList.Columns["Qty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Price":
					rocTradesList.Columns["Price"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Value":
					rocTradesList.Columns["Value"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Exchange":
					rocTradesList.Columns["Exchange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Time":
					rocTradesList.Columns["Time"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Account":
					rocTradesList.Columns["Account"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Mult":
					rocTradesList.Columns["ContractSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TSize":
					rocTradesList.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DFactor":
					rocTradesList.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
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

		#region - Filters Sources -

		public bool Extended
		{
			get
			{
				return !rocTradesList.FilterOutTPOS;
			}
		}

		private menuBaseFilter _menuSources;
		private PropertyContainer _menuSourcesContainer;
		private void cmdSources_Click(object sender, EventArgs e)
		{
			if (_menuSources == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();
				items.Add("ROC", new FilterItem("ROC", rocTradesList.FilterOutROC));
				items.Add("TPOS", new FilterItem("TPOS", rocTradesList.FilterOutTPOS));

				_menuSources = new menuBaseFilter(items);
				_menuSources.PropertyChanged += new PropertyChangedEventHandler(menuSources_PropertyChanged);
			}

			if (_menuSourcesContainer == null)
			{
				_menuSourcesContainer = new PropertyContainer(_menuSources);
				_menuSourcesContainer.Closed += new ToolStripDropDownClosedEventHandler(menuSourcesContainer_Closed);
			}

			_menuSourcesContainer.Show((VistaButton)sender);
		}

		private void menuSources_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ROC":
					rocTradesList.FilterOutROC = _menuSources.Items[e.PropertyName].Hide;
					Form_RefreshClicked(null, EventArgs.Empty);
					break;
				case "TPOS":
					rocTradesList.FilterOutTPOS = _menuSources.Items[e.PropertyName].Hide;
					Form_RefreshClicked(null, EventArgs.Empty);
					break;
			}

			SetSourcesDisplay();
		}

		private void menuSourcesContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSourcesContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuSourcesContainer_Closed);
			_menuSourcesContainer = null;
			_menuSources.PropertyChanged -= new PropertyChangedEventHandler(menuSources_PropertyChanged);
			_menuSources = null;
		}

		private void SetSourcesDisplay()
		{
			string filters = "Sources:";

			if (rocTradesList.FilterOutROC)
			{
			}
			else
			{
				filters = string.Concat(filters, " Roc");
			}

			if (rocTradesList.FilterOutTPOS)
			{
				GLOBAL.HRDS.Extended = GLOBAL.HWindows.IsExtended;
			}
			else
			{
				GLOBAL.HRDS.Extended = true;
				filters = string.Concat(filters, " Tpos");
			}

			cmdSources.Text = filters;
		}

		#endregion

		#region - Filter Securities -

		private menuBaseFilter _menuSecurities;
		private PropertyContainer _menuSecuritiesContainer;
		private void cmdSecurities_Click(object sender, EventArgs e)
		{
			if (_menuSecurities == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();

				items.Add("Stock", new FilterItem("Stock", rocTradesList.FilterOutStock));
				items.Add("Future", new FilterItem("Future", rocTradesList.FilterOutFuture));
				items.Add("Option", new FilterItem("Option", rocTradesList.FilterOutOption));

				_menuSecurities = new menuBaseFilter(items);
				_menuSecurities.PropertyChanged += new PropertyChangedEventHandler(menuSecurities_PropertyChanged);
			}

			if (_menuSourcesContainer == null)
			{
				_menuSecuritiesContainer = new PropertyContainer(_menuSecurities);
				_menuSecuritiesContainer.Closed += new ToolStripDropDownClosedEventHandler(menuSecuritiesContainer_Closed);
			}

			_menuSecuritiesContainer.Show((VistaButton)sender);
		}

		private void menuSecurities_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Stock":
					rocTradesList.FilterOutStock = _menuSecurities.Items[e.PropertyName].Hide;
					break;
				case "Future":
					rocTradesList.FilterOutFuture = _menuSecurities.Items[e.PropertyName].Hide;
					break;
				case "Option":
					rocTradesList.FilterOutOption = _menuSecurities.Items[e.PropertyName].Hide;
					break;
			}

			SetSecuritiesDisplay();
		}

		private void menuSecuritiesContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSecuritiesContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuSecuritiesContainer_Closed);
			_menuSecuritiesContainer = null;
			_menuSecurities.PropertyChanged -= new PropertyChangedEventHandler(menuSecurities_PropertyChanged);
			_menuSecurities = null;
		}

		private void SetSecuritiesDisplay()
		{
			string filters = "Securities:";

			if (rocTradesList.FilterOutStock)
			{
			}
			else
			{
				filters = string.Concat(filters, " Stock");
			}

			if (rocTradesList.FilterOutFuture)
			{
			}
			else
			{
				filters = string.Concat(filters, " Future");
			}

			if (rocTradesList.FilterOutOption)
			{
			}
			else
			{
				filters = string.Concat(filters, " Option");
			}

			cmdSecurities.Text = filters;
		}

		#endregion

		#region - Accounts -

		private menuBaseFilter _menuAccounts;
		private PropertyContainer _menuAccountsContainer;
		private void cmdAccounts_Click(object sender, EventArgs e)
		{
			if (GLOBAL.HRDS.GotUserProfiles)
			{

				if (_menuAccounts == null)
				{
					Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();
					foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
					{
						SetAccounts(trader.CSAccounts, ref items);
						SetAccounts(trader.FUTAccounts, ref items);
						SetAccounts(trader.OPTAccounts, ref items);
					}

					_menuAccounts = new menuBaseFilter(items);
					_menuAccounts.PropertyChanged += new PropertyChangedEventHandler(menuAccounts_PropertyChanged);
				}

				if (_menuAccountsContainer == null)
				{
					_menuAccountsContainer = new PropertyContainer(_menuAccounts);
					_menuAccountsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuAccountsContainer_Closed);
				}

				_menuAccountsContainer.Show((VistaButton)sender);
			}
		}

		private void menuAccounts_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				default:
					if (rocTradesList.FilterOutAccounts.Contains(e.PropertyName))
					{
						rocTradesList.FilterOutAccounts = rocTradesList.FilterOutAccounts.Replace(string.Concat(new object[] { e.PropertyName }), "");
					}
					else
					{
						rocTradesList.FilterOutAccounts = string.Concat(new object[] { rocTradesList.FilterOutAccounts, " ", e.PropertyName });
					}
					break;
			}

			SetAccountDisplay();
		}

		private void menuAccountsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuAccountsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuAccountsContainer_Closed);
			_menuAccountsContainer = null;
			_menuAccounts.PropertyChanged -= new PropertyChangedEventHandler(menuAccounts_PropertyChanged);
			_menuAccounts = null;
		}

		private void SetAccounts(Dictionary<string, AccountMap> accts, ref Dictionary<string, FilterItem> items)
		{
			foreach (AccountMap acct in accts.Values)
			{
				if (rocTradesList.FilterOutAccounts.Contains(acct.clearingAcIDShort))
				{
					if (!items.ContainsKey(acct.clearingAcIDShort))
					{
						items.Add(acct.clearingAcIDShort, new FilterItem(acct.clearingAcIDShort, true));
					}
				}
				else
				{
					if (!items.ContainsKey(acct.clearingAcIDShort))
					{
						items.Add(acct.clearingAcIDShort, new FilterItem(acct.clearingAcIDShort, false));
					}
				}
			}
		}

		private void SetAccountDisplay()
		{
			string filters = "Accounts:";

			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				filters = SetAccountFilters(trader.CSAccounts, filters);
				filters = SetAccountFilters(trader.FUTAccounts, filters);
				filters = SetAccountFilters(trader.OPTAccounts, filters);
			}

			cmdAccounts.Text = filters;

			//cmdAccounts.Text = string.Concat(new object[] { "Acct Filtered:", " ", rocTradesList.FilterOutAccounts });
		}

		private string SetAccountFilters(Dictionary<string, AccountMap> accts, string filters)
		{
			foreach (AccountMap acct in accts.Values)
			{
				if (!rocTradesList.FilterOutAccounts.Contains(acct.clearingAcIDShort))
				{
					if (filters == "")
					{
						filters = acct.clearingAcIDShort;
					}
					else if (!filters.Contains(acct.clearingAcIDShort))
					{
						filters = string.Concat(new object[] { filters, " ", acct.clearingAcIDShort });
					}
				}
			}

			return filters;
		}

		#endregion

		#region - Symbols -

		private void txtSymbolFilter_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					rocTradesList.FilterOutSymbols = txtSymbolFilter.Text;
					break;
			}
		}

		private void SetSymbolDisplay()
		{
			txtSymbolFilter.Text = rocTradesList.FilterOutSymbols;
			lblSymbols.Text = "Show Only : " + rocTradesList.FilterOutSymbols;
		}

		#endregion
	}
}