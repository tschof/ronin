using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseTypeColumn : DataGridViewColumn
	{
		public DGVBaseTypeColumn()
		{
			base.CellTemplate = new DGVBaseTypeCell();
		}
	}
}
