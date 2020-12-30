using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using FormEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using CSVEx;
using ROMEx;
using RDSEx;
using System.Diagnostics;
using System.IO;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmBatchTicket : BaseBatchTicket
	{
		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return BatchGrid.Processing;
			}
			set
			{
				BatchGrid.Processing = value;
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
					_searchView = new DataView(BatchGrid.RocGridTable);
					_searchView.Sort = "OrderID";
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

		private bool _updatingUI = false;
		private List<ROCOrder> _rocOrders = new List<ROCOrder>();

		#endregion

		#region - Constructor -

		public frmBatchTicket()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			BatchGrid = rocBatch;

			FitClicked += new EventHandler(frmBatchTicket_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);
			RefreshClicked += new EventHandler(frmBatchTicket_RefreshClicked);
			DisplayClicked += new EventHandler(frmBatchTicket_DisplayClicked);
			UITimerTicked += new EventHandler(frmBatchTicket_UITimerTicked);

			Load += new EventHandler(frmBatchTicket_Load);
			FormClosing += new FormClosingEventHandler(frmBatchTicket_FormClosing);

			BatchGrid.ProcessingChanged += new EventHandler(rocBatch_ProcessingChanged);
			BatchGrid.ShouldScrollToLastRowChanged += new EventHandler(rocBatch_ShouldScrollToLastRowChanged);
			BatchGrid.RefreshAggragationChanged += new EventHandler(rocBatch_RefreshAggragationChanged);

			BatchGrid.Initialize();

			BatchGrid.MouseEnter += new EventHandler(rocBatch_MouseEnter);

			BatchGrid.PropertyChanged += new PropertyChangedEventHandler(rocBatch_PropertyChanged);
			BatchGrid.KeyDown += new KeyEventHandler(rocBatch_KeyDown);

			StartTimer(Configuration.User.Default.UIUpdateRate);
		}

		#endregion

		#region - Form Event -

		private void frmBatchTicket_UITimerTicked(object sender, EventArgs e)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				LoadRocOrderByTimer();
			}
		}

		private void frmBatchTicket_FitClicked(object sender, EventArgs e)
		{
			if (!BatchGrid.Processing)
			{
				SetColumnMode(BatchGrid, DataGridViewAutoSizeColumnMode.DisplayedCells);
				ResizeToFit(BatchGrid);
				SetColumnMode(BatchGrid, DataGridViewAutoSizeColumnMode.None);
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

		private void frmBatchTicket_RefreshClicked(object sender, EventArgs e)
		{
		}

		private void frmBatchTicket_DisplayClicked(object sender, EventArgs e)
		{
		}

		private void frmBatchTicket_Load(object sender, EventArgs e)
		{
		}

		private void frmBatchTicket_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void updMultiple_ValueChanged(object sender, EventArgs e)
		{
			if (BatchGrid.RocGridTable.Rows.Count > 0)
			{
				BatchGrid.RefreshAggragation = true;
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

		private void rocBatch_ProcessingChanged(object sender, EventArgs e)
		{
			BatchGrid.Enabled = !BatchGrid.Processing;
			//CaptionBar.Enabled = !BatchGrid.Processing;
			//panelTradeCommands.Enabled = !BatchGrid.Processing;

			if (BatchGrid.Processing)
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

		private void rocBatch_ShouldScrollToLastRowChanged(object sender, EventArgs e)
		{
			if (!BatchGrid.Processing && BatchGrid.ShouldScrollToLastRow)
			{
				BatchGrid.ShouldScrollToLastRow = false;
				BatchGrid.ScrollToLastRow();
			}
		}

		private void rocBatch_RefreshAggragationChanged(object sender, EventArgs e)
		{
			if (!IsProcessing && BatchGrid.RefreshAggragation)
			{
				//BatchGrid.UnshareRows();

				BatchGrid.RefreshAggragation = false;
				UpdateAggragation();
			}
		}

		private void rocBatch_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				//case "ShowColumnMenu":
				//    ShowColumnMenu();
				//    break;
				case "ShowStatusMenu":
					ShowStatusMenu();
					break;
				case "ShowSideMenu":
					ShowSideMenu();
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

		private void rocBatch_KeyDown(object sender, KeyEventArgs e)
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

		private void rocBatch_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusBatchTicket)
			{
				ActivateGrid(BatchGrid);
			}
		}

		#endregion

		#region - Binding -

		#region - Used by Process Thread -

		public void LoadRocOrders(object input)
		{
			List<ROCOrder> orders = (List<ROCOrder>)input;
			if (orders.Count > 0)
			{
				if (GLOBAL.UseDelayedUpdate)
				{
					lock (_rocOrders)
					{
						_rocOrders.AddRange(orders.ToArray());
					}
				}
				else
				{
					lock (BatchGrid)
					{
						foreach (ROCOrder order in orders)
						{
							LoadRocOrder(order);
							//System.Threading.Thread.Sleep(1);
						}
					}
				}
			}
		}

		private void LoadRocOrderByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;
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
						lock (BatchGrid)
						{
							foreach (ROCOrder order in orders)
							{
								LoadRocOrder(order);
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
			GLOBAL.HROC.AddToStatusLogs("Batch Ticket ExecuteSelected Clicked");
			if (chkAutoReset.Checked)
			{
				GLOBAL.HROC.AddToStatusLogs("Batch Ticket ExecuteSelected AutoReset");
				ResetDataRow();
			}

			BuildOrder(true);
		}

		private void cmdExecuteAll_Click(object sender, EventArgs e)
		{
			GLOBAL.HROC.AddToStatusLogs("Batch Ticket ExecuteAll Chlicked");
			if (chkAutoReset.Checked)
			{
				GLOBAL.HROC.AddToStatusLogs("Batch Ticket ExecuteAll AutoReset");
				ResetDataRow();
			}

			BuildOrder();
		}

		#endregion

		#region - Export & Import -

		public override string ExportGrid()
		{
			ROCBatchProfile prof = new ROCBatchProfile(rocBatch);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public override void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCBatchProfile prof = (ROCBatchProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCBatchProfile(rocBatch, prof);
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
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion
	}
}