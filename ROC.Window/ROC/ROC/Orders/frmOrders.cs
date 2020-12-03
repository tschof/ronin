using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

using FormEx;
using RDSEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using ButtonEx;
using BindingListEx;
using CSVEx;
using ROMEx;
using MarketDataEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmOrders : VistaWindowBorder
	{
		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return rocOrdersList.Processing;
			}
			set
			{
				rocOrdersList.Processing = value;
			}
		}

		// Keep a symbol detail list for IM to match option symbol to <SymbolDetail,Symbol>
		private Dictionary<string, string> _imSymbolNeeded;
		public Dictionary<string, string> ImSymbolNeeded
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

		#endregion

		#region - Local Variable -

		private bool _autoFocusLast = true;
		private int _avgPriceResolution = 2;

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private List<ROCOrder> _rocOrders = new List<ROCOrder>();
		private Dictionary<string, MDServerToClient> _deltas = new Dictionary<string, MDServerToClient>();

		#endregion

		#region - Constructor -

		public frmOrders()
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

			Load += new EventHandler(frmOrders_Load);
			FormClosing += new FormClosingEventHandler(frmOrders_FormClosing);

			rocOrdersList.Initialize(GLOBAL.HOrders.Table);

			_autoFocusLast = Configuration.User.Default.OrderAutoFocusLast;
			if (_avgPriceResolution != GLOBAL.AvgPriceResolution)
			{
				_avgPriceResolution = GLOBAL.AvgPriceResolution;
				rocOrdersList.UpdateResolution(_avgPriceResolution);
			}

			foreach (int key in GLOBAL.HRDS.DestIDToName.Keys)
			{
				((DGVOrderDestinationColumn)rocOrdersList.Columns["Exchange"]).DestIDToName.Add(key, GLOBAL.HRDS.DestIDToName[key]);
			}

			rocOrdersList.PropertyChanged += new PropertyChangedEventHandler(rocOrdersList_PropertyChanged);

			rocOrdersList.MouseEnter += new EventHandler(rocOrdersList_MouseEnter);

			rocOrdersList.ProcessingChanged += new EventHandler(rocOrdersList_ProcessingChanged);
			rocOrdersList.ShouldScrollToLastRowChanged += new EventHandler(rocOrdersList_ShouldScrollToLastRowChanged);
			rocOrdersList.RefreshAggragationChanged += new EventHandler(rocOrdersList_RefreshAggragationChanged);
			rocOrdersList.RefreshSharedRowsChanged += new EventHandler(rocOrdersList_RefreshSharedRowsChanged);
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (IsProcessing)
			{
				if (menuProcessing.Caption != rocOrdersList.GridKeys.Count.ToString())
				{
					menuProcessing.Caption = rocOrdersList.GridKeys.Count.ToString();
				}
			}

			if (_autoFocusLast != Configuration.User.Default.OrderAutoFocusLast)
			{
				_autoFocusLast = Configuration.User.Default.OrderAutoFocusLast;
			}

			if (GLOBAL.UseDelayedUpdate)
			{
				AddUpdateOrdersByProcessByTimer();
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
					lock (rocOrdersList)
					{
						SetColumnMode(rocOrdersList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocOrdersList);
						SetColumnMode(rocOrdersList, DataGridViewAutoSizeColumnMode.None);
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
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshROCOrders);
			}

			if (_autoFocusLast != Configuration.User.Default.OrderAutoFocusLast)
			{
				_autoFocusLast = Configuration.User.Default.OrderAutoFocusLast;
			}

			if (_avgPriceResolution != GLOBAL.AvgPriceResolution)
			{
				_avgPriceResolution = GLOBAL.AvgPriceResolution;
				rocOrdersList.UpdateResolution(_avgPriceResolution);
			}
		}

		#region - Display -

		private frmOrdersDisplay _ordersDisplay;
		private frmOrdersDisplay ordersDisplay
		{
			get
			{
				if (_ordersDisplay == null)
				{
					_ordersDisplay = new frmOrdersDisplay(new ROCOrderProfile(rocOrdersList));
					_ordersDisplay.PropertyChanged += new PropertyChangedEventHandler(Form_PropertyChanged);
					_ordersDisplay.Disposed += new EventHandler(Form_Disposed);
				}
				return _ordersDisplay;
			}
			set
			{
				_ordersDisplay = value;
			}
		}
		private void Form_DisplayClicked(object sender, EventArgs e)
		{
			GLOBAL.MainForm.TopMost = false;
			bool orgTopMost = TopMost;
			TopMost = false;

			ordersDisplay.ShowDialog();

			TopMost = orgTopMost;
			GLOBAL.MainForm.TopMost = true;
		}

		private void Form_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Apply":
					SetProfile(ordersDisplay.GetProfile(new ROCOrderProfile(rocOrdersList)));
					break;
				case "ApplyToAll":
					ROCOrderProfile prof = ordersDisplay.GetProfile(new ROCOrderProfile(rocOrdersList));
					foreach (frmOrders w in GLOBAL.HWindows.OrderWindows.Values)
					{
						w.SetProfile(ordersDisplay.GetProfile(prof));
					}
					break;
			}
		}

		private void Form_Disposed(object sender, EventArgs e)
		{
			if (_ordersDisplay != null)
			{
				_ordersDisplay.PropertyChanged -= new PropertyChangedEventHandler(Form_PropertyChanged);
				_ordersDisplay.Disposed -= new EventHandler(Form_Disposed);
			}

			_ordersDisplay = null;
		}

		public void SetProfile(ROCOrderProfile prof)
		{
			new ROCOrderProfile(rocOrdersList, prof);
		}

		#endregion

		private void frmOrders_Load(object sender, EventArgs e)
		{
			SetStatusDisplay();
			SetSecuritiesDisplay();
			SetOrderTypesDisplay();
			SetTIFsDisplay();
			SetSymbolDisplay();
			SetAccountDisplay();
			RefreshROCOrders(null);
		}

		private void frmOrders_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void cmdCancelAll_Click(object sender, EventArgs e)
		{
			if (Configuration.User.Default.ConfirmOnCancellAll)
			{
				if (MessageBox.Show("Cancel All Open Orders?", "Confirmation", MessageBoxButtons.YesNo) == DialogResult.Yes)
				{
					GLOBAL.HROM.CancelAllOpenOrders("");
				}
			}
			else
			{
				GLOBAL.HROM.CancelAllOpenOrders("");
			}
		}

		private void cmdShowAll_Click(object sender, EventArgs e)
		{
			rocOrdersList.FilterReset();

			SetStatusDisplay();
			SetSecuritiesDisplay();
			SetOrderTypesDisplay();
			SetTIFsDisplay();
			SetSymbolDisplay();
			SetAccountDisplay();
		}

		#endregion

		#region - Grid Event -

		private void rocOrdersList_ProcessingChanged(object sender, EventArgs e)
		{
			rocOrdersList.Enabled = !IsProcessing;
			//CaptionBar.Enabled = !IsProcessing;
			//panelOrderCommands.Enabled = !IsProcessing;

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

		private void rocOrdersList_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocOrdersList.ShouldScrollToLastRow)
			{
				rocOrdersList.ShouldScrollToLastRow = false;
				rocOrdersList.ScrollToLastRow();
			}
		}

		private void rocOrdersList_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocOrdersList.RefreshAggragation)
			{
				rocOrdersList.RefreshAggragation = false;
				UpdateAggragation();
			}
		}

		private void rocOrdersList_RefreshSharedRowsChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && rocOrdersList.RefreshSharedRows)
			{
				rocOrdersList.RefreshSharedRows = false;
				//rocOrdersList.UnshareRows();
			}
		}

		private void rocOrdersList_PropertyChanged(object sender, PropertyChangedEventArgs e)
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
				case "ShowThisOMTag":
					GetTrades();
					break;
				case "ShowMenu":
					ShowOrderModificationMenu();
					break;
			}
		}

		private void rocOrdersList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusOrders)
			{
				ActivateGrid(rocOrdersList);
			}
		}

		#endregion

		#region - Export & Import -

		public string ExportGrid()
		{
			ROCOrderProfile prof = new ROCOrderProfile(rocOrdersList);

			ToBinary b = new ToBinary();
			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCOrderProfile prof = (ROCOrderProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCOrderProfile(rocOrdersList, prof);
				}
			}
		}

		public void Clone()
		{
			lock (GLOBAL.HWindows.OrderWindows)
			{
				frmOrders w = new frmOrders();

				w.Width = this.Width;
				w.Height = this.Height;

				w.ImportGrid(ExportGrid());

				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}

		#endregion

		#region - Binding -

		private void RefreshROCOrders(object input)
		{
			UpdateOrderStart();

			ROCOrder[] orders = new ROCOrder[0];
			lock (GLOBAL.HOrders.RocItems)
			{
				orders = new ROCOrder[GLOBAL.HOrders.RocItems.Values.Count];
				GLOBAL.HOrders.RocItems.Values.CopyTo(orders, 0);
			}

			AddUpdateOrders(orders);

			UpdateOrderStop();
		}

		// Used by Main Refresh and Load Thread
		private delegate void UpdateOrderDelegate(ROCOrder[] orders);
		private void AddUpdateOrders(ROCOrder[] orders)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateOrderDelegate(AddUpdateOrders), new object[] { orders });
				return;
			}

			try
			{
				lock (rocOrdersList.RocGridTable)
				{
					//rocOrdersList.SuspendLayout();

					rocOrdersList.RocGridTable.Rows.Clear();
					rocOrdersList.OrderStatuses.Clear();
					rocOrdersList.GridKeys.Clear();
					rocOrdersList.Symbols.Clear();
					rocOrdersList.Symbols.Add("");
					rocOrdersList.Details.Clear();
					rocOrdersList.Details.Add("");

					if (orders.Length > 0)
					{
						foreach (ROCOrder order in orders)
						{
							LoadRocOrder(order);
						}
					}

					//rocOrdersList.ResumeLayout();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#region - Used by Process Thread -

		private delegate void AddUpdateOrdersByProcessDelegate(bool updateIM, ROCOrder[] orders, Dictionary<string, MDServerToClient> deltas);
		public void AddUpdateOrdersByProcess(bool updateIM, ROCOrder[] orders, Dictionary<string, MDServerToClient> deltas)
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
					BeginInvoke(new AddUpdateOrdersByProcessDelegate(AddUpdateOrdersByProcess), new object[] { updateIM, orders, deltas });
					return;
				}
				try
				{
					if (!IsProcessing)
					{
						lock (rocOrdersList.RocGridTable)
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
								}
							}
						}
						if (_menuOrderModification != null && (updateIM || deltas.Count > 0))
						{
							lock (_menuOrderModification)
							{
								_menuOrderModification.UpdateOrderModificationTicketByProcess(updateIM, deltas);
							}
						}
					}
					if (orders.Length > 0)
					{
						rocOrdersList.RefreshAggragation = true;
						rocOrdersList.ShouldScrollToLastRow = _autoFocusLast;
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void AddUpdateOrdersByProcessByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;

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
					if (!IsProcessing)
					{
						bool updateIM = _updateIM;
						lock (rocOrdersList.RocGridTable)
						{
							if (updateIM)
							{
								_updateIM = false;
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
						if (_menuOrderModification != null && (updateIM || deltas.Count > 0))
						{
							lock (_menuOrderModification)
							{
								_menuOrderModification.UpdateOrderModificationTicketByProcess(updateIM, deltas);
							}
						}
					}
					if (orders.Length > 0)
					{
						rocOrdersList.RefreshAggragation = true;
						rocOrdersList.ShouldScrollToLastRow = _autoFocusLast;
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

				DataRow[] rows = new DataRow[0];
				List<string> removeList = new List<string>();

				BaseSecurityInfo secInfo = null;
				lock (rocOrdersList.RocGridTable)
				{
					rocOrdersList.RocGridTable.BeginLoadData();

					foreach (string symbolDetail in lcoImSymbolNeeded.Keys)
					{
						secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
						if (secInfo != null)
						{
							rows = rocOrdersList.RocGridTable.Select(string.Concat(new object[] { "SymbolDetail = '", symbolDetail, "'" }));
							foreach (DataRow row in rows)
							{
								row["TickSize"] = secInfo.TickSize;
								row["ContractSize"] = secInfo.ContractSize;
							}
						}

						if (!removeList.Contains(symbolDetail))
						{
							removeList.Add(symbolDetail);
						}
					}

					rocOrdersList.RocGridTable.EndLoadData();
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

		#endregion

		private void LoadRocOrder(ROCOrder order)
		{
			lock (rocOrdersList.RocGridTable)
			{
				if (!rocOrdersList.OrderStatuses.ContainsKey(order.Tag))
				{
					rocOrdersList.OrderStatuses.Add(order.Tag, order.Status);

					if (!rocOrdersList.Symbols.Contains(order.Symbol))
					{
						rocOrdersList.Symbols.Add(order.Symbol);
					}

					if (!rocOrdersList.Details.Contains(order.SymbolDetail))
					{
						rocOrdersList.Details.Add(order.SymbolDetail);
					}

					order = UpdateOrdersWithSecurityInfo(order);

					rocOrdersList.RocGridTable.Rows.Add(new object[] {
						order.Tag,
						order.Symbol,
						order.SymbolDetail,
						order.SymbolDisplay,
						order.Status,
						order.Side,
						order.Qty,
						order.Price,
						order.StopPrice,
						order.PegPrice,
						order.LeaveQty,
						order.CumQty,
						order.AvgPrice,
						order.OrderType,
						order.TIF,
						order.AlgoType,
						order.EndTime,
						order.DestID,
						order.OmTime,
						order.ClearingAcct,
						order.ContractSize,
						order.TickSize,
						order.DisplayFactor,
						order.OmTag,
						order.SecType,
						order.Text,
						order.ParentTag,
						order.ClientEcho });

						switch (order.SecType)
						{
							case CSVFieldIDs.SecutrityTypes.Option:
								GLOBAL.HRDS.GetOptionChain(order.Underlying);
								break;
							case CSVFieldIDs.SecutrityTypes.SingleStockFuture:
								GLOBAL.HRDS.GetSSFutureChain(order.Underlying);
								break;
							default:
								GLOBAL.HRDS.GetSecurityByTicker(order.SymbolDetail);
								break;
						}
				}
				else
				{
					if (CSVFieldIDs.StatusCodes.IsActive(rocOrdersList.OrderStatuses[order.Tag]))
					{
						DataRow[] rows = rocOrdersList.RocGridTable.Select(string.Format("Tag = '{0}'", order.Tag));

						long status = 0;
						foreach (DataRow row in rows)
						{
							status = 0;
							if (row["Status"] != DBNull.Value && row["Status"].ToString() != "")
							{
								Int64.TryParse(row["Status"].ToString(), out status);
							}

							switch (status)
							{
								case CSVFieldIDs.StatusCodes.Busted:
								case CSVFieldIDs.StatusCodes.Expired:
								case CSVFieldIDs.StatusCodes.Rejected:
								case CSVFieldIDs.StatusCodes.Canceled:
								case CSVFieldIDs.StatusCodes.Filled:
								case CSVFieldIDs.StatusCodes.FilledAndCancelled:
								case CSVFieldIDs.StatusCodes.ReplacedAndFilled:
									// Already dead order, don't update status anymore
									break;
								default:
									row["Status"] = order.Status;
									row["Qty"] = order.Qty;
									row["Price"] = order.Price;
									row["StopPrice"] = order.StopPrice;
									row["PegPrice"] = order.PegPrice;
									row["LeaveQty"] = order.LeaveQty;
									row["CumQty"] = order.CumQty;
									row["AvgPrice"] = order.AvgPrice;
									row["OrderType"] = order.OrderType;
									row["TIF"] = order.TIF;
									row["AlgoType"] = order.AlgoType;
									row["OmTime"] = order.OmTime;
									row["Text"] = order.Text;
									row["ParentTag"] = order.ParentTag;
									row["Echo"] = order.ClientEcho;
									break;
							}
						}
					}
				}
			}
		}

		private ROCOrder UpdateOrdersWithSecurityInfo(ROCOrder order)
		{
			BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(order.SymbolDetail);

			if (secInfo != null)
			{
				if (order.Symbol == "")
				{
					order.Symbol = secInfo.MDSymbol;
				}
				order.TickSize = secInfo.TickSize;
				order.ContractSize = secInfo.ContractSize;

				switch (order.SecType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
						rocOrdersList.UpdateSymbol(order.Symbol);
						rocOrdersList.UpdateTickSize(order.Symbol, order.TickSize);
						break;
					default:
						rocOrdersList.UpdateTickSize(order.Symbol, order.TickSize);
						break;
				}
			}

			lock (ImSymbolNeeded)
			{
				if (!ImSymbolNeeded.ContainsKey(order.SymbolDetail))
				{
					ImSymbolNeeded.Add(order.SymbolDetail, order.Symbol);
				}
				else
				{
					ImSymbolNeeded[order.SymbolDetail] = order.Symbol;
				}
			}

			return order;
		}

		private delegate void UpdateOrderStartDelegate();
		private void UpdateOrderStart()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateOrderStartDelegate(UpdateOrderStart));
				return;
			}

			IsProcessing = true;

			rocOrdersList.LastFilter = rocOrdersList.RocGridTable.DefaultView.RowFilter;
			rocOrdersList.LastSort = rocOrdersList.RocGridTable.DefaultView.Sort;

			rocOrdersList.RocGridTable.BeginLoadData();
			rocOrdersList.RocGridTable.DefaultView.RowFilter = "";
			rocOrdersList.RocGridTable.DefaultView.Sort = "";
		}

		private delegate void UpdateOrderStopDelegate();
		private void UpdateOrderStop()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateOrderStopDelegate(UpdateOrderStop));
				return;
			}

			rocOrdersList.RocGridTable.DefaultView.Sort = rocOrdersList.LastSort;
			rocOrdersList.RocGridTable.DefaultView.RowFilter = rocOrdersList.LastFilter;
			rocOrdersList.RocGridTable.EndLoadData();

			IsProcessing = false;
			rocOrdersList.ImportDone = true;

			rocOrdersList.RefreshSharedRows = true;
			rocOrdersList.RefreshAggragation = true;
			rocOrdersList.ShouldScrollToLastRow = true;
		}

		private delegate void UpdateAggragationDelegate();
		private void UpdateAggragation()
		{
			if (InvokeRequired)
			{
				BeginInvoke(new UpdateAggragationDelegate(UpdateAggragation));
				return;
			}

			lock (rocOrdersList.RocGridTable)
			{
				long buyQty = 0;
				long shortQty = 0;
				long sellQty = 0;

				lblOrdersQty.Value = rocOrdersList.Rows.Count;

				// Buy
				lblBuyOpened.Value = rocOrdersList.OpenBuySharesCount;
				buyQty = rocOrdersList.FilledBuySharesCount;
				lblBuyFilled.Value = buyQty;

				// Short
				lblShortOpened.Value = rocOrdersList.OpenShortSharesCount;
				shortQty = rocOrdersList.FilledShortSharesCount;
				lblShortFilled.Value = shortQty;

				// Sell
				lblSellOpened.Value = rocOrdersList.OpenSellSharesCount;
				sellQty = rocOrdersList.FilledSellSharesCount;
				lblSellFilled.Value = sellQty;

				lblNet.Value = buyQty - sellQty - shortQty;
			}
		}

		#endregion

		#region - Actions -

		private menuBaseAction _menuActions;
		private PropertyContainer _menuActionsContainer;
		private void ShowActionMenu()
		{
			if (_menuActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("CancelAll", "Cancel All");
				items.Add("Clone", "Clone");
				items.Add("Copy", "Copy");
				items.Add("CopySpecial", "CopySpecial");
				items.Add("Print", "Print");
				items.Add("SaveAs", "SaveAs");
				items.Add("SaveAsSpecial", "SaveAsSpecial");

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
			_menuActionsContainer.Close();
			switch (e.PropertyName)
			{
				case "Clone":
					Clone();
					break;
				case "Copy":
					rocOrdersList.CopyToClipboard(true);
					break;
				case "CopySpecial":
					rocOrdersList.CopyToClipboard(true, rocOrdersList.Columns["OrderID"].DisplayIndex);
					break;
				case "Print":
					rocOrdersList.Printer.Owner = this;
					rocOrdersList.Printer.PrintPreviewDataGridView(rocOrdersList, true);
					break;
				case "SaveAs":
					rocOrdersList.Save();
					break;
				case "SaveAsSpecial":
					rocOrdersList.Save(rocOrdersList.Columns["OrderID"].DisplayIndex);
					break;
				case "CancelAll":
					GLOBAL.HROM.CancelAllOpenOrders("");
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
				items.Add("OrderID", new FilterItem("OrderID", !rocOrdersList.Columns["OrderID"].Visible));
				items.Add("BaseSymbol", new FilterItem("BaseSymbol", !rocOrdersList.Columns["Symbol"].Visible));
				items.Add("Symbol", new FilterItem("Symbol", !rocOrdersList.Columns["SymbolDetail"].Visible));
				items.Add("Display", new FilterItem("Display", !rocOrdersList.Columns["SymbolDisplay"].Visible));
				items.Add("Status", new FilterItem("Status", !rocOrdersList.Columns["Status"].Visible));
				items.Add("Side", new FilterItem("Side", !rocOrdersList.Columns["Side"].Visible));
				items.Add("Qty", new FilterItem("Qty", !rocOrdersList.Columns["Qty"].Visible));
				items.Add("Leave", new FilterItem("Leave", !rocOrdersList.Columns["Leave"].Visible));
				items.Add("Price", new FilterItem("Price", !rocOrdersList.Columns["Price"].Visible));
				items.Add("StopPrice", new FilterItem("StopPrice", !rocOrdersList.Columns["StopPrice"].Visible));
				items.Add("PegPrice", new FilterItem("PegPrice", !rocOrdersList.Columns["PegPrice"].Visible));
				items.Add("Filled", new FilterItem("Filled", !rocOrdersList.Columns["Filled"].Visible));
				items.Add("AvgPrice", new FilterItem("AvgPrice", !rocOrdersList.Columns["AvgPrice"].Visible));
				items.Add("Type", new FilterItem("Type", !rocOrdersList.Columns["Type"].Visible));
				items.Add("Duration", new FilterItem("Duration", !rocOrdersList.Columns["Duration"].Visible));
				items.Add("AlgoType", new FilterItem("AlgoType", !rocOrdersList.Columns["AlgoType"].Visible));
				items.Add("AlgoEndTime", new FilterItem("AlgoEndTime", !rocOrdersList.Columns["AlgoEndTime"].Visible));
				items.Add("Exchange", new FilterItem("Exchange", !rocOrdersList.Columns["Exchange"].Visible));
				items.Add("Time", new FilterItem("Time", !rocOrdersList.Columns["Time"].Visible));
				items.Add("Account", new FilterItem("Account", !rocOrdersList.Columns["Account"].Visible));
				items.Add("Mult", new FilterItem("Mult", !rocOrdersList.Columns["ContractSize"].Visible));
				items.Add("TSize", new FilterItem("TSize", !rocOrdersList.Columns["TickSize"].Visible));
				items.Add("DFactor", new FilterItem("DFactor", !rocOrdersList.Columns["DisplayFactor"].Visible));
				items.Add("PTag", new FilterItem("PTag", !rocOrdersList.Columns["ParentTag"].Visible));
				items.Add("Echo", new FilterItem("Echo", !rocOrdersList.Columns["Echo"].Visible));

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
				case "OrderID":
					rocOrdersList.Columns["OrderID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "BaseSymbol":
					rocOrdersList.Columns["Symbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Symbol":
					rocOrdersList.Columns["SymbolDetail"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Display":
					rocOrdersList.Columns["SymbolDisplay"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Status":
					rocOrdersList.Columns["Status"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Side":
					rocOrdersList.Columns["Side"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Qty":
					rocOrdersList.Columns["Qty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Leave":
					rocOrdersList.Columns["Leave"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Price":
					rocOrdersList.Columns["Price"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "StopPrice":
					rocOrdersList.Columns["StopPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "PegPrice":
					rocOrdersList.Columns["PegPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Filled":
					rocOrdersList.Columns["Filled"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AvgPrice":
					rocOrdersList.Columns["AvgPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Type":
					rocOrdersList.Columns["Type"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AlgoType":
					rocOrdersList.Columns["AlgoType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "AlgoEndTime":
					rocOrdersList.Columns["AlgoEndTime"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Duration":
					rocOrdersList.Columns["Duration"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Exchange":
					rocOrdersList.Columns["Exchange"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Time":
					rocOrdersList.Columns["Time"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Account":
					rocOrdersList.Columns["Account"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Mult":
					rocOrdersList.Columns["ContractSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TSize":
					rocOrdersList.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DFactor":
					rocOrdersList.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "PTag":
					rocOrdersList.Columns["ParentTag"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Echo":
					rocOrdersList.Columns["Echo"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
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

		#region - Filter Status -

		private menuBaseFilter _menuStatus;
		private PropertyContainer _menuStatusContainer;
		private void cmdStatus_Click(object sender, EventArgs e)
		{
			if (_menuStatus == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();
				items.Add("Filled", new FilterItem("Filled", rocOrdersList.FilterOutFilled));
				items.Add("PartialFilled", new FilterItem("PartialFilled", rocOrdersList.FilterOutPartialFilled));
				items.Add("Replaced", new FilterItem("Replaced", rocOrdersList.FilterOutReplaced));
				items.Add("Open", new FilterItem("Open", rocOrdersList.FilterOutOpen));
				items.Add("Rejected", new FilterItem("Rejected", rocOrdersList.FilterOutRejected));
				items.Add("Canceled", new FilterItem("Canceled", rocOrdersList.FilterOutCanceled));

				_menuStatus = new menuBaseFilter(items);
				_menuStatus.PropertyChanged += new PropertyChangedEventHandler(menuStatus_PropertyChanged);
			}

			if (_menuStatusContainer == null)
			{
				_menuStatusContainer = new PropertyContainer(_menuStatus);
				_menuStatusContainer.Closed += new ToolStripDropDownClosedEventHandler(menuStatusContainer_Closed);
			}

			_menuStatusContainer.Show((VistaButton)sender);
		}

		private void menuStatus_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Filled":
					rocOrdersList.FilterOutFilled = _menuStatus.Items[e.PropertyName].Hide;
					break;
				case "PartialFilled":
					rocOrdersList.FilterOutPartialFilled = _menuStatus.Items[e.PropertyName].Hide;
					break;
				case "Replaced":
					rocOrdersList.FilterOutReplaced = _menuStatus.Items[e.PropertyName].Hide;
					break;
				case "Open":
					rocOrdersList.FilterOutOpen = _menuStatus.Items[e.PropertyName].Hide;
					break;
				case "Rejected":
					rocOrdersList.FilterOutRejected = _menuStatus.Items[e.PropertyName].Hide;
					break;
				case "Canceled":
					rocOrdersList.FilterOutCanceled = _menuStatus.Items[e.PropertyName].Hide;
					break;
			}

			SetStatusDisplay();
		}

		private void menuStatusContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuStatusContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuStatusContainer_Closed);
			_menuStatusContainer = null;
			_menuStatus.PropertyChanged -= new PropertyChangedEventHandler(menuStatus_PropertyChanged);
			_menuStatus = null;
		}

		private void SetStatusDisplay()
		{
			string filters = "Status:";

			if (rocOrdersList.FilterOutFilled)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Filled" });
			}

			if (rocOrdersList.FilterOutPartialFilled)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " PartialFilled" });
			}

			if (rocOrdersList.FilterOutReplaced)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Replaced" });
			}

			if (rocOrdersList.FilterOutOpen)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Open" });
			}

			if (rocOrdersList.FilterOutRejected)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Rejected" });
			}

			if (rocOrdersList.FilterOutCanceled)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Canceled" });
			}

			cmdStatus.Text = filters;
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

				items.Add("Stock", new FilterItem("Stock", rocOrdersList.FilterOutStock));
				items.Add("Future", new FilterItem("Future", rocOrdersList.FilterOutFuture));
				items.Add("Option", new FilterItem("Option", rocOrdersList.FilterOutOption));

				_menuSecurities = new menuBaseFilter(items);
				_menuSecurities.PropertyChanged += new PropertyChangedEventHandler(menuSecurities_PropertyChanged);
			}

			if (_menuSecuritiesContainer == null)
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
					rocOrdersList.FilterOutStock = _menuSecurities.Items[e.PropertyName].Hide;
					break;
				case "Future":
					rocOrdersList.FilterOutFuture = _menuSecurities.Items[e.PropertyName].Hide;
					break;
				case "Option":
					rocOrdersList.FilterOutOption = _menuSecurities.Items[e.PropertyName].Hide;
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

			if (rocOrdersList.FilterOutStock)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Stock" });
			}

			if (rocOrdersList.FilterOutFuture)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Future" });
			}

			if (rocOrdersList.FilterOutOption)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Option" });
			}

			cmdSecurities.Text = filters;
		}

		#endregion

		#region - Filter OrderTypes -

		private menuBaseFilter _menuOrderTypes;
		private PropertyContainer _menuOrderTypesContainer;
		private void cmdOrderType_Click(object sender, EventArgs e)
		{
			if (_menuOrderTypes == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();

				items.Add("Market", new FilterItem("Market", rocOrdersList.FilterOutMarket));
				items.Add("Limit", new FilterItem("Limit", rocOrdersList.FilterOutLimit));

				_menuOrderTypes = new menuBaseFilter(items);
				_menuOrderTypes.PropertyChanged += new PropertyChangedEventHandler(menuOrderTypes_PropertyChanged);
			}

			if (_menuOrderTypesContainer == null)
			{
				_menuOrderTypesContainer = new PropertyContainer(_menuOrderTypes);
				_menuOrderTypesContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOrderTypesContainer_Closed);
			}

			_menuOrderTypesContainer.Show((VistaButton)sender);
		}

		private void menuOrderTypes_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Market":
					rocOrdersList.FilterOutMarket = _menuOrderTypes.Items[e.PropertyName].Hide;
					break;
				case "Limit":
					rocOrdersList.FilterOutLimit = _menuOrderTypes.Items[e.PropertyName].Hide;
					break;
			}

			SetOrderTypesDisplay();
		}

		private void menuOrderTypesContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuOrderTypesContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuOrderTypesContainer_Closed);
			_menuOrderTypesContainer = null;
			_menuOrderTypes.PropertyChanged -= new PropertyChangedEventHandler(menuOrderTypes_PropertyChanged);
			_menuOrderTypes = null;
		}

		private void SetOrderTypesDisplay()
		{
			string filters = "OrderType:";

			if (rocOrdersList.FilterOutMarket)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Market" });
			}

			if (rocOrdersList.FilterOutLimit)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Limit" });
			}

			cmdOrderType.Text = filters;
		}

		#endregion

		#region - Filter TIF -

		private menuBaseFilter _menuTIFs;
		private PropertyContainer _menuTIFsContainer;
		private void cmdTIF_Click(object sender, EventArgs e)
		{
			if (_menuTIFs == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();

				items.Add("Day", new FilterItem("Day", rocOrdersList.FilterOutDay));

				_menuTIFs = new menuBaseFilter(items);
				_menuTIFs.PropertyChanged += new PropertyChangedEventHandler(menuTIFs_PropertyChanged);
			}

			if (_menuTIFsContainer == null)
			{
				_menuTIFsContainer = new PropertyContainer(_menuTIFs);
				_menuTIFsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuTIFsContainer_Closed);
			}

			_menuTIFsContainer.Show((VistaButton)sender);
		}

		private void menuTIFs_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Day":
					rocOrdersList.FilterOutDay = _menuTIFs.Items[e.PropertyName].Hide;
					break;
			}

			SetTIFsDisplay();
		}

		private void menuTIFsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuTIFsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuTIFsContainer_Closed);
			_menuTIFsContainer = null;
			_menuTIFs.PropertyChanged -= new PropertyChangedEventHandler(menuTIFs_PropertyChanged);
			_menuTIFs = null;
		}

		private void SetTIFsDisplay()
		{
			string filters = "TIF:";

			if (rocOrdersList.FilterOutDay)
			{
			}
			else
			{
				filters = String.Concat(new object[] { filters, " Day" });
			}


			cmdTIF.Text = filters;
		}

		#endregion

		#region - Fitler Accounts -

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
					if (rocOrdersList.FilterOutAccounts.Contains(e.PropertyName))
					{
						rocOrdersList.FilterOutAccounts = rocOrdersList.FilterOutAccounts.Replace(string.Concat(new object[] { e.PropertyName }), "");
					}
					else
					{
						rocOrdersList.FilterOutAccounts = string.Concat(new object[] { rocOrdersList.FilterOutAccounts, " ", e.PropertyName });
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
				if (rocOrdersList.FilterOutAccounts.Contains(acct.clearingAcIDShort))
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
				if (!rocOrdersList.FilterOutAccounts.Contains(acct.clearingAcIDShort))
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
					rocOrdersList.FilterOutSymbols = txtSymbolFilter.Text;
					break;
			}
		}

		private void SetSymbolDisplay()
		{
			txtSymbolFilter.Text = rocOrdersList.FilterOutSymbols;
			lblSymbols.Text = "Show Only : " + rocOrdersList.FilterOutSymbols;
		}

		#endregion

		#region - Trades -

		private void GetTrades()
		{
			if (rocOrdersList.ShowThisOMTag != "")
			{
				DataRow[] rows = GLOBAL.HExecutions.Table.Select(
					string.Format( "OmTag = '{0}' and TPOS = 0",
					rocOrdersList.ShowThisOMTag));
				
				if (rows.Length > 0)
				{
					UpdateTradeSecurityInfo(ref rows);
					ShowSubTrades(rows);
				}
			}
		}

		private void UpdateTradeSecurityInfo(ref DataRow[] rows)
		{
			object imObj = null;
			string symbolDetail = "";
			string secType = "";

			// Update the rows with right tick size
			foreach (DataRow row in rows)
			{
				symbolDetail = (string)row["symbolDetail"];
				secType = (string)row["SecType"];

				row["TickSize"] = 0.01;
				row["ContractSize"] = 1;

				switch (secType)
				{
					case CSVFieldIDs.SecutrityTypes.Option:
						imObj = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
						if (imObj != null && imObj is IMOptionInfo)
						{
							row["Symbol"] = ((IMOptionInfo)imObj).MDSymbol;
							row["TickSize"] = ((IMOptionInfo)imObj).TickSize;
						}
						row["ContractSize"] = 100;
						break;
					case CSVFieldIDs.SecutrityTypes.Future:
						imObj = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
						if (imObj != null && imObj is IMSecurityInfo)
						{
							row["TickSize"] = ((IMSecurityInfo)imObj).TickSize;
							row["ContractSize"] = ((IMSecurityInfo)imObj).ContractSize;
						}
						break;
				}
			}
		}

		private menuBaseTrades _menuSubTrades;
		private PropertyContainer _menuSubTradesContainer;
		private void ShowSubTrades(DataRow[] trades)
		{
			if (_menuSubTrades == null)
			{
				_menuSubTrades = new menuBaseTrades(trades, new ROCOrderProfile(rocOrdersList));
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

		#region - Order Modification Menu -

		private menuFullOrderModification _menuOrderModification;
		private PropertyContainer _menuOrderModificationContainer;
		private void ShowOrderModificationMenu()
		{
			ROCOrder order = GLOBAL.HOrders.RocItems[rocOrdersList.ShowMenuSymbol.orderID];

			if (order.Side != null)
			{
				SymbolSettingData data = GLOBAL.HSymbolSettingData.GetSymbolDefaults(order.SymbolDetail, order.SecType);
				if (_menuOrderModification == null)
				{
					try
					{
						_menuOrderModification = new menuFullOrderModification(
							order.Tag,
							data.qtyIncrement,
							data.showIncrement,
							data.limitPriceIncrement,
							data.stopPriceIncrement,
							data.pegPriceIncrement);
					}
					catch (Exception ex)
					{
						_menuOrderModification = null;
						GLOBAL.HROC.AddToException(ex);
					}

					_menuOrderModification.PropertyChanged += new PropertyChangedEventHandler(menuOrderModification_PropertyChanged);
				}

				if (_menuOrderModificationContainer == null)
				{
					_menuOrderModificationContainer = new PropertyContainer(_menuOrderModification);
					_menuOrderModificationContainer.Closed += new ToolStripDropDownClosedEventHandler(menuOrderModificationContainer_Closed);
				}
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
					GLOBAL.HROM.CancelAllOpenOrders("");
					break;
				case "CancelAllOf":
					GLOBAL.HROM.CancelBySymbol(_menuOrderModification.OrderID);
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
	}
}