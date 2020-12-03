using System;
using System.Windows.Forms;

namespace LabelEx
{
	[System.ComponentModel.DesignerCategory("code")]
	public class ToolTipLabel : Label
	{
		public ToolTipLabel()
		{
		}

		#region - ToolTip -

		private ToolTip _toolTip = new ToolTip();
		public void SetToolTip(string tip)
		{
			_toolTip.SetToolTip(this, tip);
		}

		#endregion
	}
}
