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
	public partial class menuTradesFile : UserControl, INotifyPropertyChanged
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

		public menuTradesFile()
		{
			InitializeComponent();
		}

		private void cmdClone_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Clone"));
		}

		private void cmdCopy_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Copy"));
		}

		private void cmdPrint_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Print"));
		}

		private void cmdSaveAs_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("SaveAs"));
		}
	}
}
