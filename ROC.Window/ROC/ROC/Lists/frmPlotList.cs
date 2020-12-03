using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using FormEx;
using PlotEx;
using DataGridViewEx;
using MarketDataEx;
using ContextMenuEx;
using CSVEx;
using RDSEx;
using ButtonEx;
using SerializationEx;
using ArrayEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmPlotList : BaseList
	{
		#region - Latency Check -

		// Key = Symbol + Source and the first linetime to current time delta;
		private Dictionary<string, double> _monitorBaseLineDelta_MAMA = new Dictionary<string, double>();
		private Dictionary<string, double> _monitorBaseLineDelta_MDS = new Dictionary<string, double>();

		// Key = Symbol + Source and List of Deltas (for basline calculation 
		private Dictionary<string, List<double>> _monitorBaseLineDeltaLists_MAMA = new Dictionary<string, List<double>>();
		private Dictionary<string, List<double>> _monitorBaseLineDeltaLists_MDS = new Dictionary<string, List<double>>();

		//private int _latencyLimit = 1000;
		private int _driftLimit = 20;
		private bool _checkLatency = false;
		private bool checkLatency
		{
			get
			{
				if (rocPlotList.Columns["LineTime"].Visible ||
					rocPlotList.Columns["DecodeTime"].Visible ||
					rocPlotList.Columns["OnMsgTime"].Visible ||
					rocPlotList.Columns["ServerSentTime"].Visible ||
					rocPlotList.Columns["ClientRecivedTime"].Visible)
				{
					_checkLatency = true;
				}
				else
				{
					_checkLatency = false;
				}
				return _checkLatency;
			}
		}

		#endregion

		#region - Properties -

		[Browsable(false)]
		public bool IsProcessing
		{
			get
			{
				return rocPlotList.Processing;
			}
			set
			{
				rocPlotList.Processing = value;
			}
		}

		private List<string> _lastSavedSymbolDetailList;
		[Browsable(false)]
		public List<string> LastSavedSymbolDetailList
		{
			get
			{
				if (_lastSavedSymbolDetailList == null)
				{
					_lastSavedSymbolDetailList = new List<string>();
				}
				return _lastSavedSymbolDetailList;
			}
			set
			{
				_lastSavedSymbolDetailList = value;
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
					_searchView = new DataView(rocPlotList.RocGridTable);
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

		private Dictionary<string, HelperPlot.BarGraphValues> _sybmolBars = new Dictionary<string, HelperPlot.BarGraphValues>();
		private Dictionary<string, HelperPlot.LineGraphValues> _symbolLines = new Dictionary<string, HelperPlot.LineGraphValues>();

		#endregion

		#region - Constructor -

		public frmPlotList()
		{
			InitializeComponent();

			Initialize();
		}

		public void Initialize()
		{
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);

			RefreshClicked += new EventHandler(Form_RefreshClicked);
			DisplayClicked += new EventHandler(Form_DisplayClicked);
			UITimerTicked += new EventHandler(Form_UITimerTicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);

			rocPlotList.Initialize();
			rocPlotList.PropertyChanged += new PropertyChangedEventHandler(rocPlotList_PropertyChanged);

			rocPlotList.CellMouseDoubleClick += new DataGridViewCellMouseEventHandler(rocPlotList_CellMouseDoubleClick);
			
			rocPlotList.MouseEnter += new EventHandler(rocPlotList_MouseEnter);

			rocPlotList.SelectionChanged += new EventHandler(rocPlotList_SelectionChanged);

			rocPlotList.ProcessingChanged += new EventHandler(rocPlotList_ProcessingChanged);
			rocPlotList.ShouldScrollToLastRowChanged += new EventHandler(rocPlotList_ShouldScrollToLastRowChanged);
			rocPlotList.RefreshAggragationChanged += new EventHandler(rocPlotList_RefreshAggragationChanged);

			zedTradedVolumesGraph.PointValueEvent += new ZedGraphControl.PointValueHandler(zedTradedVolumesGraph_PointValueEvent);
			zedTradedVolumesGraph.MouseEnter += new EventHandler(zedTradedVolumesGraph_MouseEnter);

			zedTradedGraph.PointValueEvent += new ZedGraphControl.PointValueHandler(zedTradedGraph_PointValueEvent);
			zedTradedGraph.ContextMenuBuilder += new ZedGraphControl.ContextMenuBuilderEventHandler(zedTradedGraph_ContextMenuBuilder);
			zedTradedGraph.MouseEnter += new EventHandler(zedTradedGraph_MouseEnter);

			StartTimer();
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			Caption = _currentSymbolDetail;

			// TODO: Make sure all selected rows are also refreshed
			rocPlotList.RefreshSelectedRows();

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
				if (!IsProcessing)
				{
					lock (rocPlotList)
					{
						SetColumnMode(rocPlotList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocPlotList);
						SetColumnMode(rocPlotList, DataGridViewAutoSizeColumnMode.None);
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
			DialogResult dr = MessageBox.Show("Save current symbol order before refresh?", "Alert", MessageBoxButtons.YesNoCancel);
			if (!IsProcessing)
			{
				switch (dr)
				{
					case DialogResult.Yes:
						SaveSymbolList();
						System.Threading.ThreadPool.QueueUserWorkItem(RefreshWatchList);
						break;
					case DialogResult.No:
						System.Threading.ThreadPool.QueueUserWorkItem(RefreshWatchList);
						break;
					case DialogResult.Cancel:
					default:
						break;
				}
			}
		}

		#region - Display -

		private frmWatchListDisplay _plotListDisplay;
		private frmWatchListDisplay plotListDisplay
		{
			get
			{
				if (_plotListDisplay == null)
				{
					_plotListDisplay = new frmWatchListDisplay(new ROCWatchListProfile(rocPlotList));
					_plotListDisplay.PropertyChanged += new PropertyChangedEventHandler(plotListDisplay_PropertyChanged);
					_plotListDisplay.Disposed += new EventHandler(plotListDisplay_Disposed);
				}
				return _plotListDisplay;
			}
			set
			{
				_plotListDisplay = value;
			}
		}
		private void Form_DisplayClicked(object sender, EventArgs e)
		{
			plotListDisplay.ShowDialog();
		}

		private void plotListDisplay_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Apply":
					SetProfile(plotListDisplay.GetProfile(new ROCWatchListProfile(rocPlotList)));
					break;
				case "ApplyToAll":
					ROCWatchListProfile prof = plotListDisplay.GetProfile(new ROCWatchListProfile(rocPlotList));
					foreach (frmPlotList w in GLOBAL.HWindows.PlotListWindows.Values)
					{
						w.SetProfile(plotListDisplay.GetProfile(prof));
					}
					break;
			}
		}

		private void plotListDisplay_Disposed(object sender, EventArgs e)
		{
			if (_plotListDisplay != null)
			{
				_plotListDisplay.PropertyChanged -= new PropertyChangedEventHandler(plotListDisplay_PropertyChanged);
				_plotListDisplay.Disposed -= new EventHandler(plotListDisplay_Disposed);
			}

			_plotListDisplay = null;
		}

		public void SetProfile(ROCWatchListProfile prof)
		{
			new ROCWatchListProfile(rocPlotList, prof);
		}

		#endregion

		private void Form_Load(object sender, EventArgs e)
		{
			CreateGraphTradedVolumes();
			CreateGraphTraded();

			RefreshWatchList(null);
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		#endregion

		#region - Grid Event -

		private void rocPlotList_ProcessingChanged(object sender, EventArgs e)
		{
			rocPlotList.Enabled = !IsProcessing;
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

		private void rocPlotList_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocPlotList.ShouldScrollToLastRow)
			{
				rocPlotList.ShouldScrollToLastRow = false;
				//rocWatchList.ScrollToLastRow();
			}
		}

		private void rocPlotList_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocPlotList.RefreshAggragation)
			{
				//rocWatchList.UnshareRows();

				rocPlotList.RefreshAggragation = false;
				//UpdateAggragation();
			}
		}

		private void rocPlotList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			lock (rocPlotList.RocGridTable)
			{
				switch (e.PropertyName)
				{
					case "ShowColumnMenu":
						ShowColumnMenu();
						break;
					case "ShowActionMenu":
						ShowActionMenu();
						break;
					case "SubscribeSymbolDetail":
						UpdateDataRows(rocPlotList.SubscribeSymbolDetail);
						break;
					case "ShowMenu":
						ShowGridMenu();
						break;
				}
			}
		}

		private void rocPlotList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusPlotList)
			{
				ActivateGrid(rocPlotList);
			}
		}

		private void rocPlotList_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (rocPlotList.Rows[e.RowIndex].Cells["SecType"].Value != null &&
					rocPlotList.Rows[e.RowIndex].Cells["SecType"].Value.ToString() != "" &&
					rocPlotList.Rows[e.RowIndex].Cells["SymbolDetail"].Value != null &&
					rocPlotList.Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString() != "")
				{
					GLOBAL.HWindows.OpenWindow(
						rocPlotList.Rows[e.RowIndex].Cells["SecType"].Value.ToString().ToUpper(),
						rocPlotList.Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().ToUpper());
				}
			}
		}

		private void rocPlotList_SelectionChanged(object sender, EventArgs e)
		{
			try
			{
				if (rocPlotList.SelectedRows.Count > 0 &&
					rocPlotList.SelectedRows[0].Cells["SymbolDetail"].Value != null &&
					rocPlotList.SelectedRows[0].Cells["SymbolDetail"].Value.ToString() != "")
				{
					_currentSymbolDetail = rocPlotList.SelectedRows[0].Cells["SymbolDetail"].Value.ToString().ToUpper();

					UpdateGraphScales(zedTradedVolumesGraph, _currentSymbolDetail);
					UpdateGraphTradedScales(zedTradedGraph, _currentSymbolDetail);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Graph Event -

		private string _currentSymbolDetail = "";
		public string CurrentSymbolDetail
		{
			get
			{
				return _currentSymbolDetail;
			}
			set
			{
				_currentSymbolDetail = value;
			}
		}

		private string zedTradedVolumesGraph_PointValueEvent(object sender, GraphPane pane, CurveItem curve, int iPt)
		{
			PointPair pt = curve[iPt];
			return string.Format("P:{1}{0}V:{2}", Environment.NewLine, pt.X.ToString("F2"), pt.Y.ToString("G0"));
		}

		private void zedTradedVolumesGraph_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusPlotList)
			{
				Activate();
				zedTradedVolumesGraph.Select();
			}
		}

		private void zedTradedGraph_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusPlotList)
			{
				Activate();
				zedTradedGraph.Select();
			}
		}

		private string zedTradedGraph_PointValueEvent(object sender, GraphPane pane, CurveItem curve, int iPt)
		{
			PointPair pt = curve[iPt];
			return string.Format("T:{1}{0}P:{2}{0}V:{3}", Environment.NewLine, XDate.XLDateToDateTime(pt.X).ToString("HH:mm:ss.fff"), pt.Y.ToString("F2"), pt.Z.ToString("G0"));
		}

		private void zedTradedGraph_ContextMenuBuilder(ZedGraphControl control, ContextMenuStrip menuStrip, Point mousePt, ZedGraphControl.ContextMenuObjectState objState)
		{
			ToolStripMenuItem item;

			item = new ToolStripMenuItem();
			item.Name = "Show Data Points";
			item.Tag = "show_data_points";
			item.Text = "Show Points";
			item.Click += new System.EventHandler(TradedGraphSpeicalCommands);
			menuStrip.Items.Add(item);

			item = new ToolStripMenuItem();
			item.Name = "Hide Data Points";
			item.Tag = "hide_data_points";
			item.Text = "Hide Points";
			item.Click += new System.EventHandler(TradedGraphSpeicalCommands);
			menuStrip.Items.Add(item);

			item = new ToolStripMenuItem();
			item.Name = "Show Line";
			item.Tag = "show_line";
			item.Text = "Show Line";
			item.Click += new System.EventHandler(TradedGraphSpeicalCommands);
			menuStrip.Items.Add(item);

			item = new ToolStripMenuItem();
			item.Name = "Hide Line";
			item.Tag = "hide_line";
			item.Text = "Hide Line";
			item.Click += new System.EventHandler(TradedGraphSpeicalCommands);
			menuStrip.Items.Add(item);
		}

		protected void TradedGraphSpeicalCommands(object sender, EventArgs e)
		{
			// do something here.  For example, remove all curves from the graph
			zedTradedGraph.GraphPane.CurveList.Clear();
			zedTradedGraph.Invalidate();

			switch (((ToolStripMenuItem)sender).Tag.ToString())
			{
				case "show_data_points":
					foreach (HelperPlot.LineGraphValues lineValues in _symbolLines.Values)
					{
						lineValues.ShowDataPoints = true;

						lineValues.PriceCurve = zedTradedGraph.GraphPane.AddCurve(lineValues.SymbolDetail, lineValues.PointPairs, Color.Red, SymbolType.Circle);
						lineValues.PriceCurve.Symbol.Fill = new Fill(Color.White);
						lineValues.PriceCurve.Symbol.Size = 7;

						lineValues.PriceCurve.Line.IsVisible = lineValues.ShowLine;
					}
					break;
				case "hide_data_points":
					foreach (HelperPlot.LineGraphValues lineValues in _symbolLines.Values)
					{
						lineValues.ShowDataPoints = false;

						lineValues.PriceCurve = zedTradedGraph.GraphPane.AddCurve(lineValues.SymbolDetail, lineValues.PointPairs, Color.Red, SymbolType.None);

						lineValues.PriceCurve.Line.IsVisible = lineValues.ShowLine;
					}
					break;
				case "show_line":
					foreach (HelperPlot.LineGraphValues lineValues in _symbolLines.Values)
					{
						if (lineValues.ShowDataPoints)
						{
							lineValues.PriceCurve = zedTradedGraph.GraphPane.AddCurve(lineValues.SymbolDetail, lineValues.PointPairs, Color.Red, SymbolType.Circle);
							lineValues.PriceCurve.Symbol.Fill = new Fill(Color.White);
							lineValues.PriceCurve.Symbol.Size = 7;
						}
						else
						{
							lineValues.PriceCurve = zedTradedGraph.GraphPane.AddCurve(lineValues.SymbolDetail, lineValues.PointPairs, Color.Red, SymbolType.None);
						}

						lineValues.ShowLine = true;
						lineValues.PriceCurve.Line.IsVisible = lineValues.ShowLine;
					}
					break;
				case "hide_line":
					foreach (HelperPlot.LineGraphValues lineValues in _symbolLines.Values)
					{
						if (lineValues.ShowDataPoints)
						{
							lineValues.PriceCurve = zedTradedGraph.GraphPane.AddCurve(lineValues.SymbolDetail, lineValues.PointPairs, Color.Red, SymbolType.Circle);
							lineValues.PriceCurve.Symbol.Fill = new Fill(Color.White);
							lineValues.PriceCurve.Symbol.Size = 7;
						}
						else
						{
							lineValues.PriceCurve = zedTradedGraph.GraphPane.AddCurve(lineValues.SymbolDetail, lineValues.PointPairs, Color.Red, SymbolType.None);
						}

						lineValues.ShowLine = false;
						lineValues.PriceCurve.Line.IsVisible = lineValues.ShowLine;
					}
					break;
			}
		}

		#region - Bar -
		
		private void CreateGraphTradedVolumes()
		{
			zedTradedVolumesGraph.GraphPane.Legend.IsVisible = false;

			zedTradedVolumesGraph.GraphPane.Title.Text = "";

			zedTradedVolumesGraph.GraphPane.XAxis.Title.Text = "";
			zedTradedVolumesGraph.GraphPane.XAxis.Type = AxisType.Linear;
			zedTradedVolumesGraph.GraphPane.XAxis.Scale.MajorStepAuto = false;
			zedTradedVolumesGraph.GraphPane.XAxis.Scale.MinorStepAuto = false;
			zedTradedVolumesGraph.GraphPane.XAxis.Scale.FontSpec.Size = 10;
			zedTradedVolumesGraph.GraphPane.XAxis.Scale.FontSpec.Angle = 0;

			zedTradedVolumesGraph.GraphPane.YAxis.Title.Text = "";
			zedTradedVolumesGraph.GraphPane.YAxis.Type = AxisType.Linear;
			//zedTradedVolumesGraph.GraphPane.YAxis.Scale.MajorStepAuto = false;
			//zedTradedVolumesGraph.GraphPane.YAxis.Scale.MinorStepAuto = false;
			zedTradedVolumesGraph.GraphPane.YAxis.MajorGrid.IsVisible = true;
			zedTradedVolumesGraph.GraphPane.YAxis.Scale.FontSpec.Size = 10;

			zedTradedVolumesGraph.GraphPane.Chart.Fill = new Fill(Color.White, Color.LightGoldenrodYellow, 45.0f);

			zedTradedVolumesGraph.GraphPane.IsPenWidthScaled = false;
			zedTradedVolumesGraph.GraphPane.IsFontsScaled = false;
			zedTradedVolumesGraph.IsShowPointValues = true;

			zedTradedVolumesGraph.AxisChange();
			zedTradedVolumesGraph.Invalidate();
		}

		private void UpdateSymbolBarList(string symbolDetail, double tradedPrice, double tradedVolume, double tickSize)
		{
			try
			{
				SetBarGraphValues(symbolDetail, tradedPrice, tradedVolume, tickSize);

				UpdateGraphScales(zedTradedVolumesGraph, symbolDetail);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void SetBarGraphValues(string symbolDetail, double tradedPrice, double tradedVolume, double tickSize)
		{
			if (_sybmolBars.ContainsKey(symbolDetail))
			{
				_sybmolBars[symbolDetail].AddUpdate(tradedPrice, tradedVolume);
			}
			else
			{
				HelperPlot.BarGraphValues barValues = new HelperPlot.BarGraphValues(symbolDetail, tickSize);
				_sybmolBars.Add(symbolDetail, barValues);
				_sybmolBars[symbolDetail].Curve = zedTradedVolumesGraph.GraphPane.AddBar(symbolDetail, _sybmolBars[symbolDetail].PointPairs, Color.Red);
				
				// Default
				_sybmolBars[symbolDetail].YMin = 0;
				zedTradedVolumesGraph.GraphPane.YAxis.Scale.Min = _sybmolBars[symbolDetail].YMin;

				_sybmolBars[symbolDetail].Curve.Bar.Fill = new Fill(Color.Red, Color.White, Color.Red);

				// First Load, read from Current MarketData
				Dictionary<double, long> tradedVolumes = new Dictionary<double, long>();
				lock (GLOBAL.HMarketData.Current)
				{
					if (GLOBAL.HMarketData.Current.ContainsKey(symbolDetail))
					{
						tradedVolumes = new Dictionary<double, long>(GLOBAL.HMarketData.Current[symbolDetail].TradedVolumes);
					}
				}

				foreach (double price in tradedVolumes.Keys)
				{
					_sybmolBars[symbolDetail].AddUpdate(price, tradedVolumes[price]);
				}
			}
		}

		private void UpdateGraphScales(ZedGraphControl zedGraph, string symbolDetail)
		{
			if (_currentSymbolDetail == symbolDetail && _sybmolBars.ContainsKey(symbolDetail))
			{
				zedGraph.AxisChange();

				zedGraph.GraphPane.XAxis.Scale.Min = _sybmolBars[symbolDetail].XMin - zedGraph.GraphPane.XAxis.Scale.MajorStep;
				zedGraph.GraphPane.XAxis.Scale.Max = _sybmolBars[symbolDetail].XMax + zedGraph.GraphPane.XAxis.Scale.MajorStep;

				zedGraph.GraphPane.YAxis.Scale.Max = _sybmolBars[symbolDetail].YMax + zedGraph.GraphPane.YAxis.Scale.MajorStep;

				zedGraph.Invalidate();
			}
		}

		#endregion

		#region - Line -

		private void CreateGraphTraded()
		{
			zedTradedGraph.GraphPane.Legend.IsVisible = false;

			zedTradedGraph.GraphPane.Title.Text = "";

			zedTradedGraph.GraphPane.YAxis.Title.Text = "";
			zedTradedGraph.GraphPane.YAxis.Scale.FontSpec.Size = 10;
			zedTradedGraph.GraphPane.YAxis.MajorGrid.IsVisible = true;

			zedTradedGraph.GraphPane.XAxis.Title.Text = "";
			zedTradedGraph.GraphPane.XAxis.Type = AxisType.Date;
			zedTradedGraph.GraphPane.XAxis.Scale.MajorUnit = DateUnit.Second;
			zedTradedGraph.GraphPane.XAxis.Scale.MinorUnit = DateUnit.Millisecond;
			zedTradedGraph.GraphPane.XAxis.Scale.FontSpec.Size = 10;
			zedTradedGraph.GraphPane.XAxis.Scale.FontSpec.Angle = 0;
			zedTradedGraph.PointDateFormat = "HH:mm:ss.fff";

			zedTradedGraph.GraphPane.Chart.Fill = new Fill(Color.White, Color.SteelBlue, 45.0f);

			zedTradedGraph.GraphPane.IsPenWidthScaled = false;
			zedTradedGraph.GraphPane.IsFontsScaled = false;
			zedTradedGraph.IsShowPointValues = true;

			zedTradedGraph.AxisChange();
			zedTradedGraph.Invalidate();
		}

		private void UpdateSymbolLineList(string symbolDetail, double tradedPrice, double tradedVolume, double tickSize)
		{
			try
			{
				SetLineGraphValues(symbolDetail, tradedPrice, tradedVolume, tickSize);

				UpdateGraphTradedScales(zedTradedGraph, symbolDetail);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void SetLineGraphValues(string symbolDetail, double price, double volume, double tickSize)
		{
			if (_symbolLines.ContainsKey(symbolDetail))
			{
				_symbolLines[symbolDetail].AddUpdate(price, volume);
			}
			else
			{
				_symbolLines.Add(symbolDetail, new HelperPlot.LineGraphValues(symbolDetail, tickSize));
				_symbolLines[symbolDetail].PriceCurve = zedTradedGraph.GraphPane.AddCurve(symbolDetail, _symbolLines[symbolDetail].PointPairs, Color.Red, SymbolType.Circle);

				// Default
				_symbolLines[symbolDetail].XTimeMin = XDate.DateTimeToXLDate(DateTime.Now.AddSeconds(-5));
				zedTradedGraph.GraphPane.XAxis.Scale.Min = _symbolLines[symbolDetail].XTimeMin;

				_symbolLines[symbolDetail].PriceCurve.Line.IsVisible = true;
				_symbolLines[symbolDetail].PriceCurve.Symbol.Fill = new Fill(Color.White);
				_symbolLines[symbolDetail].PriceCurve.Symbol.Size = 7;

				_symbolLines[symbolDetail].AddUpdate(price, volume);
			}
		}

		private void UpdateGraphTradedScales(ZedGraphControl zedGraph, string symbolDetail)
		{
			if (_currentSymbolDetail == symbolDetail && _symbolLines.ContainsKey(symbolDetail))
			{
				zedGraph.AxisChange();

				zedGraph.GraphPane.XAxis.Scale.Max = _symbolLines[symbolDetail].XTimeMax;

				zedGraph.GraphPane.YAxis.Scale.Max = _symbolLines[symbolDetail].YPriceMax + zedGraph.GraphPane.YAxis.Scale.MajorStep;
				zedGraph.GraphPane.YAxis.Scale.Min = _symbolLines[symbolDetail].YPriceMin - zedGraph.GraphPane.YAxis.Scale.MajorStep;

				zedGraph.Invalidate();
			}
		}

		#endregion

		#endregion

		#region - Binding -

		#region - Refresh & First Load -

		public void RefreshWatchList(object input)
		{
			UpdatePlotListStart();

			RefreshLoadSymbolDetailList();

			UpdatePlotListStop();
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
				lock (rocPlotList.RocGridTable)
				{
					rocPlotList.SuspendLayout();

					rocPlotList.RocGridTable.Rows.Clear();
					rocPlotList.GridKeys.Clear();
					rocPlotList.Symbols.Clear();
					rocPlotList.Symbols.Add("");
					rocPlotList.Details.Clear();
					rocPlotList.Details.Add("");

					LoadSymbolDetailList();

					if (rocPlotList.RocGridTable.Rows.Count == 0)
					{
						// Insert at least one blank row for user to edit
						rocPlotList.RocGridTable.Rows.Add(rocPlotList.RocGridTable.NewRow());
					}

					rocPlotList.ResumeLayout();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void LoadSymbolDetailList()
		{
			if (LastSavedSymbolDetailList.Count > 0)
			{
				// Check to see if the last item is null or not, if null remove from list
				int index = LastSavedSymbolDetailList.Count - 1;
				while (LastSavedSymbolDetailList[index] == "")
				{
					LastSavedSymbolDetailList.RemoveAt(index);
					index = LastSavedSymbolDetailList.Count - 1;
					if (index < 0)
					{
						break;
					}
				}

				foreach (string symbolDetail in LastSavedSymbolDetailList)
				{
					AddSymbolDetail(symbolDetail);
				}
			}
		}

		// Refresh or First Load
		private void AddSymbolDetail(string symbolDetail)
		{
			lock (rocPlotList.RocGridTable)
			{
				DataRow row = rocPlotList.RocGridTable.NewRow();
				row = UpdateDataRow(symbolDetail, row, true);

				rocPlotList.UpdateSymbol(row["Symbol"].ToString());

				if (!rocPlotList.Details.Contains(symbolDetail))
				{
					rocPlotList.Details.Add(symbolDetail);
				}

				rocPlotList.RocGridTable.Rows.Add(row);
			}
		}

		#endregion

		#region - On Symbol Detail Changed Event -

		private void UpdateDataRows(string symbolDetail)
		{
			DataRow[] rows = rocPlotList.RocGridTable.Select("SymbolDetail = '" + symbolDetail + "'");

			if (rows.Length > 0)
			{
				foreach (DataRow row in rows)
				{
					ResetDataRow(row);
					UpdateDataRow(symbolDetail, row, false);
				}
			}
		}

		private void ResetDataRow(DataRow row)
		{
			row["BidPrice"] = DBNull.Value;
			row["BidSize"] = DBNull.Value;
			row["BidPartID"] = DBNull.Value;
			row["BidHigh"] = DBNull.Value;
			row["BidLow"] = DBNull.Value;

			row["AskPrice"] = DBNull.Value;
			row["AskPartID"] = DBNull.Value;
			row["AskHigh"] = DBNull.Value;
			row["AskLow"] = DBNull.Value;

			row["LastTraded"] = DBNull.Value;
			row["TradePartID"] = DBNull.Value;
			row["TradeVolume"] = DBNull.Value;
			row["Tick"] = DBNull.Value;
			row["NetChange"] = DBNull.Value;
			row["PctChange"] = DBNull.Value;
			row["Volume"] = DBNull.Value;
			row["High"] = DBNull.Value;
			row["Low"] = DBNull.Value;
			row["Open"] = DBNull.Value;
			row["Close"] = DBNull.Value;
			row["Vwap"] = DBNull.Value;
			row["OpenInterest"] = DBNull.Value;
			row["SettlePrice"] = DBNull.Value;

			row["SecurityStatus"] = DBNull.Value;

			row["LineTime"] = DBNull.Value;
			row["DecodeTime"] = DBNull.Value;
			row["OnMsgTime"] = DBNull.Value;
			row["ServerSentTime"] = DBNull.Value;
			row["ClientRecivedTime"] = DBNull.Value;
		}

		#endregion

		#region - Used By Process Thread -

		private delegate void UpdatePlotListByProcessDelegate(bool updateIM, Dictionary<string, MDServerToClient> deltas);
		public void UpdatePlotListByProcess(bool updateIM, Dictionary<string, MDServerToClient> deltas)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdatePlotListByProcessDelegate(UpdatePlotListByProcess), new object[] { updateIM, deltas });
				return;
			}

			try
			{
				if (!IsProcessing)
				{
					lock (rocPlotList.RocGridTable)
					{
						//rocPlotList.SuspendLayout();
						//rocPlotList.RocGridTable.BeginLoadData();

						if (updateIM)
						{
							UpdateSecurityInfo();
						}

						if (deltas.Count > 0)
						{
							UpdateMarketDataDeltas(deltas);
						}

						//rocPlotList.RocGridTable.EndLoadData();
						//rocPlotList.ResumeLayout();
					}
				}
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

				lock (rocPlotList.RocGridTable)
				{
					rocPlotList.RocGridTable.BeginLoadData();

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

					rocPlotList.RocGridTable.EndLoadData();
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
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecutrityTypes.Option:
					HelperSubscriber.SubscribeOptionNBBO(secInfo.MDSymbol, secInfo.MDSource);
					break;
				default:
					HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
					break;
			}

			DataRow[] rows = rocPlotList.RocGridTable.Select(string.Concat(new object[] { "SymbolDetail = '", symbolDetail, "'" }));
			foreach (DataRow row in rows)
			{
				row["Symbol"] = secInfo.MDSymbol.Replace("/P", "/PR");
				if (!rocPlotList.Symbols.Contains(secInfo.MDSymbol.Replace("/P", "/PR")))
				{
					rocPlotList.Symbols.Add(secInfo.MDSymbol.Replace("/P", "/PR"));
				}
				row["TickSize"] = secInfo.TickSize;
				row["SecType"] = secInfo.SecType;
				row["Name"] = secInfo.LongName;
			}
		}

		private void UpdateMarketDataDeltas(Dictionary<string, MDServerToClient> deltas)
		{
			DataRowView[] rows = new DataRowView[0];
			TPOSPosition position = new TPOSPosition();

			bool _tradedPriceChanged = false;
			bool _tradedVolumeChanged = false;

			lock (rocPlotList.RocGridTable)
			{
				//rocWatchList.SyncToDefaultView("Symbol", rocWatchList.RocGridTable);

				foreach (MDServerToClient delta in deltas.Values)
				{
					if (IsProcessing) return;

					if (rocPlotList.Symbols.Contains(delta.IssueSymbol))
					{
						rows = SearchView.FindRows(delta.IssueSymbol);

						if (rows.Length > 0)
						{
							double uOnMsgTimeDeltaLag = 0;
							double uClientRecivedTimeDeltaLag = 0;

							if (_checkLatency)
							{
								MonitorLatency(delta, out uOnMsgTimeDeltaLag, out uClientRecivedTimeDeltaLag);
							}

							foreach (DataRowView row in rows)
							{
								#region - Row Update -

								if (delta.BidPrice != null)
								{
									row["BidPrice"] = (double)delta.BidPrice;
								}

								if (delta.BidSize != null)
								{
									row["BidSize"] = (long)delta.BidSize;
								}

								if (delta.BidPartId != null)
								{
									row["BidPartID"] = (string)delta.BidPartId;
								}

								if (delta.BidHigh != null)
								{
									row["BidHigh"] = (double)delta.BidHigh;
								}

								if (delta.BidLow != null)
								{
									row["BidLow"] = (double)delta.BidLow;
								}

								if (delta.AskPrice != null)
								{
									row["AskPrice"] = (double)delta.AskPrice;
								}

								if (delta.AskSize != null)
								{
									row["AskSize"] = (long)delta.AskSize;
								}

								if (delta.AskPartId != null)
								{
									row["AskPartID"] = (string)delta.AskPartId;
								}

								if (delta.AskHigh != null)
								{
									row["AskHigh"] = (double)delta.AskHigh;
								}

								if (delta.AskLow != null)
								{
									row["AskLow"] = (double)delta.AskLow;
								}

								if (delta.TradePrice != null && delta.TradePrice != 0)
								{
									row["LastTraded"] = (double)delta.TradePrice;
									_tradedPriceChanged = true;
								}
								//else
								//{
								//    if (delta.ClosePrice != null && delta.ClosePrice != 0)
								//    {
								//        row["LastTraded"] = (double)delta.ClosePrice;
								//    }
								//    else if (delta.PrevClosePrice != null && delta.PrevClosePrice != 0)
								//    {
								//        row["LastTraded"] = (double)delta.PrevClosePrice;
								//    }
								//}

								if (delta.TradeVolume != null)
								{
									row["TradeVolume"] = (long)delta.TradeVolume;
									_tradedVolumeChanged = true;
								}

								if (delta.TradePartId != null)
								{
									row["TradePartID"] = (string)delta.TradePartId;
								}

								if (delta.TradeTick != null && delta.TradeTick != "")
								{
									row["Tick"] = delta.TradeTick;
								}

								if (delta.NetChange != null)
								{
									row["NetChange"] = (double)delta.NetChange;
								}

								if (delta.PctChange != null)
								{
									row["PctChange"] = (double)delta.PctChange;
								}

								if (delta.TotalVolume != null)
								{
									row["Volume"] = (long)delta.TotalVolume;
								}

								if (delta.HighPrice != null)
								{
									row["High"] = (double)delta.HighPrice;
								}

								if (delta.LowPrice != null)
								{
									row["Low"] = (double)delta.LowPrice;
								}

								if (delta.OpenPrice != null)
								{
									row["Open"] = (double)delta.OpenPrice;
								}

								if (delta.ClosePrice != null && (double)delta.ClosePrice != 0)
								{
									row["Close"] = (double)delta.ClosePrice;
								}
								else
								{
									if (delta.PrevClosePrice != null && (double)delta.PrevClosePrice != 0)
									{
										row["Close"] = (double)delta.PrevClosePrice;
									}
								}

								if (delta.Vwap != null)
								{
									row["Vwap"] = (double)delta.Vwap;
								}

								if (delta.OpenInterest != null)
								{
									row["OpenInterest"] = (long)delta.OpenInterest;
								}

								if (delta.SettlePrice != null)
								{
									row["SettlePrice"] = (double)delta.SettlePrice;
								}

								if (delta.DisplayConversionFactor != null)
								{
									row["DisplayFactor"] = (double)delta.DisplayConversionFactor;
								}

								switch (delta.SecurityStatusINT)
								{
									case SecurityStates.None:
										break;
									case SecurityStates.Normal:
										row["SecurityStatus"] = DBNull.Value;
										break;
									default:
										row["SecurityStatus"] = delta.SecurityStatus;
										break;
								}

								if (_checkLatency)
								{
									if (delta.LineTime != "")
									{
										row["LineTime"] = delta.LineTime;
									}

									if (delta.uDecodedTime != null)
									{
										row["DecodeTime"] = ((double)delta.uDecodedTime).ToString("F4");
									}

									//if (delta.uOnMsgTimeDelta != null)
									//{
									//    row["OnMsgTime"] =  ((double)delta.uOnMsgTimeDelta).ToString("F0");
									//}

									if (uOnMsgTimeDeltaLag != 0)
									{
										row["OnMsgTime"] = uOnMsgTimeDeltaLag.ToString("F0");
									}

									if (delta.uServerSentTime != null)
									{
										row["ServerSentTime"] = ((DateTime)delta.uServerSentTime).ToString("HH:mm:ss.fff");
									}

									//if (delta.uClientRecivedTimeDelta != null)
									//{
									//    row["ClientRecivedTime"] = ((double)delta.uClientRecivedTimeDelta).ToString("F0");
									//}

									if (uClientRecivedTimeDeltaLag != 0)
									{
										row["ClientRecivedTime"] = uClientRecivedTimeDeltaLag.ToString("F4");
									}
								}

								#endregion

								if (_tradedPriceChanged && _tradedVolumeChanged)
								{
									if (_currentSymbolDetail == "")
									{
										_currentSymbolDetail = delta.IssueSymbol;
									}

									double lastTraded = 0;
									double tradeVolume = 0;
									double tickSize = 0;

									if (Double.TryParse(row["LastTraded"].ToString(), out lastTraded) &&
										Double.TryParse(row["TradeVolume"].ToString(), out tradeVolume) &&
										Double.TryParse(row["TickSize"].ToString(), out tickSize))
									{
										UpdateSymbolBarList(delta.IssueSymbol, lastTraded, tradeVolume, tickSize);
										UpdateSymbolLineList(delta.IssueSymbol, lastTraded, tradeVolume, tickSize);
									}
								}
							}
						}
					}
				}
			}
		}

		private void MonitorLatency(MDServerToClient delta, out double uOnMsgTimeDeltaLag, out double uClientRecivedTimeDeltaLag)
		{
			string key = delta.uSubscriptionKey;
			uOnMsgTimeDeltaLag = 0;
			uClientRecivedTimeDeltaLag = 0;

			if (_checkLatency)
			{
				#region - uOnMsgTimeDeltaLag -

				if (delta.uOnMsgTimeDelta != null)
				{
					if (_monitorBaseLineDeltaLists_MAMA.ContainsKey(key))
					{
						if (_monitorBaseLineDeltaLists_MAMA[key].Count > _driftLimit)
						{
							double total = 0;
							foreach (double d in _monitorBaseLineDeltaLists_MAMA[key])
							{
								total = total + d;
							}
							if (!_monitorBaseLineDelta_MAMA.ContainsKey(key))
							{
								_monitorBaseLineDelta_MAMA.Add(key, total / _monitorBaseLineDeltaLists_MAMA[key].Count);
							}
						}
						else
						{
							_monitorBaseLineDeltaLists_MAMA[key].Add((double)delta.uOnMsgTimeDelta);
						}
					}
					else
					{
						_monitorBaseLineDeltaLists_MAMA.Add(key, new List<double>());
						_monitorBaseLineDeltaLists_MAMA[key].Add((double)delta.uOnMsgTimeDelta);
					}

					if (_monitorBaseLineDelta_MAMA.ContainsKey(key))
					{
						uOnMsgTimeDeltaLag = Math.Abs(_monitorBaseLineDelta_MAMA[key] - (double)delta.uOnMsgTimeDelta);
					}
				}

				#endregion

				#region - uClientRecivedTimeDeltaLag -

				if (delta.uClientRecivedTimeDelta != null)
				{
					if (_monitorBaseLineDeltaLists_MDS.ContainsKey(key))
					{
						if (_monitorBaseLineDeltaLists_MDS[key].Count > _driftLimit)
						{
							double total = 0;
							foreach (double d in _monitorBaseLineDeltaLists_MDS[key])
							{
								total = total + d;
							}
							if (!_monitorBaseLineDelta_MDS.ContainsKey(key))
							{
								_monitorBaseLineDelta_MDS.Add(key, total / _monitorBaseLineDeltaLists_MDS[key].Count);
							}
						}
						else
						{
							_monitorBaseLineDeltaLists_MDS[key].Add((double)delta.uClientRecivedTimeDelta);
						}
					}
					else
					{
						_monitorBaseLineDeltaLists_MDS.Add(key, new List<double>());
						_monitorBaseLineDeltaLists_MDS[key].Add((double)delta.uClientRecivedTimeDelta);
					}

					if (_monitorBaseLineDelta_MDS.ContainsKey(key))
					{
						uClientRecivedTimeDeltaLag = Math.Abs(_monitorBaseLineDelta_MDS[key] - (double)delta.uClientRecivedTimeDelta) / 10;
					}
				}

				#endregion
			}
		}

		#endregion

		// Update single watch list row
		private DataRow UpdateDataRow(string symbolDetail, DataRow row, bool isRefresh)
		{
			string mdSymbol = symbolDetail;
			double tickSize = 0.01;
			string secType = "";
			string name = "";
			MDServerToClient delta = new MDServerToClient();

			UpdatePlotListWithSecurityInfo(symbolDetail, ref mdSymbol, ref tickSize, ref secType, ref name);
			UpdatePlotListWithCurrentMarketData(mdSymbol.ToUpper(), ref delta);

			row["Symbol"] = mdSymbol;
			if (isRefresh)
			{
				row["symbolDetail"] = symbolDetail;
			}

			if (delta.BidPrice != null)
			{
				row["BidPrice"] = (double)delta.BidPrice;
			}

			if (delta.BidSize != null)
			{
				row["BidSize"] = (long)delta.BidSize;
			}

			if (delta.BidPartId != null)
			{
				row["BidPartID"] = (string)delta.BidPartId;
			}

			if (delta.BidHigh != null)
			{
				row["BidHigh"] = (double)delta.BidHigh;
			}

			if (delta.BidLow != null)
			{
				row["BidLow"] = (double)delta.BidLow;
			}

			if (delta.AskPrice != null)
			{
				row["AskPrice"] = (double)delta.AskPrice;
			}

			if (delta.AskSize != null)
			{
				row["AskSize"] = (long)delta.AskSize;
			}

			if (delta.AskPartId != null)
			{
				row["AskPartID"] = (string)delta.AskPartId;
			}

			if (delta.AskHigh != null)
			{
				row["AskHigh"] = (double)delta.AskHigh;
			}

			if (delta.AskLow != null)
			{
				row["AskLow"] = (double)delta.AskLow;
			}

			if (delta.TradePrice != null && delta.TradePrice != 0)
			{
				row["LastTraded"] = (double)delta.TradePrice;
			}
			else
			{
				if (delta.ClosePrice != null && delta.ClosePrice != 0)
				{
					row["LastTraded"] = (double)delta.ClosePrice;
				}
				else if (delta.PrevClosePrice != null && delta.PrevClosePrice != 0)
				{
					row["LastTraded"] = (double)delta.PrevClosePrice;
				}
			}

			if (delta.TradePartId != null)
			{
				row["TradePartID"] = (string)delta.TradePartId;
			}

			if (delta.TradeVolume != null)
			{
				row["TradeVolume"] = (double)delta.TradeVolume;
			}

			if (delta.TradeTick != null)
			{
				row["Tick"] = delta.TradeTick;
			}

			if (delta.NetChange != null)
			{
				row["NetChange"] = (double)delta.NetChange;
			}

			if (delta.PctChange != null)
			{
				row["PctChange"] = (double)delta.PctChange;
			}

			if (delta.TotalVolume != null)
			{
				row["Volume"] = (long)delta.TotalVolume;
			}

			if (delta.HighPrice != null)
			{
				row["High"] = (double)delta.HighPrice;
			}

			if (delta.LowPrice != null)
			{
				row["Low"] = (double)delta.LowPrice;
			}

			if (delta.OpenPrice != null)
			{
				row["Open"] = (double)delta.OpenPrice;
			}

			if (delta.ClosePrice != null && (double)delta.ClosePrice != 0)
			{
				row["Close"] = (double)delta.ClosePrice;
			}
			else
			{
				if (delta.PrevClosePrice != null && (double)delta.PrevClosePrice != 0)
				{
					row["Close"] = (double)delta.PrevClosePrice;
				}
			}

			if (delta.Vwap != null)
			{
				row["Vwap"] = (double)delta.Vwap;
			}

			if (delta.OpenInterest != null)
			{
				row["OpenInterest"] = (long)delta.OpenInterest;
			}

			if (delta.SettlePrice != null)
			{
				row["SettlePrice"] = (double)delta.SettlePrice;
			}

			row["Name"] = name;
			row["TickSize"] = tickSize;

			if (delta.DisplayConversionFactor != null)
			{
				row["DisplayFactor"] = (double)delta.DisplayConversionFactor;
			}
			else
			{
				row["DisplayFactor"] = 1;
			}

			switch (delta.SecurityStatusINT)
			{
				case SecurityStates.None:
					break;
				case SecurityStates.Normal:
					row["SecurityStatus"] = DBNull.Value;
					break;
				default:
					row["SecurityStatus"] = delta.SecurityStatus;
					break;
			}

			row["SecType"] = secType;

			//if (delta.LineTime != "")
			//{
			//    row["LineTime"] = delta.LineTime;
			//}

			//if (delta.uDecodedTime != null)
			//{
			//    row["DecodeTime"] = ((double)delta.uDecodedTime).ToString("F4");
			//}

			//if (delta.uOnMsgTimeDelta != null)
			//{
			//    row["OnMsgTime"] = ((double)delta.uOnMsgTimeDelta).ToString("F0");
			//}

			//if (delta.uServerSentTime != null)
			//{
			//    row["ServerSentTime"] = ((DateTime)delta.uServerSentTime).ToString("HH:mm:ss.fff");
			//}

			//if (delta.uClientRecivedTimeDelta != null)
			//{
			//    row["ClientRecivedTime"] = ((double)delta.uClientRecivedTimeDelta).ToString("F0");
			//}

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

		// Update with Play back & onLoad
		private void UpdatePlotListWithCurrentMarketData(string mdSymbol, ref MDServerToClient delta)
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
		private void UpdatePlotListWithSecurityInfo(string symbolDetail, ref string mdSymbol, ref double tickSize, ref string secType, ref string name)
		{
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
						rocPlotList.UpdateSymbol(mdSymbol);
						rocPlotList.UpdateTickSize(mdSymbol, tickSize);
						break;
					default:
						rocPlotList.UpdateTickSize(mdSymbol, tickSize);
						break;
				}
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
		}

		private delegate void UpdateWatchListStartDelegate();
		private void UpdatePlotListStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateWatchListStartDelegate(UpdatePlotListStart));
				return;
			}

			IsProcessing = true;

			// Don't save the Sort Order between refreshs for watch list
			//rocWatchList.LastFilter = rocWatchList.RocGridTable.DefaultView.RowFilter;
			//rocWatchList.LastSort = rocWatchList.RocGridTable.DefaultView.Sort;

			rocPlotList.RocGridTable.DefaultView.RowFilter = "";
			rocPlotList.RocGridTable.DefaultView.Sort = "";

			rocPlotList.LastSortColumnName = "";
			rocPlotList.LastSortColumnDirection = SortOrder.None;

			rocPlotList.RocGridTable.BeginLoadData();
		}

		private delegate void UpdateWatchListStopDelegate();
		private void UpdatePlotListStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateWatchListStopDelegate(UpdatePlotListStop));
				return;
			}

			// Don't save the Sort Order between refreshs for watch list
			//rocWatchList.RocGridTable.DefaultView.Sort = rocWatchList.LastSort;
			//rocWatchList.RocGridTable.DefaultView.RowFilter = rocWatchList.LastFilter;

			rocPlotList.RocGridTable.EndLoadData();

			rocPlotList.RocGridTable.AcceptChanges();

			IsProcessing = false;

			rocPlotList.RefreshSharedRows = true;
			rocPlotList.RefreshAggragation = true;
			rocPlotList.ShouldScrollToLastRow = true;
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
				items.Add("SaveList", "SaveList");
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
				case "SaveList":
					if (MessageBox.Show("Override the current WatchList?", "Alert", MessageBoxButtons.YesNo) == DialogResult.Yes)
					{
						SaveSymbolList();
					}
					break;
				case "Clone":
					lock (GLOBAL.HWindows.WatchListWindows)
					{
						frmWatchList w = new frmWatchList();
						w.ImportGrid(ExportGrid());
						GLOBAL.HWindows.OpenWindow(w, true);
					}
					break;
				case "Copy":
					rocPlotList.CopyToClipboard(true);
					break;
				case "Print":
					rocPlotList.Printer.Owner = this;
					rocPlotList.Printer.PrintPreviewDataGridView(rocPlotList, true);
					break;
				case "SaveAs":
					rocPlotList.Save();
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
				items.Add("Symbol", new FilterItem("Symbol", !rocPlotList.Columns["Symbol"].Visible));
				items.Add("SymSymbolDetailbol", new FilterItem("SymbolDetail", !rocPlotList.Columns["SymbolDetail"].Visible));

				items.Add("BidPrice", new FilterItem("BidPrice", !rocPlotList.Columns["BidPrice"].Visible));
				items.Add("BidSize", new FilterItem("BidSize", !rocPlotList.Columns["BidSize"].Visible));
				items.Add("BidPartID", new FilterItem("BidPartID", !rocPlotList.Columns["BidPartID"].Visible));
				items.Add("BidHigh", new FilterItem("BidHigh", !rocPlotList.Columns["BidHigh"].Visible));
				items.Add("BidLow", new FilterItem("BidLow", !rocPlotList.Columns["BidLow"].Visible));

				items.Add("AskPrice", new FilterItem("AskPrice", !rocPlotList.Columns["AskPrice"].Visible));
				items.Add("AskSize", new FilterItem("AskSize", !rocPlotList.Columns["AskSize"].Visible));
				items.Add("AskPartID", new FilterItem("AskPartID", !rocPlotList.Columns["AskPartID"].Visible));
				items.Add("AskHigh", new FilterItem("AskHigh", !rocPlotList.Columns["AskHigh"].Visible));
				items.Add("AskLow", new FilterItem("AskLow", !rocPlotList.Columns["AskLow"].Visible));

				items.Add("Last", new FilterItem("Last", !rocPlotList.Columns["LastTraded"].Visible));
				items.Add("TradeVolume", new FilterItem("TradeVolume", !rocPlotList.Columns["TradeVolume"].Visible));
				items.Add("TradePartID", new FilterItem("TradePartID", !rocPlotList.Columns["TradePartID"].Visible));

				items.Add("Tick", new FilterItem("Tick", !rocPlotList.Columns["Tick"].Visible));
				items.Add("Net", new FilterItem("Net", !rocPlotList.Columns["NetChange"].Visible));
				items.Add("Pct", new FilterItem("Pct", !rocPlotList.Columns["PctChange"].Visible));
				items.Add("Volume", new FilterItem("Volume", !rocPlotList.Columns["Volume"].Visible));
				items.Add("High", new FilterItem("High", !rocPlotList.Columns["High"].Visible));
				items.Add("Low", new FilterItem("Low", !rocPlotList.Columns["Low"].Visible));
				items.Add("Open", new FilterItem("Open", !rocPlotList.Columns["Open"].Visible));
				items.Add("Close", new FilterItem("Close", !rocPlotList.Columns["Close"].Visible));
				items.Add("Vwap", new FilterItem("Vwap", !rocPlotList.Columns["Vwap"].Visible));
				items.Add("SettlePrice", new FilterItem("SettlePrice", !rocPlotList.Columns["SettlePrice"].Visible));
				items.Add("OpenInterest", new FilterItem("OpenInterest", !rocPlotList.Columns["OpenInterest"].Visible));

				items.Add("Name", new FilterItem("Name", !rocPlotList.Columns["Name"].Visible));
				items.Add("TickSize", new FilterItem("TickSize", !rocPlotList.Columns["TickSize"].Visible));
				items.Add("DisplayFactor", new FilterItem("DisplayFactor", !rocPlotList.Columns["DisplayFactor"].Visible));
				items.Add("SecType", new FilterItem("SecType", !rocPlotList.Columns["SecType"].Visible));
				items.Add("SecurityStatus", new FilterItem("SecurityStatus", !rocPlotList.Columns["SecurityStatus"].Visible));

				items.Add("LineTime", new FilterItem("LineTime", !rocPlotList.Columns["LineTime"].Visible));
				items.Add("DecodeTime", new FilterItem("DecodeTime", !rocPlotList.Columns["DecodeTime"].Visible));
				items.Add("OnMsgTime", new FilterItem("OnMsgTime", !rocPlotList.Columns["OnMsgTime"].Visible));
				items.Add("ServerSentTime", new FilterItem("ServerSentTime", !rocPlotList.Columns["ServerSentTime"].Visible));
				items.Add("ClientRecivedTime", new FilterItem("ClientRecivedTime", !rocPlotList.Columns["ClientRecivedTime"].Visible));

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
				case "Symbol":
					rocPlotList.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SymbolDetail":
					rocPlotList.Columns["SymbolDetail"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidPrice":
					rocPlotList.Columns["BidPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidSize":
					rocPlotList.Columns["BidSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidPartID":
					rocPlotList.Columns["BidPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidHigh":
					rocPlotList.Columns["BidHigh"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidLow":
					rocPlotList.Columns["BidLow"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "AskPrice":
					rocPlotList.Columns["AskPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskSize":
					rocPlotList.Columns["AskSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskPartID":
					rocPlotList.Columns["AskPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskHigh":
					rocPlotList.Columns["AskHigh"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskLow":
					rocPlotList.Columns["AskLow"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Last":
					rocPlotList.Columns["LastTraded"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TradeVolume":
					rocPlotList.Columns["TradeVolume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TradePartID":
					rocPlotList.Columns["TradePartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Tick":
					rocPlotList.Columns["Tick"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Net":
					rocPlotList.Columns["NetChange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Pct":
					rocPlotList.Columns["PctChange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Volume":
					rocPlotList.Columns["Volume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "High":
					rocPlotList.Columns["High"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Low":
					rocPlotList.Columns["Low"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Open":
					rocPlotList.Columns["Open"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Close":
					rocPlotList.Columns["Close"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Vwap":
					rocPlotList.Columns["Vwap"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SettlePrice":
					rocPlotList.Columns["SettlePrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OpenInterest":
					rocPlotList.Columns["OpenInterest"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Name":
					rocPlotList.Columns["Name"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TickSize":
					rocPlotList.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DisplayFactor":
					rocPlotList.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecType":
					rocPlotList.Columns["SecType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecurityStatus":
					rocPlotList.Columns["SecurityStatus"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "LineTime":
					rocPlotList.Columns["LineTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DecodeTime":
					rocPlotList.Columns["DecodeTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OnMsgTime":
					rocPlotList.Columns["OnMsgTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "ServerSentTime":
					rocPlotList.Columns["ServerSentTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "ClientRecivedTime":
					rocPlotList.Columns["ClientRecivedTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
			}

			if (checkLatency) { }
		}

		private void menuColumnsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuColumnsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			_menuColumnsContainer = null;
			_menuColumns.PropertyChanged -= new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			_menuColumns = null;
		}

		#endregion

		#region - Export & Import -

		public string ExportGrid()
		{
			ROCWatchListProfile prof = new ROCWatchListProfile(rocPlotList);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCWatchListProfile prof = (ROCWatchListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					LastSavedSymbolDetailList.AddRange(prof.SymbolDetailList.ToArray());
					prof = new ROCWatchListProfile(rocPlotList, prof);
				}

				if (checkLatency) { }
			}
		}

		#endregion

		#region - Row Edit Menu -

		private menuBaseAction _menuGridActions;
		private PropertyContainer _menuGridActionsContainer;
		private void ShowGridMenu()
		{
			if (_menuGridActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("Insert", "Insert");
				items.Add("Delete", "Delete");
				items.Add("DeleteSelected", "Delete Selected");

				_menuGridActions = new menuBaseAction(items, true);
				_menuGridActions.PropertyChanged += new PropertyChangedEventHandler(menuGridActions_PropertyChanged);
			}

			if (_menuGridActionsContainer == null)
			{
				_menuGridActionsContainer = new PropertyContainer(_menuGridActions);
				_menuGridActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuGridActionsContainer_Closed);
			}

			_menuGridActionsContainer.Show(Cursor.Position);
		}

		private void menuGridActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuGridActionsContainer.Close();
			switch (e.PropertyName)
			{
				case "Insert":
					AddDataRow();
					break;
				case "Delete":
					DeleteDataRow();
					break;
				case "DeleteSelected":
					DeleteSelectedDataRow();
					break;
			}
		}

		private void menuGridActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuGridActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuGridActionsContainer_Closed);
			_menuGridActionsContainer = null;
			_menuGridActions.PropertyChanged -= new PropertyChangedEventHandler(menuGridActions_PropertyChanged);
			_menuGridActions = null;
		}

		private void AddDataRow()
		{
			lock (rocPlotList.RocGridTable)
			{
				rocPlotList.RocGridTable.Rows.InsertAt(rocPlotList.RocGridTable.NewRow(), rocPlotList.RowLocation);
			}
		}

		private void DeleteDataRow()
		{
			lock (rocPlotList.RocGridTable)
			{
				rocPlotList.SuspendLayout();
				rocPlotList.RocGridTable.BeginLoadData();

				if (rocPlotList.Rows.Count > rocPlotList.RowLocation)
				{
					if (!rocPlotList.Rows[rocPlotList.RowLocation].IsNewRow)
					{
						rocPlotList.Rows.Remove(rocPlotList.Rows[rocPlotList.RowLocation]);
					}
				}

				rocPlotList.Refresh();

				rocPlotList.RocGridTable.EndLoadData();
				rocPlotList.ResumeLayout();
			}
		}

		private void DeleteSelectedDataRow()
		{
			lock (rocPlotList.RocGridTable)
			{
				rocPlotList.SuspendLayout();
				rocPlotList.RocGridTable.BeginLoadData();

				foreach (DataGridViewRow row in rocPlotList.SelectedRows)
				{
					if (!row.IsNewRow)
					{
						rocPlotList.Rows.Remove(row);
					}
				}

				rocPlotList.Refresh();

				rocPlotList.RocGridTable.EndLoadData();
				rocPlotList.ResumeLayout();
			}
		}

		#endregion

		#region - Save Symbol List -

		private void SaveSymbolList()
		{
			LastSavedSymbolDetailList.Clear();
			foreach (DataGridViewRow row in rocPlotList.Rows)
			{
				if (row.Cells["SymbolDetail"].Value != null)
				{
					LastSavedSymbolDetailList.Add(row.Cells["SymbolDetail"].Value.ToString());
				}
				else
				{
					LastSavedSymbolDetailList.Add("");
				}
			}
		}

		#endregion
	}
}