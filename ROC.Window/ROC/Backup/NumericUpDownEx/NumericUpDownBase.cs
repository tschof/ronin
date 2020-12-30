using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace NumericUpDownEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class NumericUpDownBase : NumericUpDown
	{
		protected override void OnMouseWheel(MouseEventArgs e)
		{
			decimal newValue = Value + Increment * (e.Delta / 120);
			if (newValue < Minimum)
			{
				Value = Minimum;
			}
			else if (newValue > Maximum)
			{
				Value = Maximum;
			}
			else
			{
				Value = newValue;
			}
			//base.OnMouseWheel(e);
		}

		protected override void OnMouseEnter(EventArgs e)
		{
			base.OnMouseEnter(e);
		}
	}
}
