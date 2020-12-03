using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVTextColumn : DataGridViewColumn
	{
		public DGVTextColumn()
		{
			base.CellTemplate = new DGVTextCell();
		}
	}
}
