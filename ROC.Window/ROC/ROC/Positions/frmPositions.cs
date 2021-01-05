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
using LabelEx;
using CSVEx;
using MarketData;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmPositions : BaseList
	{
		#region - GroupBy -

		public sealed class GroupByTypes
		{
			public const int None = 0;
			public const int BySymbol = 1;
			public const int ByAccount = 2;
			public const int ByTrader = 3;
		}

		private int _groupBy = 1;
		public int GroupBy
		{
			get
			{
				return _groupBy;
			}
			set
			{
				_groupBy = value;
			}
		}

		#endregion

		#region - Show Aggregation -

		public bool ShowOpenQty
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspOpenQty.Visible;
				}
				else
				{
					return dspOpenQty_STK.Visible;
				}
			}
			set
			{
				dspOpenQty.Visible = lblOpenQty.Visible = value;
				dspOpenQty_STK.Visible = lblOpenQty_STK.Visible = value;
				dspOpenQty_FUT.Visible = lblOpenQty_FUT.Visible = value;
				dspOpenQty_OPT.Visible = lblOpenQty_OPT.Visible = value;
			}
		}

		public bool ShowBuyQty
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspBuyQty.Visible;
				}
				else
				{
					return dspBuyQty_STK.Visible;
				}
			}
			set
			{
				dspBuyQty.Visible = lblBuyQty.Visible = value;
				dspBuyQty_STK.Visible = lblBuyQty_STK.Visible = value;
				dspBuyQty_FUT.Visible = lblBuyQty_FUT.Visible = value;
				dspBuyQty_OPT.Visible = lblBuyQty_OPT.Visible = value;
			}
		}

		public bool ShowSellQty
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspSellQty.Visible;
				}
				else
				{
					return dspSellQty_STK.Visible;
				}
			}
			set
			{
				dspSellQty.Visible = lblSellQty.Visible = value;
				dspSellQty_STK.Visible = lblSellQty_STK.Visible = value;
				dspSellQty_FUT.Visible = lblSellQty_FUT.Visible = value;
				dspSellQty_OPT.Visible = lblSellQty_OPT.Visible = value;
			}
		}

		public bool ShowOpenPnL
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspOpenPnL.Visible;
				}
				else
				{
					return dspOpenPnL_STK.Visible;
				}
			}
			set
			{
				dspOpenPnL.Visible = lblOpenPnL.Visible = value;
				dspOpenPnL_STK.Visible = lblOpenPnL_STK.Visible = value;
				dspOpenPnL_FUT.Visible = lblOpenPnL_FUT.Visible = value;
				dspOpenPnL_OPT.Visible = lblOpenPnL_OPT.Visible = value;
			}
		}

		public bool ShowDayPnL
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspDayPnL.Visible;
				}
				else
				{
					return dspDayPnL_STK.Visible;
				}
			}
			set
			{
				dspDayPnL.Visible = lblDayPnL.Visible = value;
				dspDayPnL_STK.Visible = lblDayPnL_STK.Visible = value;
				dspDayPnL_FUT.Visible = lblDayPnL_FUT.Visible = value;
				dspDayPnL_OPT.Visible = lblDayPnL_OPT.Visible = value;
			}
		}

		public bool ShowRealizedPnL
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspRealizedPnL.Visible;
				}
				else
				{
					return dspRealizedPnL_STK.Visible;
				}
			}
			set
			{
				dspRealizedPnL.Visible = lblRealizedPnL.Visible = value;
				dspRealizedPnL_OPT.Visible = lblRealizedPnL_OPT.Visible = value;
				dspRealizedPnL_FUT.Visible = lblRealizedPnL_FUT.Visible = value;
				dspRealizedPnL_STK.Visible = lblRealizedPnL_STK.Visible = value;
			}
		}

		public bool ShowTotalPnL
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspTotalPnL.Visible;
				}
				else
				{
					return dspTotalPnL_STK.Visible;
				}
			}
			set
			{
				dspTotalPnL.Visible = lblTotalPnL.Visible = value;
				dspTotalPnL_STK.Visible = lblTotalPnL_STK.Visible = value;
				dspTotalPnL_FUT.Visible = lblTotalPnL_FUT.Visible = value;
				dspTotalPnL_OPT.Visible = lblTotalPnL_OPT.Visible = value;
			}
		}

		public bool ShowCurrentQty
		{
			get
			{
				if (ShowAllAggregation)
				{
					return dspCurrentQty.Visible;
				}
				else
				{
					return dspCurrentQty_STK.Visible;
				}
			}
			set
			{
				dspCurrentQty.Visible = lblCurrentQty.Visible = value;
				dspCurrentQty_STK.Visible = lblCurrentQty_STK.Visible = value;
				dspCurrentQty_FUT.Visible = lblCurrentQty_FUT.Visible = value;
				dspCurrentQty_OPT.Visible = lblCurrentQty_OPT.Visible = value;
			}
		}

		public bool ShowAllAggregation
		{
			get
			{
				return panelStatus.Visible;
			}
			set
			{
				panelStatus.Visible = value;
				panelStatus_OPT.Visible = !value;
				panelStatus_FUT.Visible = !value;
				panelStatus_STK.Visible = !value;
			}
		}

		#endregion

		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return rocPositionsList.Processing;
			}
			set
			{
				rocPositionsList.Processing = value;
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
					_searchView = new DataView(rocPositionsList.RocGridTable);
					_searchView.Sort = "Symbol";
				}
				return _searchView;
			}
			set
			{
				_searchView = value;
			}
		}

		#endregion

		#region - Local Variable -

		private int _avgPriceResolution = 2;

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private List<TPOSPosition> _rocPositions = new List<TPOSPosition>();
		private List<TPOSPosition> _tposPositions = new List<TPOSPosition>();
		private Market _deltas = new Market();

		#endregion

		#region - Constructor -

		public frmPositions()
		{
			InitializeComponent();

			Initialize();
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

			rocPositionsList.Initialize(GLOBAL.HPositions.Table);

			if (_avgPriceResolution != GLOBAL.AvgPriceResolution)
			{
				_avgPriceResolution = GLOBAL.AvgPriceResolution;
				rocPositionsList.UpdateResolution(_avgPriceResolution);
			}

			rocPositionsList.PropertyChanged += new PropertyChangedEventHandler(rocPositionsList_PropertyChanged);

			rocPositionsList.MouseEnter += new EventHandler(rocPositionsList_MouseEnter);
			rocPositionsList.CellMouseDoubleClick += new DataGridViewCellMouseEventHandler(rocPositionsList_CellMouseDoubleClick);

			rocPositionsList.ProcessingChanged += new EventHandler(rocPositionsList_ProcessingChanged);
			rocPositionsList.ShouldScrollToLastRowChanged += new EventHandler(rocPositionsList_ShouldScrollToLastRowChanged);
			rocPositionsList.RefreshAggragationChanged += new EventHandler(rocPositionsList_RefreshAggragationChanged);
			rocPositionsList.RefreshSharedRowsChanged += new EventHandler(rocPositionsList_RefreshSharedRowsChanged);
			StartTimer();
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (IsProcessing)
			{
				if (menuProcessing.Caption != rocPositionsList.GridKeys.Count.ToString())
				{
					menuProcessing.Caption = rocPositionsList.GridKeys.Count.ToString();
				}
			}
			else
			{
				// TODO: Make sure all selected rows are also refreshed
				rocPositionsList.RefreshSelectedRows();
			}

			if (GLOBAL.UseDelayedUpdate)
			{
				AddUpdatePositionsByProcessByTimer();
			}

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
				if (!IsProcessing)
				{
					lock (rocPositionsList)
					{
						SetColumnMode(rocPositionsList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocPositionsList);
						SetColumnMode(rocPositionsList, DataGridViewAutoSizeColumnMode.None);
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
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshPositions);
			}

			if (_avgPriceResolution != GLOBAL.AvgPriceResolution)
			{
				_avgPriceResolution = GLOBAL.AvgPriceResolution;
				rocPositionsList.UpdateResolution(_avgPriceResolution);
			}
		}

		#region - Display -

		private frmPositionsDisplay _positionsDisplay;
		private frmPositionsDisplay positionsDisplay
		{
			get
			{
				if (_positionsDisplay == null)
				{
					_positionsDisplay = new frmPositionsDisplay(new ROCPositionProfile(rocPositionsList));
					_positionsDisplay.PropertyChanged += new PropertyChangedEventHandler(positionsDisplay_PropertyChanged);
					_positionsDisplay.Disposed += new EventHandler(positionsDisplay_Disposed);
				}
				return _positionsDisplay;
			}
			set
			{
				_positionsDisplay = value;
			}
		}
		private void Form_DisplayClicked(object sender, EventArgs e)
		{
			GLOBAL.MainForm.TopMost = false;
			bool orgTopMost = TopMost;
			TopMost = false;

			positionsDisplay.ShowDialog();
			
			TopMost = orgTopMost;
			GLOBAL.MainForm.TopMost = true;
		}

		private void positionsDisplay_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Apply":
					SetProfile(positionsDisplay.GetProfile(new ROCPositionProfile(rocPositionsList)));
					break;
				case "ApplyToAll":
					ROCPositionProfile prof = positionsDisplay.GetProfile(new ROCPositionProfile(rocPositionsList));
					foreach (frmPositions w in GLOBAL.HWindows.PositionWindows.Values)
					{
						w.SetProfile(positionsDisplay.GetProfile(prof));
					}
					break;
			}
		}

		private void positionsDisplay_Disposed(object sender, EventArgs e)
		{
			if (_positionsDisplay != null)
			{
				_positionsDisplay.PropertyChanged -= new PropertyChangedEventHandler(positionsDisplay_PropertyChanged);
				_positionsDisplay.Disposed -= new EventHandler(positionsDisplay_Disposed);
			}

			_positionsDisplay = null;
		}

		public void SetProfile(ROCPositionProfile prof)
		{
			new ROCPositionProfile(rocPositionsList, prof);
		}

		#endregion

		private void Form_Load(object sender, EventArgs e)
		{
			SetSourcesDisplay();
			SetSecuritiesDisplay();
			SetSymbolDisplay();
			SetAccountDisplay();
			SetGroupByDisplay();
			RefreshPositions(null);
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void cmdShowAll_Click(object sender, EventArgs e)
		{
			rocPositionsList.FilterReset();

			SetSourcesDisplay();
			SetSecuritiesDisplay();
			SetSymbolDisplay();
			SetAccountDisplay();
			RefreshPositions(null);
		}

		private void cmdSnycPrice_Click(object sender, EventArgs e)
		{
			SnycSettlePrices();
		}

		private void SnycSettlePrices()
		{
			lock (rocPositionsList.RocGridTable)
			{
				foreach (DataRow row in rocPositionsList.RocGridTable.Rows)
				{
					if (row["SettlePrice"] != DBNull.Value && (double)row["SettlePrice"] != 0)
					{
						row["OpenAvg"] = (double)row["SettlePrice"];
					}
				}
			}
		}

		#endregion

		#region - Grid Event -

		private void rocPositionsList_ProcessingChanged(object sender, EventArgs e)
		{
			rocPositionsList.Enabled = !IsProcessing;
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

		private void rocPositionsList_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocPositionsList.ShouldScrollToLastRow)
			{
				rocPositionsList.ShouldScrollToLastRow = false;
				//rocPositionsList.ScrollToLastRow();
			}
		}

		private void rocPositionsList_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocPositionsList.RefreshAggragation)
			{
				rocPositionsList.RefreshAggragation = false;
				UpdateAggragation();
			}
		}

		private void rocPositionsList_RefreshSharedRowsChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocPositionsList.RefreshSharedRows)
			{
				rocPositionsList.RefreshSharedRows = false;
				rocPositionsList.Refresh();
				//rocPositionsList.UnshareRows();
			}
		}

		private void rocPositionsList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ShowColumnMenu":
					ShowColumnMenu();
					break;
				case "ShowActionMenu":
					ShowActionMenu();
					break;
				case "ShowSymbolFilterMenu":
					ShowSymbolFilterMenu(false);
					break;
				case "FilterOutSymbols":
					SetSymbolDisplay();
					break;
				case "ShowBuySymbolDetail":
					GetTrades(rocPositionsList.ShowBuySymbolDetail,
						string.Format("Side = {0}",	CSVEx.CSVFieldIDs.SideCodes.Buy));
					break;
				case "ShowSellSymbolDetail":
					GetTrades(rocPositionsList.ShowSellSymbolDetail,
						string.Format( "(Side = {0} or Side = {1})", 
							CSVEx.CSVFieldIDs.SideCodes.Sell, 
							CSVEx.CSVFieldIDs.SideCodes.Short));
					break;
			}
		}

		// In Case All Symbols are filtered in the position window.
		private void lblSymbols_MouseClick(object sender, MouseEventArgs e)
		{
			ShowSymbolFilterMenu(true);
		}

		private void rocPositionsList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusPositions)
			{
				ActivateGrid(rocPositionsList);
			}
		}

		private void rocPositionsList_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (rocPositionsList.Rows[e.RowIndex].Cells["SecType"].Value != null &&
					rocPositionsList.Rows[e.RowIndex].Cells["SecType"].Value.ToString() != "" &&
					rocPositionsList.Rows[e.RowIndex].Cells["SymbolDetail"].Value != null &&
					rocPositionsList.Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString() != "")
				{
					GLOBAL.HWindows.OpenWindow(
						rocPositionsList.Rows[e.RowIndex].Cells["SecType"].Value.ToString().ToUpper(),
						rocPositionsList.Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().ToUpper());
				}
			}
		}

		#endregion

		#region - Export & Import -

		public string ExportGrid()
		{
			ROCPositionProfile prof = new ROCPositionProfile(rocPositionsList);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCPositionProfile prof = (ROCPositionProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCPositionProfile(rocPositionsList, prof);
				}
			}
		}

		public void Clone()
		{
			lock (GLOBAL.HWindows.PositionWindows)
			{
				frmPositions w = new frmPositions();

				w.ImportGrid(ExportGrid());

				w.Width = this.Width;
				w.Height = this.Height;

				w.GroupBy = GroupBy;

				w.ShowOpenQty = ShowOpenQty;
				w.ShowBuyQty = ShowBuyQty;
				w.ShowSellQty = ShowSellQty;

				w.ShowOpenPnL = ShowOpenPnL;
				w.ShowDayPnL = ShowDayPnL;
				w.ShowRealizedPnL = ShowRealizedPnL;
				w.ShowTotalPnL = ShowTotalPnL;

				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}

		#endregion

		#region - Binding -
	
		// Called by Refresh and Load - Main Thread
		public void RefreshPositions(object input)
		{
			UpdatePositionStart();

			List<TPOSPosition> rocPositions = new List<TPOSPosition>();
			List<TPOSPosition> tposPositions = new List<TPOSPosition>();
			if (!rocPositionsList.FilterOutTPOS && !rocPositionsList.FilterOutROC)
			{
				rocPositions.AddRange(LoadRocPositions());
				tposPositions.AddRange(LoadTposPositions());
			}
			else if (rocPositionsList.FilterOutTPOS && !rocPositionsList.FilterOutROC)
			{
				rocPositions.AddRange(LoadRocPositions());
			}
			else if (!rocPositionsList.FilterOutTPOS && rocPositionsList.FilterOutROC)
			{
				tposPositions.AddRange(LoadTposPositions());
			}
			else
			{
				// Load Nothing
			}

			AddUpdatePositions(rocPositions, tposPositions);

			UpdatePositionStop();
		}

		// Get All ROC Positions from Main Collection
		private TPOSPosition[] LoadRocPositions()
		{
			TPOSPosition[] positions = new TPOSPosition[0];
			lock (GLOBAL.HPositions.RocItems)
			{
				positions = new TPOSPosition[GLOBAL.HPositions.RocItems.Values.Count];
				GLOBAL.HPositions.RocItems.Values.CopyTo(positions, 0);
			}
			return positions;
		}

		// Get All TPOS Positions from Main Collection
		private TPOSPosition[] LoadTposPositions()
		{
			TPOSPosition[] positions = new TPOSPosition[0];
			lock (GLOBAL.HPositions.TposItems)
			{
				positions = new TPOSPosition[GLOBAL.HPositions.TposItems.Values.Count];
				GLOBAL.HPositions.TposItems.Values.CopyTo(positions, 0);
			}
			return positions;
		}

		// Used by Main Refresh and Load Thread
		private delegate void AddUpdatePositionsDelegate(List<TPOSPosition> rocPositions, List<TPOSPosition> tposPositions);
		private void AddUpdatePositions(List<TPOSPosition> rocPositions, List<TPOSPosition> tposPositions)
		{
			if (InvokeRequired)
			{
			    BeginInvoke(new AddUpdatePositionsDelegate(AddUpdatePositions), new object[] { rocPositions, tposPositions });
			    return;
			}

			try
			{
				lock (rocPositionsList.RocGridTable)
				{
					rocPositionsList.RocGridTable.Rows.Clear();
					rocPositionsList.GridKeys.Clear();
					rocPositionsList.Symbols.Clear();
					rocPositionsList.Symbols.Add("");
					rocPositionsList.Details.Clear();
					rocPositionsList.Details.Add("");

					LoadRocPositions(rocPositions.ToArray(), true);
					LoadTposPositions(tposPositions.ToArray(), true);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#region - Used by Process Thread -

		private delegate void AddUpdatePositionsByProcessDelegate(bool updateIM, List<TPOSPosition> rocPositions, List<TPOSPosition> tposPositions, Market deltas);
		public void AddUpdatePositionsByProcess(bool updateIM, List<TPOSPosition> rocPositions, List<TPOSPosition> tposPositions, Market deltas)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				try
				{
					_updateIM = updateIM;
					lock (_rocPositions)
					{
						_rocPositions.AddRange(rocPositions.ToArray());
					}
					lock (_tposPositions)
					{
						_tposPositions.AddRange(tposPositions.ToArray());
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
					BeginInvoke(new AddUpdatePositionsByProcessDelegate(AddUpdatePositionsByProcess), new object[] { updateIM, rocPositions, tposPositions, deltas });
					return;
				}
				try
				{
					if (!IsProcessing)
					{
						lock (rocPositionsList.RocGridTable)
						{
							if (rocPositions.Count > 0)
							{
								LoadRocPositions(rocPositions.ToArray(), false);
							}
							if (tposPositions.Count > 0)
							{
								LoadTposPositions(tposPositions.ToArray(), false);
							}
							if (updateIM)
							{
								UpdateSecurityInfo();
							}
							if (!deltas.Empty)
							{
								UpdateMarketDataDeltas(deltas);
							}
							if (rocPositions.Count > 0 || tposPositions.Count > 0 || !deltas.Empty)
							{
								rocPositionsList.RefreshAggragation = true;
							}
						}
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void AddUpdatePositionsByProcessByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;
					if (!IsProcessing)
					{
						lock (rocPositionsList.RocGridTable)
						{
							List<TPOSPosition> rocPositions = new List<TPOSPosition>();
							lock (_rocPositions)
							{
								if (_rocPositions.Count > 0)
								{
									rocPositions = new List<TPOSPosition>(_rocPositions.ToArray());
									_rocPositions.Clear();
								}
							}
							if (rocPositions.Count > 0)
							{
								LoadRocPositions(rocPositions.ToArray(), false);
							}

							List<TPOSPosition> tposPositions = new List<TPOSPosition>();
							lock (_tposPositions)
							{
								if (_tposPositions.Count > 0)
								{
									tposPositions = new List<TPOSPosition>(_tposPositions.ToArray());
									_tposPositions.Clear();
								}
							}
							if (tposPositions.Count > 0)
							{
								LoadTposPositions(tposPositions.ToArray(), false);
							}

							if (_updateIM)
							{
								_updateIM = false;
								UpdateSecurityInfo();
							}

							Market deltas = new Market();
							lock (_deltas)
							{
								if (!deltas.Empty)
									deltas = Market.Replace(_deltas);
							}
							if (!deltas.Empty)
							{
								UpdateMarketDataDeltas(deltas);
							}

							if (rocPositions.Count > 0 || tposPositions.Count > 0 || !deltas.Empty)
							{
								rocPositionsList.RefreshAggragation = true;
							}
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

				BaseSecurityInfo secInfo = null;
				List<string> removeList = new List<string>();

				lock (rocPositionsList.RocGridTable)
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
						{
							ImSymbolNeeded.Remove(symbolDetail);
						}
					}
				}
			}
		}

		private void UpdateIMInfo(string symbolDetail, BaseSecurityInfo secInfo)
		{
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Option:
					HelperSubscriber.SubscribeOptionNBBO(secInfo.MDSymbol, secInfo.MDSource);
					break;
				default:
					HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
					break;
			}

			DataRow[] rows = rocPositionsList.RocGridTable.Select(string.Concat("SymbolDetail = '", symbolDetail, "'"));
			foreach (DataRow row in rows)
			{
				row["Symbol"] = secInfo.MDSymbol.Replace("/P", "/PR");
				if (!rocPositionsList.Symbols.Contains(secInfo.MDSymbol.Replace("/P", "/PR")))
				{
					rocPositionsList.Symbols.Add(secInfo.MDSymbol.Replace("/P", "/PR"));
				}
				//else if (!rocPositionsList.Symbols.Contains(secInfo.MDSymbol.Replace("/RT", "/RT")))
				//{
				//    rocPositionsList.Symbols.Add(secInfo.MDSymbol.Replace("/RT", "/RT"));
				//}
				//else if (!rocPositionsList.Symbols.Contains(secInfo.MDSymbol.Replace("/RTWI", "/RTWI")))
				//{
				//    rocPositionsList.Symbols.Add(secInfo.MDSymbol.Replace("/RTWI", "/RTWI"));
				//}

				row["TickSize"] = secInfo.TickSize;
				row["ContractSize"] = secInfo.ContractSize;
			}
		}

		private void UpdateMarketDataDeltas(Market deltas)
		{
			DataRowView[] rows = new DataRowView[0];
			TPOSPosition position = new TPOSPosition();

			lock (rocPositionsList.RocGridTable)
			{
				double price;
				long size;
				string text;

				foreach ((string symbol, Book delta) in deltas)
				{
					if (IsProcessing) return;

					if (delta.TryGetField(Book.FieldEnum.IssueSymbol, out string issueSymbol) && rocPositionsList.Symbols.Contains(issueSymbol))
					{
						rows = SearchView.FindRows(issueSymbol);

						foreach (DataRowView row in rows)
						{
							#region - Row Update -

							row.BeginEdit();

							if (delta.TryGetField(Book.FieldEnum.NetChange, out price))
								row["NetChange"] = price;

							if (delta.TryGetField(Book.FieldEnum.PctChange, out price))
								row["PctChange"] = price;

							if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size))
								row["Volume"] = size;

							if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
								row["BidPrice"] = price;

							if (delta.TryGetField(Book.FieldEnum.BidSize, out size))
								row["BidSize"] = size;

							if (delta.TryGetField(Book.FieldEnum.AskPrice, out price))
								row["AskPrice"] = price;

							if (delta.TryGetField(Book.FieldEnum.AskSize, out size))
								row["AskSize"] = size;

							if (delta.TryGetField(Book.FieldEnum.DisplayConversionFactor, out price))
								row["DisplayFactor"] = price;
							else if (row["DisplayFactor"] == DBNull.Value)
								row["DisplayFactor"] = 1;

							if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price))
								row["LastTraded"] = price;

							if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out text)) {
								switch (text.ToLower()) {
									case "none":
										break;
									case "normal":
										row["SecurityStatus"] = DBNull.Value;
										break;
									default:
										row["SecurityStatus"] = text;
										break;
								}
							}

							switch (row["SecType"].ToString())
							{
								case "F":
									if (delta.TryGetNonZero(Book.FieldEnum.SettlePrice, out price))
									{
										if (delta.TryGetField(Book.FieldEnum.SettleDateDT, out DateTime settleDate) && (settleDate.Day < DateTime.Today.Day))
										{
											// Previous Day SettlePrice
											row["OpenAvg"] = price;
											row["SettlePrice"] = price;
										}
										else
										{
											// Todays SettlePrice
											if ((double)row["OpenAvg"] == 0)
											{
												// Todays First Load
												row["OpenAvg"] = price;
											}
											row["SettlePrice"] = price;
										}
									}
									else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price))
									{
										row["OpenAvg"] = price;
									}
									break;
								case "E":
									if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price))
									{
										row["OpenAvg"] = price;
										row["SettlePrice"] = price;
									}
									else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
										row["OpenAvg"] = price;
									}
									break;
								default:
									if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
										row["OpenAvg"] = price;
										row["SettlePrice"] = price;
									}
									else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
										row["OpenAvg"] = price;
									}
									break;
							}

							row.EndEdit();

							UpdateSinglePosition(row);

							#endregion

							if (GLOBAL.Interrupt)
							{
								GLOBAL.Interrupt = false;
								Application.DoEvents();
							}
						}
					}
				}
			}
		}

		#endregion

		private void LoadRocPositions(TPOSPosition[] positions, bool isRefresh)
		{
			if (positions.Length > 0)
			{
				if (!isRefresh)
				{
					positions = ConsolidateExecutions(positions, 0);
				}

				foreach (TPOSPosition position in positions)
				{
					AddUpdatePosition(position, 0, isRefresh);
				}
			}
		}

		private void LoadTposPositions(TPOSPosition[] positions, bool isRefresh)
		{
			if (Extended && positions.Length > 0)
			{
				if (!isRefresh)
				{
					positions = ConsolidateExecutions(positions, 1);
				}

				foreach (TPOSPosition position in positions)
				{
					AddUpdatePosition(position, 1, isRefresh);
				}
			}
		}

		private TPOSPosition[] ConsolidateExecutions(TPOSPosition[] newExecs, int flag)
		{
			Dictionary<string, TPOSPosition> consolidatedPositions = new Dictionary<string, TPOSPosition>();

			foreach (TPOSPosition newExec in newExecs)
			{
				// Make a local copy
				TPOSPosition locExec = new TPOSPosition();
				if (flag == 1)
				{
					locExec.IsTPOS = true;
				}
				locExec.Merge(newExec);

				string key = GetPositionKey(locExec);

				if (consolidatedPositions.TryGetValue(key, out TPOSPosition position))
				{
					if (locExec.OpenQty != 0)
					{
						position.OpenQty += locExec.OpenQty;
					}

					if (locExec.BuyQty > 0 && locExec.BuyAvg > 0)
					{
						double cost = (position.BuyQty * position.BuyAvg) + (locExec.BuyQty * locExec.BuyAvg);
						position.BuyQty += locExec.BuyQty;
						position.BuyAvg = cost / position.BuyQty;
					}

					if (locExec.SellQty > 0 && locExec.SellAvg > 0)
					{
						double cost = (position.SellQty * position.SellAvg) + (locExec.SellQty * locExec.SellAvg);
						position.SellQty += locExec.SellQty;
						position.SellAvg = cost / position.SellQty;
					}
				}
				else
				{
					consolidatedPositions.Add(key, locExec);
				}
			}

			TPOSPosition[] result = new TPOSPosition[consolidatedPositions.Count];
			consolidatedPositions.Values.CopyTo(result, 0);

			return result;
		}

		// Single or First Position Load
		private void AddUpdatePosition(TPOSPosition position, int flag, bool isRefresh)
		{
			if (flag == 1)
			{
				position.IsTPOS = true;
			}
			if (position.SymbolDetail != "")
			{
				string key = GetPositionKey(position);

				lock (rocPositionsList.RocGridTable)
				{
					if (!rocPositionsList.GridKeys.Contains(key))
					{
						rocPositionsList.GridKeys.Add(key);

						rocPositionsList.UpdateSymbol(position.Symbol);

						if (!rocPositionsList.Details.Contains(position.SymbolDetail))
						{
							rocPositionsList.Details.Add(position.SymbolDetail);
						}

						position = UpdatePositionsWithSecurityInfo(position);
						position = UpdatePositionsWithCurrentMarketData(position);

						rocPositionsList.RocGridTable.Rows.Add(new object[] {
						key,
						position.Symbol,
						position.SymbolDetail,
						position.SymbolDisplay,
						position.BidPrice,
						position.BidSize,
						position.AskPrice,
						position.AskSize,
						position.LastTraded,
						position.NetChange,
						position.PctChange,
						position.TotalVolume,
						position.ClearingAcct,
						position.Trader,
						position.OpenQty,
						position.OpenAvg,
						position.BuyQty,
						position.BuyAvg,
						position.SellQty,
						position.SellAvg,
						position.CurrentQty,
						position.ContractSize,
						position.TickSize,
						position.DisplayFactor,
						position.OpenPnL,
						position.DayPnL,
						position.DayRealizedPnL,
						position.TotalPnL,
						position.MarketValue,
						position.SecType,
						position.SecurityStatus,
						position.Underlying,
						position.ExpDate,
						position.StrikePrice,
						position.CallPut,
						flag,
						position.SettlePrice});

						switch (position.SecType)
						{
							case CSVFieldIDs.SecurityTypes.Option:
								GLOBAL.HRDS.GetOptionChain(position.Underlying);
								break;
							case CSVFieldIDs.SecurityTypes.SingleStockFuture:
								GLOBAL.HRDS.GetSSFutureChain(position.Underlying);
								break;
							default:
								GLOBAL.HRDS.GetSecurityByTicker(position.SymbolDetail);
								break;
						}
					}
					else
					{
						DataRow[] rows = rocPositionsList.RocGridTable.Select(string.Concat("PositionKey = '", key, "'"));

						if (rows.Length > 0)
						{
							if (isRefresh)
							{
								position = UpdatePositionsWithSecurityInfo(position);
								position = UpdatePositionsWithCurrentMarketData(position);
							}

							foreach (DataRow row in rows)
							{
								UpdateSinglePosition(row, position);
							}
						}
					}
				}
			}
		}

		// Update with Play back & onLoad
		private TPOSPosition UpdatePositionsWithCurrentMarketData(TPOSPosition position)
		{
			double price;
			long size;

			if (!string.IsNullOrEmpty(position.SymbolDetail) && GLOBAL.HMarketData.Current.TryGet(position.SymbolDetail, out Book delta)) {
				if (delta.TryGetField(Book.FieldEnum.NetChange, out price))
				{
					position.NetChange = price;
				}
				if (delta.TryGetField(Book.FieldEnum.PctChange, out price)) {
					position.PctChange = price;
				}
				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size))
				{
					position.TotalVolume = size;
				}
				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price)) {
					position.BidPrice = price;
				}
				if (delta.TryGetField(Book.FieldEnum.BidSize, out size)) {
					position.BidSize = size;
				}
				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
					position.AskPrice = price;
				}
				if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
					position.AskSize = size;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price)) {
					position.LastTraded = price;
				}

				switch (position.SecType)
				{
					case "F":
						if (delta.TryGetNonZero(Book.FieldEnum.SettlePrice, out price)) {
							if (delta.TryGetField(Book.FieldEnum.SettleDateDT, out DateTime when) && (when.Day < DateTime.Today.Day))
							{
								// Previous Day SettlePrice
								position.OpenAvg = price;
								position.SettlePrice = price;
							}
							else
							{
								// Todays SettlePrice
								if (position.OpenAvg == 0)
								{
									// Todays First Load
									position.OpenAvg = price;
								}
								position.SettlePrice = price;
							}
						}
						else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
							position.OpenAvg = price;
						}
						break;
					case "E":
						if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out string status)) {
							switch (status.ToLower())
							{
								case "none":
									break;
								case "normal":
									position.SecurityStatus = "";
									break;
								default:
									position.SecurityStatus = status;
									break;
							}
						}

						if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
							position.OpenAvg = price;
							position.SettlePrice = price;
						}
						else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
							position.OpenAvg = price;
						}
						break;
					default:
						if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
							position.OpenAvg = price;
							position.SettlePrice = price;
						}
						else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
							position.OpenAvg = price;
						}
						break;
				}

				// Need to build PnL
				if (position.LastTraded == 0)
				{
					position.LastTraded = position.OpenAvg;
				}
			}

			return position;
		}

		// Update with Security Info On Play back & onLoad
		private TPOSPosition UpdatePositionsWithSecurityInfo(TPOSPosition position)
		{
			BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(position.SymbolDetail);

			if (secInfo != null)
			{
				if (position.Symbol == "")
				{
					position.Symbol = secInfo.MDSymbol;
				}
				position.TickSize = secInfo.TickSize;
				position.ContractSize = secInfo.ContractSize;

				switch (position.SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
						rocPositionsList.UpdateSymbol(position.Symbol);
						rocPositionsList.UpdateTickSize(position.Symbol, position.TickSize);
						break;
					default:
						rocPositionsList.UpdateTickSize(position.Symbol, position.TickSize);
						break;
				}
			}
			else
			{
				position.Symbol = "";
			}

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[position.SymbolDetail] = position.Symbol;
			}

			return position;
		}

		private void UpdateSinglePosition(DataRow row, TPOSPosition locExec)
		{
			double cost = 0;
			TPOSPosition orgPos = new TPOSPosition();

			orgPos.ContractSize = (double)row["ContractSize"];
			orgPos.OpenQty = (long)row["OpenQty"];
			orgPos.OpenAvg = (double)row["OpenAvg"];
			orgPos.SettlePrice = (double)row["SettlePrice"];

			orgPos.LastTraded = (double)row["LastTraded"];
			if (orgPos.LastTraded == 0)
			{
				orgPos.LastTraded = orgPos.OpenAvg;
				row["LastTraded"] = orgPos.LastTraded;
			}
			if (orgPos.OpenAvg == 0)
			{
				orgPos.OpenAvg = orgPos.LastTraded;
			}

			if (locExec.OpenQty != 0)
			{
				orgPos.OpenQty = orgPos.OpenQty + locExec.OpenQty;
				row["OpenQty"] = orgPos.OpenQty;
			}

			orgPos.BuyQty = (long)row["BuyQty"];
			orgPos.BuyAvg = (double)row["BuyAvg"];
			if (locExec.BuyQty > 0 && locExec.BuyAvg > 0)
			{
				cost = (orgPos.BuyQty * orgPos.BuyAvg) + (locExec.BuyQty * locExec.BuyAvg);
				orgPos.BuyQty = orgPos.BuyQty + locExec.BuyQty;
				orgPos.BuyAvg = cost / orgPos.BuyQty;

				row["BuyQty"] = orgPos.BuyQty;
				row["BuyAvg"] = orgPos.BuyAvg;
			}

			orgPos.SellQty = (long)row["SellQty"];
			orgPos.SellAvg = (double)row["SellAvg"];
			if (locExec.SellQty > 0 && locExec.SellAvg > 0)
			{
				cost = (orgPos.SellQty * orgPos.SellAvg) + (locExec.SellQty * locExec.SellAvg);
				orgPos.SellQty = orgPos.SellQty + locExec.SellQty;
				orgPos.SellAvg = cost / orgPos.SellQty;

				row["SellQty"] = orgPos.SellQty;
				row["SellAvg"] = orgPos.SellAvg;
			}

			row["CurrentQty"] = orgPos.CurrentQty;
			row["OpenPnL"] = orgPos.OpenPnL;
			row["DayPnL"] = orgPos.DayPnL;
			row["DayRealizedPnL"] = orgPos.DayRealizedPnL;
			row["TotalPnL"] = orgPos.TotalPnL;
			row["MarketValue"] = orgPos.MarketValue;

			if (GroupBy != 0)
			{
				if (row["Trader"] == null || row["Trader"].ToString() == "")
				{
					row["Trader"] = locExec.Trader;
				}
				else if (locExec.Trader != "" && !row["Trader"].ToString().Contains(locExec.Trader))
				{
					row["Trader"] = row["Trader"] + "," + locExec.Trader;
				}

				if (row["ClearingAcct"] == null || row["ClearingAcct"].ToString() == "")
				{
					row["ClearingAcct"] = locExec.ClearingAcct;
				}
				else if (locExec.ClearingAcct != "" && !row["ClearingAcct"].ToString().Contains(locExec.ClearingAcct))
				{
					row["ClearingAcct"] = row["ClearingAcct"] + "," + locExec.ClearingAcct;
				}
			}

			//row.AcceptChanges();
		}

		private string GetPositionKeyFromDataRow(DataRow row)
		{
			TPOSPosition pos = new TPOSPosition();

			pos.SymbolDetail = (string)row["SymbolDetail"];
			pos.Trader = (string)row["Trader"];
			pos.ClearingAcct = (string)row["ClearingAcct"];

			return pos.PositionKey;
		}

		private void UpdateSinglePosition(DataRowView row)
		{
			TPOSPosition pos = new TPOSPosition();

			try
			{
				pos.ContractSize = (double)row["ContractSize"];
				pos.OpenQty = (long)row["OpenQty"];
				pos.OpenAvg = (double)row["OpenAvg"];
				pos.SettlePrice = (double)row["SettlePrice"];

				pos.LastTraded = (double)row["LastTraded"];
				if (pos.LastTraded == 0)
				{
					pos.LastTraded = pos.OpenAvg;
					row["LastTraded"] = pos.LastTraded;
				}
				if (pos.OpenAvg == 0)
				{
					pos.OpenAvg = pos.LastTraded;
				}

				pos.BuyQty = (long)row["BuyQty"];
				pos.BuyAvg = (double)row["BuyAvg"];

				pos.SellQty = (long)row["SellQty"];
				pos.SellAvg = (double)row["SellAvg"];

				row["CurrentQty"] = pos.CurrentQty;
				row["OpenPnL"] = pos.OpenPnL;
				row["DayPnL"] = pos.DayPnL;
				row["DayRealizedPnL"] = pos.DayRealizedPnL;
				row["TotalPnL"] = pos.TotalPnL;
				row["MarketValue"] = pos.MarketValue;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private string GetPositionKey(TPOSPosition position)
		{
			string key = "";

			switch (GroupBy)
			{
				case GroupByTypes.ByAccount:
					if (position.ClearingAcct.Length > 5)
					{
						key = string.Concat(position.ClearingAcct.Substring(0, 5));
					}
					else
					{
						key = string.Concat(position.ClearingAcct);
					}
					break;
				case GroupByTypes.BySymbol:
					key = position.SymbolDetail;
					break;
				case GroupByTypes.ByTrader:
					key = position.Trader;
					break;
				case GroupByTypes.None:
				default:
					key = position.PositionKey;
					//key = position.SymbolDetail;
					break;
			}

			return key;
		}

		private delegate void UpdatePositionStartDelegate();
		private void UpdatePositionStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdatePositionStartDelegate(UpdatePositionStart));
				return;
			}

			IsProcessing = true;

			rocPositionsList.LastFilter = rocPositionsList.RocGridTable.DefaultView.RowFilter;
			rocPositionsList.LastSort = rocPositionsList.RocGridTable.DefaultView.Sort;

			rocPositionsList.RocGridTable.DefaultView.RowFilter = "";
			rocPositionsList.RocGridTable.DefaultView.Sort = "";
		}

		private delegate void UpdatePositionStopDelegate();
		private void UpdatePositionStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdatePositionStopDelegate(UpdatePositionStop));
				return;
			}

			rocPositionsList.RocGridTable.DefaultView.Sort = rocPositionsList.LastSort;
			rocPositionsList.RocGridTable.DefaultView.RowFilter = rocPositionsList.LastFilter;

			IsProcessing = false;
			rocPositionsList.ImportDone = true;

			rocPositionsList.RefreshSharedRows = true;
			rocPositionsList.RefreshAggragation = true;
			rocPositionsList.ShouldScrollToLastRow = true;
		}

		private delegate void UpdateAggragationDelegate();
		private void UpdateAggragation()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateAggragationDelegate(UpdateAggragation));
				return;
			}

			lock (rocPositionsList.RocGridTable)
			{
				lblPositionsQty.Value = rocPositionsList.Rows.Count;

				if (ShowAllAggregation)
				{
					lblOpenQty.Value = rocPositionsList.OpenQty;
					lblBuyQty.Value = rocPositionsList.BuyQty;
					lblSellQty.Value = rocPositionsList.SellQty;
					lblCurrentQty.Value = rocPositionsList.CurrentQty;

					lblOpenPnL.Value = rocPositionsList.OpenPnL;
					lblDayPnL.Value = rocPositionsList.DayPnL;
					lblRealizedPnL.Value = rocPositionsList.RealizedPnL;
					lblTotalPnL.Value = rocPositionsList.TotalPnL;
				}
				else
				{
					lblOpenQty_STK.Value = rocPositionsList.OpenQty_STK;
					lblOpenQty_FUT.Value = rocPositionsList.OpenQty_FUT;
					lblOpenQty_OPT.Value = rocPositionsList.OpenQty_OPT;

					lblBuyQty_STK.Value = rocPositionsList.BuyQty_STK;
					lblBuyQty_FUT.Value = rocPositionsList.BuyQty_FUT;
					lblBuyQty_OPT.Value = rocPositionsList.BuyQty_OPT;

					lblSellQty_STK.Value = rocPositionsList.SellQty_STK;
					lblSellQty_FUT.Value = rocPositionsList.SellQty_FUT;
					lblSellQty_OPT.Value = rocPositionsList.SellQty_OPT;

					lblCurrentQty_STK.Value = rocPositionsList.CurrentQty_STK;
					lblCurrentQty_FUT.Value = rocPositionsList.CurrentQty_FUT;
					lblCurrentQty_OPT.Value = rocPositionsList.CurrentQty_OPT;

					lblOpenPnL_STK.Value = rocPositionsList.OpenPnL_STK;
					lblOpenPnL_FUT.Value = rocPositionsList.OpenPnL_FUT;
					lblOpenPnL_OPT.Value = rocPositionsList.OpenPnL_OPT;

					lblDayPnL_STK.Value = rocPositionsList.DayPnL_STK;
					lblDayPnL_FUT.Value = rocPositionsList.DayPnL_FUT;
					lblDayPnL_OPT.Value = rocPositionsList.DayPnL_OPT;

					lblRealizedPnL_STK.Value = rocPositionsList.RealizedPnL_STK;
					lblRealizedPnL_FUT.Value = rocPositionsList.RealizedPnL_FUT;
					lblRealizedPnL_OPT.Value = rocPositionsList.RealizedPnL_OPT;

					lblTotalPnL_STK.Value = rocPositionsList.TotalPnL_STK;
					lblTotalPnL_FUT.Value = rocPositionsList.TotalPnL_FUT;
					lblTotalPnL_OPT.Value = rocPositionsList.TotalPnL_OPT;
				}
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
					rocPositionsList.CopyToClipboard(true);
					break;
				case "Print":
					rocPositionsList.Printer.Owner = this;
					rocPositionsList.Printer.PrintPreviewDataGridView(rocPositionsList, true);
					break;
				case "SaveAs":
					rocPositionsList.Save();
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
				items.Add("Key", new FilterItem("Key", !rocPositionsList.Columns["PositionKey"].Visible));
				items.Add("MD Symbol", new FilterItem("MD Symbol", !rocPositionsList.Columns["Symbol"].Visible));
				items.Add("SecType", new FilterItem("SecType", !rocPositionsList.Columns["SecType"].Visible));
				items.Add("SecurityStatus", new FilterItem("SecurityStatus", !rocPositionsList.Columns["SecurityStatus"].Visible));
				items.Add("Symbol", new FilterItem("Symbol", !rocPositionsList.Columns["SymbolDetail"].Visible));
				items.Add("Display", new FilterItem("Display", !rocPositionsList.Columns["SymbolDisplay"].Visible));
				items.Add("BidPrice", new FilterItem("BidPrice", !rocPositionsList.Columns["BidPrice"].Visible));
				items.Add("BidSize", new FilterItem("BidSize", !rocPositionsList.Columns["BidSize"].Visible));
				items.Add("AskPrice", new FilterItem("AskPrice", !rocPositionsList.Columns["AskPrice"].Visible));
				items.Add("AskSize", new FilterItem("AskSize", !rocPositionsList.Columns["AskSize"].Visible));
				items.Add("Last", new FilterItem("Last", !rocPositionsList.Columns["LastTraded"].Visible));
				items.Add("Net", new FilterItem("Net", !rocPositionsList.Columns["NetChange"].Visible));
				items.Add("Pct", new FilterItem("Pct", !rocPositionsList.Columns["PctChange"].Visible));
				items.Add("Volume", new FilterItem("Volume", !rocPositionsList.Columns["Volume"].Visible));
				items.Add("Acct", new FilterItem("Acct", !rocPositionsList.Columns["ClearingAcct"].Visible));
				items.Add("Trader", new FilterItem("Trader", !rocPositionsList.Columns["Trader"].Visible));
				items.Add("OQty", new FilterItem("OQty", !rocPositionsList.Columns["OpenQty"].Visible));
				items.Add("OAvg", new FilterItem("OAvg", !rocPositionsList.Columns["OpenAvg"].Visible));
				items.Add("SettlePrice", new FilterItem("SettlePrice", !rocPositionsList.Columns["SettlePrice"].Visible));
				items.Add("BQty", new FilterItem("BQty", !rocPositionsList.Columns["BuyQty"].Visible));
				items.Add("BAvg", new FilterItem("BAvg", !rocPositionsList.Columns["BuyAvg"].Visible));
				items.Add("SQty", new FilterItem("SQty", !rocPositionsList.Columns["SellQty"].Visible));
				items.Add("SAvg", new FilterItem("SAvg", !rocPositionsList.Columns["SellAvg"].Visible));
				items.Add("CQty", new FilterItem("CQty", !rocPositionsList.Columns["CurrentQty"].Visible));
				items.Add("Mult", new FilterItem("Mult", !rocPositionsList.Columns["ContractSize"].Visible));
				items.Add("TSize", new FilterItem("TSize", !rocPositionsList.Columns["TickSize"].Visible));
				items.Add("DFactor", new FilterItem("DFactor", !rocPositionsList.Columns["DisplayFactor"].Visible));
				items.Add("OPnL", new FilterItem("OPnL", !rocPositionsList.Columns["OpenPnL"].Visible));
				items.Add("DPnL", new FilterItem("DPnL", !rocPositionsList.Columns["DayPnL"].Visible));
				items.Add("TPnL", new FilterItem("TPnL", !rocPositionsList.Columns["TotalPnL"].Visible));
				items.Add("RPnL", new FilterItem("RPnL", !rocPositionsList.Columns["DayRealizedPnL"].Visible));
				items.Add("Market", new FilterItem("Market", !rocPositionsList.Columns["MarketValue"].Visible));

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
				case "Key":
					rocPositionsList.Columns["PositionKey"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "MD Symbol":
					rocPositionsList.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecType":
					rocPositionsList.Columns["SecType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecurityStatus":
					rocPositionsList.Columns["SecurityStatus"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Symbol":
					rocPositionsList.Columns["SymbolDetail"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Display":
					rocPositionsList.Columns["SymbolDisplay"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidPrice":
					rocPositionsList.Columns["BidPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidSize":
					rocPositionsList.Columns["BidSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskPrice":
					rocPositionsList.Columns["AskPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskSize":
					rocPositionsList.Columns["AskSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Last":
					rocPositionsList.Columns["LastTraded"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Net":
					rocPositionsList.Columns["NetChange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Pct":
					rocPositionsList.Columns["PctChange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Volume":
					rocPositionsList.Columns["Volume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Acct":
					rocPositionsList.Columns["ClearingAcct"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Trader":
					rocPositionsList.Columns["Trader"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OQty":
					rocPositionsList.Columns["OpenQty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OAvg":
					rocPositionsList.Columns["OpenAvg"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SettlePrice":
					rocPositionsList.Columns["SettlePrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BQty":
					rocPositionsList.Columns["BuyQty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BAvg":
					rocPositionsList.Columns["BuyAvg"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SQty":
					rocPositionsList.Columns["SellQty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SAvg":
					rocPositionsList.Columns["SellAvg"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "CQty":
					rocPositionsList.Columns["CurrentQty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Mult":
					rocPositionsList.Columns["ContractSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TSize":
					rocPositionsList.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DFactor":
					rocPositionsList.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OPnL":
					rocPositionsList.Columns["OpenPnL"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DPnL":
					rocPositionsList.Columns["DayPnL"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TPnL":
					rocPositionsList.Columns["TotalPnL"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "RPnL":
					rocPositionsList.Columns["DayRealizedPnL"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Market":
					rocPositionsList.Columns["MarketValue"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
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
				return !rocPositionsList.FilterOutTPOS;
			}
		}

		private menuBaseFilter _menuSources;
		private PropertyContainer _menuSourcesContainer;
		private void cmdSources_Click(object sender, EventArgs e)
		{
			if (_menuSources == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();
				items.Add("ROC", new FilterItem("ROC", rocPositionsList.FilterOutROC));
				items.Add("TPOS", new FilterItem("TPOS", rocPositionsList.FilterOutTPOS));

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
					rocPositionsList.FilterOutROC = _menuSources.Items[e.PropertyName].Hide;
					Form_RefreshClicked(null, EventArgs.Empty);
					break;
				case "TPOS":
					rocPositionsList.FilterOutTPOS = _menuSources.Items[e.PropertyName].Hide;
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

			if (rocPositionsList.FilterOutROC)
			{
			}
			else
			{
				filters = string.Concat(filters, " Roc");
			}

			if (rocPositionsList.FilterOutTPOS)
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

				items.Add("Stock", new FilterItem("Stock", rocPositionsList.FilterOutStock));
				items.Add("Future", new FilterItem("Future", rocPositionsList.FilterOutFuture));
				items.Add("Option", new FilterItem("Option", rocPositionsList.FilterOutOption));

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
					rocPositionsList.FilterOutStock = _menuSecurities.Items[e.PropertyName].Hide;
					break;
				case "Future":
					rocPositionsList.FilterOutFuture = _menuSecurities.Items[e.PropertyName].Hide;
					break;
				case "Option":
					rocPositionsList.FilterOutOption = _menuSecurities.Items[e.PropertyName].Hide;
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

			if (rocPositionsList.FilterOutStock)
			{
			}
			else
			{
				filters = string.Concat(filters, " Stock");
			}

			if (rocPositionsList.FilterOutFuture)
			{
			}
			else
			{
				filters = string.Concat(filters, " Future");
			}

			if (rocPositionsList.FilterOutOption)
			{
			}
			else
			{
				filters = string.Concat(filters, " Option");
			}

			cmdSecurities.Text = filters;
		}

		#endregion

		#region - Filter Accounts -

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
					if (rocPositionsList.FilterOutAccounts.Contains(e.PropertyName))
					{
						rocPositionsList.FilterOutAccounts = rocPositionsList.FilterOutAccounts.Replace(e.PropertyName, "");
					}
					else
					{
						rocPositionsList.FilterOutAccounts = string.Concat(rocPositionsList.FilterOutAccounts, " ", e.PropertyName);
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
				if (rocPositionsList.FilterOutAccounts.Contains(acct.clearingAcIDShort))
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
		}

		private string SetAccountFilters(Dictionary<string, AccountMap> accts, string filters)
		{
			foreach (AccountMap acct in accts.Values)
			{
				if (!rocPositionsList.FilterOutAccounts.Contains(acct.clearingAcIDShort))
				{
					if (filters == "")
					{
						filters = acct.clearingAcIDShort;
					}
					else if (!filters.Contains(acct.clearingAcIDShort))
					{
						filters = string.Concat(filters, " ", acct.clearingAcIDShort);
					}
				}
			}

			return filters;
		}

		#endregion

		#region - Fitler Symbols -

		private menuBaseAction _menuSymbolFilter;
		private PropertyContainer _menuSymbolFilterContainer;
		private void ShowSymbolFilterMenu(bool addOnly)
		{
			if (_menuSymbolFilter == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();

				// Remove Symbol
				if (!addOnly && rocPositionsList.SelectedSymbolDetail != "")
				{
					string symbol = GetBaseSymbol(rocPositionsList.SelectedSymbolDetail);
					items.Add("REM:" + symbol, "REM:" + symbol);
				}

				// Add Symbol List
				string[] filteredSymbols = GetFilteredSymbols();
				if (filteredSymbols.Length > 0)
				{
					foreach (string filteredSymbol in filteredSymbols)
					{
						items.Add("ADD:" + filteredSymbol, "ADD:" + filteredSymbol);
					}
				}

				_menuSymbolFilter = new menuBaseAction(items, true);
				_menuSymbolFilter.PropertyChanged += new PropertyChangedEventHandler(menuSymbolFilter_PropertyChanged);
			}

			if (_menuSymbolFilterContainer == null)
			{
				_menuSymbolFilterContainer = new PropertyContainer(_menuSymbolFilter);
				_menuSymbolFilterContainer.Closed += new ToolStripDropDownClosedEventHandler(menuSymbolFilterContainer_Closed);
			}

			_menuSymbolFilterContainer.Show(Cursor.Position);
		}

		private void menuSymbolFilter_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			string symbol = "";
			if (e.PropertyName.Contains("ADD:"))
			{
				symbol = e.PropertyName.Replace("ADD:", "");
				txtSymbolFilter.Text = RemoveSymbolFromFilter(symbol);
				txtSymbolFilter_KeyDown(null, new KeyEventArgs(Keys.Enter));
			}
			else if (e.PropertyName.Contains("REM:"))
			{
				symbol = e.PropertyName.Replace("REM:", "");
				txtSymbolFilter.Text = AddSymbolToFilter(symbol);
				txtSymbolFilter_KeyDown(null, new KeyEventArgs(Keys.Enter));
			}

			_menuSymbolFilterContainer.Close();
		}

		private void menuSymbolFilterContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSymbolFilterContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuSymbolFilterContainer_Closed);
			_menuSymbolFilterContainer = null;
			_menuSymbolFilter.PropertyChanged -= new PropertyChangedEventHandler(menuSymbolFilter_PropertyChanged);
			_menuSymbolFilter = null;
		}

		private string RemoveSymbolFromFilter(string symbol)
		{
			string[] filteredSymbols = GetFilteredSymbols();

			if (filteredSymbols.Length > 0)
			{
				List<string> symbols = new List<string>();
				foreach (string filteredSymbol in filteredSymbols)
				{
					if (symbol != filteredSymbol && !symbols.Contains(filteredSymbol))
					{
						symbols.Add(filteredSymbol);
					}
				}

				if (symbols.Count > 1)
				{
					return string.Join(",", symbols.ToArray());
				}
				else if (symbols.Count == 1)
				{
					return symbols[0];
				}
			}

			return "";
		}

		private string AddSymbolToFilter(string symbol)
		{
			symbol = GetBaseSymbol(symbol);

			string[] filteredSymbols = GetFilteredSymbols();
			
			if (filteredSymbols.Length > 0)
			{
				List<string> symbols = new List<string>();
				symbols.Add(symbol);
				foreach (string filteredSymbol in filteredSymbols)
				{
					if (!symbols.Contains(filteredSymbol))
					{
						symbols.Add(filteredSymbol);
					}
				}

				if (symbols.Count > 1)
				{
					return string.Join(",", symbols.ToArray());
				}
				else if (symbols.Count == 1)
				{
					return symbols[0];
				}
			}

			return symbol;
		}

		private string[] GetFilteredSymbols()
		{
			if (rocPositionsList.FilterOutSymbols != "")
			{
				return rocPositionsList.FilterOutSymbols.Trim().Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
			}
			else
			{
				return new string[0];
			}
		}

		private string GetBaseSymbol(string symbol)
		{
			if (symbol.Contains(" "))
			{
				string[] baseSymbols = symbol.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

				if (baseSymbols.Length > 0)
				{
					symbol = baseSymbols[0];
				}
			}

			return symbol;
		}

		private void txtSymbolFilter_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					rocPositionsList.FilterOutSymbols = txtSymbolFilter.Text;
					break;
			}
		}

		private void SetSymbolDisplay()
		{
			txtSymbolFilter.Text = rocPositionsList.FilterOutSymbols;
			lblSymbols.Text = "Removed Symbols : " + rocPositionsList.FilterOutSymbols;
		}

		#endregion

		#region - Group By -

		private menuBaseAction _menuGroupBy;
		private PropertyContainer _menuGroupByContainer;
		private void cmdGroupBy_Click(object sender, EventArgs e)
		{
			if (_menuGroupBy == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("Symbol", "Symbol");
				items.Add("None", "None");

				_menuGroupBy = new menuBaseAction(items);
				_menuGroupBy.PropertyChanged += new PropertyChangedEventHandler(menuGroupBy_PropertyChanged);
			}

			if (_menuGroupByContainer == null)
			{
				_menuGroupByContainer = new PropertyContainer(_menuGroupBy);
				_menuGroupByContainer.Closed += new ToolStripDropDownClosedEventHandler(menuGroupByContainer_Closed);
			}

			_menuGroupByContainer.Show((VistaButton)sender);
		}

		private void menuGroupBy_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Symbol":
					GroupBy = GroupByTypes.BySymbol;
					Form_RefreshClicked(null, EventArgs.Empty);
					break;
				case "None":
					GroupBy = GroupByTypes.None;
					Form_RefreshClicked(null, EventArgs.Empty);
					break;
			}

			SetGroupByDisplay();
		}

		private void menuGroupByContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuGroupByContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuGroupByContainer_Closed);
			_menuGroupByContainer = null;
			_menuGroupBy.PropertyChanged -= new PropertyChangedEventHandler(menuGroupBy_PropertyChanged);
			_menuGroupBy = null;
		}

		private void SetGroupByDisplay()
		{
			string groupBy = "Group By:";

			switch(GroupBy)
			{
				case GroupByTypes.BySymbol:
					groupBy = string.Concat(groupBy, " Symbol");
					break;
				case GroupByTypes.None:
					break;
			}

			cmdGroupBy.Text = groupBy;
		}

		#endregion

		#region - Trades -

		private void GetTrades(string symbol, string sideFilter)
		{
			if (symbol != "")
			{
				DataRow[] rows = GLOBAL.HExecutions.Table.Select(
					string.Format( "SymbolDetail = '{0}' and {1}",
						symbol,
						sideFilter));

				if (rows.Length > 0)
				{
					UpdateTradeSecurityInfo(ref rows);
					ShowSubTrades(rows);
				}
			}
		}

		private void UpdateTradeSecurityInfo(ref DataRow[] rows)
		{
			string symbolDetail = "";
			string secType = "";

			// Update the rows with right tick size
			foreach (DataRow row in rows)
			{
				symbolDetail = (string)row["symbolDetail"];
				secType = (string)row["SecType"];

				BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

				if (secInfo != null)
				{
					row["Symbol"] = secInfo.MDSymbol;
					row["TickSize"] = secInfo.TickSize;

					switch (secType)
					{
						case CSVFieldIDs.SecurityTypes.Option:
							row["ContractSize"] = 100;
							break;
						case CSVFieldIDs.SecurityTypes.Future:
							row["ContractSize"] = secInfo.ContractSize;
							break;
						default:
							row["ContractSize"] = 1;
							break;
					}
				}
			}
		}

		private menuBaseTrades _menuSubTrades;
		private PropertyContainer _menuSubTradesContainer;
		private void ShowSubTrades(DataRow[] trades)
		{
			if (_menuSubTrades == null)
			{
				_menuSubTrades = new menuBaseTrades(trades, new ROCPositionProfile(rocPositionsList));
			}

			if (_menuSubTradesContainer == null)
			{
				_menuSubTradesContainer = new PropertyContainer(_menuSubTrades);
				_menuSubTradesContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOrderTradesContainer_Closed);
			}

			_menuSubTradesContainer.Show(Control.MousePosition.X, Control.MousePosition.Y);
		}

		private void menuOrderTradesContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSubTradesContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuOrderTradesContainer_Closed);
			_menuSubTradesContainer = null;
			_menuSubTrades = null;
		}
		
		#endregion

		#region - Filter Aggregations -

		private menuBaseFilter _menuAggregation;
		private PropertyContainer _menuAggregationContainer;
		private void Aggregation_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuAggregation == null)
					{
						Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();

						items.Add("OpenQty", new FilterItem("OpenQty", !ShowOpenQty));
						items.Add("BuyQty", new FilterItem("BuyQty", !ShowBuyQty));
						items.Add("SellQty", new FilterItem("SellQty", !ShowSellQty));
						items.Add("OpenPnL", new FilterItem("OpenPnL", !ShowOpenPnL));
						items.Add("DayPnL", new FilterItem("DayPnL", !ShowDayPnL));
						items.Add("RealizedPnL", new FilterItem("RealizedPnL", !ShowRealizedPnL));
						items.Add("TotalPnL", new FilterItem("TotalPnL", !ShowTotalPnL));
						items.Add("CurrentQty", new FilterItem("CurrentQty", !ShowCurrentQty));
						items.Add("ShowAll", new FilterItem("ShowAll", !ShowAllAggregation));
						
						_menuAggregation = new menuBaseFilter(items, true);
						_menuAggregation.PropertyChanged += new PropertyChangedEventHandler(menuAggregation_PropertyChanged);

					}

					if (_menuAggregationContainer == null)
					{
						_menuAggregationContainer = new PropertyContainer(_menuAggregation);
						_menuAggregationContainer.Closed += new ToolStripDropDownClosedEventHandler(menuAggregationContainer_Closed);
					}

					_menuAggregationContainer.Show((LabelVista)sender);
					break;
			}
		}

		private void menuAggregation_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "OpenQty":
					ShowOpenQty = !ShowOpenQty;
					break;
				case "BuyQty":
					ShowBuyQty = !ShowBuyQty;
					break;
				case "SellQty":
					ShowSellQty = !ShowSellQty;
					break;
				case "OpenPnL":
					ShowOpenPnL = !ShowOpenPnL;
					break;
				case "DayPnL":
					ShowDayPnL = !ShowDayPnL;
					break;
				case "RealizedPnL":
					ShowRealizedPnL = !ShowRealizedPnL;
					break;
				case "TotalPnL":
					ShowTotalPnL = !ShowTotalPnL;
					break;
				case "CurrentQty":
					ShowCurrentQty = !ShowCurrentQty;
					break;
				case "ShowAll":
					ShowAllAggregation = !ShowAllAggregation;
					break;
			}
		}

		private void menuAggregationContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuAggregationContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuAggregationContainer_Closed);
			_menuAggregationContainer = null;
			_menuAggregation.PropertyChanged -= new PropertyChangedEventHandler(menuAggregation_PropertyChanged);
			_menuAggregation = null;
		}

		#endregion

		#region - Cover Menu -

		private class ClipboardColums
		{
			public const int Symbol = 0;
			public const int Qty = 1;
			public const int Side = 2;
			public const int Price = 3;
			public const int Exchange = 4;
			public const int ClearingAcct = 5;
			public const int Trader = 6;
			public const int Display = 7;
			public const int ShortLender = 8;
			public const int TIF = 9;
			public const int Type = 10;
		}

		private menuBaseAction _menuCover;
		private PropertyContainer _menuCoverContainer;
		private void cmdCover_Click(object sender, EventArgs e)
		{
			if (_menuCover == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("SEL 5%", "SEL 5%");
				items.Add("SEL 15%", "SEL 15%");
				items.Add("SEL 25%", "SEL 25%");
				items.Add("SEL 50%", "SEL 50%");
				items.Add("SEL 75%", "SEL 75%");
				items.Add("SEL ALL", "SEL ALL");
				items.Add("", "");
				items.Add("5%", "5%");
				items.Add("15%", "15%");
				items.Add("25%", "25%");
				items.Add("50%", "50%");
				items.Add("75%", "75%");
				items.Add("ALL", "ALL");

				_menuCover = new menuBaseAction(items, true);
				_menuCover.PropertyChanged += new PropertyChangedEventHandler(menuCover_PropertyChanged);
			}

			if (_menuCoverContainer == null)
			{
				_menuCoverContainer = new PropertyContainer(_menuCover);
				_menuCoverContainer.Closed += new ToolStripDropDownClosedEventHandler(menuCoverContainer_Closed);
			}

			_menuCoverContainer.Show((VistaButton)sender);
		}

		private void menuCover_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "5%":
					CopyCoverToClipboard((decimal)5 / 100);
					break;
				case "15%":
					CopyCoverToClipboard((decimal)15 / 100);
					break;
				case "25%":
					CopyCoverToClipboard((decimal)25 / 100);
					break;
				case "50%":
					CopyCoverToClipboard((decimal)50 / 100);
					break;
				case "75%":
					CopyCoverToClipboard((decimal)75 / 100);
					break;
				case "ALL":
					CopyCoverToClipboard(1);
					break;
				case "SEL 5%":
					CopyCoverToClipboardSelected((decimal)5 / 100);
					break;
				case "SEL 15%":
					CopyCoverToClipboardSelected((decimal)15 / 100);
					break;
				case "SEL 25%":
					CopyCoverToClipboardSelected((decimal)25 / 100);
					break;
				case "SEL 50%":
					CopyCoverToClipboardSelected((decimal)50 / 100);
					break;
				case "SEL 75%":
					CopyCoverToClipboardSelected((decimal)75 / 100);
					break;
				case "SEL ALL":
					CopyCoverToClipboardSelected(1);
					break;
			}

			_menuCoverContainer.Close();
		}

		private void menuCoverContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuCoverContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuCoverContainer_Closed);
			_menuCoverContainer = null;
			_menuCover.PropertyChanged -= new PropertyChangedEventHandler(menuCover_PropertyChanged);
			_menuCover = null;
		}

		private void CopyCoverToClipboardSelected(decimal pct)
		{
			try
			{
				lock (rocPositionsList)
				{
					List<string> rowValues = new List<string>();
					List<string> tableValues = new List<string>();
					foreach (DataGridViewRow row in rocPositionsList.SelectedRows)
					{
						if (row.Cells["SymbolDetail"].Value != null && row.Cells["SymbolDetail"].Value.ToString() != "")
						{
							// Symbol (0)
							rowValues = new List<string>();
							AddRowValues(row, "SymbolDetail", ref rowValues);

							if (AddRowVluesQtySide(row, pct, ref rowValues))
							{
								// Price (3)
								rowValues.Add("");

								// Exchange (4)
								rowValues.Add("");

								// ClearingAcct (5)
								AddRowValues(row, "ClearingAcct", ref rowValues);

								// Trader (6)
								AddRowValues(row, "Trader", ref rowValues);

								// If There is match exchange setup
								LoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(rowValues[0], CSVEx.CSVFieldIDs.SecurityTypes.Equity), ref rowValues);

								// Display (7)
								rowValues.Add("");

								// ShortLender (8)
								rowValues.Add("");

								// TIF (9)
								rowValues.Add("DAY");

								// Type (10)
								rowValues.Add("MKT");

								tableValues.Add(String.Join(rocPositionsList.ColSplit, rowValues.ToArray()));
							}
						}
					}

					if (tableValues.Count > 0)
					{
						Clipboard.SetDataObject(String.Join(rocPositionsList.RowSplit, tableValues.ToArray()));
						lock (GLOBAL.HWindows.BatchMarketTicketWindows)
						{
							GLOBAL.HWindows.OpenWindow(new frmBatchMarketTicket(true), true);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void CopyCoverToClipboard(decimal pct)
		{
			try
			{
				lock (rocPositionsList)
				{
					DataRow[] rows = rocPositionsList.GetCurrentNonZeroPositions();
					List<string> rowValues = new List<string>();
					List<string> tableValues = new List<string>();
					foreach (DataRow row in rows)
					{
						if (row["SymbolDetail"] != DBNull.Value)
						{
							// Symbol
							rowValues = new List<string>();
							AddRowValues(row, "SymbolDetail", ref rowValues);

							// QtySide
							if (AddRowVluesQtySide(row, pct, ref rowValues))
							{
								// If There is Qty to Cover

								// Price
								rowValues.Add("");

								// Exchange
								rowValues.Add("");

								// ClearingAcct
								AddRowValues(row, "ClearingAcct", ref rowValues);

								// Trader
								AddRowValues(row, "Trader", ref rowValues);

								// If There is match exchange setup
								LoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(rowValues[0], CSVEx.CSVFieldIDs.SecurityTypes.Equity), ref rowValues);

								// Display (7)
								rowValues.Add("");

								// ShortLender (8)
								rowValues.Add("");

								// TIF (9)
								rowValues.Add("DAY");

								// Type (10)
								rowValues.Add("MKT");

								tableValues.Add(String.Join(rocPositionsList.ColSplit, rowValues.ToArray()));
							}
						}
					}

					if (tableValues.Count > 0)
					{
						Clipboard.SetDataObject(String.Join(rocPositionsList.RowSplit, tableValues.ToArray()));
						lock (GLOBAL.HWindows.BatchMarketTicketWindows)
						{
							GLOBAL.HWindows.OpenWindow(new frmBatchMarketTicket(true), true);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void LoadSymbolDefaults(SymbolSettingData data, ref List<string> rowValues)
		{
			if (data.tradeFor == rowValues[ClipboardColums.Trader] && data.account == rowValues[ClipboardColums.ClearingAcct])
			{
				rowValues[ClipboardColums.Exchange] = data.exchange;
			}
		}

		private bool AddRowVluesQtySide(DataGridViewRow row, decimal pct, ref List<string> rowValues)
		{
			if (row.Cells["CurrentQty"].Value != DBNull.Value && row.Cells["CurrentQty"].Value.ToString() != "")
			{
				// Qty
				long qty = (long)Math.Floor((long)row.Cells["CurrentQty"].Value * pct);
				rowValues.Add(Math.Abs(qty).ToString("N0"));

				if (qty == 0)
				{
					return false;
				}

				if (row.Cells["SecType"].Value != DBNull.Value && row.Cells["SecType"].Value.ToString() != "")
				{
					switch (row.Cells["SecType"].Value.ToString())
					{
						case CSVEx.CSVFieldIDs.SecurityTypes.Option:
							// don't set the side for options
							rowValues.Add("");
							break;
						default:
							// Side
							if (qty < 0)
							{
								rowValues.Add("BUY");
							}
							else if (qty > 0)
							{
								rowValues.Add("SELL");
							}
							else
							{
								rowValues.Add("");
							}
							break;
					}
				}

				return true;
			}
			else
			{
				rowValues.Add("");
				rowValues.Add("");
			}

			return false;
		}

		private void AddRowValues(DataGridViewRow row, string fieldName, ref List<string> rowValues)
		{
			if (row.Cells[fieldName].Value != DBNull.Value && row.Cells[fieldName].Value.ToString() != "")
			{
				rowValues.Add(row.Cells[fieldName].Value.ToString());
			}
			else
			{
				rowValues.Add("");
			}
		}

		private bool AddRowVluesQtySide(DataRow row, decimal pct, ref List<string> rowValues)
		{
			if (row["CurrentQty"] != DBNull.Value)
			{
				// Qty
				long qty = (long)((long)row["CurrentQty"] * pct);
				rowValues.Add(Math.Abs(qty).ToString("N0"));

				if (qty == 0)
				{
					return false;
				}

				if (row["SecType"] != DBNull.Value)
				{
					switch ((string)row["SecType"])
					{
						case CSVEx.CSVFieldIDs.SecurityTypes.Option:
							// don't set the side for options
							rowValues.Add("");
							break;
						default:
							// Side
							if (qty < 0)
							{
								rowValues.Add("BUY");
							}
							else if (qty > 0)
							{
								rowValues.Add("SELL");
							}
							else
							{
								rowValues.Add("");
							}
							break;
					}
				}

				return true;
			}
			else
			{
				rowValues.Add("");
				rowValues.Add("");
			}

			return false;
		}

		private void AddRowValues(DataRow row, string fieldName, ref List<string> rowValues)
		{
			if (row[fieldName] != DBNull.Value)
			{
				rowValues.Add((string)row[fieldName]);
			}
			else
			{
				rowValues.Add("");
			}
		}

		#endregion
	}
}