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
	public partial class itemBasePriceSizePair : UserControl
	{
		public itemBasePriceSizePair()
		{
			InitializeComponent();
		}

		public string PZPName
		{
			get
			{
				return lblName.Text;
			}
			set
			{
				lblName.Text = value;
			}
		}

		[Browsable(false)]
		public double PZPPrice
		{
			set
			{
				dspPrice.Value = value;
			}
		}

		[Browsable(false)]
		public long PZPSize
		{
			set
			{
				dspSize.Value = value;
			}
		}
	}
}
