using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using FormEx;
using RDSEx;
using DataGridViewEx;
using SerializationEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmOrders : VistaWindowBorder
	{
		public frmOrders()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			FitClicked += new EventHandler(frmOrders_FitClicked);
			RefreshClicked += new EventHandler(frmOrders_RefreshClicked);
			FormClosing += new FormClosingEventHandler(frmOrders_FormClosing);

			rocOrdersList.Initialize();

			LoadROCOrders();
		}

		private void frmOrders_FitClicked(object sender, EventArgs e)
		{
			SetColumnMode(rocOrdersList, DataGridViewAutoSizeColumnMode.AllCells);
			ResizeToFit(rocOrdersList);
			SetColumnMode(rocOrdersList, DataGridViewAutoSizeColumnMode.None);
		}

		private void frmOrders_RefreshClicked(object sender, EventArgs e)
		{
			LoadROCOrders();
		}

		private void frmOrders_FormClosing(object sender, FormClosingEventArgs e)
		{
			GLOBAL.HWindows.CloseWindow(this);
		}

		public string ExportGrid()
		{
			ROCOrdersProfile prof = new ROCOrdersProfile(rocOrdersList);

			ToBinary b = new ToBinary();
			byte[] bytes = b.Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportGrid(string s)
		{
			if (s != null && s != "")
			{
				ToBinary b = new ToBinary();
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCOrdersProfile prof = new ROCOrdersProfile(rocOrdersList, (ROCOrdersProfile)b.Deserialize(bytes, SerializationTypes.Normal));
			}
		}

		private void LoadROCOrders()
		{
			if (GLOBAL.HRDS.GotRocOrders)
			{
				rocOrdersList.OrdersTable.Clear();
				foreach (ROCOrder ord in GLOBAL.HRDS.RocOrders.Values)
				{
					rocOrdersList.OrdersTable.Rows.Add(
						new object[] {
						ord.Tag,
						ord.Symbol,
						ord.Status,
						ord.Side,
						ord.Qty,
						ord.Price,
						ord.CumQty,
						ord.AvgPrice,
						ord.OrderType,
						ord.TIF,
						ord.DestID,
						ord.OmTime,
						ord.ClearingAcct });
				}
			}
		}

		public void BindGrid(ROCOrder[] orders)
		{
			foreach (ROCOrder ord in orders)
			{
				DataRow[] rows = rocOrdersList.OrdersTable.Select("Tag = '" + ord.Tag + "'");

				if (rows.Length > 0)
				{
					foreach (DataRow row in rows)
					{
						if ((long)row["Status"] != ord.Status) row["Status"] = ord.Status;
						if ((long)row["Qty"] != ord.Qty) row["Qty"] = ord.Qty;
						if ((double)row["Price"] != ord.Price) row["Price"] = ord.Price;
						if ((long)row["CumQty"] != ord.CumQty) row["CumQty"] = ord.CumQty;
						if ((double)row["AvgPrice"] != ord.AvgPrice) row["AvgPrice"] = ord.AvgPrice;
						if ((DateTime)row["OmTime"] != ord.OmTime) row["OmTime"] = ord.OmTime;

						row.AcceptChanges();
					}
				}
				else
				{
					// New Order
					rocOrdersList.OrdersTable.Rows.Add(
						new object[] {
							ord.Tag,
							ord.Symbol,
							ord.Status,
							ord.Side,
							ord.Qty,
							ord.Price,
							ord.CumQty,
							ord.AvgPrice,
							ord.OrderType,
							ord.TIF,
							ord.DestID,
							ord.OmTime,
							ord.ClearingAcct });
				}
			}
		}
	}
}