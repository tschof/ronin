using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Windows.Forms;

using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using CSVEx;
using RDSEx;
using MarketData;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmBatchMarketTicket : BaseBatchTicket
	{
		private enum PriceSouces
		{
			None,
			Bid,
			Ask,
			Last,
			Middle,
		}

		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return rocBatchMarket.Processing;
			}
			set
			{
				rocBatchMarket.Processing = value;
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
					_searchView = new DataView(rocBatchMarket.RocGridTable);
					_searchView.Sort = "OrderID";
				}
				return _searchView;
			}
			set
			{
				_searchView = value;
			}
		}

		private DataView _searchViewSymbol;
		[Browsable(false)]
		public DataView SearchViewSymbol
		{
			get
			{
				if (_searchViewSymbol == null)
				{
					_searchViewSymbol = new DataView(rocBatchMarket.RocGridTable);
					_searchViewSymbol.Sort = "SymbolDetail";
				}
				return _searchViewSymbol;
			}
			set
			{
				_searchViewSymbol = value;
			}
		}

		private DataView _searchViewMDSymbol;
		[Browsable(false)]
		public DataView SearchViewMDSymbol
		{
			get
			{
				if (_searchViewMDSymbol == null)
				{
					_searchViewMDSymbol = new DataView(rocBatchMarket.RocGridTable);
					_searchViewMDSymbol.Sort = "Symbol";
				}
				return _searchViewMDSymbol;
			}
			set
			{
				_searchViewMDSymbol = value;
			}
		}

		#endregion

		#region - Local Variable -

		private PriceSouces _priceSouce = PriceSouces.None;
		private int _priceOffset = 0;

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private List<ROCOrder> _rocOrders = new List<ROCOrder>();
		private Market _deltas = new Market();

		private char[] symbolDetailSpliter = new char[] { ' ' };

		private bool _initializeWithClipboard = false;

		#endregion

		#region - Constructor -

		public frmBatchMarketTicket()
		{
			InitializeComponent();

			Initialize();
		}

		public frmBatchMarketTicket(bool initializeWithClipboard)
		{
			InitializeComponent();

			_initializeWithClipboard = initializeWithClipboard;

			Initialize();
		}

		private void Initialize()
		{
			BatchGrid = rocBatchMarket;

			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			RefreshClicked += new EventHandler(Form_RefreshClicked);
			DisplayClicked += new EventHandler(Form_DisplayClicked);
			UITimerTicked += new EventHandler(Form_UITimerTicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);

			BatchGrid.ProcessingChanged += new EventHandler(BatchGrid_ProcessingChanged);
			BatchGrid.ShouldScrollToLastRowChanged += new EventHandler(BatchGrid_ShouldScrollToLastRowChanged);
			BatchGrid.RefreshAggragationChanged += new EventHandler(BatchGrid_RefreshAggragationChanged);

			BatchGrid.Initialize();

			BatchGrid.MouseEnter += new EventHandler(BatchGrid_MouseEnter);

			BatchGrid.PropertyChanged += new PropertyChangedEventHandler(BatchGrid_PropertyChanged);
			BatchGrid.KeyDown += new KeyEventHandler(BatchGrid_KeyDown);

			StartTimer(Configuration.User.Default.UIUpdateRate);
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				UpdateBatchMarketByProcessByTimer();
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

		private void Form_FitClicked(object sender, EventArgs e)
		{
			if (!rocBatchMarket.Processing)
			{
				SetColumnMode(rocBatchMarket, DataGridViewAutoSizeColumnMode.DisplayedCells);
				ResizeToFit(rocBatchMarket);
				SetColumnMode(rocBatchMarket, DataGridViewAutoSizeColumnMode.None);
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
		}

		private void Form_DisplayClicked(object sender, EventArgs e)
		{
		}

		private void Form_Load(object sender, EventArgs e)
		{
			if (_initializeWithClipboard)
			{
				_initializeWithClipboard = false;
				cmdPaste_Click(null, EventArgs.Empty);
			}
		}

		private void Form_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void updMultiple_ValueChanged(object sender, EventArgs e)
		{
			if (rocBatchMarket.RocGridTable.Rows.Count > 0)
			{
				rocBatchMarket.RefreshAggragation = true;
			}
		}

		private void cboPriceSouce_SelectedIndexChanged(object sender, EventArgs e)
		{
			switch (cboPriceSouce.Text)
			{
				case "BID":
					_priceSouce = PriceSouces.Bid;
					break;
				case "ASK":
					_priceSouce = PriceSouces.Ask;
					break;
				case "LAST":
					_priceSouce = PriceSouces.Last;
					break;
				case "MID":
					_priceSouce = PriceSouces.Middle;
					break;
				default:
					_priceSouce = PriceSouces.None;
					break;
			}
			System.Threading.ThreadPool.QueueUserWorkItem(RefreshBatchMarket);
		}

		private void updPriceOffset_ValueChanged(object sender, EventArgs e)
		{
			if (_priceOffset != Convert.ToInt32(updPriceOffset.Value))
			{
				_priceOffset = Convert.ToInt32(updPriceOffset.Value);
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshBatchMarket);
			}
		}

		private void cmdPriceDown_Click(object sender, EventArgs e)
		{
			if (_priceOffset != 0)
			{
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshBatchMarket);
			}
		}

		private void cmdPriceUp_Click(object sender, EventArgs e)
		{
			if (_priceOffset != 0)
			{
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshBatchMarket);
			}
		}

		private void cmdToggleAutoReset_Click(object sender, EventArgs e)
		{
			chkAutoReset.Checked = !chkAutoReset.Checked;
			AutoResetShow();
		}

		public override void AutoResetShow()
		{
			if (chkAutoReset.Checked)
			{
				cmdToggleAutoReset.BackColor = Color.LimeGreen;
				cmdToggleAutoReset.Text = "Auto Reset";
			}
			else
			{
				cmdToggleAutoReset.BackColor = Color.DarkRed;
				cmdToggleAutoReset.Text = "No Reset";
			}
		}

		#endregion

		#region - Grid Event -

		private void BatchGrid_ProcessingChanged(object sender, EventArgs e)
		{
			BatchGrid.Enabled = !BatchGrid.Processing;
			//CaptionBar.Enabled = !BatchGrid.Processing;
			//panelTradeCommands.Enabled = !BatchGrid.Processing;

			if (BatchGrid.Processing)
			{
				BatchGrid.Visible = false;
				menuProcessing.Show();
				FormBorderStyle = FormBorderStyle.FixedDialog;
				cboPriceSouce.Enabled = false;
				updPriceOffset.Enabled = false;
			}
			else
			{
				BatchGrid.Visible = true;
				menuProcessing.Hide();
				FormBorderStyle = FormBorderStyle.Sizable;
				cboPriceSouce.Enabled = true;
				updPriceOffset.Enabled = true;
			}

			menuProcessing.Invalidate();
		}

		private void BatchGrid_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!BatchGrid.Processing && BatchGrid.ShouldScrollToLastRow)
			{
				BatchGrid.ShouldScrollToLastRow = false;
				BatchGrid.ScrollToLastRow();
			}
		}

		private void BatchGrid_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && BatchGrid.RefreshAggragation)
			{
				//BatchGrid.UnshareRows();

				BatchGrid.RefreshAggragation = false;
				UpdateAggragation();
			}
		}

		private void BatchGrid_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "SubscribeSymbolDetail":
					UpdateDataRows(rocBatchMarket.SubscribeSymbolDetail);
					break;
				case "ShowColumnMenu":
					ShowColumnMenu();
					break;
				case "ShowStatusMenu":
					ShowStatusMenu();
					break;
				case "ShowSideMenu":
					ShowSideMenu();
					break;
				case "ShowOptionSideMenu":
					ShowOptionSideMenu();
					break;
				case "ShowExchangeMenu":
					ShowExchangeMenu();
					break;
				case "ShowAccountMenu":
					ShowAccountMenu();
					break;
				case "ShowTraderMenu":
					ShowTraderMenu();
					break;
				case "ShowDurationMenu":
					ShowDurationMenu();
					break;
				case "ShowOrderTypeMenu":
					ShowOrderTypeMenu();
					break;
				case "ShowAlgoTypeMenu":
					ShowAlgoTypeMenu();
					break;
				case "ShowStartTimeMenu":
					ShowStartTimeMenu();
					break;
				case "ShowEndTimeMenu":
					ShowEndTimeMenu();
					break;
				case "ShowShortLenderMenu":
					ShowShortLenderMenu();
					break;
				case "ShowMenu":
					ShowGridMenu();
					break;
				case "AlertMessage":
					GLOBAL.HROC.AddToStatusLogs(BatchGrid.AlertMessage);
					GLOBAL.HROC.AddToAlerts(BatchGrid.AlertMessage);

					BatchGrid.AlertMessage = "";
					break;
			}
		}

		private void BatchGrid_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.C && e.Control)
			{
				BatchGrid.CopyToClipboard(false);
			}
			else if (e.KeyCode == Keys.V && e.Control)
			{
				PasteFromClipboard();
			}
		}

		private void BatchGrid_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket)
			{
				ActivateGrid(BatchGrid);
			}
		}

		#endregion

		#region - Binding -

		#region - Refresh - 

		public void RefreshBatchMarket(object input)
		{
			UpdateBatchMarketPricesStart();

			RefreshBatchMarketPrices();

			UpdateBatchMarketPricesStop();
		}

		#endregion

		// Used by Main Refresh and Load Thread
		private delegate void RefreshBatchMarketPricesDelegate();
		private void RefreshBatchMarketPrices()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new RefreshBatchMarketPricesDelegate(RefreshBatchMarketPrices));
				return;
			}

			try
			{
				lock (rocBatchMarket.RocGridTable)
				{
					if (IsProcessing)
					{
						System.Threading.Thread.Sleep(1);
					}

					//rocBatchMarket.SuspendLayout();

					foreach (DataRow row in rocBatchMarket.RocGridTable.Rows)
					{
						UpdatePriceByDataGridViewRow(row);

						#region - Not Used -
						/*switch (_priceSouce)
						{
							case PriceSouces.Bid:
								if (row["BidPrice"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
								{
									if (row["SecType"].ToString() == "E")
									{
										row["Price"] = Math.Round((double)row["BidPrice"] + (double)row["TickSize"] * _priceOffset, 2);
									}
									else
									{
										row["Price"] = (double)row["BidPrice"] + (double)row["TickSize"] * _priceOffset;
									}
								}
								else
								{
									row["Price"] = DBNull.Value;
								}
								break;
							case PriceSouces.Ask:
								if (row["AskPrice"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
								{
									if (row["SecType"].ToString() == "E")
									{
										row["Price"] = Math.Round((double)row["AskPrice"] + (double)row["TickSize"] * _priceOffset, 2);
									}
									else
									{
										row["Price"] = (double)row["AskPrice"] + (double)row["TickSize"] * _priceOffset;
									}
								}
								else
								{
									row["Price"] = DBNull.Value;
								}
								break;
							case PriceSouces.Last:
								if (row["LastTraded"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
								{
									if (row["SecType"].ToString() == "E")
									{
										row["Price"] = Math.Round((double)row["LastTraded"] + (double)row["TickSize"] * _priceOffset, 2);
									}
									else
									{
										row["Price"] = (double)row["LastTraded"] + (double)row["TickSize"] * _priceOffset;
									}
								}
								else
								{
									row["Price"] = DBNull.Value;
								}
								break;
							case PriceSouces.Middle:
								if (row["BidPrice"] != DBNull.Value && row["AskPrice"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
								{
									double bidaskspan = Math.Abs((double)row["AskPrice"] - (double)row["BidPrice"]);
									int midoffset = Convert.ToInt32(bidaskspan / (double)row["TickSize"] / 2);
									if (row["SecType"].ToString() == "E")
									{
										row["Price"] = Math.Round((double)row["BidPrice"] + (double)row["TickSize"] * (_priceOffset + midoffset), 2);
									}
									else
									{
										row["Price"] = (double)row["BidPrice"] + (double)row["TickSize"] * (_priceOffset + midoffset);
									}
								}
								else
								{
									row["Price"] = DBNull.Value;
								}
								break;
							case PriceSouces.None:
								if (row["Price"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
								{
									if (row["SecType"].ToString() == "E")
									{
										row["Price"] = Math.Round((double)row["Price"] + (double)row["TickSize"] * _priceOffset, 2);
									}
									else
									{
										row["Price"] = (double)row["Price"] + (double)row["TickSize"] * _priceOffset;
									}
								}
								break;
							default:
								row["Price"] = DBNull.Value;
								break;
						}*/
						#endregion
					}

					//rocBatchMarket.ResumeLayout();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#region - On Symbol Detail Changed Event -

		private void UpdateDataRows(string symbolDetail)
		{
			DataRow[] rows = rocBatchMarket.RocGridTable.Select("SymbolDetail = '" + symbolDetail + "'");

			if (rows.Length > 0)
			{
				foreach (DataRow row in rows)
				{
					UpdateDataRow(symbolDetail, row, false);
				}
			}
		}

		#endregion

		#region - Used By Process Thread Update Market Data -

		private delegate void UpdateBatchMarketByProcessDelegate(bool updateIM, ROCOrder[] orders, Market deltas);
		public void UpdateBatchMarketByProcess(bool updateIM, ROCOrder[] orders, Market deltas)
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
					BeginInvoke(new UpdateBatchMarketByProcessDelegate(UpdateBatchMarketByProcess), new object[] { updateIM, orders, deltas });
					return;
				}
				try
				{
					if (!IsProcessing && rocBatchMarket.Rows.Count > 1)
					{
						lock (rocBatchMarket.RocGridTable)
						{
							if (updateIM)
							{
								UpdateSecurityInfo();
							}

							if (orders.Length > 0)
							{
								foreach (ROCOrder order in orders)
								{
									LoadRocOrder(order);
									//System.Threading.Thread.Sleep(1);
								}
							}

							if (!deltas.Empty)
							{
								UpdateMarketDataDeltas(deltas);
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

		private void UpdateBatchMarketByProcessByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;
					if (!IsProcessing && rocBatchMarket.Rows.Count > 1)
					{
						lock (rocBatchMarket.RocGridTable)
						{
							if (_updateIM)
							{
								_updateIM = false;
								UpdateSecurityInfo();
							}

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
								foreach (ROCOrder order in orders)
								{
									LoadRocOrder(order);
								}
							}

							Market deltas = Market.Replace(_deltas);
							if (!deltas.Empty)
							{
								UpdateMarketDataDeltas(deltas);
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

				lock (rocBatchMarket.RocGridTable)
				{
					rocBatchMarket.RocGridTable.BeginLoadData();

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

					rocBatchMarket.RocGridTable.EndLoadData();
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
				case CSVFieldIDs.SecurityTypes.Option:
					HelperSubscriber.SubscribeOptionNBBO(secInfo.MDSymbol, secInfo.MDSource);
					break;
				default:
					HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
					break;
			}

			DataRow[] rows = rocBatchMarket.RocGridTable.Select(string.Concat(new object[] { "SymbolDetail = '", symbolDetail, "'" }));
			foreach (DataRow row in rows)
			{
				row["Symbol"] = secInfo.MDSymbol;
				if (!rocBatchMarket.Symbols.Contains(secInfo.MDSymbol))
				{
					rocBatchMarket.Symbols.Add(secInfo.MDSymbol);
				}
				row["TickSize"] = secInfo.TickSize;
				row["SecType"] = secInfo.SecType;
				
				switch (secInfo.SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
					case CSVFieldIDs.SecurityTypes.OptionFuture:
						string[] optionDetails = symbolDetail.Split(symbolDetailSpliter);
						row["CallPut"] = optionDetails[5];

						if (secInfo.TryGetExpiration(out DateTime when))
						{
							DateTime tempDT = when;
							row["ExpDate"] = tempDT.ToString("yyyyMM");
							row["MaturityDay"] = tempDT.ToString("dd");
						}

						row["Strike"] = optionDetails[4];
						row["Underlying"] = optionDetails[0];
						break;
				}
			}
		}

		private void UpdateMarketDataDeltas(Market deltas)
		{
			DataRowView[] rows = new DataRowView[0];
			double price;

			lock (rocBatchMarket.RocGridTable)
			{
				foreach ((string symbol, Book delta) in deltas)
				{
					if (IsProcessing) return;

					if (delta.TryGetNonEmpty(Book.FieldEnum.IssueSymbol, out string issueSymbol) && rocBatchMarket.Symbols.Contains(issueSymbol))
					{
						rows = SearchViewMDSymbol.FindRows(issueSymbol);

						foreach (DataRowView row in rows)
						{
							if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
								row["BidPrice"] = price;

							if (delta.TryGetField(Book.FieldEnum.AskPrice, out price))
								row["AskPrice"] = price;

							if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price))
								row["LastTraded"] = price;

							UpdatePriceByDataGridViewRow(row.Row);
						}
					}
				}

				if (_priceSouce != PriceSouces.None)
				{
					// TODO: Make sure all selected rows are also refreshed
					rocBatchMarket.RefreshSelectedRows();
				}
			}
		}

		private void UpdatePriceByDataGridViewRow(DataRow row)
		{
			try
			{
				switch (_priceSouce)
				{
					case PriceSouces.Bid:
						if (row["BidPrice"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
						{
							if (row["SecType"].ToString() == "E")
							{
								row["Price"] = Math.Round((double)row["BidPrice"] + (double)row["TickSize"] * _priceOffset, 2);
							}
							else
							{
								row["Price"] = (double)row["BidPrice"] + (double)row["TickSize"] * _priceOffset;
							}
						}
						else
						{
							row["Price"] = DBNull.Value;
						}
						break;
					case PriceSouces.Ask:
						if (row["AskPrice"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
						{
							if (row["SecType"].ToString() == "E")
							{
								row["Price"] = Math.Round((double)row["AskPrice"] + (double)row["TickSize"] * _priceOffset, 2);
							}
							else
							{
								row["Price"] = (double)row["AskPrice"] + (double)row["TickSize"] * _priceOffset;
							}
						}
						else
						{
							row["Price"] = DBNull.Value;
						}
						break;
					case PriceSouces.Last:
						if (row["LastTraded"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
						{
							if (row["SecType"].ToString() == "E")
							{
								row["Price"] = Math.Round((double)row["LastTraded"] + (double)row["TickSize"] * _priceOffset, 2);
							}
							else
							{
								row["Price"] = (double)row["LastTraded"] + (double)row["TickSize"] * _priceOffset;
							}
						}
						else
						{
							row["Price"] = DBNull.Value;
						}
						break;
					case PriceSouces.Middle:
						if (row["BidPrice"] != DBNull.Value && row["AskPrice"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
						{
							double bidaskspan = Math.Abs((double)row["AskPrice"] - (double)row["BidPrice"]);
							int midoffset = Convert.ToInt32(bidaskspan / (double)row["TickSize"] / 2);
							if (row["SecType"].ToString() == "E")
							{
								row["Price"] = Math.Round((double)row["BidPrice"] + (double)row["TickSize"] * (_priceOffset + midoffset), 2);
							}
							else
							{
								row["Price"] = (double)row["BidPrice"] + (double)row["TickSize"] * (_priceOffset + midoffset);
							}
						}
						else
						{
							row["Price"] = DBNull.Value;
						}
						break;
					case PriceSouces.None:
						if (row["Price"] != DBNull.Value && row["TickSize"] != DBNull.Value && row["SecType"] != DBNull.Value)
						{
							if (row["SecType"].ToString() == "E")
							{
								row["Price"] = Math.Round((double)row["Price"] + (double)row["TickSize"] * _priceOffset, 2);
							}
							else
							{
								row["Price"] = (double)row["Price"] + (double)row["TickSize"] * _priceOffset;
							}
						}
						break;
					default:
						row["Price"] = DBNull.Value;
						break;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void LoadRocOrder(ROCOrder order)
		{
			if (orderIDs.Contains(order.Tag))
			{
				DataRowView[] rows = SearchView.FindRows(order.Tag);

				foreach (DataRowView row in rows)
				{
					row["Status"] = order.Status;
					row.Row.AcceptChanges();
				}
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
			string callput = "";
			string expDate = "";
			string maturityDay = "";
			string strike = "";
			string underlying = "";

			Book delta = new Book();
			double price;

			UpdateBatchMarketTicketWithSecurityInfo(symbolDetail, ref mdSymbol, ref tickSize, ref secType, ref name, ref callput, ref expDate, ref maturityDay, ref strike, ref underlying);
			UpdateBatchMarketTicketWithCurrentMarketData(mdSymbol.ToUpper(), ref delta);

			row["Symbol"] = mdSymbol;
			if (isRefresh)
			{
				row["symbolDetail"] = symbolDetail;
			}

			if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
			{
				row["BidPrice"] = price;
			}

			if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
				row["AskPrice"] = price;
			}

			if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price)) {
				row["LastTraded"] = price;
			}
			else if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out price)) {
				row["LastTraded"] = price;
			}
			else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
				row["LastTraded"] = price;
			}

			row["TickSize"] = tickSize;

			if (delta.TryGetField(Book.FieldEnum.DisplayConversionFactor, out price))
			{
				row["DisplayFactor"] = price;
			}
			else
			{
				row["DisplayFactor"] = 1;
			}

			if (secType != "")
			{
				row["SecType"] = secType;
			}
			if (callput != "")
			{
				row["CallPut"] = callput;
			}
			if (expDate != "")
			{
				row["ExpDate"] = expDate;
			}
			if (maturityDay != "")
			{
				row["MaturityDay"] = maturityDay;
			}
			if (strike != "")
			{
				row["Strike"] = strike;
			}
			if (underlying != "")
			{
				row["Underlying"] = underlying;
			}

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
		private void UpdateBatchMarketTicketWithCurrentMarketData(string mdSymbol, ref Book delta)
		{
			if (mdSymbol != null && mdSymbol != "")
			{
				if (GLOBAL.HMarketData.Current.TryGet(mdSymbol, out Book found))
				{
					delta.Merge(found);
				}
			}
		}

		// Update with Security Info On Play back & onLoad
		private void UpdateBatchMarketTicketWithSecurityInfo(string symbolDetail, ref string mdSymbol, ref double tickSize, ref string secType, ref string name, ref string callput, ref string expDate, ref string maturityDay, ref string strike, ref string underlying)
		{
			BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
			if (secInfo != null)
			{
				mdSymbol = secInfo.MDSymbol;
				tickSize = secInfo.TickSize;
				secType = secInfo.SecType;
				name = secInfo.LongName;
				//underlying = secInfo.Underlying;

				switch (secInfo.SecType)
				{
					case CSVFieldIDs.SecurityTypes.Option:
					//case CSVFieldIDs.SecutrityTypes.OptionFuture:
						rocBatchMarket.UpdateSymbol(mdSymbol);
						rocBatchMarket.UpdateTickSize(mdSymbol, tickSize);

						string[] optionDetails = symbolDetail.Split(symbolDetailSpliter);
						callput = optionDetails[5];

						if (secInfo.TryGetExpiration(out DateTime when)) {
							expDate = when.ToString("yyyyMM");
							maturityDay = when.ToString("dd");
						}

						strike = optionDetails[4];
						underlying = optionDetails[0];
						break;
					default:
						rocBatchMarket.UpdateTickSize(mdSymbol, tickSize);

						callput = "";
						expDate = "";
						maturityDay = "";
						strike = "";
						break;
				}
			}

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[symbolDetail] = mdSymbol;
			}
		}

		private delegate void UpdateAggragationDelegate();
		private void UpdateAggragation()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateAggragationDelegate(UpdateAggragation));
				return;
			}

			lock (BatchGrid.RocGridTable)
			{
				lblOrdersQty.Value = BatchGrid.OrderCount * (double)updMultiple.Value;

				lblBuyQty.Value = BatchGrid.BuySharesCount * (double)updMultiple.Value;
				lblSellQty.Value = BatchGrid.SellSharesCount * (double)updMultiple.Value;
				lblShortQty.Value = BatchGrid.ShortSharesCount * (double)updMultiple.Value;

				lblTotalQty.Value = (double)lblBuyQty.Value + (double)lblSellQty.Value + (double)lblShortQty.Value;
			}
		}

		private delegate void UpdateBatchMarketPricesStartDelegate();
		private void UpdateBatchMarketPricesStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateBatchMarketPricesStartDelegate(UpdateBatchMarketPricesStart));
				return;
			}

			IsProcessing = true;

			rocBatchMarket.RocGridTable.DefaultView.RowFilter = "";
			rocBatchMarket.RocGridTable.DefaultView.Sort = "";

			rocBatchMarket.RocGridTable.BeginLoadData();
		}

		private delegate void UpdateBatchMarketPricesStopDelegate();
		private void UpdateBatchMarketPricesStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateBatchMarketPricesStopDelegate(UpdateBatchMarketPricesStop));
				return;
			}

			rocBatchMarket.RocGridTable.EndLoadData();

			rocBatchMarket.RocGridTable.AcceptChanges();

			IsProcessing = false;

			rocBatchMarket.RefreshSharedRows = true;
			rocBatchMarket.RefreshAggragation = true;
			rocBatchMarket.ShouldScrollToLastRow = true;
		}

		#endregion

		#region - Action -

		private void cmdCopy_Click(object sender, EventArgs e)
		{
			BatchGrid.CopyToClipboard(false);
		}

		private void cmdClear_Click(object sender, EventArgs e)
		{
			ClearDataRow();
		}

		private void cmdPaste_Click(object sender, EventArgs e)
		{
			PasteFromClipboard();
		}

		private void cmdExecuteSelected_Click(object sender, EventArgs e)
		{
			GLOBAL.HROC.AddToStatusLogs("MD Batch Ticket ExecuteSelected Clicked");
			if (chkAutoReset.Checked)
			{
				GLOBAL.HROC.AddToStatusLogs("MD Batch Ticket ExecuteSelected AutoReset");
				ResetDataRow();
			}

			BuildOrder(true);
		}

		private void cmdExecuteAll_Click(object sender, EventArgs e)
		{
			GLOBAL.HROC.AddToStatusLogs("MD Batch Ticket ExecuteAll Chlicked");
			if (chkAutoReset.Checked)
			{
				GLOBAL.HROC.AddToStatusLogs("MD Batch Ticket ExecuteAll AutoReset");
				ResetDataRow();
			}

			BuildOrder();
		}

		#endregion

		#region - Export & Import -

		public override string ExportGrid()
		{
			try
			{
				ROCBatchMarketProfile prof = new ROCBatchMarketProfile(rocBatchMarket);

				byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
				return System.Convert.ToBase64String(bytes);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return "";
		}

		public override void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCBatchMarketProfile prof = (ROCBatchMarketProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCBatchMarketProfile(rocBatchMarket, prof);
				}
			}
		}

		#endregion

		#region - CSV Load -

		private void cmdLoadCSV_Click(object sender, EventArgs e)
		{
			LoadCSVFrom();
		}

		#endregion

		#region - Build Order -

		public override void BuildOrder(bool selectedOnly)
		{
			updMultiple.Enabled = false;

			lock (BatchGrid)
			{
				BatchGrid.SuspendLayout();
				BatchGrid.RocGridTable.BeginLoadData();

				BuildOrderList((int)updMultiple.Value, selectedOnly);

				BatchGrid.RocGridTable.EndLoadData();
				BatchGrid.RocGridTable.AcceptChanges();
				BatchGrid.ResumeLayout();

				SendOrderList();

				// Reset SearchView for Status Update later
				//BatchGrid.SearchView = new DataView();
				//BatchGrid.SyncToDefaultView("OrderID", BatchGrid.RocGridTable);
			}

			updMultiple.Enabled = true;
		}

		#endregion

		#region - Row Menu Support -

		public override void ClearDataRow()
		{
			try
			{
				chkAutoReset.Checked = false;
				AutoResetShow();

				lock (BatchGrid)
				{
					BatchGrid.RocGridTable.Clear();
				}

				cboPriceSouce.Text = "NONE";
				updPriceOffset.Value = 0;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Row Option Side Menu -

		private menuBaseAction _menuOptionSideActions;
		private PropertyContainer _menuOptionSideActionsContainer;
		internal void ShowOptionSideMenu()
		{
			if (_menuOptionSideActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("BUY OPEN", "BUY OPEN");
				items.Add("BUY CLOSE", "BUY CLOSE");
				items.Add("SELL OPEN", "SELL OPEN");
				items.Add("SELL CLOSE", "SELL CLOSE");
				items.Add("ALL BUY OPEN", "ALL BUY OPEN");
				items.Add("ALL BUY CLOSE", "ALL BUY CLOSE");
				items.Add("ALL SELL OPEN", "ALL SELL OPEN");
				items.Add("ALL SELL CLOSE", "ALL SELL CLOSE");

				_menuOptionSideActions = new menuBaseAction(items, true);
				_menuOptionSideActions.PropertyChanged += new PropertyChangedEventHandler(menuOptionSideActions_PropertyChanged);
			}

			if (_menuOptionSideActionsContainer == null)
			{
				_menuOptionSideActionsContainer = new PropertyContainer(_menuOptionSideActions);
				_menuOptionSideActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOptionSideActionsContainer_Closed);
			}

			_menuOptionSideActionsContainer.Show(Cursor.Position);
		}

		private void menuOptionSideActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "BUY OPEN":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "BUY OPEN", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "BUY OPEN", true);
					}
					break;
				case "BUY CLOSE":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "BUY CLOSE", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "BUY CLOSE", true);
					}
					break;
				case "ALL BUY OPEN":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "BUY OPEN");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "BUY OPEN");
					}
					break;
				case "ALL BUY CLOSE":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "BUY CLOSE");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "BUY CLOSE");
					}
					break;
				case "SELL OPEN":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SELL OPEN", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SELL OPEN", true);
					}
					break;
				case "SELL CLOSE":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SELL CLOSE", true);
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SELL CLOSE", true);
					}
					break;
				case "ALL SELL OPEN":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SELL OPEN");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SELL OPEN");
					}
					break;
				case "ALL SELL CLOSE":
					if (BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value == null)
					{
						ReplaceValue("Side", "", "SELL CLOSE");
					}
					else
					{
						ReplaceValue("Side", BatchGrid.Rows[BatchGrid.RowLocation].Cells["Side"].Value.ToString(), "SELL CLOSE");
					}
					break;
			}

			_menuOptionSideActionsContainer.Close();
		}

		private void menuOptionSideActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuOptionSideActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuOptionSideActionsContainer_Closed);
			_menuOptionSideActionsContainer = null;
			_menuOptionSideActions.PropertyChanged -= new PropertyChangedEventHandler(menuOptionSideActions_PropertyChanged);
			_menuOptionSideActions = null;
		}

		#endregion
	}
}