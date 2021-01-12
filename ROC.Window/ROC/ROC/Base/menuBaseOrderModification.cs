using System;
using System.ComponentModel;
using CSVEx;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class menuBaseOrderModification : BaseUserControl, INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			PropertyChanged?.Invoke(this, e);
		}

		#endregion

		private MenuOrderModification _menu;

		public string OrderID => _menu == null ? "" : _menu.CurrentOrder.Tag;

		public long Side => (_menu != null) ? _menu.CurrentOrder.Side : CSVFieldIDs.SideCodes.None;

		public string NewShare => numQuantity.Value.ToString();

		public string DeltaShare
		{
			get
			{
				decimal delta = numQuantity.Value - _menu.InitialLeaveQty;
				return delta == 0 ? "" : delta.ToString();
			}
		}

		public string NewPrice => numLimitPrice.Value == 0 ? "" : numLimitPrice.Value.ToString();

		public string NewStopPrice => numStopPrice.Value == 0 ? "" : numStopPrice.Value.ToString();

		public string NewPegPrice => numPegPrice.Value == 0 ? "" : numPegPrice.Value.ToString();

		public string NewDuration => (_menu == null) ? CSVFieldIDs.TIFCodes.Day.ToString() : _menu.NewDuration;

		public menuBaseOrderModification(
			string orderID,
			decimal qtyIncrement,
			decimal showIncrement,
			decimal limitPriceIncrement,
			decimal stopPriceIncrement,
			decimal pegPriceIncrement)
		{
			InitializeComponent();

			_menu = new MenuOrderModification(this);
			_menu.Initialize(orderID, qtyIncrement, showIncrement, limitPriceIncrement, stopPriceIncrement, pegPriceIncrement);
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Cancel"));
		}

		private void cmdClose_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Close"));
		}

		private void cmdReplace_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Replace"));
		}

		private void cmdCancelAll_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("CancelAll"));
		}
	}
}
