using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

using RDSEx;
using DataGridViewEx;

namespace ROC
{
	[ToolboxItem(false), System.ComponentModel.DesignerCategory("UserControl")]
	public partial class menuBaseTrades : BaseUserControl
	{
		public menuBaseTrades(DataRow[] trades, DGVBaseProfile prof)
		{
			InitializeComponent();
			Initialize(trades, prof);
		}

		private void Initialize(DataRow[] trades, DGVBaseProfile prof)
		{
			BackColor = prof.GridBackColor;
			rocOrderTradesList.Initialize();
			rocOrderTradesList.SyncGUI(prof);

			try
			{
				foreach (DataRow trade in trades)
				{
					rocOrderTradesList.ROCGridData.Add(
						new DataGridViewEx.OrderSubTradesListData(
							(string)trade["ExecID"],
							(string)trade["SymbolDetail"],
							(long)trade["Side"],
							(long)trade["Qty"],
							(double)trade["Price"],
							(DateTime)trade["OmTime"],
							(double)trade["ContractSize"],
							(double)trade["TickSize"],
							(double)trade["DisplayFactor"]));
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void menuOrderTrades_Load(object sender, EventArgs e)
		{
			SetColumnMode(rocOrderTradesList, DataGridViewAutoSizeColumnMode.AllCells);
			ResizeToFitBoth(rocOrderTradesList);
			SetColumnMode(rocOrderTradesList, DataGridViewAutoSizeColumnMode.None);
		}
	}
}
