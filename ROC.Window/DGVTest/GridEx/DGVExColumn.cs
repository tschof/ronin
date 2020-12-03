using System;
using System.Windows.Forms;

namespace GridEx
{
	public class DGVExColumn : DataGridViewColumn
	{
		public DGVExColumn()
		{
			base.CellTemplate = new DGVExCell();
		}
	}
}
