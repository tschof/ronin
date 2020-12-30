using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using RDSEx;
using CSVEx;
using MarketDataEx;
using LabelEx;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class menuFullOrderModification : BaseUserControl, INotifyPropertyChanged
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
		
		private NumericUpDown _crrentPriceObj;

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

		public menuFullOrderModification(
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

			_crrentPriceObj = numLimitPrice;
		}

		public void UpdateOrderModificationTicketByProcess(bool updateIM, Dictionary<string, MDServerToClient> deltas)
		{
			_menu.UpdateOrderModificationTicketByProcess(updateIM, deltas);
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

		private void cmdCancelAllOf_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("CancelAllOf"));
		}

		private void LimitPriceChanged(object sender, EventArgs e)
		{
			_menu.SetOptionPriceIncrement(numLimitPrice, dspLimitPriceTickSize);
		}

		private void StopPriceChanged(object sender, EventArgs e)
		{
			_menu.SetOptionPriceIncrement(numStopPrice, dspStopPriceTickSize);
		}

		#region - Price Event -

		private void PriceSelection_Click(object sender, EventArgs e)
		{
			try
			{
				if (((Control)sender).Name.ToUpper().Contains("LIMITPRICE"))
				{
					SelectLimitPrice();
				}
				else if (((Control)sender).Name.ToUpper().Contains("STOPPRICE"))
				{
					SelectStopPrice();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Price_MouseDown(object sender, MouseEventArgs e)
		{
			try
			{
				((LabelBase)sender).ForeColor = Color.LimeGreen;
				if (((LabelBase)sender).Value != null && ((LabelBase)sender).Value.ToString() != "")
				{
					_crrentPriceObj.Value = Convert.ToDecimal(((LabelBase)sender).Value);
				}

				TogglePriceSelection();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Price_MouseUp(object sender, MouseEventArgs e)
		{
			try
			{
				((LabelBase)sender).ForeColor = Color.Gainsboro;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void TogglePriceSelection()
		{
			if (panelLimitPrice.Enabled && panelStopPrice.Enabled)
			{
				if (_crrentPriceObj.Name.ToUpper().Contains("LIMITPRICE"))
				{
					SelectStopPrice();
				}
				else
				{
					SelectLimitPrice();
				}
			}
		}

		private void SelectLimitPrice()
		{
			_crrentPriceObj = numLimitPrice;
			panelLimitPrice.BackColor = Color.CornflowerBlue;
			panelStopPrice.BackColor = Color.Transparent;
		}

		private void SelectStopPrice()
		{
			_crrentPriceObj = numStopPrice;
			panelLimitPrice.BackColor = Color.Transparent;
			panelStopPrice.BackColor = Color.CornflowerBlue;
		}

		#endregion
	}
}
