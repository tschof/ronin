using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseAlgoTypeColumn : DataGridViewColumn
	{
		public DGVBaseAlgoTypeColumn()
		{
			base.CellTemplate = new DGVBaseAlgoTypeCell();
		}
	}
}
