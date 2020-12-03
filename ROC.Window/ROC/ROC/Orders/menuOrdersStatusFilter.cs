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
	public partial class menuOrdersStatusFilter : UserControl, INotifyPropertyChanged
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

		private bool _hidePartialFilled = false;
		public bool HidePartialFilled
		{
			get
			{
				return _hidePartialFilled;
			}
			set
			{
				if (_hidePartialFilled != value)
				{
					_hidePartialFilled = value;
					onPropertyChanged(new PropertyChangedEventArgs("HidePartialFilled"));
				}
			}
		}

		private bool _hideReplaced = false;
		public bool HideReplaced
		{
			get
			{
				return _hideReplaced;
			}
			set
			{
				if (_hideReplaced != value)
				{
					_hideReplaced = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideReplaced"));
				}
			}
		}

		private bool _hideOpen = false;
		public bool HideOpen
		{
			get
			{
				return _hideOpen;
			}
			set
			{
				if (_hideOpen != value)
				{
					_hideOpen = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideOpen"));
				}
			}
		}

		private bool _hideRejected = false;
		public bool HideRejected
		{
			get
			{
				return _hideRejected;
			}
			set
			{
				if (_hideRejected != value)
				{
					_hideRejected = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideRejected"));
				}
			}
		}

		private bool _hideCancelled = false;
		public bool HideCancelled
		{
			get
			{
				return _hideCancelled;
			}
			set
			{
				if (_hideCancelled != value)
				{
					_hideCancelled = value;
					onPropertyChanged(new PropertyChangedEventArgs("HideCancelled"));
				}
			}
		}

		public menuOrdersStatusFilter(bool hideFilled, bool hidePartialFilled, bool hideReplaced, bool hideOpen, bool hideRejected, bool hideCancelled)
		{
			InitializeComponent();

			_hideFilled = hideFilled;
			_hidePartialFilled = hidePartialFilled;
			_hideReplaced = hideReplaced;
			_hideOpen = hideOpen;
			_hideRejected = hideRejected;
			_hideCancelled = hideCancelled;

			UpdateColor(lblFilterFilled, HideFilled);
			UpdateColor(lblFilterPartialFilled, HidePartialFilled);
			UpdateColor(lblFilterReplaced, HideReplaced);
			UpdateColor(lblFilterOpen, HideOpen);
			UpdateColor(lblFilterRejected, HideRejected);
			UpdateColor(lblFilterCancelled, HideCancelled);
		}

		private void lblFilterFilled_Click(object sender, EventArgs e)
		{
			HideFilled = !HideFilled;
			UpdateColor(lblFilterFilled, HideFilled);
		}

		private void lblFilterPartialFilled_Click(object sender, EventArgs e)
		{
			HidePartialFilled = !HidePartialFilled;
			UpdateColor(lblFilterPartialFilled, HidePartialFilled);
		}

		private void lblFilterReplaced_Click(object sender, EventArgs e)
		{
			HideReplaced = !HideReplaced;
			UpdateColor(lblFilterReplaced, HideReplaced);
		}

		private void lblFilterOpen_Click(object sender, EventArgs e)
		{
			HideOpen = !HideOpen;
			UpdateColor(lblFilterOpen, HideOpen);
		}

		private void lblFilterRejected_Click(object sender, EventArgs e)
		{
			HideRejected = !HideRejected;
			UpdateColor(lblFilterRejected, HideRejected);
		}

		private void lblFilterCancelled_Click(object sender, EventArgs e)
		{
			HideCancelled = !HideCancelled;
			UpdateColor(lblFilterCancelled, HideCancelled);
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
