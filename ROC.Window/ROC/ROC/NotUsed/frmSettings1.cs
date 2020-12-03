using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmSettings : VistaWindowBorder
	{
		#region - Constructor -

		public frmSettings()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			DGVAutoFocusSettings.Rows.Clear();

			DGVAutoFocusSettings.Rows.Add("WatchList", Configuration.ROC.Default.EnableAutoFocusWatchList);
			DGVAutoFocusSettings.Rows.Add("PlotList", Configuration.ROC.Default.EnableAutoFocusPlotList);
			DGVAutoFocusSettings.Rows.Add("StockTicket", Configuration.ROC.Default.EnableAutoFocusStockTicket);
			DGVAutoFocusSettings.Rows.Add("FutureTicket", Configuration.ROC.Default.EnableAutoFocusFutureTicket);
			DGVAutoFocusSettings.Rows.Add("OptionTicket", Configuration.ROC.Default.EnableAutoFocusOptionTicket);
			DGVAutoFocusSettings.Rows.Add("QuickTicket", Configuration.ROC.Default.EnableAutoFocusQuickTicket);
			DGVAutoFocusSettings.Rows.Add("BatchTicket", Configuration.ROC.Default.EnableAutoFocusBatchTicket);
			DGVAutoFocusSettings.Rows.Add("AutoSpreadTicket", Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket);
			DGVAutoFocusSettings.Rows.Add("Orders", Configuration.ROC.Default.EnableAutoFocusOrders);
			DGVAutoFocusSettings.Rows.Add("Trades", Configuration.ROC.Default.EnableAutoFocusTrades);
			DGVAutoFocusSettings.Rows.Add("Positions", Configuration.ROC.Default.EnableAutoFocusPositions);

			FormClosing += new FormClosingEventHandler(Form_FormClosing);

			DGVAutoFocusSettings.SelectionChanged += new EventHandler(DGVAutoFocusSettings_SelectionChanged);
			DGVAutoFocusSettings.CellMouseDown += new DataGridViewCellMouseEventHandler(DGVAutoFocusSettings_CellMouseDown);

			DGVAutoFocusSettings.MouseEnter += new EventHandler(DGVAutoFocusSettings_MouseEnter);
		}

		#endregion

		#region - Form Event -

		private void Form_FormClosing(object sender, EventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}

		#endregion

		#region - Grid Event -

		private void DGVAutoFocusSettings_SelectionChanged(object sender, EventArgs e)
		{
			// Make sure nothing is selected
			foreach (DataGridViewCell cell in DGVAutoFocusSettings.SelectedCells)
			{
				cell.Selected = false;
			}
		}

		private void DGVAutoFocusSettings_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
			}
		}

		private void DGVAutoFocusSettings_MouseEnter(object sender, EventArgs e)
		{
			ActivateGrid(DGVAutoFocusSettings);
		}

		#endregion

		private void cmdApply_Click(object sender, EventArgs e)
		{
			try
			{
				for (int rowCount = 0; rowCount < DGVAutoFocusSettings.Rows.Count; rowCount++)
				{
					switch (DGVAutoFocusSettings.Rows[rowCount].Cells["Window"].Value.ToString())
					{
						case "WatchList":
							Configuration.ROC.Default.EnableAutoFocusWatchList = GetAutoFocusEnableFlag(rowCount);
							break;
						case "PlotList":
							Configuration.ROC.Default.EnableAutoFocusPlotList = GetAutoFocusEnableFlag(rowCount);
							break;
						case "StockTicket":
							Configuration.ROC.Default.EnableAutoFocusStockTicket = GetAutoFocusEnableFlag(rowCount);
							break;
						case "FutureTicket":
							Configuration.ROC.Default.EnableAutoFocusFutureTicket = GetAutoFocusEnableFlag(rowCount);
							break;
						case "OptionTicket":
							Configuration.ROC.Default.EnableAutoFocusOptionTicket = GetAutoFocusEnableFlag(rowCount);
							break;
						case "QuickTicket":
							Configuration.ROC.Default.EnableAutoFocusQuickTicket = GetAutoFocusEnableFlag(rowCount);
							break;
						case "BatchTicket":
							Configuration.ROC.Default.EnableAutoFocusBatchTicket = GetAutoFocusEnableFlag(rowCount);
							break;
						case "AutoSpreadTicket":
							Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket = GetAutoFocusEnableFlag(rowCount);
							break;
						case "Orders":
							Configuration.ROC.Default.EnableAutoFocusOrders = GetAutoFocusEnableFlag(rowCount);
							break;
						case "Trades":
							Configuration.ROC.Default.EnableAutoFocusTrades = GetAutoFocusEnableFlag(rowCount);
							break;
						case "Positions":
							Configuration.ROC.Default.EnableAutoFocusPositions = GetAutoFocusEnableFlag(rowCount);
							break;
					}
				}

				Configuration.ROC.Default.Save();
				Close();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private bool GetAutoFocusEnableFlag(int rowCount)
		{
			if (DGVAutoFocusSettings.Rows[rowCount].Cells["Enable"].Value != null)
			{
				return (bool)DGVAutoFocusSettings.Rows[rowCount].Cells["Enable"].Value;
			}
			else
			{
				return false;
			}
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}
	}
}