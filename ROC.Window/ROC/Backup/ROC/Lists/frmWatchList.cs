using System;
using System.Data;
using System.Text;
using System.Diagnostics;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;

using FormEx;
using DataGridViewEx;
using SerializationEx;
using RDSEx;
using MarketDataEx;
using ContextMenuEx;
using ButtonEx;
using CSVEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmWatchList : BaseList
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
				if (rocWatchList.Columns["LineTime"].Visible ||
					rocWatchList.Columns["DecodeTime"].Visible ||
					rocWatchList.Columns["OnMsgTime"].Visible ||
					rocWatchList.Columns["ServerSentTime"].Visible ||
					rocWatchList.Columns["ClientRecivedTime"].Visible)
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

		public bool IsProcessing
		{
			get
			{
				return rocWatchList.Processing;
			}
			set
			{
				rocWatchList.Processing = value;
			}
		}

		private List<string> _lastSavedSymbolDetailList;
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
					_searchView = new DataView(rocWatchList.RocGridTable);
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

		#region - Constructor -

		public frmWatchList()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			RefreshClicked += new EventHandler(Form_RefreshClicked);
			DisplayClicked += new EventHandler(Form_DisplayClicked);
			UITimerTicked += new EventHandler(Form_UITimerTicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);

			rocWatchList.Initialize();
			rocWatchList.PropertyChanged += new PropertyChangedEventHandler(rocWatchList_PropertyChanged);

			rocWatchList.CellMouseDoubleClick += new DataGridViewCellMouseEventHandler(rocWatchList_CellMouseDoubleClick);

			rocWatchList.MouseEnter += new EventHandler(rocWatchList_MouseEnter);

			rocWatchList.ProcessingChanged += new EventHandler(rocWatchList_ProcessingChanged);
			rocWatchList.ShouldScrollToLastRowChanged += new EventHandler(rocWatchList_ShouldScrollToLastRowChanged);
			rocWatchList.RefreshAggragationChanged += new EventHandler(rocWatchList_RefreshAggragationChanged);

			StartTimer();
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			// TODO: Make sure all selected rows are also refreshed
			rocWatchList.RefreshSelectedRows();

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
					lock (rocWatchList)
					{
						SetColumnMode(rocWatchList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocWatchList);
						SetColumnMode(rocWatchList, DataGridViewAutoSizeColumnMode.None);
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
		
		private frmWatchListDisplay _watchListDisplay;
		private frmWatchListDisplay watchListDisplay
		{
			get
			{
				if (_watchListDisplay == null)
				{
					_watchListDisplay = new frmWatchListDisplay(new ROCWatchListProfile(rocWatchList));
					_watchListDisplay.PropertyChanged += new PropertyChangedEventHandler(watchListDisplay_PropertyChanged);
					_watchListDisplay.Disposed += new EventHandler(watchListDisplay_Disposed);
				}
				return _watchListDisplay;
			}
			set
			{
				_watchListDisplay = value;
			}
		}
		private void Form_DisplayClicked(object sender, EventArgs e)
		{
			watchListDisplay.ShowDialog();
		}

		private void watchListDisplay_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Apply":
					SetProfile(watchListDisplay.GetProfile(new ROCWatchListProfile(rocWatchList)));
					break;
				case "ApplyToAll":
					ROCWatchListProfile prof = watchListDisplay.GetProfile(new ROCWatchListProfile(rocWatchList));
					foreach (frmWatchList w in GLOBAL.HWindows.WatchListWindows.Values)
					{
						w.SetProfile(watchListDisplay.GetProfile(prof));
					}
					break;
			}
		}

		private void watchListDisplay_Disposed(object sender, EventArgs e)
		{
			if (_watchListDisplay != null)
			{
				_watchListDisplay.PropertyChanged -= new PropertyChangedEventHandler(watchListDisplay_PropertyChanged);
				_watchListDisplay.Disposed -= new EventHandler(watchListDisplay_Disposed);
			}

			_watchListDisplay = null;
		}

		public void SetProfile(ROCWatchListProfile prof)
		{
			new ROCWatchListProfile(rocWatchList, prof);
		}

		#endregion

		private void Form_Load(object sender, EventArgs e)
		{
			RefreshWatchList(null);
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		#endregion

		#region - Grid Event -

		private void rocWatchList_ProcessingChanged(object sender, EventArgs e)
		{
			rocWatchList.Enabled = !IsProcessing;
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

		private void rocWatchList_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocWatchList.ShouldScrollToLastRow)
			{
				rocWatchList.ShouldScrollToLastRow = false;
				//rocWatchList.ScrollToLastRow();
			}
		}

		private void rocWatchList_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocWatchList.RefreshAggragation)
			{
				//rocWatchList.UnshareRows();

				rocWatchList.RefreshAggragation = false;
				//UpdateAggragation();
			}
		}

		private void rocWatchList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			lock (rocWatchList.RocGridTable)
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
						UpdateDataRows(rocWatchList.SubscribeSymbolDetail);
						break;
					case "ShowMenu":
						ShowGridMenu();
						break;
					case "RowDragged":
						rocWatchList.DraggedDataRow();
						break;
				}
			}
		}

		private void rocWatchList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusWatchList)
			{
				ActivateGrid(rocWatchList);
			}
		}

		private void rocWatchList_CellMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (rocWatchList.Rows[e.RowIndex].Cells["SecType"].Value != null &&
					rocWatchList.Rows[e.RowIndex].Cells["SecType"].Value.ToString() != "" &&
					rocWatchList.Rows[e.RowIndex].Cells["SymbolDetail"].Value != null &&
					rocWatchList.Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString() != "")
				{
					GLOBAL.HWindows.OpenWindow(
						rocWatchList.Rows[e.RowIndex].Cells["SecType"].Value.ToString().ToUpper(), 
						rocWatchList.Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().ToUpper());
				}
			}
		}

		#endregion

		#region - Binding -

		#region - Refresh & First Load -

		public void RefreshWatchList(object input)
		{
			UpdateWatchListStart();

			RefreshLoadSymbolDetailList();

			UpdateWatchListStop();
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
				lock (rocWatchList.RocGridTable)
				{
					rocWatchList.SuspendLayout();

					rocWatchList.RocGridTable.Rows.Clear();
					rocWatchList.GridKeys.Clear();
					rocWatchList.Symbols.Clear();
					rocWatchList.Symbols.Add("");
					rocWatchList.Details.Clear();
					rocWatchList.Details.Add("");

					LoadSymbolDetailList();

					if (rocWatchList.RocGridTable.Rows.Count == 0)
					{
						// Insert at least one blank row for user to edit
						rocWatchList.RocGridTable.Rows.Add(rocWatchList.RocGridTable.NewRow());
					}

					rocWatchList.ResumeLayout();
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
			lock (rocWatchList.RocGridTable)
			{
				DataRow row = rocWatchList.RocGridTable.NewRow();
				row = UpdateDataRow(symbolDetail, row, true);

				rocWatchList.UpdateSymbol(row["Symbol"].ToString());

				if (!rocWatchList.Details.Contains(symbolDetail))
				{
					rocWatchList.Details.Add(symbolDetail);
				}

				rocWatchList.RocGridTable.Rows.Add(row);
			}
		}

		#endregion

		#region - On Symbol Detail Changed Event -

		private void UpdateDataRows(string symbolDetail)
		{
			DataRow[] rows = rocWatchList.RocGridTable.Select("SymbolDetail = '" + symbolDetail + "'");

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

		private delegate void UpdateWatchListByProcessDelegate(bool updateIM, Dictionary<string, MDServerToClient> deltas);
		public void UpdateWatchListByProcess(bool updateIM, Dictionary<string, MDServerToClient> deltas)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateWatchListByProcessDelegate(UpdateWatchListByProcess), new object[] { updateIM, deltas });
				return;
			}

			try
			{
				if (!IsProcessing)
				{
					lock (rocWatchList.RocGridTable)
					{
						//rocWatchList.SuspendLayout();
						//rocWatchList.RocGridTable.BeginLoadData();

						if (updateIM)
						{
							UpdateSecurityInfo();
						}

						if (deltas.Count > 0)
						{
							UpdateMarketDataDeltas(deltas);
						}

						//rocWatchList.RocGridTable.EndLoadData();
						//rocWatchList.ResumeLayout();
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

				lock (rocWatchList.RocGridTable)
				{
					rocWatchList.RocGridTable.BeginLoadData();

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

					rocWatchList.RocGridTable.EndLoadData();
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

			DataRow[] rows = rocWatchList.RocGridTable.Select(string.Concat(new object[] { "SymbolDetail = '", symbolDetail, "'" }));
			foreach (DataRow row in rows)
			{
				row["Symbol"] = secInfo.MDSymbol.Replace("/P", "/PR");
				if (!rocWatchList.Symbols.Contains(secInfo.MDSymbol.Replace("/P", "/PR")))
				{
					rocWatchList.Symbols.Add(secInfo.MDSymbol.Replace("/P", "/PR"));
				}
				row["TickSize"] = secInfo.TickSize;
				row["SecType"] = secInfo.SecType;
				row["Name"] = secInfo.LongName;
			}
		}

		private void UpdateMarketDataDeltas(Dictionary<string, MDServerToClient> deltas)
		{
			DataRowView[] rows = new DataRowView[0];

			lock (rocWatchList.RocGridTable)
			{
				//rocWatchList.SyncToDefaultView("Symbol", rocWatchList.RocGridTable);

				foreach (MDServerToClient delta in deltas.Values)
				{
					if (IsProcessing) return;

					if (rocWatchList.Symbols.Contains(delta.IssueSymbol))
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

			UpdateWatchListWithSecurityInfo(symbolDetail, ref mdSymbol, ref tickSize, ref secType, ref name);
			UpdateWatchListWithCurrentMarketData(mdSymbol.ToUpper(), ref delta);

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
				if (delta.ClosePrice != null && delta.ClosePrice != 0 )
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
		private void UpdateWatchListWithCurrentMarketData(string mdSymbol, ref MDServerToClient delta)
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
		private void UpdateWatchListWithSecurityInfo(string symbolDetail, ref string mdSymbol, ref double tickSize, ref string secType, ref string name)
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
						rocWatchList.UpdateSymbol(mdSymbol);
						rocWatchList.UpdateTickSize(mdSymbol, tickSize);
						break;
					default:
						rocWatchList.UpdateTickSize(mdSymbol, tickSize);
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
		private void UpdateWatchListStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateWatchListStartDelegate(UpdateWatchListStart));
				return;
			}

			IsProcessing = true;
			
			// Don't save the Sort Order between refreshs for watch list
			//rocWatchList.LastFilter = rocWatchList.RocGridTable.DefaultView.RowFilter;
			//rocWatchList.LastSort = rocWatchList.RocGridTable.DefaultView.Sort;

			rocWatchList.RocGridTable.DefaultView.RowFilter = "";
			rocWatchList.RocGridTable.DefaultView.Sort = "";

			rocWatchList.LastSortColumnName = "";
			rocWatchList.LastSortColumnDirection = SortOrder.None;

			rocWatchList.RocGridTable.BeginLoadData();
		}

		private delegate void UpdateWatchListStopDelegate();
		private void UpdateWatchListStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateWatchListStopDelegate(UpdateWatchListStop));
				return;
			}

			// Don't save the Sort Order between refreshs for watch list
			//rocWatchList.RocGridTable.DefaultView.Sort = rocWatchList.LastSort;
			//rocWatchList.RocGridTable.DefaultView.RowFilter = rocWatchList.LastFilter;

			rocWatchList.RocGridTable.EndLoadData();

			rocWatchList.RocGridTable.AcceptChanges();

			IsProcessing = false;

			rocWatchList.RefreshSharedRows = true;
			rocWatchList.RefreshAggragation = true;
			rocWatchList.ShouldScrollToLastRow = true;
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
					rocWatchList.CopyToClipboard(true);
					break;
				case "Print":
					rocWatchList.Printer.Owner = this;
					rocWatchList.Printer.PrintPreviewDataGridView(rocWatchList, true);
					break;
				case "SaveAs":
					rocWatchList.Save();
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
				items.Add("Symbol", new FilterItem("Symbol", !rocWatchList.Columns["Symbol"].Visible));
				items.Add("SymSymbolDetailbol", new FilterItem("SymbolDetail", !rocWatchList.Columns["SymbolDetail"].Visible));
				
				items.Add("BidPrice", new FilterItem("BidPrice", !rocWatchList.Columns["BidPrice"].Visible));
				items.Add("BidSize", new FilterItem("BidSize", !rocWatchList.Columns["BidSize"].Visible));
				items.Add("BidPartID", new FilterItem("BidPartID", !rocWatchList.Columns["BidPartID"].Visible));
				items.Add("BidHigh", new FilterItem("BidHigh", !rocWatchList.Columns["BidHigh"].Visible));
				items.Add("BidLow", new FilterItem("BidLow", !rocWatchList.Columns["BidLow"].Visible));

				items.Add("AskPrice", new FilterItem("AskPrice", !rocWatchList.Columns["AskPrice"].Visible));
				items.Add("AskSize", new FilterItem("AskSize", !rocWatchList.Columns["AskSize"].Visible));
				items.Add("AskPartID", new FilterItem("AskPartID", !rocWatchList.Columns["AskPartID"].Visible));
				items.Add("AskHigh", new FilterItem("AskHigh", !rocWatchList.Columns["AskHigh"].Visible));
				items.Add("AskLow", new FilterItem("AskLow", !rocWatchList.Columns["AskLow"].Visible));
				
				items.Add("Last", new FilterItem("Last", !rocWatchList.Columns["LastTraded"].Visible));
				items.Add("TradeVolume", new FilterItem("TradeVolume", !rocWatchList.Columns["TradeVolume"].Visible));
				items.Add("TradePartID", new FilterItem("TradePartID", !rocWatchList.Columns["TradePartID"].Visible));

				items.Add("Tick", new FilterItem("Tick", !rocWatchList.Columns["Tick"].Visible));
				items.Add("Net", new FilterItem("Net", !rocWatchList.Columns["NetChange"].Visible));
				items.Add("Pct", new FilterItem("Pct", !rocWatchList.Columns["PctChange"].Visible));
				items.Add("Volume", new FilterItem("Volume", !rocWatchList.Columns["Volume"].Visible));
				items.Add("High", new FilterItem("High", !rocWatchList.Columns["High"].Visible));
				items.Add("Low", new FilterItem("Low", !rocWatchList.Columns["Low"].Visible));
				items.Add("Open", new FilterItem("Open", !rocWatchList.Columns["Open"].Visible));
				items.Add("Close", new FilterItem("Close", !rocWatchList.Columns["Close"].Visible));
				items.Add("Vwap", new FilterItem("Vwap", !rocWatchList.Columns["Vwap"].Visible));
				items.Add("SettlePrice", new FilterItem("SettlePrice", !rocWatchList.Columns["SettlePrice"].Visible));
				items.Add("OpenInterest", new FilterItem("OpenInterest", !rocWatchList.Columns["OpenInterest"].Visible));
				
				items.Add("Name", new FilterItem("Name", !rocWatchList.Columns["Name"].Visible));
				items.Add("TickSize", new FilterItem("TickSize", !rocWatchList.Columns["TickSize"].Visible));
				items.Add("DisplayFactor", new FilterItem("DisplayFactor", !rocWatchList.Columns["DisplayFactor"].Visible));
				
				items.Add("SecType", new FilterItem("SecType", !rocWatchList.Columns["SecType"].Visible));
				items.Add("SecurityStatus", new FilterItem("SecurityStatus", !rocWatchList.Columns["SecurityStatus"].Visible));

				items.Add("LineTime", new FilterItem("LineTime", !rocWatchList.Columns["LineTime"].Visible));
				items.Add("DecodeTime", new FilterItem("DecodeTime", !rocWatchList.Columns["DecodeTime"].Visible));
				items.Add("OnMsgTime", new FilterItem("OnMsgTime", !rocWatchList.Columns["OnMsgTime"].Visible));
				items.Add("ServerSentTime", new FilterItem("ServerSentTime", !rocWatchList.Columns["ServerSentTime"].Visible));
				items.Add("ClientRecivedTime", new FilterItem("ClientRecivedTime", !rocWatchList.Columns["ClientRecivedTime"].Visible));

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
					rocWatchList.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SymbolDetail":
					rocWatchList.Columns["SymbolDetail"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidPrice":
					rocWatchList.Columns["BidPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidSize":
					rocWatchList.Columns["BidSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidPartID":
					rocWatchList.Columns["BidPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidHigh":
					rocWatchList.Columns["BidHigh"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BidLow":
					rocWatchList.Columns["BidLow"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "AskPrice":
					rocWatchList.Columns["AskPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskSize":
					rocWatchList.Columns["AskSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskPartID":
					rocWatchList.Columns["AskPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskHigh":
					rocWatchList.Columns["AskHigh"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AskLow":
					rocWatchList.Columns["AskLow"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Last":
					rocWatchList.Columns["LastTraded"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TradeVolume":
					rocWatchList.Columns["TradeVolume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TradePartID":
					rocWatchList.Columns["TradePartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Tick":
					rocWatchList.Columns["Tick"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Net":
					rocWatchList.Columns["NetChange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Pct":
					rocWatchList.Columns["PctChange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Volume":
					rocWatchList.Columns["Volume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "High":
					rocWatchList.Columns["High"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Low":
					rocWatchList.Columns["Low"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Open":
					rocWatchList.Columns["Open"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Close":
					rocWatchList.Columns["Close"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Vwap":
					rocWatchList.Columns["Vwap"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SettlePrice":
					rocWatchList.Columns["SettlePrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OpenInterest":
					rocWatchList.Columns["OpenInterest"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Name":
					rocWatchList.Columns["Name"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TickSize":
					rocWatchList.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DisplayFactor":
					rocWatchList.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecType":
					rocWatchList.Columns["SecType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "SecurityStatus":
					rocWatchList.Columns["SecurityStatus"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "LineTime":
					rocWatchList.Columns["LineTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DecodeTime":
					rocWatchList.Columns["DecodeTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "OnMsgTime":
					rocWatchList.Columns["OnMsgTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "ServerSentTime":
					rocWatchList.Columns["ServerSentTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "ClientRecivedTime":
					rocWatchList.Columns["ClientRecivedTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
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
			ROCWatchListProfile prof = new ROCWatchListProfile(rocWatchList);

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
					prof = new ROCWatchListProfile(rocWatchList, prof);

					if (checkLatency) { }
				}
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
					rocWatchList.AddDataRow();
					break;
				case "Delete":
					rocWatchList.DeleteDataRow();
					break;
				case "DeleteSelected":
					rocWatchList.DeleteSelectedDataRow();
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

		//private void AddDataRow()
		//{
		//    lock (rocWatchList.RocGridTable)
		//    {
		//        rocWatchList.RocGridTable.Rows.InsertAt(rocWatchList.RocGridTable.NewRow(), rocWatchList.RowLocation);
		//    }
		//}

		//private void DeleteDataRow()
		//{
		//    lock (rocWatchList.RocGridTable)
		//    {
		//        rocWatchList.SuspendLayout();
		//        rocWatchList.RocGridTable.BeginLoadData();

		//        if (rocWatchList.Rows.Count > rocWatchList.RowLocation)
		//        {
		//            if (!rocWatchList.Rows[rocWatchList.RowLocation].IsNewRow)
		//            {
		//                rocWatchList.Rows.Remove(rocWatchList.Rows[rocWatchList.RowLocation]);
		//            }
		//        }

		//        rocWatchList.Refresh();

		//        rocWatchList.RocGridTable.EndLoadData();
		//        rocWatchList.ResumeLayout();
		//    }
		//}

		//private void DeleteSelectedDataRow()
		//{
		//    lock (rocWatchList.RocGridTable)
		//    {
		//        rocWatchList.SuspendLayout();
		//        rocWatchList.RocGridTable.BeginLoadData();

		//        foreach (DataGridViewRow row in rocWatchList.SelectedRows)
		//        {
		//            if (!row.IsNewRow)
		//            {
		//                rocWatchList.Rows.Remove(row);
		//            }
		//        }

		//        rocWatchList.Refresh();

		//        rocWatchList.RocGridTable.EndLoadData();
		//        rocWatchList.ResumeLayout();
		//    }
		//}

		#endregion

		#region - Save Symbol List -

		private void SaveSymbolList()
		{
			LastSavedSymbolDetailList.Clear();
			foreach (DataGridViewRow row in rocWatchList.Rows)
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