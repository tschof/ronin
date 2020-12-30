using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class menuBaseProcessing : UserControl
	{
		public menuBaseProcessing()
		{
			InitializeComponent();
		}

		public string Caption
		{
			get
			{
				return lblProcessing.Text;
			}
			set
			{
				lblProcessing.Text = value;
			}
		}
	}
}
