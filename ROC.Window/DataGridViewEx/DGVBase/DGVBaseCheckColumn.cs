using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseCheckColumn : DataGridViewColumn
	{
		public DGVBaseCheckColumn()
		{
			base.CellTemplate = new DGVBaseCheckCell();
		}
	}
}
