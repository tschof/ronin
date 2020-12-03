using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class dndStock : Form
	{
		public dndStock(Color backcolor, double price, long size)
		{
			InitializeComponent();

			BackColor = backcolor;
			dspPrice.Text = price.ToString("F2");
			dspSize.Text = size.ToString();
		}
	}
}