using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("code")]
	public class L1GridView : DataGridView
	{
		public L1GridView()
		{
			this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer | ControlStyles.OptimizedDoubleBuffer, true);
		}
	}
}
