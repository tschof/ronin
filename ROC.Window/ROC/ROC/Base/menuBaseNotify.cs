using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class menuBaseNotify : UserControl, INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		public menuBaseNotify()
		{
			InitializeComponent();
		}

		private void cmdWatchList_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("WatchList"));
		}

		private void cmdPlotList_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("PlotList"));
		}

		private void cmdStockTicket_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("StockTicket"));
		}

		private void cmdOptionTicket_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("OptionTicket"));
		}

		private void cmdFutureTicket_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("FutureTicket"));
		}

		private void cmdQuickTradeTicket_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("QuickTradeTicket"));
		}

		private void cmdBatchTicket_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("BatchTicket"));
		}

		private void cmdOrders_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Orders"));
		}

		private void cmdTrades_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Trades"));
		}

		private void cmdPositions_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Positions"));
		}

		private void cmdShowAll_Click(object sender, EventArgs e)
		{
			GLOBAL.HWindows.ShowAll();
		}

		private void cmdHideAll_Click(object sender, EventArgs e)
		{
			GLOBAL.HWindows.HideAll();
		}

		private void cmdExit_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Exit"));
		}
	}
}
