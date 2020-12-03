using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class menuTradesColumnFilter : UserControl, INotifyPropertyChanged
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

		private bool _hideExecID = false;
		public bool HideExecID
		{
			get
			{
				return _hideExecID;
			}
			set
			{
				if (_hideExecID != value)
				{
					_hideExecID = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideExecID"));
				}
			}
		}

		private bool _hideSymbol = false;
		public bool HideSymbol
		{
			get
			{
				return _hideSymbol;
			}
			set
			{
				if (_hideSymbol != value)
				{
					_hideSymbol = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideSymbol"));
				}
			}
		}

		private bool _hideSide = false;
		public bool HideSide
		{
			get
			{
				return _hideSide;
			}
			set
			{
				if (_hideSide != value)
				{
					_hideSide = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideSide"));
				}
			}
		}

		private bool _hideQty = false;
		public bool HideQty
		{
			get
			{
				return _hideQty;
			}
			set
			{
				if (_hideQty != value)
				{
					_hideQty = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideQty"));
				}
			}
		}

		private bool _hidePrice = false;
		public bool HidePrice
		{
			get
			{
				return _hidePrice;
			}
			set
			{
				if (_hidePrice != value)
				{
					_hidePrice = value;
					onPropertyChanged(new PropertyChangedEventArgs("HidePrice"));
				}
			}
		}

		private bool _hideValue = false;
		public bool HideValue
		{
			get
			{
				return _hideValue;
			}
			set
			{
				if (_hideValue != value)
				{
					_hideValue = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideValue"));
				}
			}
		}

		private bool _hideExchange = false;
		public bool HideExchange
		{
			get
			{
				return _hideExchange;
			}
			set
			{
				if (_hideExchange != value)
				{
					_hideExchange = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideExchange"));
				}
			}
		}

		private bool _hideTime = false;
		public bool HideTime
		{
			get
			{
				return _hideTime;
			}
			set
			{
				if (_hideTime != value)
				{
					_hideTime = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideTime"));
				}
			}
		}

		private bool _hideAccount = false;
		public bool HideAccount
		{
			get
			{
				return _hideAccount;
			}
			set
			{
				if (_hideAccount != value)
				{
					_hideAccount = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideAccount"));
				}
			}
		}

		public menuTradesColumnFilter(
			bool hideExecID, bool hideSymbol, bool hideSide, 
			bool hideQty, bool hidePrice, bool hideValue, 
			bool hideExchange, bool hideTime, bool hideAccount)
		{
			InitializeComponent();

			_hideExecID = hideExecID;
			_hideSymbol = hideSymbol;
			_hideSide = hideSide;
			_hideQty = hideQty;
			_hidePrice = hidePrice;
			_hideValue = hideValue;
			_hideExchange = hideExchange;
			_hideTime = hideTime;
			_hideAccount = hideAccount;

			UpdateColor(lblFilterExecID, HideExecID);
			UpdateColor(lblFilterSymbol, HideSymbol);
			UpdateColor(lblFilterSide, HideSide);
			UpdateColor(lblFilterQty, HideQty);
			UpdateColor(lblFilterPrice, HidePrice);
			UpdateColor(lblFilterValue, HideValue);
			UpdateColor(lblFilterExchange, HideExchange);
			UpdateColor(lblFilterTime, HideTime);
			UpdateColor(lblFilterAccount, HideAccount);
		}

		private void lblFilterExecID_Click(object sender, EventArgs e)
		{
			HideExecID = !HideExecID;
			UpdateColor(lblFilterExecID, HideExecID);
		}

		private void lblFilterSymbol_Click(object sender, EventArgs e)
		{
			HideSymbol = !HideSymbol;
			UpdateColor(lblFilterSymbol, HideSymbol);
		}

		private void lblFilterSide_Click(object sender, EventArgs e)
		{
			HideSide = !HideSide;
			UpdateColor(lblFilterSide, HideSide);
		}

		private void lblFilterQty_Click(object sender, EventArgs e)
		{
			HideQty = !HideQty;
			UpdateColor(lblFilterQty, HideQty);
		}

		private void lblFilterPrice_Click(object sender, EventArgs e)
		{
			HidePrice = !HidePrice;
			UpdateColor(lblFilterPrice, HidePrice);
		}

		private void lblFilterValue_Click(object sender, EventArgs e)
		{
			HideValue = !HideValue;
			UpdateColor(lblFilterValue, HideValue);
		}

		private void lblFilterExchange_Click(object sender, EventArgs e)
		{
			HideExchange = !HideExchange;
			UpdateColor(lblFilterExchange, HideExchange);
		}

		private void lblFilterTime_Click(object sender, EventArgs e)
		{
			HideTime = !HideTime;
			UpdateColor(lblFilterTime, HideTime);
		}

		private void lblFilterAccount_Click(object sender, EventArgs e)
		{
			HideAccount = !HideAccount;
			UpdateColor(lblFilterAccount, HideAccount);
		}

		private void UpdateColor(Label lbl, bool hideVal)
		{
			if (hideVal)
			{
				lbl.ForeColor = Color.White;
			}
			else
			{
				lbl.ForeColor = Color.GreenYellow;
			}
		}
	}
}
