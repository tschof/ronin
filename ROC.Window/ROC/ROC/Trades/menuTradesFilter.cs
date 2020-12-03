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
	public partial class menuTradesFilter : UserControl, INotifyPropertyChanged
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

		private bool _hideStocks = false;
		public bool HideStocks
		{
			get
			{
				return _hideStocks;
			}
			set
			{
				if (_hideStocks != value)
				{
					_hideStocks = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowStocks"));
				}
			}
		}

		private bool _hideFutures = false;
		public bool HideFutures
		{
			get
			{
				return _hideFutures;
			}
			set
			{
				if (_hideFutures != value)
				{
					_hideFutures = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowFutures"));
				}
			}
		}

		private bool _hideOptions = false;
		public bool HideOptions
		{
			get
			{
				return _hideOptions;
			}
			set
			{
				if (_hideOptions != value)
				{
					_hideOptions = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowOptions"));
				}
			}
		}

		public menuTradesFilter(bool hideStocks, bool hideFutures, bool hideOptions)
		{
			InitializeComponent();

			_hideStocks = hideStocks;
			_hideFutures = hideFutures;
			_hideOptions = hideOptions;

			UpdateColor(lblFilterStocks, HideStocks);
			UpdateColor(lblFilterFutures, HideFutures);
			UpdateColor(lblFilterOptions, HideOptions);
		}

		private void lblFilterStocks_Click(object sender, EventArgs e)
		{
			HideStocks = !HideStocks;
			UpdateColor(lblFilterStocks, HideStocks);
		}

		private void lblFilterFutures_Click(object sender, EventArgs e)
		{
			HideFutures = !HideFutures;
			UpdateColor(lblFilterFutures, HideFutures);
		}

		private void lblFilterOptions_Click(object sender, EventArgs e)
		{
			HideOptions = !HideOptions;
			UpdateColor(lblFilterOptions, HideOptions);
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
