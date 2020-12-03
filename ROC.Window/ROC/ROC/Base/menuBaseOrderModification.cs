using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using RDSEx;
using CSVEx;
using LabelEx;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class menuBaseOrderModification : BaseUserControl, INotifyPropertyChanged
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
		
		#region - Local Variable -

		private MenuOrderModification _menu;

		#endregion

		public string OrderID
		{
			get
			{
				if (_menu != null)
				{
					return _menu.CurrentOrder.Tag;
				}
				return "";
			}
		}

		public long Side
		{
			get
			{
				if (_menu != null && _menu.CurrentOrder.Side != null)
				{
					return (long)_menu.CurrentOrder.Side;
				}
				return 0;
			}
		}

		public string NewShare
		{
			get
			{
				return numQuantity.Value.ToString();
			}
		}

		public string DeltaShare
		{
			get
			{
				decimal delta = numQuantity.Value - _menu.InitialLeaveQty;
				if (delta == 0)
				{
					return "";
				}
				else
				{
					return Convert.ToString(delta);
				}
			}
		}

		public string NewPrice
		{
			get
			{
				if (numLimitPrice.Value == 0)
				{
					return "";
				}
				else
				{
					return numLimitPrice.Value.ToString();
				}
			}
		}

		public string NewStopPrice
		{
			get
			{
				if (numStopPrice.Value == 0)
				{
					return "";
				}
				else
				{
					return numStopPrice.Value.ToString();
				}
			}
		}

		public string NewPegPrice
		{
			get
			{
				if (numPegPrice.Value == 0)
				{
					return "";
				}
				else
				{
					return numPegPrice.Value.ToString();
				}
			}
		}

		public string NewDuration
		{
			get
			{
				if (_menu != null)
				{
					return _menu.NewDuration;
				}
				return CSVFieldIDs.TIFCodes.Day.ToString();
			}
		}

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
