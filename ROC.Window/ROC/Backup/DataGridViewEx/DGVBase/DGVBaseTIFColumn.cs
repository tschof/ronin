using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseTIFColumn : DataGridViewColumn
	{
		public DGVBaseTIFColumn()
		{
			base.CellTemplate = new DGVBaseTIFCell();
		}
	}
}
