using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace NumericUpDownEx
{
	public partial class NumericUpDownVista : UserControl
	{
		public NumericUpDownVista()
		{
			InitializeComponent();
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			dspBase.Text = numBase.Value.ToString();
			base.OnPaint(e);
		}

		protected override void OnSizeChanged(EventArgs e)
		{
			dspBase.Width = Width - 18;
			dspBase.Height = Height - 5;

			base.OnSizeChanged(e);
		}
	}
}
