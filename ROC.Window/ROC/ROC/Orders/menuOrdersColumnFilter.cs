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
	public partial class menuOrdersColumnFilter : UserControl, INotifyPropertyChanged
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

		private bool _hideOrderID = false;
		public bool HideOrderID
		{
			get
			{
				return _hideOrderID;
			}
			set
			{
				if (_hideOrderID != value)
				{
					_hideOrderID = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideOrderID"));
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

		private bool _hideStatus = false;
		public bool HideStatus
		{
			get
			{
				return _hideStatus;
			}
			set
			{
				if (_hideStatus != value)
				{
					_hideStatus = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideStatus"));
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

		private bool _hideLeave = false;
		public bool HideLeave
		{
			get
			{
				return _hideLeave;
			}
			set
			{
				if (_hideLeave != value)
				{
					_hideLeave = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideLeave"));
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

		private bool _hideFilled = false;
		public bool HideFilled
		{
			get
			{
				return _hideFilled;
			}
			set
			{
				if (_hideFilled != value)
				{
					_hideFilled = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideFilled"));
				}
			}
		}

		private bool _hideAvgPrice = false;
		public bool HideAvgPrice
		{
			get
			{
				return _hideAvgPrice;
			}
			set
			{
				if (_hideAvgPrice != value)
				{
					_hideAvgPrice = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideAvgPrice"));
				}
			}
		}

		private bool _hideType = false;
		public bool HideType
		{
			get
			{
				return _hideType;
			}
			set
			{
				if (_hideType != value)
				{
					_hideType = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideType"));
				}
			}
		}

		private bool _hideDuration = false;
		public bool HideDuration
		{
			get
			{
				return _hideDuration;
			}
			set
			{
				if (_hideDuration != value)
				{
					_hideDuration = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideDuration"));
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

		public menuOrdersColumnFilter(
			bool hideOrderID, bool hideSymbol, bool hideStatus, 
			bool hideSide, bool hideQty, bool hideLeave, bool hidePrice,
			bool hideFilled, bool hideAvgPrice, bool hideType, bool hideDuration,
			bool hideExchange, bool hideTime, bool hideAccount)
		{
			InitializeComponent();

			_hideOrderID = hideOrderID;
			_hideSymbol = hideSymbol;
			_hideStatus = hideStatus;
			_hideSide = hideSide;
			_hideQty = hideQty;
			_hideLeave = hideLeave;
			_hidePrice = hidePrice;
			_hideFilled = hideFilled;
			_hideAvgPrice = hideAvgPrice;
			_hideType = hideType;
			_hideDuration = hideDuration;
			_hideExchange = hideExchange;
			_hideTime = hideTime;
			_hideAccount = hideAccount;

			UpdateColor(lblFilterOrderID, HideOrderID);
			UpdateColor(lblFilterSymbol, HideSymbol);
			UpdateColor(lblFilterStatus, HideStatus);
			UpdateColor(lblFilterSide, HideSide);
			UpdateColor(lblFilterQty, HideQty);
			UpdateColor(lblFilterLeave, HideLeave);
			UpdateColor(lblFilterPrice, HidePrice);
			UpdateColor(lblFilterFilled, HideFilled);
			UpdateColor(lblFilterAvgPrice, HideAvgPrice);
			UpdateColor(lblFilterType, HideType);
			UpdateColor(lblFilterDuration, HideDuration);
			UpdateColor(lblFilterExchange, HideExchange);
			UpdateColor(lblFilterTime, HideTime);
			UpdateColor(lblFilterAccount, HideAccount);
		}

		private void lblFilterOrderID_Click(object sender, EventArgs e)
		{
			HideOrderID = !HideOrderID;
			UpdateColor(lblFilterOrderID, HideOrderID);
		}

		private void lblFilterSymbol_Click(object sender, EventArgs e)
		{
			HideSymbol = !HideSymbol;
			UpdateColor(lblFilterSymbol, HideSymbol);
		}

		private void lblFilterStatus_Click(object sender, EventArgs e)
		{
			HideStatus = !HideStatus;
			UpdateColor(lblFilterStatus, HideStatus);
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

		private void lblFilterLeave_Click(object sender, EventArgs e)
		{
			HideLeave = !HideLeave;
			UpdateColor(lblFilterLeave, HideLeave);
		}

		private void lblFilterPrice_Click(object sender, EventArgs e)
		{
			HidePrice = !HidePrice;
			UpdateColor(lblFilterPrice, HidePrice);
		}

		private void lblFilterFilled_Click(object sender, EventArgs e)
		{
			HideFilled = !HideFilled;
			UpdateColor(lblFilterFilled, HideFilled);
		}

		private void lblFilterAvgPrice_Click(object sender, EventArgs e)
		{
			HideAvgPrice = !HideAvgPrice;
			UpdateColor(lblFilterAvgPrice, HideAvgPrice);
		}

		private void lblFilterType_Click(object sender, EventArgs e)
		{
			HideType = !HideType;
			UpdateColor(lblFilterType, HideType);
		}

		private void lblFilterDuration_Click(object sender, EventArgs e)
		{
			HideDuration = !HideDuration;
			UpdateColor(lblFilterDuration, HideDuration);
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
