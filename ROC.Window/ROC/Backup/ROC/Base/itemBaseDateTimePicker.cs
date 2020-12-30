using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class itemBaseDateTimePicker : UserControl, INotifyPropertyChanged
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

		public itemBaseDateTimePicker()
		{
			InitializeComponent();
		}

		public string SelectedTime
		{
			get
			{
				DateTime dt = dateTimePicker.Value.ToUniversalTime();
				return dt.ToString("yyyyMMdd-HH:mm:ss");
			}
		}

		private void cmdOk_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Ok"));
		}

		private void cmdExit_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Exit"));
		}
	}
}
